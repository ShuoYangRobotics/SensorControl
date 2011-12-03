#include <Timer.h>
#include <math.h>
#include "common.h"
#include "const.h"

module serverC
{
	uses interface Boot;
	uses interface Leds;
	uses interface Timer<TMilli> as Timer0;
	uses interface Packet; 
	uses interface CC2420Packet;

	uses interface AMPacket;
	uses interface SplitControl as AMControl;
	uses interface AMSend;
	uses interface Receive as AMReceive;

	uses interface SplitControl as serialControl;
	uses interface Packet as serialPacket;
	uses interface Receive as serialReceive;
	uses interface AMSend as serialSend;
}
implementation
{
	//message skeleton
	message_t pkt;

	SERVER_RESPONSE* srpkt;
	CONTROL_COMMAND* ccpkt;
	SERVER_QUERY* sqpkt;
	ANCHOR_RESPONSE* arpkt;

	//a counter used for counting how many CONTROL_REGISTER messages have been sent out
	uint8_t counter = 0;

	//iterator used in loops
	uint8_t i;

	uint8_t decisionMade = 0;
	uint8_t waitingForAnchor = 0;
	uint8_t inList = 0;

	uint16_t control_idList[NUM_CONTROLS];
	uint8_t control_decisionList[NUM_CONTROLS];

	//a variable prevents multiple access to communication units
	bool serial_busy = FALSE;
	bool radio_busy = FALSE;

	uint8_t tmp_msg_len;
	uint8_t tmp_msg[REPLYMSG_LEN];
	uint16_t tmp_control_nodeID;

	/*
	 * A function used for control Leds, it will convert the last three bits of a 8-bit number val to the states of leds. Bit 0 means led off, bit 1 means led on.
	 */
	void setLeds(uint16_t val)
	{
		if (val & 0x01) call Leds.led0On();
		else call Leds.led0Off();
		if (val & 0x02) call Leds.led1On();
		else call Leds.led1Off();
		if (val & 0x04) call Leds.led2On();
		else call Leds.led2Off();
	}
	
	/*
	 * This function sends the SERVER_RESPONSE message
	 */
	void SERVER_RESPONSE_send(uint8_t msg_len, uint8_t *msg, uint16_t ctrlid)
	{
		if (!radio_busy)
		{
			//get payload
			srpkt = (SERVER_RESPONSE*) (call Packet.getPayload(&pkt, sizeof(SERVER_RESPONSE)));
			if (srpkt ==NULL)
				return;

			//add content
			srpkt -> packet_type = SERVER_RESPONSE_T;
			srpkt -> server_nodeID = TOS_NODE_ID;
			srpkt -> control_nodeID = ctrlid;
			srpkt -> reply_length = msg_len;
			for (i=0;i<msg_len;i++)
				srpkt -> reply[i] = msg[i];
			//end of the content

			//for debug only
				setLeds(srpkt -> reply_length);
			//for debug only

			//send the packet
			if (call AMSend.send(AM_BROADCAST_ADDR, &pkt, sizeof(SERVER_RESPONSE)) ==SUCCESS)
				radio_busy = TRUE;
		}
	}

	/*
	 * Function for sending CONTROL_COMMAND
	 */
	void CONTROL_COMMAND_upload(uint8_t msg_len, uint8_t* msg, uint16_t server_id)
	{
		if (!serial_busy)
		{
			//get payload
			ccpkt = (CONTROL_COMMAND*) (call Packet.getPayload(&pkt, sizeof(CONTROL_COMMAND)));
			if (ccpkt ==NULL)
				return;

			//add content
			ccpkt -> packet_type = CONTROL_COMMAND_T;
			ccpkt -> control_nodeID = TOS_NODE_ID;
			ccpkt -> server_nodeID = server_id;
			ccpkt -> command_length = msg_len;
			for (i=0;i<msg_len;i++)
				ccpkt -> command_content[i] = msg[i];
			//end of the content

			//send the packet
			if (call serialSend.send(AM_BROADCAST_ADDR, &pkt, sizeof(CONTROL_COMMAND)) ==SUCCESS)
				serial_busy = TRUE;
		}
	}

	/*
	 */
	void SERVER_QUERY_send(uint16_t ctrlid)
	{
			//for debug only
			//	setLeds(6);
			//for debug only
		if (!radio_busy)
		{
			//get payload
			sqpkt = (SERVER_QUERY*) (call Packet.getPayload(&pkt, sizeof(SERVER_QUERY)));
			if (sqpkt ==NULL)
				return;

			//add content
			sqpkt -> packet_type = SERVER_QUERY_T;
			sqpkt -> server_nodeID = TOS_NODE_ID;
			sqpkt -> control_nodeID = ctrlid;
			//end of the content

			//for debug only
			//	setLeds(sqpkt -> control_nodeID);
			//for debug only

			//send the packet
			if (call AMSend.send(AM_BROADCAST_ADDR, &pkt, sizeof(SERVER_QUERY)) ==SUCCESS)
				radio_busy = TRUE;
		}
	}

	event void Boot.booted()
	{
		call serialControl.start();
	}
	
	event void serialControl.startDone(error_t err)
	{
		if (err == SUCCESS)
		{
			call AMControl.start();
		}
		else
		{
			call serialControl.start();
		}
	}

	event void AMControl.startDone(error_t err)
	{
		if (err != SUCCESS)
		{
			call AMControl.start();
		}
	//	else call Timer0.startPeriodic(1000);
	}

	event void Timer0.fired()
	{
		//counter++;
		//setLeds(counter);
	}

	event message_t* serialReceive.receive(message_t* msg, void* payload, uint8_t len)
	{
		if (len == sizeof(SERVER_RESPONSE))
		{
			srpkt = (SERVER_RESPONSE*) payload;
			SERVER_RESPONSE_send(srpkt->reply_length, srpkt->reply, srpkt->control_nodeID);
		}
		return msg;
	}

	event message_t* AMReceive.receive(message_t* msg, void* payload, uint8_t len)
	{
		//for debug only
		//	counter++;
		//	setLeds(counter);
		//for debug only
		if (len == sizeof(ANCHOR_RESPONSE))
		{
			arpkt = (ANCHOR_RESPONSE*) payload;
			for (i=0;i<NUM_CONTROLS;i++)
			{
				if (arpkt -> control_nodeID == control_idList[i])
				{
					inList = 1;
					control_decisionList[i] = arpkt -> decision;
					break;
				}
			}

			if (!inList)
			{
				for (i=0;i<NUM_CONTROLS;i++)
				{
					if (control_idList[i] == 0)					
					{
						control_idList[i] = arpkt -> control_nodeID;
						control_decisionList[i] = arpkt -> decision;
					}
				}
			}
			inList = 0;
			//for debug only
				setLeds(arpkt -> decision);
			//for debug only

			if ((arpkt -> decision == 1) && (arpkt -> control_nodeID == tmp_control_nodeID))
			{
				waitingForAnchor = 0;
				CONTROL_COMMAND_upload(tmp_msg_len,tmp_msg,TOS_NODE_ID);
			}
		}
		else if (len == sizeof(CONTROL_COMMAND))
		{
			//for debug only
			//	counter = counter+2;
			//	setLeds(counter);
			//for debug only
			ccpkt = (CONTROL_COMMAND*) payload;
			tmp_msg_len = ccpkt -> command_length;
			tmp_control_nodeID = ccpkt -> control_nodeID;
			for (i=0;i<tmp_msg_len;i++)
				tmp_msg[i] = ccpkt -> command_content[i];

			//for debug only
			//setLeds(ccpkt -> command_length);
			//for debug only

			//for debug only
			//	control_idList[0] = tmp_control_nodeID;
			//	control_decisionList[0] = 1;
			//for debug only
			for (i=0;i<NUM_CONTROLS;i++)
			{
				if (control_idList[i] == tmp_control_nodeID)
				{
					decisionMade = 1;
					if (control_decisionList[i] == 1)
					{
			   			CONTROL_COMMAND_upload(tmp_msg_len,tmp_msg,TOS_NODE_ID);
                        setLeds(2);
						break;
					}
					else if (control_decisionList[i] == 0) 
					{
						SERVER_QUERY_send(tmp_control_nodeID);
						waitingForAnchor = 1;
                        setLeds(1);
						break;
					}
				}
			}

			if (!decisionMade)
			{
				for (i=0;i<NUM_CONTROLS;i++)
				{
					if (control_idList[i] == 0)
					{
						control_idList[i] = tmp_control_nodeID;
						control_decisionList[i] = 0;
						//for debug only
						//	setLeds(7);
						//for debug only
						SERVER_QUERY_send(tmp_control_nodeID);
						waitingForAnchor = 1;
						break;
					}
				}
			}
			decisionMade = 0;
		}
		return msg;
	}

	event void AMSend.sendDone(message_t* msg, error_t error)
	{
		if (&pkt == msg)
		{
			radio_busy = FALSE;
		}
		//for debug only
		//setLeds(7);
		//for debug only
	}

	event void serialSend.sendDone(message_t* msg, error_t error)
	{
		if (&pkt == msg)
		{
			serial_busy = FALSE;
		}
	}

	event void AMControl.stopDone(error_t err) {}

	event void serialControl.stopDone(error_t err) {}
}
