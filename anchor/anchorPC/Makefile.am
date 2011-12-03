AUTOMAKE_OPTIONS = foreign

MIGFLAGS = -D_POSIX_C_SOURCE
TOS= /opt/tinyos-2.1.1/tos
SERIAL_H = $(CURDIR)/../../Serial.h
CFLAGS += -I$(CURDIR)/../../
CFLAGS += -lm

BUILT_SOURCES =serialsource.h serialprotocol.h infoPacket.h decisionPacket.h

noinst_PROGRAMS=anchorPC 
noinst_LIBRARIES=libmote.a

anchorPC_SOURCES = anchorPC.c
anchorPC_LDADD = libmote.a

libmote_a_SOURCES = \
	message.c \
	serialsource.c \
	infoPacket.c \
	decisionPacket.c 

infoPacket.c infoPacket.h: $(SERIAL_H)
	mig -o infoPacket.h -c-prefix=infopacket c $(SERIAL_H) ANCHOR_POSINFO_with_header $(MIGFLAGS)
decisionPacket.c decisionPacket.h: $(SERIAL_H)
	mig -o decisionPacket.h -c-prefix=dcspacket c $(SERIAL_H) ANCHOR_DECISION_with_header $(MIGFLAGS)

serialprotocol.h: $(SERIAL_H)
	ncg -o $@ -c-prefix=SERIAL c $(SERIAL_H) Serial.h $(MIGFLAGS)
