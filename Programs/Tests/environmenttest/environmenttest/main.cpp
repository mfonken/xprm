//
//  main.c
//  environmenttest
//
//  Created by Matthew Fonken on 1/22/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#include <stdio.h>

#include "environment_master.h"

#define DURATION 1
#define RATE_A 1
#define RATE_B 10

#define NUM_EVENTS 4

class A
: public TestInterface
{
public:
    void  init( void ) { printf("(%d)-i\n", id); };
    void  trigger( void ) { /*printf("(%d)-!\n", id);*/ counter++; };
    string serialize( void ) { /*printf("(%d)-s\n", id);*/ return ""; };
    
    A() { id = 0; counter = 0; };
    A(int i) { id = i; counter = 0; };
    int id;
    int counter;
};


int main(int argc, const char * argv[]) {
    printf("Starting main\n");
    
    printf("Initializing serial communication: ");
    SerialWriter s(USB);
    if(s.isInitialized()) printf("success\n");
    else printf("failure\n");
    
    printf("Initializing test unit: ");
    A tests[NUM_EVENTS];
    for(int i = 0; i < NUM_EVENTS; i++) tests[i] = A(i+1);
    printf("success\n");
    
    printf("Initializing test environment with %dHz trigger: ", RATE_A);
    Environment test(&tests[0], &s, RATE_A);
    for(int i = 1; i < NUM_EVENTS; i++) test.addTest(&tests[i], i*10);
    printf("success\n");
    
    printf("Starting test\n");
    test.start();
    
    printf("Sleeping for %d seconds\n", DURATION);
    for(int i = 0; i < DURATION; i++) usleep(1000000);
    printf("done\n");
    
    printf("Pausing test\n");
    test.pause();

    printf("Sleeping for %d seconds\n", DURATION);
    for(int i = 0; i < DURATION; i++) usleep(1000000);

    printf("Final trigger counts are:\n");
    for(int i = 0; i < NUM_EVENTS; i++) printf("\t(%d)>%d\n", tests[i].id, tests[i].counter);
    
    printf("Ending main\n");
    return 0;
}
