#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_LEN 100
#define SCALE   10

typedef double FLOAT;

typedef struct
{
    FLOAT   *array;
    uint16_t length;
} gaussian_t;

void initGaussian( gaussian_t * g, int l )
{
    (*g).array = (FLOAT*)malloc(sizeof(FLOAT)*l);
    (*g).length = 0;
}

FLOAT gaussianDistribution( int x, int mu, FLOAT sigma)
{
    int d = x - mu;
    FLOAT n = 1.0 / (sqrt(2 * M_PI) * sigma);
    return exp(-d*d/(2 * sigma * sigma)) * n;
};

void generateGaussian( gaussian_t * g, int l, FLOAT s )
{
    int m = floor(l/2);
    for(int i = 0, d = l; i <= m; i++, d--)
    {
        g->array[i] = gaussianDistribution(i,m,s);
        if(i != d) g->array[d] = g->array[i];
    }
    g->length = l;
}

void printGaussian( gaussian_t * g )
{
  int l = g->length;
  for(int i = 0; i < l; i++)
  {
    printf("%3d | ",i);
    double curr = g->array[i];
    printf("%2.5f:",curr);
      curr *= SCALE;
    for(int j = 0; j < curr; j++) printf("*");
    printf("\n");
  }
}

int main( int argc, char * args[] )
{
  int l = 11;
  FLOAT s = 1.0;
  if(argc >= 2) l = atoi(args[1]);
  if(argc >= 3) s = atof(args[2]);

  printf("Generating gaussian of length %d and sigma %.2f:\n", l, s);

  gaussian_t g;
  initGaussian(&g, MAX_LEN);
  generateGaussian(&g, l, s);
  printGaussian(&g);
  return 0;
}
