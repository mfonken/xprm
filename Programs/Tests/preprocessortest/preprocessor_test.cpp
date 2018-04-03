#include <stdio.h>

#define FORA(L,A) for( int i = L; i > 0; --i, A[i] )

int main(int argc, char * argv[])
{
    int l = 10, a[l], t, m = 0;
    FORA(l,t=a)
        if( t > m ) m = t;
    
    return 0;
}
