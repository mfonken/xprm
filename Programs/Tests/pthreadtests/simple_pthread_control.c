#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define uS(X) X*1000000

void trigger()
{
    printf("-trigger-\n");
}

typedef struct
{
    pthread_mutex_t *mutex;
    int rate;
} interval_trigger_t;

static inline long getTime( struct timeval tv )
{
    gettimeofday(&tv, NULL);
    return long(tv.tv_usec + tv.tv_sec*1000000);
}

void * test( void * data )
{
    interval_trigger_t timer = *(interval_trigger_t*)data;
    int sl = 1000000/timer.rate, test = 1;
    struct timeval time;
    long curr_time, end_time;
    while( pthread_mutex_trylock(e.mutex) )
    {
        end_time = getTime(time) + sl;
        trigger();
        while( (curr_time = getTime(time)) < end_time );
    }
    return NULL;
}

int main(int argc, char * argv[])
{
    int duration = 1, trigger = 1;
    if(argc >= 2) duration = atoi(argv[1]);
    if(argc >= 3) trigger = atoi(argv[2]);
    printf("Duration is set to %dsec and trigger rate is set to %dHz\n", duration, trigger);
    
    printf("Setting up thread and mutex\n");
    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    interval_trigger_t myTimer = {&mutex, trigger};
    
    printf("Starting thread\n");
    pthread_mutex_lock(&mutex);
    pthread_create(&thread, NULL, test, (void*)&myTimer);
    
    usleep(uS(duration));
    
    printf("Pausing thread\n");
    pthread_mutex_unlock(&mutex);
    
    return 0;
}
