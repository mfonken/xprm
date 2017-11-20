//
//  kmat.c
//  kmatrix
//
//  Created by Matthew Fonken on 10/9/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include "kmat.h"

#define KMAT_DEBUG
//#define EXT_DEBUG

#define MAX_DENSITY_DIFF        1 // pixel units
#define KALMAN_SORT_SCALE_STEP  50
#define KALMAN_PAIR_MAX         3
#define MIN_PROB                0.1
#define MAX_KALMAN_LIFE         10.0 //(seconds)
#define SELECTION_THRESHOLD     0

#define GTTHR(X,Y,T) X>(Y+T)

void reverseLookup( lookup_t * l, int len )
{
    for(int i = 0; i < len/2; i++)
    {
        int o = len - i;
        int t = l[o];
        l[o] = l[i];
        l[i] = t;
    }
}

void printKM( kmat_t * m )
{
#ifdef KMAT_DEBUG
    int ml = MAX_KALMANS;// m->k_index,
    int pl = MAX_PEAKS;
//    printf("****KALMAN MATRIX*****\n");
//    printf("Index\tSort\tValue\tSel\tPair\n");
//    for(int i = 0, il = m->lookup[0]; i < ml; i++, il = m->lookup[i])
//    {
//        printf("%3d\t\t%3d",i,il);
//        printf("\t\t%3.2f\t%d\t%d\n",m->value[il],m->selection[il],m->pair[il]);
//    }
    
    /* Draw matrix */
    printf("%dx%d Matrix:\n\t\t",ml, pl);
    for(int j = 0; j < pl; j++) printf("\t\t\t\t\t%d", j);
    printf("\n");
    for(int i = 0, il = m->lookup[0]; i < ml; i++, il = m->lookup[i])
    {
        
        double s = m->value[il] + ( KALMAN_SORT_SCALE_STEP * m->pair[il] );
        printf("%3d[%3d](%.3f)",i, il, s);
        for(int j = 0; j < pl; j++)
        {
            kalman_t k = m->kalmans[il][j];
            double v = (k.value < 1000)?k.value:999;
            double p = (k.prev < 1000)?k.prev:999;
            double d = (k.density < 1000)?k.density:999;
            char c = ' ';
            if(j == m->selection[il]) c = '|';
            printf("\t\t%cv%3.2f|p%3.2f|d%3.2f%c",c,v,p,d,c);
        }
        printf("\n");
        printf("s%3dv%3.2f\t", m->selection[il], m->value[il]);
        for(int j = 0; j < pl; j++)
        {
            kalman_t k = m->kalmans[il][j];
            double k0 = k.K[0], k1 = k.K[1];
            char c = ' ';
            if(j == m->selection[il]) c = '|';
            printf("\t\t%cK0>%1.2f|K1>%1.2f%c",c,k0,k1,c);
        }
        printf("\n\t\t\t\t");
        for(int j = 0; j < pl; j++)
        {
            kalman_t k = m->kalmans[i][j];
            double p00 = k.P_k[0][0], p01 = k.P_k[0][1];
            char c = ' ';
            if(j == m->selection[il]) c = '|';
            printf("\t%cP00>%1.2f|P01>%1.2f%c\t",c,p00,p01,c);
        }
        printf("\n\t\t\t\t");
        for(int j = 0; j < pl; j++)
        {
            kalman_t k = m->kalmans[i][j];
            double p10 = k.P_k[1][0], p11 = k.P_k[1][1];
            char c = ' ';
            if(j == m->selection[il]) c = '|';
            printf("\t%cP10>%1.2f|P11>%1.2f%c\t",c,p10,p11,c);
        }
        printf("\n\n");
    }
//    printf("**********************\n");
#endif
}

