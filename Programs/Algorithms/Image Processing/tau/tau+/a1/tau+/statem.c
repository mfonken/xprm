//
//  statem.c
//  taustates
//
//  Created by Matthew Fonken on 10/17/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include "statem.h"

#define DISTANCE_SQ(X,Y)                (X*X)+(Y*Y)
#define PROBABILITY_TUNING_FACTOR       0.5
#define PROBABILITY_THRESHOLD           0.7
#define ABSENCE_FACTOR                  PROBABILITY_TUNING_FACTOR
#define PROBABILITY_TUNING_THRESHOLD    PROBABILITY_THRESHOLD * PROBABILITY_THRESHOLD
#define PROBABILITY_STABILITY_THRESHOLD 0.5
#define STATE_DISTANCE                  2.0

/* Temporary macros */
#define numberOfSelectedPeaks 1

void initSystem( system_t * s )
{
    s->state                = STABLE_NONE;
    s->prev                 = UNKNOWN_STATE;
    s->next                 = UNKNOWN_STATE;
    s->selection_index      = 0;
    s->stability.primary    = 0.0;
    s->stability.secondary  = 0.0;
    s->stability.alternate  = 0.0;
    s->stability.overall    = 0.0;
//    s->filter               = NO_FILTER;
    initDefaultBayesianMap( &s->probabilities );
}
void updateSystem( system_t * s, prediction_pair_t * p )
{
    /** Assumptions
     *  - Column is current state
     *  - Row is next state
     */
//    if( s->probabilities.scheduled_normalize ) normalizeBayesianMap( &s->probabilities );
    
    int l = s->probabilities.length;
    int c = (int)s->state;
    double max_v = 0.0;
    int max_i = 0;
    
    /* Find most probable next state */
    for( int i = 0; i < l; i++ )
    {
        double p = s->probabilities.map[i][c];
        if( p > max_v )
        {
            max_v = p;
            max_i = i;
        }
    }
    if( max_i != s->state ) s->next = max_i;
    
#ifdef STATEM_DEBUG
    printf("\n###### Current state is %s. ######\n\n", stateString(s->state));
    printf("Next state is %s(%.2f).\n", stateString(s->next), max_v);
#endif
    
    /* Update self-diagnostics based on state */
    
#define SHADOW_TOLERANCE 0.2
    double prob[] = {0.0, 0.0, 0.0, 0.0};
    double xpp = p->x.primary_probability, xsp = p->x.secondary_probability, ypp = p->y.primary_probability, ysp = p->y.secondary_probability;
    if( ( xpp > SHADOW_TOLERANCE || xsp > SHADOW_TOLERANCE ) && ( ypp > SHADOW_TOLERANCE || ysp > SHADOW_TOLERANCE ) )
    {
        if(INRANGE(xpp, xsp, SHADOW_TOLERANCE) && !INRANGE(ypp, ysp, SHADOW_TOLERANCE))
        {
#ifdef STATEM_DEBUG
            printf("Using shadow handling on Y axis\n");
            printf("\tb: xp>%.2f xs>%.2f yp%.2f ys>%.2f\n", xpp, xsp, ypp, ysp);
#endif
            if( ypp < SHADOW_TOLERANCE ) ypp = ysp;
            if( ysp < SHADOW_TOLERANCE ) ysp = ypp;
#ifdef STATEM_DEBUG
            printf("\ta: xp>%.2f xs>%.2f yp%.2f ys>%.2f\n", xpp, xsp, ypp, ysp);
#endif
        }
        if(INRANGE(ypp, ysp, SHADOW_TOLERANCE) && !INRANGE(xpp, xsp, SHADOW_TOLERANCE))
        {
#ifdef STATEM_DEBUG
            printf("Using shadow handling on X axis\n");
            printf("\tb: xp>%.2f xs>%.2f yp%.2f ys>%.2f\n", xpp, xsp, ypp, ysp);
#endif
            if( xpp < SHADOW_TOLERANCE ) xpp = xsp;
            if( xsp < SHADOW_TOLERANCE ) xsp = xpp;
#ifdef STATEM_DEBUG
            printf("\ta: xp>%.2f xs>%.2f yp%.2f ys>%.2f\n", xpp, xsp, ypp, ysp);
#endif
        }
    }
    if( p->selection_pair == SIMILAR )
    {
        
        prob[1] = DISTANCE_SQ(xpp, ypp);
        prob[2] = DISTANCE_SQ(xsp, ysp);
    }
    else if( p->selection_pair == OPPOSITE )
    {
        prob[1] = DISTANCE_SQ(xpp, ysp);
        prob[2] = DISTANCE_SQ(xsp, ypp);
    }
    /* Alternate probability is 2D chaos related, thus dependent on both existing */
    prob[3] = p->x.alternate_probability * p->y.alternate_probability;
    
#ifdef STATEM_DEBUG
    printf("Before: non-%.3f pri-%.3f sec-%.3f alt-%.3f\n", prob[0], prob[1], prob[2], prob[3]);
#endif
    
    int num_selections = 0;
   
    prob[0] = ABSENCE_FACTOR;
    
    if( prob[1] < PROBABILITY_TUNING_THRESHOLD ) prob[1] = 0.0;
    else num_selections++;
    if( prob[2] < PROBABILITY_TUNING_THRESHOLD ) prob[2] = 0.0;
    else num_selections++;
    if( prob[3] < PROBABILITY_TUNING_THRESHOLD )prob[3] = 0.0;
    else num_selections++;
    if(num_selections) prob[0] = 0.0;
    
#ifdef STATEM_DEBUG
    printf("After: non-%.3f pri-%.3f sec-%.3f alt-%.3f\n", prob[0], prob[1], prob[2], prob[3]);
#endif
    
    s->selection_index      = num_selections;
    s->stability.primary    = 0.0;
    s->stability.secondary  = 0.0;
    s->stability.alternate  = 0.0;
    
    int k = s->selection_index;
    
    double out[4];
    double f;
    for(int i = 0; i <= k; i++)
    {
        out[i] = 0.0;
        double p = prob[i] * PROBABILITY_TUNING_FACTOR;
        f = (double)( i ) / 3.0;
//        f /= 2;
//        f += 0.5;
//        if( i == x ) f = 1.0;
        out[i] = p * f;
#ifdef STATEM_DEBUG
        printf("Punishing prob[%d]-%.3f by a factor of %.3f for a result of %.3f\n", i, p, f, out[i]);
#endif
    }
    updateProbabilities( s, out );
    normalizeBayesianState( &s->probabilities, s->state );
    updateState( s );
}


