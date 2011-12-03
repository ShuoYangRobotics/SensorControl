AUTOMAKE_OPTIONS = foreign

MIGFLAGS = -D_POSIX_C_SOURCE
TOS= /opt/tinyos-2.1.1/tos
SERIAL_H = $(CURDIR)/../../Serial.h
CFLAGS += -I$(CURDIR)/../../

BUILT_SOURCES =serialsource.h serialprotocol.h informationPacket.h instructionPacket.h

noinst_PROGRAMS=serverPC 
noinst_LIBRARIES=libmote.a

serverPC_SOURCES = serverPC.c
serverPC_LDADD = libmote.a

libmote_a_SOURCES = \
	message.c \
	serialsource.c \
	informationPacket.c \
	instructionPacket.c 

informationPacket.c informationPacket.h: $(SERIAL_H)
	mig -o informationPacket.h -c-prefix=infopacket c $(SERIAL_H) SERVER_RESPONSE_with_header $(MIGFLAGS)
instructionPacket.c instructionPacket.h: $(SERIAL_H)
	mig -o instructionPacket.h -c-prefix=cmdpacket c $(SERIAL_H) CONTROL_COMMAND_with_header $(MIGFLAGS)

serialprotocol.h: $(SERIAL_H)
	ncg -o $@ -c-prefix=SERIAL c $(SERIAL_H) Serial.h $(MIGFLAGS)