void initKmatPair( kmat_pair_t * m )
{
    initKmat(&m->x.primary);
    initKmat(&m->y.secondary);
    initKmat(&m->x.primary);
    initKmat(&m->y.secondary);
}
void initKmat( kmat_t * m )
{
    for(int i = 0; i < MAX_KALMANS; i++)
    {
        m->lookup[i]    = i;
        m->selection[i] = 0;
        m->pair[i]      = NO_PAIR;
        for(int j = 0; j < MAX_PEAKS; j++)
        {
            kalman_t * k = &m->kalmans[i][j];
            Kalman_Init(k, 0.0);
//            printf("Kalman:\tV%.2f\n", k->value);
//            printf("XP P:\t[%3.10f][%3.10f]\n\t\t[%3.10f][%3.10f]\n", k->P_k[0][0], k->P_k[0][1], k->P_k[1][0], k->P_k[1][1] );
//            printf("XP K:\t[%3.10f][%3.10f]\n", k->K[0], k->K[1]);
        }
        
    }
    m->k_index = 0;
}

void updateKmatPair( kmat_pair_t * m, peak_list_pair_t * p )
{
//    quickSortProbabilityListPair(r);
    
    ///TODO: Combine x/y pairs in update
    updateKmat( &m->x.primary, &p->x );
    updateKmat( &m->x.secondary, &p->x );
    updateKmat( &m->y.primary, &p->y );
    updateKmat( &m->y.secondary, &p->y );
}

bool isExpired( kalman_t * k )
{
    return ((timestamp() - k->timestamp) > MAX_KALMAN_LIFE);
}

