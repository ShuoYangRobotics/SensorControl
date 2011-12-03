#include <AM.h>
#include <Timer.h>
#include "Ctp.h"
#include "common.h"

configuration anchorAppC
{
}

implementation
{
	components MainC;
	components LedsC;
	//components DelugeC;
	components anchorC as App;
	components new TimerMilliC() as Timer0;
	components ActiveMessageC;
	components new AMSenderC(26);
	components new AMReceiverC(26);
	components CC2420ActiveMessageC;
	components SerialActiveMessageC;

	//components DisseminationC;
	//components new DisseminatorC(ANCHOR_SYNC, 0x1234) as anchorSync;
	//components CollectionC;
	//components new CollectionSenderC(0xee);
	
	App.Boot -> MainC;
	App.Leds -> LedsC;
	App.Timer0 -> Timer0;
	App.Packet -> AMSenderC;

	App.AMPacket -> AMSenderC;
	App.AMControl -> ActiveMessageC;
	App.AMSend -> AMSenderC;
	App.AMReceive -> AMReceiverC;

	App.CC2420Packet -> CC2420ActiveMessageC;

	App.serialPacket -> SerialActiveMessageC;
	App.serialControl -> SerialActiveMessageC;
	App.serialSend -> SerialActiveMessageC.AMSend[AM_MOTETOPC];
	App.serialReceive -> SerialActiveMessageC.Receive[AM_MOTETOPC];

	//App.collectSend -> CollectionSenderC;
	//App.collectReceive -> CollectionC.Receive[0xee];

	//App.disseminationControl -> DisseminationC;
	//App.Value -> anchorSync;
	//App.Update -> anchorSync;

	//App.routingControl -> CollectionC;
	//App.RootControl -> CollectionC;
}
	
	
