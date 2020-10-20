#include <stdint.h>

typedef enum { OPEN, CLOSE } open_close_state;
typedef enum { EAST, WEST } east_west_state;
typedef enum { START, STOP } start_stop_state;
typedef enum { EMERGENCY, NOT_EMERGENCY } emergency_state;

typedef struct {
  open_close_state cur_state;
  open_close_state nx_state;
  bool to_tx;
  bool is_pressed;
} open_close_button;

typedef struct {
  east_west_state cur_state;
  east_west_state nx_state;
  bool to_tx;
  bool is_pressed;
} east_west_button;

typedef struct {
  start_stop_state cur_state;
  start_stop_state nx_state;
  bool to_tx;
  bool is_pressed;
} start_stop_button;

typedef struct {
  uint8_t btn_delay;
  open_close_button open_close_btn;
  east_west_button east_west_btn;
  start_stop_button start_stop_btn;
} btns_state_machine;

typedef struct {
  char line1[16];
  char line2[16];
  bool to_tx;
} lcd_state_machine;