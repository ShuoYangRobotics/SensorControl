#include <AM.h>
#include <Timer.h>
#include "common.h"

configuration serverAppC
{
}

implementation
{
	components MainC;
	components LedsC;
	components serverC as App;
	components new TimerMilliC() as Timer0;
	components ActiveMessageC;
	components new AMSenderC(26);
	components new AMReceiverC(26);
	components CC2420ActiveMessageC;
	components SerialActiveMessageC;
	
	App.Boot -> MainC;
	App.Leds -> LedsC;
	App.Timer0 -> Timer0;
	App.Packet -> AMSenderC;
	App.AMPacket -> AMSenderC;
	App.AMControl -> ActiveMessageC;
	App.AMSend -> AMSenderC;
	App.AMReceive -> AMReceiverC;
	App.CC2420Packet -> CC2420ActiveMessageC;
	App.serialControl -> SerialActiveMessageC;
	App.serialPacket -> SerialActiveMessageC;

	App.serialSend -> SerialActiveMessageC.AMSend[AM_MOTETOPC];
	App.serialReceive -> SerialActiveMessageC.Receive[AM_MOTETOPC];
}
	
	