void updateProbabilities( system_t * s, double p[4] )
{
    int c = s->state;
    int x = stateNumber(c);
#ifdef STATEM_DEBUG
    printf("Probabilies are [0]%.2f [1]%.2f [2]%.2f [3]%.2f.\n", p[0], p[1], p[2], p[3]);
    printf("State %s is ", stateString(c));
    if(!isStable(c)) printf("not ");
    printf("stable.\n");
#endif
    for( int i = 0; i < 4; i++ )
    {
        int k = -1;
        int j = x - i;
        if(!isStable(c))
        {
            if( x == i )
                k = c - 1;
            else
                k = c - ( 2 * j );
        }
        else if( x != i )
            k = ( c + 1 ) - ( 2 * j );
        else
            k = c;
        if(k != -1)
        {
#ifdef STATEM_DEBUG
            printf("Updating %s by %.2f.\n", stateString(k), p[i]);
#endif
            s->probabilities.map[k][c] += p[i];
        }
    }
//    s->probabilities.scheduled_normalize = true;
}



void updateState( system_t * s )
{
    if(s->next != UNKNOWN_STATE )
    {
#ifdef STATEM_DEBUG
        printf("Updating state from %s to %s\n", stateString(s->state), stateString(s->next));
#endif
        s->prev = s->state;
        s->state = s->next;
        s->next = UNKNOWN_STATE;
        resetBayesianState( &s->probabilities, s->prev );
    }
}

void initDefaultBayesianMap( state_bayesian_map_t * b )
{
    int l = NUM_STATES;
    b->length = l;
    for( int i = 0; i < l; i++ )
    {
        for( int j = 0; j < l; j++ )
        {
            b->map[i][j] = 0.0;//DEF_BAYESIAN_MAP[i][j];
        }
        b->map[i][i] = 1.0;
    }
    normalizeBayesianMap(b);
}

void normalizeBayesianMap( state_bayesian_map_t * b )
{
    int l = b->length;
    
    for( int i = 0; i < l; i++ )
    {
        normalizeBayesianState( b, i );
    }
//    b->scheduled_normalize = false;
}

void normalizeBayesianState( state_bayesian_map_t * b, int i )
{
    int l = b->length;
    double total = 0.0;
    for( int j = 0; j < l; j++ ) total += b->map[j][i];
    if(total) for( int j = 0; j < l; j++ ) b->map[j][i] /= total;
    else b->map[i][i] = 1.0;
}

void resetBayesianState( state_bayesian_map_t * b, int i )
{
    int l = b->length;
    for( int j = 0; j < l; j++ ) b->map[j][i] = 0.0;//DEF_BAYESIAN_MAP[j][i];
    b->map[i][i] = 1.0;
    normalizeBayesianState( b, i );
}

void printBayesianMap( state_bayesian_map_t * b, state_t s)
{
    int l = b->length;
    for( int i = 0; i < l; i++ ) printf("\t\t %s-[%d]", stateString(i), i);
    for( int i = 0; i < l; i++ )
    {
        printf("\n%s-[%d]", stateString(i), i);
        for( int j = 0; j < l; j++ )
        {
            char c = ' ';
            if(j == (int)s) c = '|';
            printf("\t%c[%.2f]%c",c,b->map[i][j],c);
        }
    }
    printf("\n");
}
