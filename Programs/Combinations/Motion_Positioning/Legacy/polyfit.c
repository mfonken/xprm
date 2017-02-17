/* Least Squares Polynomial Fit - See http://www.ocs.net/~jfurman/misc/synthesis/ */


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stddef.h>

#define POLY_DEGREE_MIN 1
#define POLY_DEGREE_MAX 10

double gauss_elimination( double *a, double *b, int n );
char ch = 0;

int getdouble( double *val )
{
    int start,open_brackets;
    int scale = -1;
    int characteristic = 0;
    int exp = 0;
    double factor,sign = 1.0;
    
    *val = 0;
    start = 0;
    open_brackets = 0;

    for(;;)
    {
        if ( scanf( "%c", &ch ) == EOF )
        {
            if ( start != 0 ) return( 0 );
            else return( EOF );
        }
        if ( ch == '{' ) open_brackets++;
        if ( open_brackets > 0 )
        {
            printf( "%c",ch );
            if ( ch == '}' ) open_brackets--;
            continue;
        }
        
        switch ( ch )
        {
            case '\n':
            case '\r':
            case '\t':
            case ' ':
            case ',':
            case ';':
                if (start != 0){
                    if (scale == -1) scale = 0;
                    characteristic = (exp * characteristic) - scale;
                    if (characteristic < 0)
                    {characteristic = -characteristic; factor = 0.1;}
                    else
                    {factor = 10.0;};
                    for(exp = 0; exp < characteristic; exp++){*val = (*val) * factor;};
                    *val = (*val) * sign;
                    return(0);
                }
                else
                    printf("%c",ch);
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                start = 1;
                if ( exp != 0 )
                {
                    characteristic = ( characteristic * 10 ) + ( ch - '0' );
                }
                else
                {
                    *val = ( ( * val ) * 10.0 ) + ( ch - '0' );
                    if ( scale >= 0 ) scale++;
                };
                break;
            case '.':
                if ( ( scale >= 0 ) || ( exp != 0 ) ) break;
                scale = 0;
                break;
            case 'e':
            case 'E':
                exp = 1;
                break;
            case '-':
                if ( exp == 1 ) exp = -1;
                if ( start == 0 )
                {
                    start = 1;
                    sign = -1.0;
                }
                break;
            default:
                break;
        }
    }
}


int getint( int *val )
{
    int start,open_brackets;
    
    *val = 0;
    start = 0;
    
    open_brackets = 0;
    
    for(;;){
        if (scanf("%c",&ch) == EOF)
        {
            if ( start != 0 ) return(0);
            else return( EOF );
        }
        if ( ch == '{' ) open_brackets++;
        if (open_brackets > 0)
        {
            printf("%c",ch);
            if (ch == '}' ) open_brackets--;
            continue;
        }
        
        switch ( ch )
        {
            case '\n':
            case '\r':
            case '\t':
            case ' ':
            case ',':
            case ';':
                if ( start != 0 )
                {
                    return( 0 );
                }
                else
                    printf( "%c",ch );
                break;
            case '?':
                return(-1);
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                start = 1;
                *val = ( ( * val ) * 10 ) + ( ch - '0'  );
                break;
            default:
                break;
        }
    }
}


double least_squares( double    *x,
                      double    *y,
                      int       ndata,
                      double    *coeff,
                      int       n)
{
#define A(x,y) a[((x)*(n+1)) + (y)]
    
    double *a, *b, *p;
    double det, u, x_k, v, tmp1;
    int i,j,k,tmp2;
    
    a = ( double* ) malloc( ( n + 2 ) * ( n + 2 ) * sizeof( double ) );
    b = ( double* ) malloc( ( n + 2 ) * sizeof( double ) );
    p = ( double* ) malloc( ( ( 2 * ( n + 2 ) ) + 1 ) * sizeof( double ) );
    
    /* Initialize */
    for( i = 0; i <= ( n + 1 ); i++ )
    {
        p[i]            = 0.0;
        p[i + n + 2]    = 0.0;
        b[i]            = 0.0;
        for( j = 0; j <= ( n + 1 ); j++ )
        {
            A( i, j ) = 0.0;
        }
    }
    
    for( k = 0; k < ndata; k++ )
    {
        for( i = 0, u = 1.0, x_k = x[k], v = y[k]; i <= ( n * 2 ); i++ )
        {
            p[i] += u;
            u    *= x_k;
            if( i <= n )
            {
                b[i] += v;
                v    *= x_k;
            }
        }
    }
    
    for( i = 0; i <= n; i++ )
    {
        for( j = 0; j <= n; j++ )
        {
            A( i, j ) = p[( i + j )];
        }
    }
    
    det = gauss_elimination( a, b, n+1 );
    
    for( i = 0; i <= n; i++ )
    {
        coeff[i] = b[i];
    }
    
    free( a );
    free( b );
    free( p );
    return det;
}

