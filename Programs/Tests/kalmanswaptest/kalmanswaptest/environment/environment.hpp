//
//  environment.hpp
//  combine_core
//
//  Created by Matthew Fonken on 1/20/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#ifndef environment_hpp
#define environment_hpp

#include <stdio.h>

#include <string>
#include <stdlib.h>
#include <stdint.h>

#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_THREADS 3
#define MAX_FPS     60
#define OUT_FPS  60
#define OUT_UDL  1000000 / OUT_FPS

#define MAX_EVENTS 6

using namespace std;

static inline long getTime( struct timeval tv )
{
    gettimeofday(&tv, NULL);
    return long(tv.tv_usec + tv.tv_sec*1000000);
}

class TestInterface
{
public:
    int id;
    std::string name;
    virtual void  init( void ) = 0;
    virtual void  trigger( void ) = 0;
    virtual string serialize( void ) = 0;
};

typedef enum
{
    INITIALIZED = 0,
    LIVE,
    PAUSED,
    ERROR
} ENV_STATUS;

class Event
{
public:
    int id,
        rate;
    pthread_t       thread;
    pthread_mutex_t * mutex;
    TestInterface   * test;
    
    Event(int);
    Event( pthread_mutex_t*, TestInterface*, int );
    static void     * worker(void * data);
};

class EventList
{
private:
    int     index;
    Event * list[MAX_EVENTS];
public:
    
    EventList();
    int add( pthread_mutex_t*, TestInterface*, int );
    int validIndex(int, int);
    Event * get(int);
    int remove(int);
    int length();
};

class Environment
{
private:
    pthread_mutex_t lock;
    pthread_cond_t  condition;
    
    void controlTick();
public:
    ENV_STATUS status;
    EventList events;
    pthread_t thread;
    
    Environment( TestInterface*, int );
    ~Environment();
    
    void addTest( TestInterface*, int );
    
    void start();
    void pause();
    void resume();
};

#endif /* environment_hpp */