int updateKmat( kmat_t * m, peak_list_t * p )
{
    ///TODO: Combine for loops!!!
    kalman_t kal;
    kalman_t * kalp = &kal;
    Kalman_Init(kalp, 0);
    
    /* Set length of peaks to check */
    ///TODO: Figure out how to check all peaks with MAX limit
    int pl = (p->length < MAX_PEAKS)?p->length:MAX_PEAKS; // Use all peaks
    
    /*** Add new kalmans for new peaks ***/
    int i = m->k_index;
    for(; i < MAX_KALMANS && i < pl; i++)
    {
        kalp = &m->kalmans[m->lookup[i]][i];
        kalp->value = p->map[i];
        kalp->density = p->density[i];
        m->selection[m->lookup[i]] = i;
        
        for(int j = 0; j < 100; j++) Kalman_Update(kalp, p->map[i], ((rand() % 100)/ 100 ) - 0.5); // Add some jitter
    }
    m->k_index = i;
    
#ifdef KMAT_DEBUG
#ifdef EXT_DEBUG
    printf("*****************AFTER ADD****************\n");
    printKM( m );
#endif
#endif
//    for(int j = 0; j < MAX_PEAKS; pl += (r->values[r->lookup[j]] > MIN_PROB)?1:0, j++); // Use most probable peaks (only if sorted)
    
    /*** Perge all unused kalmans by row - reset all in row after expired kalman (make sure the cols are being sorted) ***/
    for(int j = m->p_index; j > pl; j--)
    {
        bool expired = false;
        for(int i = 0, il = m->lookup[0]; i < m->k_index; i++, il = m->lookup[i])
        {
            kalp = &m->kalmans[il][j];
            if(expired || (expired = isExpired(kalp)))
            {
                Kalman_Init(kalp, 0.0);
                
                /* If entire row is perged, stop tracking */
                if(i == 0) m->p_index--;
            }
            else
            {
                /* Add new kalman row for new interesting objects */
                m->k_index++;
            }
        }
    }
    
    /* Get post-perged kalman count */
    int ml = m->k_index;
    
    /*** Update kalmans with new peaks ***/
    for(int i = 0, il = m->lookup[0]; i < ml; i++, il = m->lookup[i])
    {
        kalman_t s = m->kalmans[il][m->selection[il]];
        /* Start new rows for valid new peaks */
        for(int j = 0; j < pl; j++)
        {
            kalman_t k = s;
            int val = p->map[j];
            int den = p->density[j];
            if(peakQuickCheck(val))
            {
                double vel = val - k.value;//(k.value*(((rand() % 100)/ 100) - 1/2));
                Kalman_Update(&k, val, vel);
                k.density = den;
            }
            m->kalmans[il][j] = k;
        }
    }
#ifdef KMAT_DEBUG
#ifdef EXT_DEBUG
    printf("*****************AFTER NEW PEAKS****************\n");
    printKM( m );
#endif
#endif
    //    for(int j = 0; j < MAX_PEAKS; pl += (r->values[r->lookup[j]] > MIN_PROB)?1:0, j++); // Use most probable peaks (only if sorted)
    
#define DENSITY_REFERENCE_FACTOR 0.1
    /* Adjust Kalman weights */
    for(int i = 0, il = m->lookup[0]; i < ml; i++, il = m->lookup[i])
    {
        double den_ref = m->kalmans[m->lookup[il]][0].density;
#ifdef KMAT_DEBUG
        printf("Density referene is on kalman %d is %.2f\n",il, den_ref);
        printf("[%d]",i);
#endif
        for(int j = 0; j < pl; j++, kalp = &m->kalmans[il][j])
        {
            double r = kalp->density / den_ref;
#ifdef KMAT_DEBUG
            printf("\t[%d](%.3f>%.3f)",j,kalp->density,r);
#endif
            kalp->K[0] *= ( 1 + DENSITY_REFERENCE_FACTOR * r );
        }
#ifdef KMAT_DEBUG
        printf("\n");
#endif
    }
    
#ifdef EXT_DEBUG
    printf("*****************AFTER WEIGHTS****************\n");
    printKM( m );
#endif
    
    /*** Find highest kalman tracking values ***/
    for(int i = 0, il = m->lookup[0]; i < ml; i++, il = m->lookup[i])
    {
        int s = 0;
        double v = 0.0;
        
        /* Find highest kalman gain */
        for(int j = 0; j < pl; j++, kalp = &m->kalmans[il][j])
        {
            if(kalp->K[0] > v)
            {
                v = kalp->K[0];
                s = j;
            }
        }
        
        if(GTTHR(m->value[il], v, SELECTION_THRESHOLD))
        {
#ifdef KMAT_DEBUG
            printf("Setting kalman %d to selection-%d value-%.2f density-%.2f\n",i,s,v,kalp->density);
#endif
            m->selection[il] = s;
            m->value[il] = v;
            m->density[il] = kalp->density;
       }
    }
    
    /*** Find density pairs in Kalman matrix and give them a sorting bias ***/
    int track[MAX_KALMANS];
    
    /* Start with higher pair level (higher sort bias) */
    uint8_t pair_level = KALMAN_PAIR_MAX;
    for(int i = 0, il = m->lookup[0]; i < ml; i++, il = m->lookup[i])
    {
//        int c = m->selection[m->lookup[i]];
//        int x = (int)(m->kalmans[m->lookup[i]][c].value);
        track[i] = p->density[il];
        int max = track[i] + MAX_DENSITY_DIFF;
        int min = track[i] - MAX_DENSITY_DIFF;
        
        /* Check if new (i) is similar to any old (j) */
        for(int j = 0, jl = m->lookup[0]; j < i; j++, jl = m->lookup[j])
        {
            if( ( track[j] <= max ) && ( track[j] >= min ) )
            {
                /* If old doesn't already have a pair */
                if( m->pair[jl] == NO_PAIR ) m->pair[jl] = pair_level;
                m->pair[il] = pair_level;
                
                /* Decrement pair level until zero */
                !pair_level?0:pair_level--;
            }
        }
    }
    
    /*** Sort Kalman Matrix ***/
    quickSortKalmanMatrix( m, 0, m->k_index-1 );
    
//    int arr[m->k_index];
//    for(int i = 0; i < m->k_index; i++) arr[i] = (int)weightedValue(m, i);
//    quickSort((int*)m->lookup,0,m->k_index-1, 0);
//    printA((int*)m->lookup, 0, m->k_index-1, 0);
    
//    reverseLookup(m->lookup,m->k_index-1);
    
    /* Perge kalmans by column if highest selection is too small */
    for(int i = 0, il = m->lookup[0]; i < ml; i++, il = m->lookup[i])
    {
        if(m->kalmans[il][m->selection[il]].K[0] < MIN_PROB)
        {
            m->k_index = i;
            
            /* Reset lookup table */
            ///TODO: Should it actually be reset?
//            for(;i<MAX_KALMANS;i++)
//            {
//                m->lookup[i] = i;
//            }
        }
    }
    
#ifdef KMAT_DEBUG
    printf("*****************AFTER UPDATE****************\n");
#endif
    printKM( m );

    
    /* Select top from Kalman Matrix */
    int il = m->lookup[0];
    int c = m->selection[il];
    int s = (int)(m->kalmans[il][c].value);
    return s;
}

