/**
 * This file is automatically generated by mig. DO NOT EDIT THIS FILE.
 * This file defines the layout of the 'SERVER_RESPONSE_with_header' message type.
 */

#ifndef INFOPACKET_H
#define INFOPACKET_H
#include <message.h>

enum {
  /** The default size of this message type in bytes. */
  INFOPACKET_SIZE = 73,

  /** The Active Message type associated with this message. */
  INFOPACKET_AM_TYPE = -1,

  /* Field header.dest: type uint16_t, offset (bits) 0, size (bits) 16 */
  /** Offset (in bytes) of the field 'header.dest' */
  INFOPACKET_HEADER_DEST_OFFSET = 0,
  /** Offset (in bits) of the field 'header.dest' */
  INFOPACKET_HEADER_DEST_OFFSETBITS = 0,
  /** Size (in bytes) of the field 'header.dest' */
  INFOPACKET_HEADER_DEST_SIZE = 2,
  /** Size (in bits) of the field 'header.dest' */
  INFOPACKET_HEADER_DEST_SIZEBITS = 16,

  /* Field header.src: type uint16_t, offset (bits) 16, size (bits) 16 */
  /** Offset (in bytes) of the field 'header.src' */
  INFOPACKET_HEADER_SRC_OFFSET = 2,
  /** Offset (in bits) of the field 'header.src' */
  INFOPACKET_HEADER_SRC_OFFSETBITS = 16,
  /** Size (in bytes) of the field 'header.src' */
  INFOPACKET_HEADER_SRC_SIZE = 2,
  /** Size (in bits) of the field 'header.src' */
  INFOPACKET_HEADER_SRC_SIZEBITS = 16,

  /* Field header.length: type uint8_t, offset (bits) 32, size (bits) 8 */
  /** Offset (in bytes) of the field 'header.length' */
  INFOPACKET_HEADER_LENGTH_OFFSET = 4,
  /** Offset (in bits) of the field 'header.length' */
  INFOPACKET_HEADER_LENGTH_OFFSETBITS = 32,
  /** Size (in bytes) of the field 'header.length' */
  INFOPACKET_HEADER_LENGTH_SIZE = 1,
  /** Size (in bits) of the field 'header.length' */
  INFOPACKET_HEADER_LENGTH_SIZEBITS = 8,

  /* Field header.group: type uint8_t, offset (bits) 40, size (bits) 8 */
  /** Offset (in bytes) of the field 'header.group' */
  INFOPACKET_HEADER_GROUP_OFFSET = 5,
  /** Offset (in bits) of the field 'header.group' */
  INFOPACKET_HEADER_GROUP_OFFSETBITS = 40,
  /** Size (in bytes) of the field 'header.group' */
  INFOPACKET_HEADER_GROUP_SIZE = 1,
  /** Size (in bits) of the field 'header.group' */
  INFOPACKET_HEADER_GROUP_SIZEBITS = 8,

  /* Field header.type: type uint8_t, offset (bits) 48, size (bits) 8 */
  /** Offset (in bytes) of the field 'header.type' */
  INFOPACKET_HEADER_TYPE_OFFSET = 6,
  /** Offset (in bits) of the field 'header.type' */
  INFOPACKET_HEADER_TYPE_OFFSETBITS = 48,
  /** Size (in bytes) of the field 'header.type' */
  INFOPACKET_HEADER_TYPE_SIZE = 1,
  /** Size (in bits) of the field 'header.type' */
  INFOPACKET_HEADER_TYPE_SIZEBITS = 8,

  /* Field packet_type: type uint8_t, offset (bits) 56, size (bits) 8 */
  /** Offset (in bytes) of the field 'packet_type' */
  INFOPACKET_PACKET_TYPE_OFFSET = 7,
  /** Offset (in bits) of the field 'packet_type' */
  INFOPACKET_PACKET_TYPE_OFFSETBITS = 56,
  /** Size (in bytes) of the field 'packet_type' */
  INFOPACKET_PACKET_TYPE_SIZE = 1,
  /** Size (in bits) of the field 'packet_type' */
  INFOPACKET_PACKET_TYPE_SIZEBITS = 8,

  /* Field control_nodeID: type uint16_t, offset (bits) 64, size (bits) 16 */
  /** Offset (in bytes) of the field 'control_nodeID' */
  INFOPACKET_CONTROL_NODEID_OFFSET = 8,
  /** Offset (in bits) of the field 'control_nodeID' */
  INFOPACKET_CONTROL_NODEID_OFFSETBITS = 64,
  /** Size (in bytes) of the field 'control_nodeID' */
  INFOPACKET_CONTROL_NODEID_SIZE = 2,
  /** Size (in bits) of the field 'control_nodeID' */
  INFOPACKET_CONTROL_NODEID_SIZEBITS = 16,

