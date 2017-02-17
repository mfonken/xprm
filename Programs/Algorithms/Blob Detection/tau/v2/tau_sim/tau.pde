double xk[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};//{ 1, 3, 1, 1, 3, 1};//{ 0, 0, 1, 1, 0, 0, 0 };
double yk[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};//{ 1, 3, 1, 0, 0, 0, 1, 3, 1 };//, 1, 0, 1, 1, 0 };

int xkl = xk.length;
int ykl = yk.length;

int xcl;
int ycl;

int xpn = 0;
int ypn = 0;

int max_peaks = 100;
int xp[], yp[];
double xc[], yc[];

void convolve( double Signal[], int SignalLen, double Kernel[], int KernelLen, double Result[])
{
  int n;
  int i = 0;
  for (n = 0; n < SignalLen + KernelLen - 1; n++)
  {
    int kmin, kmax, k;

    Result[n] = 0;

    kmin = (n >= KernelLen - 1) ? n - (KernelLen - 1) : 0;
    kmax = (n < SignalLen - 1) ? n : SignalLen - 1;


    for (k = kmin; k <= kmax; k++)
    {
      i++;
      Result[n] += Signal[k] * Kernel[n - k];
    }
  }
  println(i + " cycles for convolve");
}

void processImage()
{

  double x[] = new double[xi];
  double y[] = new double[yi];

  for (int i = 0; i < yi; i++ )
  {
    for (int j = 0; j < xi; j++ )
    {
      float z = in[i][j];
      x[j] += z;
      y[i] += z;
    }
  }

  /* x convolution */
  xcl = xi + xkl - 1;
  xc = new double[xcl];
  convolve(x, xi, xk, xkl, xc);

  /* y convolution */
  ycl = yi + ykl - 1;
  yc = new double[ycl];
  convolve(y, yi, yk, ykl, yc);

  map = new int[ycl][xcl];
  for (int i = 0; i < ycl; i++ )
  {
    for (int j = 0; j < xcl; j++ )
    {
      map[i][j] = (int)(yc[i] + xc[j]);
    }
  }
  xm = xcl;
  ym = ycl;
  calculatePeaks();
}

void calculatePeaks()
{
  boolean peak = true;
  xp = new int[max_peaks];
  yp = new int[max_peaks];
  double prev, diff;
  prev = xc[0];
  for (int j = 1; j < xcl; j++ )
  {
    diff = xc[j] - prev;
    if ( peak )
    {
      if ( diff < 0 )
      {
        peak = false;
        xp[xpn++] = j;
      }
      prev = xc[j];
      if (xpn == max_peaks) break;
    }
    else
    {
      if ( diff > 0 ) peak = true;
    }
  }

  prev = yc[0];
  for (int i = 1; i < ycl; i++ )
  {
    diff = yc[i] - prev;
    if ( peak )
    {
      if ( diff < 0 )
      {
        peak = false;
        yp[ypn++] = i;
      }
      prev = yc[i];
      if (ypn == max_peaks) break;
    }
    else
    {
      if ( diff > 0 ) peak = true;
    }
  }
}