bool peakQuickCheck( int v )
{
    return true;
}

double compare( lookup_t * m, kmat_p a, kmat_p b )
{
    return 0.0;
}

/************ QUICK SORT START *************
 link - (http://www.geeksforgeeks.org/quick-sort/
 */

/*
void swap( lookup_t * l, int a, int b )
{
#ifdef KMAT_DEBUG
    printf("\tSwapping lookup at %d and %d: [%d][%d]",a,b,l[a],l[b]);
#endif
    int t = l[a];
    l[a] = l[b];
    l[b] = t;
#ifdef KMAT_DEBUG
    printf(" > [%d][%d]\n",l[a],l[b]);
#endif
}

int partitionProbabilityList(probability_list_t * r, int l, int h)
{
    int pivot = r->values[h];
    int i = (l - 1);
    
    for (int j = l; j <= h-1; j++)
    {
        if (r->values[j] <= pivot) swap(r->lookup,++i,j);
    }
    swap(r->lookup,i+1,h);
    return (i + 1);
}

void quickSortProbabilityList(probability_list_t * r, int l, int h)
{
    if( l < h )
    {
        int p = partitionProbabilityList(r, l, h);
        quickSortProbabilityList(r, l, p - 1);
        quickSortProbabilityList(r, p + 1, h);
    }
}

void quickSortProbabilityListPair( probability_list_pair_t * r )
{
    quickSortProbabilityList(&r->x.primary, 0, r->x.primary.length);
    quickSortProbabilityList(&r->x.secondary, 0, r->y.secondary.length);
    quickSortProbabilityList(&r->y.primary, 0, r->x.primary.length);
    quickSortProbabilityList(&r->y.secondary, 0, r->y.secondary.length);
}
 */



void printNL(int tab)
{
    printf("\n");
    for(int i = 0; i < tab; i++) printf("\t");
}

void printA(int arr[], int low, int high, int tab)
{
    for(int i = low; i <= high; i++) printf(" %2d   ", i);
    printNL(tab);
    for(int i = low; i <= high; i++) printf("{%3d} ", arr[i]);
    printNL(tab);
}

