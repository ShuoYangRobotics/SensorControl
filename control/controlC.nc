#include <Timer.h>
#include <math.h>
#include "common.h"

module controlC
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
	CONTROL_DISCOVER* cdpkt;
	CONTROL_COMMAND* ccpkt;
	CONTROL_INSTRUCTION* cipkt;

	//a counter used for counting how many CONTROL_REGISTER messages have been sent out
	uint16_t counter =0;

	uint8_t isDiscovering = 0;
	//iterator used in loops
	uint8_t i;

	//a variable prevents multiple access to communication units
	bool busy = FALSE;

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
	 * This function sends the CONTROL_DISCOVER message
	 */
	void CONTROL_DISCOVER_send()
	{
		if (!busy)
		{
			//get payload
			cdpkt = (CONTROL_DISCOVER*) (call Packet.getPayload(&pkt, sizeof(CONTROL_DISCOVER)));
			if (cdpkt ==NULL)
				return;

			//add content
			cdpkt -> control_nodeID = TOS_NODE_ID;
			//end of the content

			//send the packet
			if (call AMSend.send(AM_BROADCAST_ADDR, &pkt, sizeof(CONTROL_DISCOVER)) ==SUCCESS)
				busy = TRUE;
		}
	}

	/*
	 * Function for sending CONTROL_COMMAND
	 */
	void CONTROL_COMMAND_send(uint8_t msg_len, uint8_t* msg, uint16_t server_id)
	{
		if (!busy)
		{
			//get payload
			ccpkt = (CONTROL_COMMAND*) (call Packet.getPayload(&pkt, sizeof(CONTROL_COMMAND)));
			if (ccpkt ==NULL)
				return;

			//add content
			ccpkt -> control_nodeID = TOS_NODE_ID;
			ccpkt -> server_nodeID = server_id;
			ccpkt -> command_length = msg_len;
			for (i=0;i<msg_len;i++)
				ccpkt -> command_content[i] = msg[i];
			//end of the content

			//send the packet
			if (call AMSend.send(AM_BROADCAST_ADDR, &pkt, sizeof(CONTROL_COMMAND)) ==SUCCESS)
				busy = TRUE;
			//for debug only
				setLeds(msg_len);
			//for debug only
		}
	}

	/*
	 * When the control mote receives a SERVER_RESPONSE,
	 * it simply forward this message to the PC to which it attached via serial port.
	 */
	void SERVER_RESPONSE_upload(SERVER_RESPONSE* payload)
	{
		if (!busy)
		{
			//get payload
			srpkt = (SERVER_RESPONSE*) (call Packet.getPayload(&pkt, sizeof(SERVER_RESPONSE)));
			if (srpkt ==NULL)
				return;

			//add content
			*srpkt = *payload;
			//end of the content

			//send the packet
			if (call serialSend.send(AM_BROADCAST_ADDR, &pkt, sizeof(SERVER_RESPONSE)) ==SUCCESS)
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
			//for debug only
			//call Timer0.startPeriodic(TIMER_PERIOD_MILLI);
		}
	}

	event void Timer0.fired()
	{
		counter++;
		if (counter >= NUM_CONTROL_REGISTER)
		{
			//commented for debug
			call Timer0.stop();
			counter = 0;
			isDiscovering = 0;
			setLeds(counter);
			return;
		}
		else
		{
			CONTROL_DISCOVER_send();
			setLeds(counter);
		}
	}

	event message_t* serialReceive.receive(message_t* msg, void* payload, uint8_t len)
	{
		if (len == sizeof(CONTROL_INSTRUCTION))
		{
			cipkt = (CONTROL_INSTRUCTION*) payload;

			if (cipkt->control_instruction == SEND_CONTROL_DISCOVER)
				CONTROL_DISCOVER_send();
			else if (cipkt->control_instruction == SEND_CONTROL_REGISTER)
				call Timer0.startPeriodic(TIMER_PERIOD_MILLI);
			else if (cipkt->control_instruction == SEND_CONTROL_COMMAND)
				CONTROL_COMMAND_send(cipkt->command_length, cipkt->command_content, cipkt->server_nodeID);
		}
		return msg;
	}

	event message_t* AMReceive.receive(message_t* msg, void* payload, uint8_t len)
	{
		if (len == sizeof(ANCHOR_QUERY))
		{
			if (!isDiscovering)
			{
				call Timer0.startPeriodic(TIMER_PERIOD_MILLI);
				isDiscovering = 1;
			}
		}
		else if (len == sizeof(SERVER_RESPONSE))
			SERVER_RESPONSE_upload(payload);
		return msg;
	}

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

	event void AMControl.stopDone(error_t err) {}

	event void serialControl.stopDone(error_t err) {}
}
