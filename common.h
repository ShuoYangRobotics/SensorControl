#ifndef COMMON_H
#define COMMON_H
#include "const.h"

//size : 4
typedef nx_struct ANCHOR_QUERY {
	nx_uint8_t packet_type;
	nx_uint8_t dummy;
	nx_uint16_t anchor_nodeID;
} ANCHOR_QUERY;

//size : 6
typedef nx_struct ANCHOR_RESPONSE {
	nx_uint8_t packet_type;
	nx_uint16_t anchor_nodeID;
	nx_uint16_t control_nodeID;
	nx_uint8_t decision;
} ANCHOR_RESPONSE;

//size : 3+3*5 = 18
typedef nx_struct ANCHOR_SYNC {
	nx_uint8_t packet_type;
	nx_uint16_t anchor_nodeID;
	nx_uint16_t rssiList[NUM_ANCHORS+NUM_CONTROLS];
	nx_uint8_t decisionList[NUM_ANCHORS+NUM_CONTROLS];
} ANCHOR_SYNC;

//size : 1+3*1 = 4
typedef nx_struct ANCHOR_DECISION {
	nx_uint8_t packet_type;
	nx_uint16_t control_nodeID[NUM_CONTROLS];
	nx_uint8_t decision[NUM_CONTROLS];
} ANCHOR_DECISION;

//size : 1+50 = 51
typedef nx_struct ANCHOR_POSINFO {
	nx_uint8_t packet_type;
	nx_uint16_t rssiList[NUM_ANCHORS+NUM_CONTROLS][NUM_ANCHORS+NUM_CONTROLS];
} ANCHOR_POSINFO;

//size : 5
typedef nx_struct SERVER_QUERY {
	nx_uint8_t packet_type;
	nx_uint16_t server_nodeID;
	nx_uint16_t control_nodeID;
} SERVER_QUERY;

//size : 6+1*50
typedef nx_struct SERVER_RESPONSE {
	nx_uint8_t packet_type;
	nx_uint16_t control_nodeID;
	nx_uint16_t server_nodeID;
	nx_uint8_t reply_length;
	nx_uint8_t reply[REPLYMSG_LEN];
}SERVER_RESPONSE;

//size : 5+1*20
typedef nx_struct CONTROL_INSTRUCTION {
	nx_uint8_t packet_type;
	nx_uint8_t control_instruction;
	nx_uint16_t server_nodeID;
	nx_uint8_t command_length;
	nx_uint8_t command_content[CTRLMSG_LEN];
} CONTROL_INSTRUCTION;

//size : 3
typedef nx_struct CONTROL_DISCOVER {
	nx_uint8_t packet_type;
	nx_uint16_t control_nodeID;
} CONTROL_DISCOVER;

//size : 6+1*50
typedef nx_struct CONTROL_COMMAND {
	nx_uint8_t packet_type;
	nx_uint16_t control_nodeID;
	nx_uint16_t server_nodeID;
	nx_uint8_t command_length;
	nx_uint8_t command_content[CTRLMSG_LEN];
} CONTROL_COMMAND;
#endif