  /* Field server_nodeID: type uint16_t, offset (bits) 80, size (bits) 16 */
  /** Offset (in bytes) of the field 'server_nodeID' */
  INFOPACKET_SERVER_NODEID_OFFSET = 10,
  /** Offset (in bits) of the field 'server_nodeID' */
  INFOPACKET_SERVER_NODEID_OFFSETBITS = 80,
  /** Size (in bytes) of the field 'server_nodeID' */
  INFOPACKET_SERVER_NODEID_SIZE = 2,
  /** Size (in bits) of the field 'server_nodeID' */
  INFOPACKET_SERVER_NODEID_SIZEBITS = 16,

  /* Field reply_length: type uint8_t, offset (bits) 96, size (bits) 8 */
  /** Offset (in bytes) of the field 'reply_length' */
  INFOPACKET_REPLY_LENGTH_OFFSET = 12,
  /** Offset (in bits) of the field 'reply_length' */
  INFOPACKET_REPLY_LENGTH_OFFSETBITS = 96,
  /** Size (in bytes) of the field 'reply_length' */
  INFOPACKET_REPLY_LENGTH_SIZE = 1,
  /** Size (in bits) of the field 'reply_length' */
  INFOPACKET_REPLY_LENGTH_SIZEBITS = 8,

  /* Field reply: type uint8_t[], element size (bits) 8 */
  /** Elementsize (in bytes) of the field 'reply' */
  INFOPACKET_REPLY_ELEMENTSIZE = 1,
  /** Elementsize (in bits) of the field 'reply' */
  INFOPACKET_REPLY_ELEMENTSIZEBITS = 8,
  /** The number of dimensions in the array 'reply'. */
  INFOPACKET_REPLY_NUMDIMENSIONS = 1,
  /** Number of elements in dimension 1 of array 'reply'. */
  INFOPACKET_REPLY_NUMELEMENTS_1 = 60,
  /** Total number of elements in the array 'reply'. */
  INFOPACKET_REPLY_NUMELEMENTS = 60,
};

/**
 * Return the value of the field 'header.dest'
 */
uint16_t infopacket_header_dest_get(tmsg_t *msg);

/**
 * Set the value of the field 'header.dest'
 */
void infopacket_header_dest_set(tmsg_t *msg, uint16_t value);

/**
 * Return the value of the field 'header.src'
 */
uint16_t infopacket_header_src_get(tmsg_t *msg);

/**
 * Set the value of the field 'header.src'
 */
void infopacket_header_src_set(tmsg_t *msg, uint16_t value);

/**
 * Return the value of the field 'header.length'
 */
uint8_t infopacket_header_length_get(tmsg_t *msg);

/**
 * Set the value of the field 'header.length'
 */
void infopacket_header_length_set(tmsg_t *msg, uint8_t value);

/**
 * Return the value of the field 'header.group'
 */
uint8_t infopacket_header_group_get(tmsg_t *msg);

/**
 * Set the value of the field 'header.group'
 */
void infopacket_header_group_set(tmsg_t *msg, uint8_t value);

/**
 * Return the value of the field 'header.type'
 */
uint8_t infopacket_header_type_get(tmsg_t *msg);

/**
 * Set the value of the field 'header.type'
 */
void infopacket_header_type_set(tmsg_t *msg, uint8_t value);

/**
 * Return the value of the field 'packet_type'
 */
uint8_t infopacket_packet_type_get(tmsg_t *msg);

/**
 * Set the value of the field 'packet_type'
 */
void infopacket_packet_type_set(tmsg_t *msg, uint8_t value);

/**
 * Return the value of the field 'control_nodeID'
 */
uint16_t infopacket_control_nodeID_get(tmsg_t *msg);

/**
 * Set the value of the field 'control_nodeID'
 */
void infopacket_control_nodeID_set(tmsg_t *msg, uint16_t value);

/**
 * Return the value of the field 'server_nodeID'
 */
uint16_t infopacket_server_nodeID_get(tmsg_t *msg);

/**
 * Set the value of the field 'server_nodeID'
 */
void infopacket_server_nodeID_set(tmsg_t *msg, uint16_t value);

/**
 * Return the value of the field 'reply_length'
 */
uint8_t infopacket_reply_length_get(tmsg_t *msg);

/**
 * Set the value of the field 'reply_length'
 */
void infopacket_reply_length_set(tmsg_t *msg, uint8_t value);

/**
 * Return the byte offset of an element of array 'reply'
 */
size_t infopacket_reply_offset(size_t index1);

/**
 * Return an element of the array 'reply'
 */
uint8_t infopacket_reply_get(tmsg_t *msg, size_t index1);

/**
 * Set an element of the array 'reply'
 */
void infopacket_reply_set(tmsg_t *msg, size_t index1, uint8_t value);

/**
 * Return the bit offset of an element of array 'reply'
 */
size_t infopacket_reply_offsetbits(size_t index1);

#endif