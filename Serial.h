#ifndef SERIAL_H
#define SERIAL_H
#include "AM.h"
#include "common.h"

typedef uint8_t uart_id_t;

#define UQ_SERIALQUEUE_SEND "SerialQueueP.Send"

enum {
  HDLC_FLAG_BYTE	   = 0x7e,
  HDLC_CTLESC_BYTE	   = 0x7d,
};

// message_t type dispatch

enum {
  TOS_SERIAL_ACTIVE_MESSAGE_ID = 0,
  TOS_SERIAL_CC1000_ID = 1,
  TOS_SERIAL_802_15_4_ID = 2,
  TOS_SERIAL_UNKNOWN_ID = 255,
};

// Framer-level dispatch
enum {
  SERIAL_PROTO_ACK = 67,
  SERIAL_PROTO_PACKET_ACK = 68,
  SERIAL_PROTO_PACKET_NOACK = 69,
  SERIAL_PROTO_PACKET_UNKNOWN = 255
};

typedef struct radio_stats {
  uint8_t version;
  uint8_t flags;
  uint8_t reserved;
  uint8_t platform;
  uint16_t MTU;
  uint16_t radio_crc_fail;
  uint16_t radio_queue_drops;
  uint16_t serial_crc_fail;
  uint16_t serial_tx_fail;
  uint16_t serial_short_packets;
  uint16_t serial_proto_drops;
} radio_stats_t;

typedef nx_struct serial_header {
  nx_am_addr_t dest;
  nx_am_addr_t src;
  nx_uint8_t length;
  nx_am_group_t group;
  nx_am_id_t type;
} serial_header_t;

typedef nx_struct serial_packet {
  serial_header_t header;
  nx_uint8_t data[];
} serial_packet_t;

typedef nx_struct serial_metadata {
  nx_uint8_t ack;
} serial_metadata_t;

typedef nx_struct SERVER_RESPONSE_with_header {
	serial_header_t header;
	nx_uint8_t packet_type;
	nx_uint16_t control_nodeID;
	nx_uint16_t server_nodeID;
	nx_uint8_t reply_length;
	nx_uint8_t reply[REPLYMSG_LEN];
}SERVER_RESPONSE_with_header;

typedef nx_struct CONTROL_INSTRUCTION_with_header {
	serial_header_t header;
	nx_uint8_t packet_type;
	nx_uint8_t control_instruction;
	nx_uint16_t server_nodeID;
	nx_uint8_t command_length;
	nx_uint8_t command_content[CTRLMSG_LEN];
} CONTROL_INSTRUCTION_with_header;

typedef nx_struct CONTROL_COMMAND_with_header {
	serial_header_t header;
	nx_uint8_t packet_type;
    nx_uint16_t control_nodeID;
    nx_uint16_t server_nodeID;
    nx_uint8_t command_length;
    nx_uint8_t command_content[CTRLMSG_LEN];
} CONTROL_COMMAND_with_header;


typedef nx_struct ANCHOR_DECISION_with_header {
	serial_header_t header;
	nx_uint8_t packet_type;
	nx_uint16_t control_nodeID[NUM_CONTROLS];
	nx_uint8_t decision[NUM_CONTROLS];
} ANCHOR_DECISION_with_header;

typedef nx_struct ANCHOR_POSINFO_with_header {
	serial_header_t header;
	nx_uint8_t packet_type;
	nx_uint16_t rssiList[NUM_ANCHORS+NUM_CONTROLS][NUM_ANCHORS+NUM_CONTROLS];
} ANCHOR_POSINFO_with_header;

#endif

