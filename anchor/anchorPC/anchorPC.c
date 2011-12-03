#include <stdio.h>
#include <stdlib.h>

#include "serialsource.h"
#include "infoPacket.h"
#include "decisionPacket.h"
#include "serialprotocol.h"
#include "communicate.h"
#include "functions.h"
#include "cal.h"

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
	int len, i, j;
	int counter1,counter2;
	int decision;
	uint16_t idList[0];
	uint8_t decisionList[0];
	system("clear");
	for (;;)
	{
		int len;
		uint8_t *packet = read_serial_packet(src, &len);

		if (!packet) exit(0);
		else
		{
			fprintf(stderr, "get position information packet ");
			tmsg_t *msg = new_tmsg(packet + 1 , len - 1);
			if (!msg) exit(0);
			else
				for (i=0;i<NUM_ANCHORS+NUM_CONTROLS;i++)
					for (j=0;j<NUM_ANCHORS+NUM_CONTROLS;j++)
					{
						rssiTable[i][j] = convert(infopacket_rssiList_get(msg,i,j));
						rssiTable_counter[i][j] += 1; 
						rssiTable_cummulated[i][j] += convert(infopacket_rssiList_get(msg,i,j));
						rssiTable_mean[i][j] = rssiTable_cummulated[i][j]/rssiTable_counter[i][j];
						if (rssiTable_counter[i][j] == 1)
                            rssiTable_variance[i][j] = 0;
                        else if (rssiTable_counter[i][j] == 2)
                            rssiTable_variance[i][j] = (rssiTable[i][j]-rssiTable_mean[i][j])*(rssiTable[i][j]-rssiTable_mean[i][j]);
                        else
                            rssiTable_variance[i][j] = (rssiTable_variance[i][j]*(rssiTable_counter[i][j]-2) + (rssiTable[i][j]-rssiTable_mean[i][j])*(rssiTable[i][j]-rssiTable_mean[i][j]))/(rssiTable_counter[i][j]-1);	
					}
			
			if (readyForCal(5))
			{
				vector1 = getVector(SIDE, rssiTable_mean[ANCHOR_B][ANCHOR_A]);
				printf(
					"reference vector: (%6.5f,%6.5f)",
					vector1.x,
					vector1.y 
				);
				decisionList[0] = calculatePosition(
					rssiTable_mean[CONTROL_A][ANCHOR_A],
					rssiTable_mean[CONTROL_A][ANCHOR_B],
					rssiTable_mean[CONTROL_A][ANCHOR_C],
					rssiTable_mean[CONTROL_A][ANCHOR_D]
				);
				idList[0] = CONTROL_A;
				send_pkt(src,idList,decisionList);	

				for (counter1=0;counter1<NUM_ANCHORS;counter1++)
				{
					for (counter2=0;counter2<NUM_ANCHORS;counter2++)
					{
						rssiTable_counter[counter1][counter2] = 0;
						rssiTable_cummulated[counter1][counter2] = 0;
                    }
                }
			}

			free(msg);
			fprintf(stderr, "\n");
		}
		free((void *)packet);
	}
}
