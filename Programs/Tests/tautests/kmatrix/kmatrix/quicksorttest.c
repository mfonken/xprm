//
//  test.c
//  tau+
//
//  Created by Matthew Fonken on 10/11/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

//#define DEBUG

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
    int pivot = arr[low];    // pivot
#ifdef DEBUG
    printNL(tab);
    printf("Pivot is %d at index %d", pivot, low);
#endif
    int i = high + 1;  // Index of smaller element
    
    for (int j = high; j > low; j--)
    {
#ifdef DEBUG
        printNL(tab);
        printf("i-%d and j-%d", i, j);
#endif
        // If current element is smaller than or
        // equal to pivot
        if (arr[j] <= pivot)
        {
            if(--i!=j)
            {
//            --i;    // increment index of smaller element
#ifdef DEBUG
                printNL(tab);
                printf("Swapping i-%d <> j-%d", i, j);
#endif
                swap(&arr[i], &arr[j]);
#ifdef DEBUG
                printNL(tab);
                printA(arr, low, high, tab);
#endif
            }
        }
    }
#ifdef DEBUG
    printNL(tab);
#endif
    
    int b = low, a = i - 1;
#ifdef DEBUG
    printf("Pivot swapping i-%d <> j-%d", a, b);
#endif
    swap(&arr[a], &arr[b]);
#ifdef DEBUG
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
#ifdef DEBUG
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
#ifdef DEBUG
        printNL(tab);
        printf("Partition split is %d",pi);
        // Separately sort elements before
        // partition and after partition
        printNL(tab);
        printf("Sorting below split %d-%d", low,pi);
#endif
        quickSort(arr, low, pi - 1, next_tab);
#ifdef DEBUG
        printNL(tab);
        printf("Sorting above split %d-%d", pi, high);
#endif
        quickSort(arr, pi + 1, high, next_tab);
    }
    return 1;
}

/* This function takes last element as pivot, places
 the pivot element at its correct position in sorted
 array, and places all smaller (smaller than pivot)
 to left of pivot and all greater elements to right
 of pivot */
int partitionMax(int arr[], int low, int high, int tab)
{
    int pivot = arr[high];    // pivot
    printNL(tab);
    printf("Pivot is %d at index %d", pivot, high);
    int i = (low - 1);  // Index of smaller element
    
    for (int j = low; j <= high- 1; j++)
    {
        printNL(tab);
        printf("i-%d and j-%d", i, j);
        // If current element is smaller than or
        // equal to pivot
        if (arr[j] <= pivot)
        {
            i++;    // increment index of smaller element
            printNL(tab);
            printf("Swapping i-%d <> j-%d", i, j);
            swap(&arr[i], &arr[j]);
            printNL(tab);
            printA(arr, low, high, tab);
        }
    }
//    printNL(tab);
//    printf("Pivot swapping i-%d <> j-%d", i+1, high);
//    swap(&arr[i + 1], &arr[high]);
    printNL(tab);
    printA(arr, low, high, tab);
    return (i + 1);
}

/* The main function that implements QuickSort
 arr[] --> Array to be sorted,
 low  --> Starting index,
 high  --> Ending index */
int quickSortMax(int arr[], int low, int high, int tab)
{
    printNL(tab);
    printf("Sorting %d-%d:", low, high);
    printNL(tab);
    printA(arr, low, high, tab);
    int next_tab = tab+1;
    if (low < high)
    {
        /* pi is partitioning index, arr[p] is now
         at right place */
        int pi = partitionMax(arr, low, high, tab);
        printNL(tab);
        printf("Partition split is %d",pi);
        // Separately sort elements before
        // partition and after partition
        printNL(tab);
        printf("Sorting below split %d-%d", low,pi);
        quickSortMax(arr, low, pi - 1, next_tab);
        printNL(tab);
        printf("Sorting above split %d-%d", pi, high);
        quickSortMax(arr, pi + 1, high, next_tab);
    }
    return 1;
}
