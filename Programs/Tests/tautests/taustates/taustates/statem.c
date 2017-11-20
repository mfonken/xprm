//
//  statem.c
//  taustates
//
//  Created by Matthew Fonken on 10/17/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include "statem.h"

#define __TEMP_PROB__   0.0
#define PROBABILITY_TUNING_FACTOR       0.5
#define PROBABILITY_THRESHOLD           0.7
#define PROBABILITY_TUNING_THRESHOLD    PROBABILITY_THRESHOLD * PROBABILITY_THRESHOLD
#define PROBABILITY_STABILITY_THRESHOLD 0.5

/* Temporary macros */
#define numberOfSelectedPeaks 1

double DEF_BAYESIAN_MAP[NUM_STATES+1][NUM_STATES+1] =
{                 /*   UNKNOWN_STAT  STABLE_NONE   UNSTABLE_NON  STABLE_SINGL  UNSTABLE_SIN  STABLE_DOUBL  UNSTABLE_DOU  STABLE_MANY   UNSTABLE_MAN  COLLAPSING  */
    /* UNKNOWN_STA */{ DEF_SELF_CON, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN },
    /* STABLE_NONE */{ DEF_NOT_CONN, DEF_SELF_CON, DEF_U0_TO_S0, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_UM_TO_S0, DEF_NOT_CONN },
    /* UNSTABLE_NO */{ DEF_NOT_CONN, DEF_NOT_CONN, DEF_SELF_CON, DEF_S1_TO_U0, DEF_NOT_CONN, DEF_S2_TO_U0, DEF_NOT_CONN, DEF_NOT_CONN, DEF_UM_TO_U0, DEF_NOT_CONN },
    /* STABLE_SING */{ DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_SELF_CON, DEF_U1_TO_S1, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_UM_TO_S1, DEF_NOT_CONN },
    /* UNSTABLE_SI */{ DEF_NOT_CONN, DEF_S0_TO_U1, DEF_NOT_CONN, DEF_NOT_CONN, DEF_SELF_CON, DEF_S2_TO_U1, DEF_NOT_CONN, DEF_NOT_CONN, DEF_UM_TO_U1, DEF_NOT_CONN },
    /* STABLE_DOUB */{ DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_SELF_CON, DEF_U2_TO_S2, DEF_NOT_CONN, DEF_UM_TO_S2, DEF_NOT_CONN },
    /* UNSTABLE_DO */{ DEF_NOT_CONN, DEF_S0_TO_U2, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_SELF_CON, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN },
    /* STABLE_MANY */{ DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_SELF_CON, DEF_UM_TO_SN, DEF_NOT_CONN },
    /* UNSTABLE_MA */{ DEF_NOT_CONN, DEF_S0_TO_UM, DEF_NOT_CONN, DEF_S1_TO_UM, DEF_NOT_CONN, DEF_S2_TO_UM, DEF_NOT_CONN, DEF_SN_TO_UM, DEF_SELF_CON, DEF_NOT_CONN },
    /* COLLAPSING  */{ DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_NOT_CONN, DEF_SELF_CON }
};

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
    s->filter               = NO_FILTER;
    initDefaultBayesianMap( &s->probabilities );
}

