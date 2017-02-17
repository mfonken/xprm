//
//  char_test.c
//  
//
//  Created by Matthew Fonken on 1/23/17.
//
//

#include <stdio.h>

void print( char * s, int l )
{
    printf( "1: Size is %d\n", l );
    printf("\"");
    for(int i = 0; i < l; i++)
    {
        char c = s[i];
        //if( c == '\0' ) return;
        printf("%c", c);
    }
    printf("\"\n");
}

void printn( char s[] )
{
    int l = 0;
    for(; l < 100; l++)
    {
        if( s[l] == '\0' )
        {
            l++;
            break;
        }
    }
    printf( "2: Size is %d\n", l );
    printf("\"");
    for(int i = 0; i < l; i++)
    {
        char c = s[i];
        //if( c == '\0' ) return;
        printf("%c", c);
    }
    printf("\"\n");
}

int main( int argc, char * args[] )
{
    char a[] = "Test string 1.";
    size_t l = sizeof(a);
    print(a, l);
    
    printn("Test string 2.");
    return 0;
}
