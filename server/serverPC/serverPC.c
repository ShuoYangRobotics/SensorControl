#include <stdio.h>
#include <stdlib.h>

#include "serialsource.h"
#include "informationPacket.h"
#include "instructionPacket.h"
#include "serialprotocol.h"
#include "communicate.h"
#include "const.h"

char result[60];
static char *msgs[] = {
	"unknown_packet_type",
	"ack_timeout"	,
	"sync"	,
	"too_long"	,
	"too_short"	,
	"bad_sync"	,
	"bad_crc"	,
	"closed"	,
	"no_memory"	,
	"unix_error"
};
/*
 * This function sends packet to server mote
 * if type = 0, then the packet sent contains a instruction
 * if type = 1, then the packet sent is only used to trigger CONTROL_DISCOVER 
 */
void send_pkt(serial_source src, char bytes[], int msg_len, uint16_t
serverID, uint16_t controlID)
{
	int count = REPLYMSG_LEN + 6 + 8;
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
	packet[5] = REPLYMSG_LEN + 6; //message length - struct length	
	packet[6] = 0;	
	packet[7] = AM_MOTETOPC; //channel	

	//variables
	packet[8] = SERVER_RESPONSE_T;
	packet[9] = (uint8_t)controlID;
	packet[10] = (uint8_t)(controlID >> 8);
	packet[11] =(uint8_t)(serverID);
	packet[12] =(uint8_t)(serverID >> 8);
	packet[13] = msg_len;
	printf("reply: ");
	for (i=0;i<msg_len;i++)
	{
		printf("%c",bytes[i]);
		packet[14+i] = bytes[i];
	}
	printf("\n");

	//send packet	
	if (write_serial_packet(src, packet, count) == 0)
		printf ("\nSend packet done.\n");
	else
		printf ("Sending error!\n");
}

void stderr_msg(serial_source_msg problem)
{
	fprintf(stderr, "Note: %s\n", msgs[problem]);
}

int main(int argc, char **argv)
{
	serial_source src;

	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s <device> <rate> - dump packets from a serial port\n", argv[0]);
		exit(2);
	}
	src = open_serial_source(argv[1], platform_baud_rate(argv[2]), 0, stderr_msg);
	if (!src)
	{
		fprintf(stderr, "Couldn't open serial port at %s:%s\n",
				argv[1], argv[2]);
		exit(1);
	}

	int instructLen = 0;
	char* instruct;
	int len, i;
	system("clear");
	for (;;)
	{
		int len,messageLength;
		uint8_t *packet = read_serial_packet(src, &len);

		if (!packet) exit(0);
		else
		{
			fprintf(stderr, "get command, ");
			tmsg_t *msg = new_tmsg(packet + 1 , len - 1);
			char cmd[CTRLMSG_LEN];
			for (i=0;i<CTRLMSG_LEN;i++)
			{
				cmd[i] = 32;
			}
			if (!msg) exit(0);
			else
			{
				uint8_t messageLength = cmdpacket_command_length_get(msg);
				fprintf(stderr, "receive(%d) from mote(%d): ",messageLength, cmdpacket_control_nodeID_get(msg));
				int j = 0;
				for (;j<messageLength;j++)
				{
					cmd[j] = cmdpacket_command_content_get(msg,j);
					if (j==(messageLength-1))
						fprintf(stderr, "%1s",cmd);
				}
			}
			executeCommand(cmd, &messageLength, result);
			
			send_pkt(src,result,messageLength,cmdpacket_server_nodeID_get(msg), cmdpacket_control_nodeID_get(msg));
			fprintf(stderr, "\n");
			free(msg);
		}
		free((void *)packet);
	}
}