void updateSystem( system_t * s, prediction_pair_t * p )
{
    /** Assumptions
     *  - Column is current state
     *  - Row is next state
     */
    if( s->probabilities.scheduled_normalize ) normalizeBayesianMap( &s->probabilities );
    
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
    
    printf("Current state is %s.\n", stateString(s->state));
    printf("Next state is %s(%.2f).\n", stateString(s->next), max_v);
    
    /* Update self-diagnostics based on state */
    
    double prob[] = {0.0, 0.0, 0.0, 0.0}; //non[0] = 0.0, pri[1] = 0.0, sec[2] = 0.0, alt[3] = 0.0;
    if( p->selection_pair == SIMILAR )
    {
        prob[1] = p->x.primary_probability * p->y.primary_probability;
        prob[2] = p->x.secondary_probability * p->y.secondary_probability;
    }
    else if( p->selection_pair == OPPOSITE )
    {
        prob[1] = p->x.primary_probability * p->y.secondary_probability;
        prob[2] = p->x.secondary_probability * p->y.primary_probability;
    }
    prob[3] = p->x.alternate_probability * p->y.alternate_probability;
    
    printf("Before: non-%.3f pri-%.3f sec-%.3f alt-%.3f\n", prob[0], prob[1], prob[2], prob[3]);
    int num_selections = 0;
    
#define ABSENCE_FACTOR 0.1
    if( prob[1] < PROBABILITY_TUNING_THRESHOLD )
    {
        prob[0] += ABSENCE_FACTOR;
        prob[1] = 0.0;
    }
    else num_selections++;
    if( prob[2] < PROBABILITY_TUNING_THRESHOLD )
    {
        prob[0] += ABSENCE_FACTOR;
        prob[2] = 0.0;
    }
    else num_selections++;
    if( prob[3] < PROBABILITY_TUNING_THRESHOLD )
    {
        prob[0] += ABSENCE_FACTOR;
        prob[3] = 0.0;
    }
    else num_selections++;
    printf("After: pri-%.3f sec-%.3f alt-%.3f\n", prob[0], prob[1], prob[2]);
    
    s->selection_index      = num_selections;
    s->stability.primary    = 0.0;
    s->stability.secondary  = 0.0;
    s->stability.alternate  = 0.0;

    int x = stateNumber(c);
    int k = (int)s->selection_index;

    double out[4];
    
    for(int i = 0; i <= k; i++)
    {
        out[i] = 0.0;
        double p = prob[i] * PROBABILITY_TUNING_FACTOR;
        if( i == x )
        {
            out[i] = p;
        }
        else
        {
            double f = fabs( 1.0 - ( ( i + 1.0 ) / ( x + 1.0 ) ) );
            out[i] = p * f;
        }
    }
    updateProbabilities(s, out);
    normalizeBayesianState( &s->probabilities, s->state );
}

void updateProbabilities( system_t * s, double p[4] )
{
    printf("Probabilies are [0]%.2f [1]%.2f [2]%.2f [3]%.2f.\n", p[0], p[1], p[2], p[3]);
    int c = s->state;
    int x = stateNumber(c);
    printf("State %s is ", stateString(c));
    if(!isStable(c)) printf("not ");
    printf("stable.\n");
    double prob = 0.0;
    for( int i = 0, o = 0; i < 4; i++ )
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
//        else o++;
        if(k != -1)
        {
            if(i == 0) prob = p[0];
            printf("Updating %s by %.2f.\n", stateString(k), p[i-o]);
            s->probabilities.map[k][c] += p[i-o];
        }
    }
    s->probabilities.scheduled_normalize = true;
}

state_t nextState( state_t s, double pri, double sec, double alt )
{
    state_t r = UNKNOWN_STATE;
    switch( s )
    {
        case STABLE_NONE:
            r = UNSTABLE_SINGLE;
            break;
        case UNSTABLE_NONE:
            r = STABLE_NONE;
            break;
        case UNSTABLE_DOUBLE:
        case STABLE_DOUBLE:
        case UNSTABLE_SINGLE:
        case STABLE_SINGLE:
            r = stabilityCheck( s, pri, sec, alt, PROBABILITY_STABILITY_THRESHOLD );
            break;
        default:
            break;
    }
    return r;
}

state_t stabilityCheck( state_t s, double pri, double sec, double alt, double thresh )
{
    int n = stateToSelection(s);
    if( pri > thresh )
    {
        if( sec > thresh )
        {
            if( alt > thresh ) return UNSTABLE_MANY;
            return (n==2)?STABLE_DOUBLE:UNSTABLE_DOUBLE;
        }
        return (n==1)?STABLE_SINGLE:UNSTABLE_SINGLE;
    }
    return UNSTABLE_NONE;
}

bool isStable( state_t s )
{
    if(s == UNKNOWN_STATE) return false;
    return (s % 2);
}

int stateNumber( state_t s )
{
    return (int)( ( (int)s - 1 ) / 2 );
}

void updateState( system_t * s )
{
    if(s->next != UNKNOWN_STATE )
    {
        printf("Updating state from %s to %s\n", stateString(s->state), stateString(s->next));
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
            b->map[i][j] = DEF_BAYESIAN_MAP[i][j];
        }
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
    b->scheduled_normalize = false;
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
    for( int j = 0; j < l; j++ ) b->map[j][i] = DEF_BAYESIAN_MAP[j][i];
    normalizeBayesianState( b, i );
}

void printBayesianMap( state_bayesian_map_t * b )
{
    int l = b->length;
    for( int i = 0; i < l; i++ ) printf("\t\t%s-[%d]", stateString(i), i);
    for( int i = 0; i < l; i++ )
    {
        printf("\n%s-[%d]", stateString(i), i);
        for( int j = 0; j < l; j++ )
        {
            printf("\t[%.2f]\t",b->map[i][j]);
        }
    }
    printf("\n");
}
