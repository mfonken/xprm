#include <stdio.h>
#include <stdint.h>

#define RHO_DEBUG

enum LogLevel
{
    TEST = 0,
    DEBUG_0,
    DEBUG_1,
    DEBUG_2,
    ALWAYS
};

#define TEST_LOG_LEVEL TEST

#ifndef LOG
#define LOG(L,...) if(L >= TEST_LOG_LEVEL) \
{  for(uint8_t i = L; i< ALWAYS; i++) printf("\t"); printf(__VA_ARGS__); }
#endif

#ifdef RHO_DEBUG
#define LOG_RHO(L,...) LOG(L,"<Rho> " __VA_ARGS__)
#else
#define LOG_RHO(...)
#endif

int main()
{
    LOG_RHO(TEST, "Test: A%d\n", 1);
    LOG_RHO(DEBUG_0, "Debug0: B%d%f\n", 1, 3.2);
    LOG_RHO(DEBUG_1, "Debug1: C%d%f%s\n", 1, 3.2, "test");
    LOG_RHO(DEBUG_2, "Debug2: D%d%f%s%x\n", 1, 3.2, "test", 0x10);
    LOG_RHO(ALWAYS, "Always: Test\n");
    size_t a = 0, b = 0, c = 0, d = 0, e = 0;
    LOG_RHO(ALWAYS, "\tSizes: RedVar-%luB SelVars-%luB PredVars-%luB Rho-%lukB > Tot-%.3fkB\n", a, b, c, d>>10, ((double)e)/1024);
	return 0;
}
