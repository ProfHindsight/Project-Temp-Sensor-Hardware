/*
TMRh20 2014

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/** General Data Transfer Rate Test
 * This example demonstrates basic data transfer functionality with the
 updated library. This example will display the transfer rates acheived using
 the slower form of high-speed transfer using blocking-writes.
 */

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <RF24/RF24.h>
#include <unistd.h>

using namespace std;

#define discover_packet_size	16
#define provision_packet_size   14
#define node_info_packet_size   5
#define node_data_packet_size   18
#define gateway_ack_packet_size 5


enum packet_types
{
	discover_packet_type = 1,
	provision_packet_type = 2,
	node_infor_packet_type = 3,
	node_data_packet_type = 4,
	gateway_ack_packet_type = 5
};

struct discover_packet
{
	uint8_t packet_type;
	char message[15];
};

struct provision_packet
{
	packet_types packet_type;
	uint8_t node_id;
	uint32_t address;
	uint32_t mswait;
};

struct node_info_packet
{
	packet_types packet_type;
	uint8_t VBUS_available;
	uint8_t bmp280_available;
	uint8_t si7020_available;
	uint8_t hardware_revision[2];
};

struct node_data_packet
{
	packet_types packet_type;
	uint8_t node_id;
	uint32_t BMP280_pres;
	int32_t BMP280_temp;
	uint32_t Si7020_humid;
	int32_t Si7020_temp;
};

struct gateway_ack_packet
{
	packet_types packet_type;
	uint32_t msWait;
};


//
// Hardware configuration
//

/****************** Raspberry Pi ***********************/

// Radio CE Pin, CSN Pin, SPI Speed
// See http://www.airspayce.com/mikem/bcm2835/group__constants.html#ga63c029bd6500167152db4e57736d0939 and the related enumerations for pin information.

// Setup for GPIO 22 CE and CE0 CSN with SPI Speed @ 4Mhz
//RF24 radio(RPI_V2_GPIO_P1_22, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_4MHZ);

// NEW: Setup for RPi B+
//RF24 radio(RPI_BPLUS_GPIO_J8_15,RPI_BPLUS_GPIO_J8_24, BCM2835_SPI_SPEED_8MHZ);

// Setup for GPIO 15 CE and CE0 CSN with SPI Speed @ 8Mhz
//RF24 radio(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);

/*** RPi Alternate ***/
//Note: Specify SPI BUS 0 or 1 instead of CS pin number.
// See http://tmrh20.github.io/RF24/RPi.html for more information on usage

//RPi Alternate, with MRAA
//RF24 radio(15,0);

//RPi Alternate, with SPIDEV - Note: Edit RF24/arch/BBB/spi.cpp and  set 'this->device = "/dev/spidev0.0";;' or as listed in /dev
RF24 radio(22,0);


/****************** Linux (BBB,x86,etc) ***********************/

// See http://tmrh20.github.io/RF24/pages.html for more information on usage
// See http://iotdk.intel.com/docs/master/mraa/ for more information on MRAA
// See https://www.kernel.org/doc/Documentation/spi/spidev for more information on SPIDEV

// Setup for ARM(Linux) devices like BBB using spidev (default is "/dev/spidev1.0" )
//RF24 radio(115,0);

//BBB Alternate, with mraa
// CE pin = (Header P9, Pin 13) = 59 = 13 + 46 
//Note: Specify SPI BUS 0 or 1 instead of CS pin number. 
//RF24 radio(59,0);

/**************************************************************/

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t gateway_address 	= {		0xdf569390d4};
const uint64_t addresses[8] 	= {		0x0a4c9afbf4,
										0x039d255293,
										0x765e51966c,
										0x501e2f44be,
										0x7548fac931,
										0x5da0cab1da,
										0x77c4d1172b,
										0x1fd5a00a8a};
const uint64_t default_node_address = {	0xac04b1a4e3};


uint8_t packet[8];
unsigned long startTime, stopTime, counter, rxTimer=0;

int main(int argc, char** argv)
{

  // Print preamble:

  	cout << "Starting the gateway thing\n";
	radio.begin();                           // Setup and configure rf radio
	radio.setChannel(1);
	radio.setPALevel(RF24_PA_MAX);
	radio.setDataRate(RF24_1MBPS);
	radio.setAutoAck(1);                     // Ensure autoACK is enabled
	radio.setRetries(2,15);                  // Optionally, increase the delay between retries & # of retries
	radio.setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance
	radio.printDetails();
	radio.openWritingPipe(default_node_address);
	radio.openReadingPipe(1,gateway_address);
	radio.startListening();

    // forever loop
	while (1)
	{


		while(radio.available())
		{
			radio.read(&packet,8);
		}

		if(millis() - rxTimer > 1000)
		{
			rxTimer = millis();
			printf("Rate: ");
			float numBytes = counter*32;
			printf("%.2f KB/s \n\r",numBytes/1000);
			printf("Packet Count: %lu \n\r", counter);
			counter = 0;
		}


	} // loop
} // main






