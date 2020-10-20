#include <Arduino.h>
#include "BTInterface.h"
#include <AltSoftSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BUFFER_SIZE 20
#define BAUD_RATE 9600
#define TICKS_PER_SECOND 61

#define OPEN_BTN 3
#define CLOSE_BTN 4
#define EAST_BTN 5
#define WEST_BTN 6
#define START_BTN 7
#define STOP_BTN 10
#define EMG_BTN 11

volatile unsigned char BUTTON_PRESSED_TIM = 0;
volatile unsigned char open_btn = 0;
volatile unsigned char close_btn = 0;
volatile unsigned char east_btn = 0;
volatile unsigned char west_btn = 0;
volatile unsigned char start_btn = 0;
volatile unsigned char stop_btn = 0;
volatile unsigned char emg_btn = 0;

AltSoftSerial blue_serial;

bt_interface bt_i = {4, "INIT", &blue_serial};
#include "Helpers.h"
comm_status comm_state = NOP;

message read_msg();
void print_message(message);
void configure_clock2();
void display_text(message);

void setup() {
  Serial.begin(BAUD_RATE);
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  initialise_interface(&bt_i);
  configure_clock2();

  // Interrupts for PD and PB
  PCICR = 0b00000101;

  // Enable interrupts for pins
  PCMSK2 = 0b11111000;
  PCMSK0 = 0b00001100;

  // INPUTS
  pinMode(OPEN_BTN, INPUT_PULLUP);
  pinMode(CLOSE_BTN, INPUT_PULLUP);
  pinMode(EAST_BTN, INPUT_PULLUP);
  pinMode(WEST_BTN, INPUT_PULLUP);
  pinMode(START_BTN, INPUT_PULLUP);
  pinMode(STOP_BTN, INPUT_PULLUP);
  pinMode(EMG_BTN, INPUT_PULLUP);

  display.display();
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();
  delay(100);

}

void loop() {
  message blue_in = read_msg();
  print_message(blue_in);
  if (open_btn) {
    open_btn = 0;
    display_text(OPEN);
    blue_serial.write(OPEN);
  }
  if (close_btn) {
    close_btn = 0;
    display_text(CLOSE);
    blue_serial.write(CLOSE);
  }
  if (east_btn) {
    east_btn = 0;
    display_text(EAST);
    blue_serial.write(EAST);
  }
  if (west_btn) {
    west_btn = 0;
    display_text(WEST);
    blue_serial.write(WEST);
  }
  if (start_btn) {
    start_btn = 0;
    display_text(START);
    blue_serial.write(START);
  }
  if (stop_btn) {
    stop_btn = 0;
    display_text(STOP);
    blue_serial.write(STOP);
  }
  if (emg_btn) {
    emg_btn = 0;
    display_text(EMERGENCY);
    blue_serial.write(EMERGENCY);
  }

}

// Interrupt Service Routing for all of the timers and counters
// Used to control any non blocking delays
ISR(TIMER2_COMPB_vect){
	TCNT2 = 0;

  if (BUTTON_PRESSED_TIM <= 250) {
    BUTTON_PRESSED_TIM ++;
  }
}

// Interrupt Service Routine for PORT D
ISR(PCINT2_vect) {
  if (BUTTON_PRESSED_TIM >= 10) {
    BUTTON_PRESSED_TIM = 0;
    // Equivalent to: if (digitalRead(pin) == 0)
    if (!(PIND & ( 1 << OPEN_BTN ))) open_btn = 1; 
    if (!(PIND & ( 1 << CLOSE_BTN ))) close_btn = 1;
    if (!(PIND & ( 1 << EAST_BTN ))) east_btn = 1;
    if (!(PIND & ( 1 << WEST_BTN ))) west_btn = 1;
    if (!(PIND & ( 1 << START_BTN ))) start_btn = 1;
  }
}

// Interrupt Service Routine for PORT B -- Only EMG Button
ISR(PCINT0_vect) {
  if (BUTTON_PRESSED_TIM >= TICKS_PER_SECOND) {
    BUTTON_PRESSED_TIM = 0;
    // The emergency button is actually PB2
    if (!(PINB & ( 1 << 2 ))) stop_btn = 1; 
    if (!(PINB & ( 1 << 3 ))) emg_btn = 1; 
  }
}


// Read from the Serial Monitor and send to the Bluetooth module
void serialEvent(){
  comm_state = NOP;
  while (Serial.available()){
    char in = Serial.read();
    comm_state = build_frame(&bt_i, in);
    Serial.write(in);      
  }

  switch(comm_state) {
    case TRANSMIT:
      comm_state = transmit_frame(&bt_i);
      break;
    case OVERFLOW:
      reset_frame(&bt_i);
      break;
    case SUCCESS:
      // Serial.println(sent)
      break;
    case FAILURE:
      // Serial.println(failed to send)
      break;
    case NOP:
      // Serial.println(doing nothing)
      break;
    default:
      // Serial.println(undefined behaviour)
      break;
  }
}

//function to read from bluetooth
message read_msg(){
  message blue_in = NONE;
  if (blue_serial.available() > 0) blue_in = (message)blue_serial.read();
  return blue_in;
}

void print_message(message msg) {
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);

  switch(msg) {
  case NONE:
    break;
  case EAST:
    display.clearDisplay();
    display.println(F("RX: EAST"));
    break;
  case WEST:
    display.clearDisplay();
    display.println(F("RX: WEST"));
    break;
  case OPEN:
    display.clearDisplay();
    display.println(F("RX: OPEN"));
    break;
  case CLOSE:
    display.clearDisplay();
    display.println(F("RX: CLOSE"));
    break;
  case START:
    display.clearDisplay();
    display.println(F("RX: START"));
    break;
  case STOP:
    display.clearDisplay();
    display.println(F("RX: STOP"));
    break;
  case EMERGENCY:
    display.clearDisplay();
    display.println(F("RX: EMG"));
    break;
  default:
    break;
  }  
  display.display();      // Show initial text
  delay(100);
}

void configure_clock2(){
	TIMSK2 = 0b00000100; //Setting flag on compare b
	/* TCCR2B Timer/Counter Control Reg B
	 * [FOC2A][FOC2B][-][-][WGM22][CS22][CS21][CS20]
	 */
	TCCR2B = 0; // turning clock off

	/* TCCR2A Timer/Counter Control Reg A
	 * [COM2A1][COM2A0][COM2B1][COM2B0][-][-][WGM21][WGM20]
	 */
	TCCR2A = 0; // Setting waveform to normal op

	OCR2B = 255; // Setting max compare

	TIFR2 = 0b00000100; // ISR on compare match b

	/*
	 * (16,000,000/1024)/255 = 0.01632
	 * 1/0.01632 = 61 -> need 61 for 1 second
	 */
	TCCR2B = 0b00000111; // Setting prescaler to 1024

	TCNT2 = 0; // Setting clock to 0
}
void display_text(message msg) {
  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  switch (msg){
  case NONE:
    break;
  case EAST:
    display.println(F("TX: EAST"));
    break;
  case WEST:
    display.println(F("TX: WEST"));
    break;
  case OPEN:
    display.println(F("TX: OPEN"));
    break;
  case CLOSE:
    display.println(F("TX: CLOSE"));
    break;
  case START:
    display.println(F("TX: START"));
    break;
  case STOP:
    display.println(F("TX: STOP"));
    break;
  case EMERGENCY:
    display.println(F("TX: EMG"));
    break;
  default:
    break;
  }
  display.display();      // Show initial text
  delay(100);

}