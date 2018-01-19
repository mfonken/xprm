//
//  imu_general_test.cpp
//  
//
//  Created by Matthew Fonken on 9/9/17.
//
//

//#define MAG_TEST

#include <stdio.h>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#ifdef MAG_TEST
	#include "mag_cal_test.cpp"
#else
	#include "imu_general_test.cpp"
#endif

int uart0_filestream = -1;

std::string msg("");

void TEST_SETUP()
{
    setup( &msg );
	int count = write(uart0_filestream, msg.c_str(), msg.size());
	if (count < 0) printf("UART TX error\n");
}

void TEST_LOOP()
{
    loop( &msg );
	int count = write(uart0_filestream, msg.c_str(), msg.size());
	if (count < 0) printf("UART TX error\n");
}

void INIT_SERCOM()
{
	uart0_filestream = open("/dev/rfcomm0", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
	if (uart0_filestream == -1) printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
	struct termios options;
	tcgetattr(uart0_filestream, &options);
	options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;		//<Set baud rate
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart0_filestream, TCIFLUSH);
	tcsetattr(uart0_filestream, TCSANOW, &options);
}
           
int main( int argc, char * args[] )
{
	bool mag_test = false;
#ifdef MAG_TEST
	mag_test = true;
#endif
	printf("Starting %s test.\n", mag_test?"MAG":"GENERAL");
    printf("Starting serial comm...\n");
	INIT_SERCOM();
	
   	TEST_SETUP();
   	for( int i = 0; i < 100; )
	{
		TEST_LOOP();
		usleep(50000);
		printf("%s",msg.c_str());
		msg.clear();
	}
	return 0;
}