double gauss_elimination( double    *a,
                          double    *b,
                          int       n )
{
    int *row, *col;
    double pivot, factor, result, det, tmp;
    int i, j, k, m, pi, pj, q;
    
    row = ( int * ) malloc( ( n ) * sizeof( int ) );
    col = ( int * ) malloc( ( n ) * sizeof( int ) );
    
#define X(x,y) a[((row[x])*(n)) + (y)]
#define Y(x) (b[row[x]])
    
    for( i = 0; i < n; i++ )
    {
        row[i] = i;
        col[i] = 0;
    };
    
    det = 1.0;
    
    for( i = 0; i < n; i++ )
    {
        q = 0;
        for( j = 0; j < n; j++)
        {
            if( col[j] == 0 )
            {
                if( q == 0 )
                {
                    tmp = fabs( X( j, j ) );
                    pi = j;
                    pj = j;
                }
                
                for( k =  0; k < n; k++ )
                {
                    if( col[k] == 0 )
                    {
                        if( tmp < fabs( X( k, j ) ) )
                        {
                            tmp = fabs( X( k, j ) );
                            pi = k;
                            pj = j;
                        }
                    }
                }
                q = 1;
            }
        }
        
        col[pj] = 1;
        if( pi != pj )
        {
            int temp = row[pi];
            row[pi] = row[pj];
            row[pj] = temp;
            det = -det;
        };
        pivot = X( pj, pj );
        det *= pivot;
        
        for( j = 0; j < n; j++ )
        {
            X( pj, j ) /= pivot;
        }
        
        Y(pj) /= pivot;
        
        
        for( j = 0; j < n; j++ )
        {
            if( j != pj )
            {
                tmp = X( j,pj );
                for( k = 0; k < n; k++ )
                {
                   X(j,k) -= (tmp * X(pj,k));
                }
                Y(j) -= (tmp * Y(pj));
            }
        }
    }
    
    for( i = 0; i < n; i++ )
    {
        if( row[i] != i )
        {
            for( j = i + 1; j < n; j++ )
            {
                if( row[j] == i )
                {
                    tmp     = b[j];
                    b[j]    = b[i];
                    b[i]    = tmp;
                    
                    int temp    = row[j];
                    row[j]  = row[i];
                    row[i]  = temp;
                }
            }
        }
    }
    
    free( row );
    free( col );
    return det;
}

int main()
{
    int min_poly_degree = POLY_DEGREE_MIN;
    int max_poly_degree = POLY_DEGREE_MAX;
    int dataset;
    int i, j, k, m;
    int status;
    double xx, eval, err;
    double *x, *y, *c;
    
    c = ( double * ) malloc( ( max_poly_degree + 1 ) * sizeof( double ) );
    
    status = getint( &dataset );
    
    if( ( status != 0 ) || ( dataset <= 0 ) )
    {
        printf("\nusage: minimum polynomial power(int), max polynomial power(int), number of x-y pairs to fit(int),\nx1,y1,x2,y2,...,xn,yn,(all reals)\n\n");
        free( c );
        return 0;
    };
    
    x = (double *) malloc( dataset * sizeof( double ) );
    y = (double *) malloc( dataset * sizeof( double ) );
    
    
    for( i = 0; i < dataset; i++ )
    {
        status = getdouble( &x[i] );
        status = getdouble( &y[i] );
    }
    
    printf("\n\nleast squares regression fit of polynomials of degree %d to %d to the following data:\n", min_poly_degree,max_poly_degree);
    for( i = 0; i < dataset; i++ )
    {
        printf( "x[%2d]:%g; y:%g\n", ( i + 1 ), x[i], y[i] );
    }
    printf("\n\n");
    
    for( j = min_poly_degree; j <= max_poly_degree; j++ )
    {
        printf("determinant of normal equations: %g\n", least_squares( x, y, dataset, c, j ) );
        printf("fit polynomial of degree %2d:\n",j);
        
        for( i = j; i > 0; i-- )
        {
            printf("%g*(x^%d) +  ", c[i], i);
        }
        printf("%g\n\n  ", c[0]);
        
        for( k = 0, err = 0.0; k < dataset; k++ )
        {
            xx = x[k];
            for( i = j, eval = 0.0; i >= 0; i-- )
            {
                eval = ( xx * eval ) + c[i];
            }
            err +=( ( y[k] - eval ) * ( y[k] - eval ) );
        }
        printf("rms error: %g\n\n",sqrt(err));
    }
    
    free( x );
    free( y );
    free( c );
    
    return 0;
}
