#include "BTInterface.h"

comm_status initialise_interface(bt_interface *frame) {
  frame->ss->begin(BAUD_RATE);
  comm_status status = transmit_frame(frame);
};

comm_status transmit_frame(bt_interface *frame) {
  if (frame->length == 0)
    return NOP;
  for (int i = 0; i < frame->length && i < BUFFER_SIZE; i++)
    frame->ss->write(frame->msg[i]);

  frame->ss->write("\r\n");
  frame->length = 0;
  return SUCCESS;
}

comm_status build_frame(bt_interface *frame, char c) {
  if (frame->length >= BUFFER_SIZE)
    return OVERFLOW;
  if (c == '\r')
    return TRANSMIT;
  if (c == '\n')
    return NOP;
  frame->msg[frame->length] = c;
  frame->length++;
  return SUCCESS;
}

/*
 * Sends over a single char
 */
comm_status quick_transmit(bt_interface *frame, char c) {
  frame->ss->write(c);
  frame->ss->write("\n");
  frame->length = 0;
  return SUCCESS;
}

/*
 * Resets the length of the frame to 0 so that messages can be sent again
 */
void reset_frame(bt_interface *frame) { frame->length = 0; }