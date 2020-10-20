#pragma once
#include <AltSoftSerial.h>

#define BUFFER_SIZE 20
#define BAUD_RATE 9600 

typedef struct {
  char length;
  char msg[BUFFER_SIZE];
  AltSoftSerial *ss;
} bt_interface;

typedef enum { SUCCESS, FAILURE, OVERFLOW, TRANSMIT, NOP } comm_status;
  
typedef enum {
  NONE,
  EAST,
  WEST,
  START,
  STOP,
  OPEN,
  CLOSE,
  EMERGENCY,
} message;


comm_status initialise_interface(bt_interface *);
comm_status transmit_frame(bt_interface *);
comm_status build_frame(bt_interface *, char c);
comm_status quick_transmit(bt_interface *, char c);
message interpret_msg(char msg);
void reset_frame(bt_interface *);