void swap(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

/* This function takes last element as pivot, places
 the pivot element at its correct position in sorted
 array, and places all smaller (smaller than pivot)
 to left of pivot and all greater elements to right
 of pivot */
int partition (int arr[], int low, int high, int tab)
{
    int pivot = (int)arr[low];    // pivot
#ifdef EXT_DEBUG
    printNL(tab);
    printf("Pivot is %d at index %d", pivot, low);
#endif
    int i = high + 1;  // Index of smaller element
    
    for (int j = high; j > low; j--)
    {
#ifdef EXT_DEBUG
        printNL(tab);
        printf("i-%d and j-%d", i, j);
#endif
        // If current element is smaller than or
        // equal to pivot
        if ((int)arr[j] <= pivot)
        {
            if(--i!=j)
            {
                --i;    // increment index of smaller element
#ifdef EXT_DEBUG
                printNL(tab);
                printf("Swapping i-%d <> j-%d", i, j);
#endif
                swap((int*)&arr[i], (int*)&arr[j]);
#ifdef EXT_DEBUG
                printNL(tab);
                printA(arr, low, high, tab);
#endif
            }
        }
    }
#ifdef EXT_DEBUG
    printNL(tab);
#endif
    
    int b = low, a = i - 1;
#ifdef EXT_DEBUG
    printf("Pivot swapping i-%d <> j-%d", a, b);
#endif
    swap((int*)&arr[a], (int*)&arr[b]);
#ifdef EXT_DEBUG
    printNL(tab);
    printA(arr, low, high, tab);
#endif
    return (i - 1);
}

/* The main function that implements QuickSort
 arr[] --> Array to be sorted,
 low  --> Starting index,
 high  --> Ending index */
int quickSort(int arr[], int low, int high, int tab)
{
#ifdef EXT_DEBUG
    printNL(tab);
    printf("Sorting %d-%d:", low, high);
    printNL(tab);
    printA(arr, low, high, tab);
#endif
    int next_tab = tab+1;
    if (low < high)
    {
        /* pi is partitioning index, arr[p] is now
         at right place */
        int pi = partition(arr, low, high, tab);
#ifdef EXT_DEBUG
        printNL(tab);
        printf("Partition split is %d",pi);
        // Separately sort elements before
        // partition and after partition
        printNL(tab);
        printf("Sorting below split %d-%d", low,pi);
#endif
        quickSort(arr, low, pi - 1, next_tab);
#ifdef EXT_DEBUG
        printNL(tab);
        printf("Sorting above split %d-%d", pi, high);
#endif
        quickSort(arr, pi + 1, high, next_tab);
    }
    return 1;
}
/*****************************************/

double weightedValue( kmat_t * m, int i )
{
    return m->value[m->lookup[i]] + ( KALMAN_SORT_SCALE_STEP * m->pair[m->lookup[i]] );
}

void swapKalmanMatrix( kmat_t * m, kmat_p a, kmat_p b )
{
#ifdef EXT_DEBUG
    printf("\tSwapping lookup at %d and %d: [%d][%d]",a,b,m->lookup[a],m->lookup[b]);
    printf("\n");
#endif
    kmat_p t = m->lookup[a];
    m->lookup[a] = m->lookup[b];
    m->lookup[b] = t;
}

int partitionKalmanMatrix(kmat_t * m, int l, int h)
{
    int pivot = weightedValue(m, l);
    int i = h+1;
    
    for (int j = h; j > l; j--)
    {
        if( i != j && weightedValue(m, j) <= pivot) swapKalmanMatrix(m,--i,j);
    }
#ifdef EXT_DEBUG
    printf("Pivot ");
#endif
    swapKalmanMatrix(m,i-1,l);
//    swap(m->lookup,i-1,l);
    return (i - 1);
}


void quickSortKalmanMatrix(kmat_t * m, int l, int h)
{
    if( l < h )
    {
        int p = partitionKalmanMatrix(m, l, h);
        quickSortKalmanMatrix(m, l, p - 1);
        quickSortKalmanMatrix(m, p + 1, h);
    }
}

///* QUICKSORT MAX */
//void swapKalmanMatrixMax( kmat_t * m, kmat_p a, kmat_p b )
//{
//    kmat_p t = m->lookup[a];
//    m->lookup[a] = m->lookup[b];
//    m->lookup[b] = t;
//}
//
//int partitionKalmanMatrixMax(kmat_t * m, int l, int h)
//{
//    int pivot = m->value[m->lookup[h]];
//    int i = (l - 1);
//    
//    for (int j = l; j <= h-1; j++)
//    {
//        double scaled_value = m->value[m->lookup[j]] + ( KALMAN_SORT_SCALE_STEP * m->pair[m->lookup[j]] );
//        if (scaled_value < pivot) swap(m->lookup,++i,j);
//    }
//    swap(m->lookup,i+1,h);
//    return (i + 1);
//}
//
//
//void quickSortKalmanMatrixMax(kmat_t * m, int l, int h)
//{
//    if( l < h )
//    {
//        int p = partitionKalmanMatrix(m, l, h);
//        quickSortKalmanMatrix(m, l, p - 1);
//        quickSortKalmanMatrix(m, p + 1, h);
//    }
//}

/* QUICK SORT END */
