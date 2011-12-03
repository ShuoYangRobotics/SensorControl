#ifndef _FUNCTION_H
#define _FUNCTION_H
#include "const.h"

double rssiTable[NUM_ANCHORS+NUM_CONTROLS][NUM_ANCHORS+NUM_CONTROLS];
double rssiTable_cummulated[NUM_ANCHORS+NUM_CONTROLS][NUM_ANCHORS+NUM_CONTROLS];
double rssiTable_counter[NUM_ANCHORS+NUM_CONTROLS][NUM_ANCHORS+NUM_CONTROLS];
double rssiTable_mean[NUM_ANCHORS+NUM_CONTROLS][NUM_ANCHORS+NUM_CONTROLS];
double rssiTable_variance[NUM_ANCHORS+NUM_CONTROLS][NUM_ANCHORS+NUM_CONTROLS];


static char *msgs[] = {
	"unknown_packet_type"	,
	"ack_timeout"			,
	"sync"					,
	"too_long"				,
	"too_short"				,
	"bad_sync"				,
	"bad_crc"				,
	"closed"				,
	"no_memory"				,
	"unix_error"
};

/*
 * 
 *
 * 
 */

void send_pkt(serial_source src, uint16_t idList[], uint8_t decisionList[])
{
	int count = 1 + 3 * NUM_CONTROLS + 8;
	int i;
	unsigned char *packet;
	packet = malloc(count);
	for (i=0;i<count;i++)
		packet[i]=0;
	if (!packet)
		exit(2);
	
	//adding header
	packet[0] = 0;	
	packet[1] = 255; //broadcast address upper	
	packet[2] = 255; //broadcast address lower
	packet[3] = 0;	
	packet[4] = 0;	
	packet[5] = 1 + 3 * NUM_CONTROLS; //message length - struct length	
	packet[6] = 0;	
	packet[7] = AM_MOTETOPC; //channel	

	//variables
	packet[8] = ANCHOR_DECISION_T;

	for (i=0;i<NUM_CONTROLS;i++)
	{
		if (i==0)
		{	
			packet[9+i] = (uint8_t)idList[0];
			packet[10+i] = (uint8_t)(idList[0]>>8);
			packet[11+i] = decisionList[0];
		}
		else
		{
			packet[9+i] = 0;
			packet[10+i] = 0;
			packet[11+i] = 0;
		}
	}

	//send packet	
	if (write_serial_packet(src, packet, count) == 0)
		printf ("Done.\n");
	else
		printf ("Sending error!\n");
}

void stderr_msg(serial_source_msg problem)
{
	fprintf(stderr, "Note: %s\n", msgs[problem]);
}

int convert(uint16_t b)
{
	uint8_t sign = b>>15;
	// fprintf(stderr, "sign: %d\t",sign );
	int a;
	if (sign) a = b - 65536;
	else a = b;
	return a-45;
}

void printRssiTables()
{
    int i,j;
  	fprintf(stderr, "\nRSSI Mean Table:\t\t\t\t\t|\tRSSI variance Table: \n\t");
    for (i=0;i<NUM_ANCHORS+NUM_CONTROLS;i++)
    	fprintf(stderr, "%d\t",i);
  	fprintf(stderr, "|\t\t");
    for (i=0;i<NUM_ANCHORS+NUM_CONTROLS;i++)
    	fprintf(stderr, "%d\t",i);
 	 fprintf(stderr, "\n");

    for (i=0;i<NUM_ANCHORS+NUM_CONTROLS;i++)
    {
    	fprintf(stderr, "%d\t",i);

        for (j=0;j<NUM_ANCHORS+NUM_CONTROLS;j++)
		{
			if (rssiTable_mean[i][j] != -45) 
				fprintf(stderr, "%-4.2f\t",rssiTable_mean[i][j]);
			else
				fprintf(stderr, "    \t");
			
		}

    	fprintf(stderr, "|\t%d\t",i);

        for (j=0;j<NUM_ANCHORS+NUM_CONTROLS;j++)
        	fprintf(stderr, "%-4.2f\t",rssiTable_variance[i][j]);
    	fprintf(stderr, "\n");
    }    
}

int readyForCal(int size)                                    
{
	int counter1,counter2;
    int flag = 1;
	for (counter1=0;counter1<NUM_ANCHORS+NUM_CONTROLS;counter1++)
		for (counter2=0;counter2<NUM_ANCHORS+NUM_CONTROLS;counter2++)
			if (rssiTable_counter[counter1][counter2]<size)
                return 0;
    return flag;
}
#endif
