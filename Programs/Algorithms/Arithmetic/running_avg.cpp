#include <iostream>
#include <cmath>


#define OUTPUT
#define INTEGER

void cma(long new_val, long *avg, int num)
{
    *avg += (new_val - *avg)/(num + 1);
#ifdef OUTPUT
    std::cout << "New average is " << *avg << "\n";
#endif
}

int main(int argc, char* argv[])
{
    int r = 1;
    int N = 100;
    int R = 250;
    if(argc >= 2)
    {
        r = atoi(argv[1]);
        std::cout << "Running " << r << " times.\n";
        if(argc >= 3)
        {
            N = atoi(argv[2]);
            if(argc >= 4)
            {
                R = atoi(argv[3]);
            }
        }
    }
#ifdef INTEGER
    long avgs_diffs[r], nums[N], r_avg = 0, m_avg = 0, sum = 0, avg = 0, stdv = 0;
#else
    double avgs_diffs[r], nums[N], r_avg = 0, m_avg = 0, sum = 0, avg = 0, stdv = 0;
#endif

    for(int j = 0; j < r; j++)
    {
        /* Generate numbers to average */
#ifdef OUTPUT
        std::cout << N << " numbers generated: \n";
#endif
        for(int i = 0; i < N; i++)
        {
            nums[i] = std::rand() % (int)(R + 1);
#ifdef OUTPUT
            //std::cout << nums[i] << " ";
#endif
        }
        
        sum = 0;
        /* Get real average */
        for(int i = 0; i < N; i++)
        {
            sum += nums[i];
        }
        r_avg = sum/N;
#ifdef OUTPUT
        std::cout << "\nReal average is " << r_avg << "\n";
#endif
        /* My running average estimate */
        m_avg = 0;
        for(int i = 0; i < N; i++)
        {
            //m_avg = (m_avg*i+nums[i]) / (i + 1);
            //m_avg += (nums[i]-m_avg) / (i + 1);
            cma(nums[i], &m_avg, i);
        }
#ifdef OUTPUT
        std::cout << "  My average is " << m_avg << " \n\twhich is ";
#endif
        
        avgs_diffs[j] = ((r_avg - m_avg) * 100)/r_avg;
#ifdef OUTPUT
        std::cout << avgs_diffs[j] << "% off.\n\n";
#endif
    }
    
    sum = 0;
    for(int i = 0; i < r; i++)
    {
        sum += avgs_diffs[i];
    }
    avg = sum/r;
    
    stdv = 0;
    for(int i = 0; i < r; i++)
    {
        stdv += (avg-avgs_diffs[i])*(avg-avgs_diffs[i]);
    }
    stdv = sqrt(stdv/r);
    std::cout << "\nAverage error is " << avg << "% with a standard deviation of " << stdv << "%.\n";
    
    return 0;
}
    