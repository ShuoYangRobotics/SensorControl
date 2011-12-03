#include <stdio.h>
#include <stdlib.h>

#include "serialsource.h"
#include "informationPacket.h"
#include "instructionPacket.h"
#include "serialprotocol.h"
#include "communicate.h"
#include "const.h"

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
 * This function sends packet to control mote
 * if type = 0, then the packet sent contains a instruction
 * if type = 1, then the packet sent is only used to trigger CONTROL_DISCOVER 
 */
int send_pkt(serial_source src, char* bytes, int msg_len, uint8_t type, uint16_t serverID)
{
	int count = CTRLMSG_LEN + 5 + 8;
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
	packet[5] = CTRLMSG_LEN + 5; //message length - struct length	
	packet[6] = 0;	
	packet[7] = AM_MOTETOPC; //channel	

	//variables
		packet[8] = CONTROL_INSTRUCTION_T;
	if (type == 0)
		packet[9] = SEND_CONTROL_COMMAND;
	else if (type == 1)
		packet[9] = SEND_CONTROL_DISCOVER;
	else if (type == 2)
		packet[9] = SEND_CONTROL_REGISTER;

		//temporarily broadcast to all server nodes
		packet[10] = (uint8_t)serverID;
		packet[11] =(uint8_t)(serverID >> 8);
		packet[12] = msg_len;
		for (i=0;i<msg_len;i++)
			packet[13+i] = bytes[i];

	//send packet	
	if (write_serial_packet(src, packet, count) == 0)
	{
		printf ("Done.\n");
		return 1;
	}
	else
	{
		printf ("Sending error!\n");
		return -1;
	}
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
		int getReply = 0;
		fflush(stdin);
		printf("tinyOS>>>");
		instruct = (char*)malloc(CTRLMSG_LEN+1);
		int len = CTRLMSG_LEN;
		getdelim(&instruct, &len,'\n', stdin);
		instructLen = strlen(instruct) - 1;
		instruct[instructLen] = 0;
		if ((instruct[0] == 113)&&
			(instruct[1] == 117)&&
			(instruct[2] == 105)&&
			(instruct[3] == 116))
		{
			printf("Exit control interface.\n");
			break;
		}
		if (instructLen > CTRLMSG_LEN)
		{
			printf("The message is too long!");
		}
		else
		{
			int type,c;
			printf("Type is:");
			if ( scanf("%d", &type) != EOF ) 
            	while ( (c=getchar()) != '\n' && c != EOF );
			printf("sending message '%s', length is %d, type is %d...\n",instruct, instructLen, type);
			if(send_pkt(src,instruct,instructLen,type,0xffff) == -1)
				getReply = 1;
			if (type!=0) 
			//to debug
				getReply = 1;
		}
		while(!getReply)
		{
			int len;
			uint8_t *packet = read_serial_packet(src, &len);

			if (!packet) exit(0);
			else
			{
				fprintf(stderr, "get reply, ");
				tmsg_t *msg = new_tmsg(packet + 1 , len - 1);
				char cmd[60];
				for (i=0;i<60;i++)
				{
					cmd[i] = 32;
				}
				if (!msg) exit(0);
				else
				{
					uint8_t messageLength = infopacket_reply_length_get(msg);
					fprintf(stderr, "receive(%d) from mote(%d):",messageLength,infopacket_server_nodeID_get(msg));
					int j = 0;
					for (;j<messageLength;j++)
					{
						cmd[j] = infopacket_reply_get(msg,j);
						if (j==(messageLength-1))
							fprintf(stderr, "%1s",cmd);
					}
				}
				free(msg);
				fprintf(stderr, "\n");
			}
			free((void *)packet);
			//not sure about the loop breaking situation
			getReply = 1;
		}
	}
}
