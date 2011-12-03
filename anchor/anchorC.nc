#include <Timer.h>
#include <math.h>
#include "common.h"

module anchorC
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

	//uses interface Receive as collectReceive;
	//uses interface Send as collectSend;

	//uses interface StdControl as disseminationControl;
	//uses interface DisseminationValue<ANCHOR_SYNC> as Value;
	//uses interface DisseminationUpdate<ANCHOR_SYNC> as Update;

	//uses interface StdControl as routingControl;
	//uses interface RootControl;
}

implementation
{
	//message skeleton
	message_t pkt;

	//iterator used in loops
	uint8_t i;
	uint8_t j;

	//time of ANCHOR_SYNC received, if this number equals NUM_ANCHORS+NUM_CONTROLS, then send ANCHOR_POSINFO to PC
	uint8_t num_sync;

	uint8_t counter = 0;

	uint8_t receiveDecision = 0;

	//temp variable to store RSSI, ID or decision
	uint16_t tempRSSI;
	uint16_t tempID;
	uint8_t tempDecision;

	//a variable prevents multiple access to communication units
	bool busy = FALSE;

	//a list stores the rssi value between two motes
	uint16_t rssiList[NUM_ANCHORS+NUM_CONTROLS][NUM_ANCHORS+NUM_CONTROLS];

	/*
	 * For the decision of the control motes:
	 * 0 means no decision has been made to this mote
	 * 1 means the node is trustworthy(anchors are defaultly to be 1)
	 * 2 means the node is suspicious
	 * this list is also used by anchor_master
	 */
	uint8_t decisionList[NUM_ANCHORS+NUM_CONTROLS];

	//temporary pkt pointer
	ANCHOR_DECISION* adpkt;
	ANCHOR_QUERY* aqpkt;
	ANCHOR_SYNC* aspkt;
	CONTROL_DISCOVER* cdpkt;
	SERVER_QUERY* sqpkt;
	
	//for debug, a easier version
	uint16_t rssiList_test[NUM_ANCHORS+NUM_CONTROLS];
	//for debug, a easier version
	/*
	 * A function used for control Leds, it will convert the last three bits of a 16-bit number val to the states of leds. Bit 0 means led off, bit 1 means led on.
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
	 * This function returns the rssi value measured by CC2420 chip, which is a uint16_t number
	 */	
	uint16_t getRssi(message_t *msg)
	{
		return (uint16_t) call CC2420Packet.getRssi(msg);
	}
	
	/*
	 * This function sends the ANCHOR_RESPONSE message
	 */
	void ANCHOR_RESPONSE_send(uint16_t server_nodeID, uint16_t ctrl_id,uint8_t decision)
	{
		if (!busy)
		{
			//get payload
			ANCHOR_RESPONSE* arpkt = (ANCHOR_RESPONSE*) (call Packet.getPayload(&pkt, sizeof(ANCHOR_RESPONSE)));
			if (arpkt ==NULL)
				return;

			//add content
			arpkt -> packet_type = ANCHOR_RESPONSE_T;
			arpkt -> anchor_nodeID = TOS_NODE_ID;
			arpkt -> control_nodeID = ctrl_id;
			arpkt -> decision = decision;
			//end of the content

			//send the packet
			if (call AMSend.send(server_nodeID, &pkt, sizeof(ANCHOR_RESPONSE)) ==SUCCESS)
				busy = TRUE;
		}
	}

	/*
	 * Function for sending ANCHOR_QUERY
	 */
	void ANCHOR_QUERY_send()
	{
		if (!busy)
		{
			//get payload
			aqpkt = (ANCHOR_QUERY*) (call Packet.getPayload(&pkt, sizeof(ANCHOR_QUERY)));
			if (aqpkt ==NULL)
				return;

			//add content
			aqpkt -> packet_type = ANCHOR_QUERY_T;
			aqpkt -> anchor_nodeID = TOS_NODE_ID;

			//end of the content

			//send the packet
			if (call AMSend.send(AM_BROADCAST_ADDR, &pkt, sizeof(ANCHOR_QUERY)) ==SUCCESS)
				busy = TRUE;
		}
	}

	/*
	 * Function for sending ANCHOR_SYNC among anchors
	 */
	void ANCHOR_SYNC_send()
	{
		if (!busy)
		{
			//get payload
			//if (TOS_NODE_ID == 0)
			//	aspkt = (ANCHOR_SYNC*) (call collectSend.getPayload(&pkt, sizeof(ANCHOR_SYNC)));
			//else 
				aspkt = (ANCHOR_SYNC*) (call Packet.getPayload(&pkt, sizeof(ANCHOR_SYNC)));
			if (aspkt ==NULL)
				return;

			//add content
			aspkt -> packet_type = ANCHOR_SYNC_T;
			aspkt -> anchor_nodeID = TOS_NODE_ID;
			for (i=0;i<NUM_CONTROLS+NUM_ANCHORS;i++)
			{
				aspkt -> rssiList[i] = rssiList[i][TOS_NODE_ID];
				aspkt -> decisionList[i] = decisionList[i];
			}
			//end of the content

			//send the packet
			//if (TOS_NODE_ID == 0)
			//{
			//	if (receiveDecision)
			//		call Update.change(aspkt);
			//	receiveDecision = 0;
			//}
			//else
			//{
				if (call AMSend.send(AM_BROADCAST_ADDR, &pkt, sizeof(ANCHOR_SYNC)) ==SUCCESS)
					busy = TRUE;
			//}
		}
	}

	void ANCHOR_POSINFO_upload()
	{
		if (!busy)
		{
			//get payload
			ANCHOR_POSINFO* appkt = (ANCHOR_POSINFO*) (call Packet.getPayload(&pkt, sizeof(ANCHOR_POSINFO)));
			if (appkt ==NULL)
				return;

			//add content
			appkt -> packet_type = ANCHOR_POSINFO_T; 
			for (i=0;i<NUM_ANCHORS+NUM_CONTROLS;i++)
				for (j=0;j<NUM_ANCHORS+NUM_CONTROLS;j++)
					appkt -> rssiList[i][j] = rssiList[i][j];
			//end of the content

			//send the packet
			if (call serialSend.send(AM_BROADCAST_ADDR, &pkt, sizeof(ANCHOR_POSINFO)) ==SUCCESS)
				busy = TRUE;
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
		else
		{
			//call disseminationControl.start();
			//call routingControl.start();
			//if (TOS_NODE_ID == 0)
			//{
			//	call RootControl.setRoot();
			//}
			call Timer0.startPeriodic(15000);
		}
	}

	event void Timer0.fired()
	{
		if (TOS_NODE_ID != 0)
		{
			//counter++;
			//setLeds(counter);
			ANCHOR_QUERY_send();
		}
	}

	event message_t* serialReceive.receive(message_t* msg, void* payload, uint8_t len)
	{
		if (len == sizeof(ANCHOR_DECISION))
		{
			adpkt = (ANCHOR_DECISION*) payload;
			for(i=0;i<NUM_CONTROLS;i++)
			{
				tempID = adpkt -> control_nodeID[i]; 
				tempDecision = adpkt -> decision[i];
				decisionList[tempID] = tempDecision;		
			}
			receiveDecision = 1;
		}
		return msg;
	}

	event message_t* AMReceive.receive(message_t* msg, void* payload, uint8_t len)
	{
		if (len == sizeof(CONTROL_DISCOVER))
		{
			//for debug only
				//counter ++ ;
				//setLeds(counter);
			//for debug only
			tempRSSI = getRssi(msg);
			cdpkt = (CONTROL_DISCOVER*) payload;
			tempID = cdpkt -> control_nodeID;
			rssiList[tempID][TOS_NODE_ID] = tempRSSI;

			//for debug only
			rssiList_test[tempID] = tempRSSI;
			//for debug only

			ANCHOR_SYNC_send();
		}
		else if (len == sizeof(SERVER_QUERY))
		{
			//for debug only
				//counter ++ ;
				//setLeds(counter);
			//for debug only

			sqpkt = (SERVER_QUERY*) payload;
			tempID = sqpkt -> control_nodeID; 
			tempDecision = decisionList[tempID];
			//for debug only
				setLeds(tempID);
			//for debug only
			//for debug only
			//	tempDecision = 1;
			//	if (rssiList_test[tempID]-45>65435)
			//		tempDecision = 1;
			//for debug only
				
			if (TOS_NODE_ID == 0)
			    ANCHOR_RESPONSE_send(sqpkt -> server_nodeID, tempID, tempDecision);
		}
		else if (len == sizeof(ANCHOR_QUERY))
		{
			//for debug only
				//counter ++ ;
				//setLeds(counter);
			//for debug only
			tempRSSI = getRssi(msg);
			aqpkt = (ANCHOR_QUERY*) payload;
			tempID = aqpkt -> anchor_nodeID;
			rssiList[tempID][TOS_NODE_ID] = tempRSSI;
			//for debug only
				rssiList_test[tempID] = tempRSSI;
			//for debug only
			decisionList[tempID] = 1;
		}
//		return msg;
//	}

//	event message_t* collectReceive.receive(message_t* msg, void* payload, uint8_t len)
//	{
		else if (len == sizeof(ANCHOR_SYNC))
		{
			if (TOS_NODE_ID == 0)
			{
			//for debug only
				counter ++ ;
				setLeds(counter);
			//for debug only
			aspkt = (ANCHOR_SYNC*) payload;
			for (i=0;i<NUM_ANCHORS+NUM_CONTROLS;i++)
				rssiList[i][aspkt->anchor_nodeID] = aspkt -> rssiList[i]; 
			//num_sync += 1;
			//if (num_sync>=NUM_CONTROLS+NUM_ANCHORS-1)
			ANCHOR_POSINFO_upload();
			}
			else
			{
			//	aspkt = call Value.get();
				aspkt = (ANCHOR_SYNC*) payload;
				for (i=0;i<NUM_CONTROLS+NUM_ANCHORS;i++)
				{
					decisionList[i] = aspkt -> decisionList[i];
				}
			}
		}
		return msg;
	}

	//event void Value.changed()
	//{
	//	if (TOS_NODE_ID != 0)
	//	{
	//		aspkt = call Value.get();
	//		for (i=0;i<NUM_CONTROLS+NUM_ANCHORS;i++)
	//		{
	//			decisionList[i] = aspkt -> decisionList[i];
	//		}
	//	}
	//}

	event void AMSend.sendDone(message_t* msg, error_t error)
	{
		if (&pkt == msg)
			busy = FALSE;
	}

	event void serialSend.sendDone(message_t* msg, error_t error)
	{
		if (&pkt == msg)
			busy = FALSE;
	}
	
	//event void collectSend.sendDone(message_t* msg, error_t error)
	//{
	//	if (&pkt == msg)
	//		busy = FALSE;
	//}

	event void AMControl.stopDone(error_t err) {}

	event void serialControl.stopDone(error_t err) {}
}
