#include <Gamecube.h>
#include <GamecubeAPI.h>
#include <Gamecube_N64.h>
#include <N64.h>
#include <N64API.h>
#include <Nintendo.h>

/*
  This project has been modified for Arduino Nano using MasterJhimmy's revised code
  The original code and revised code can be found at https://github.com/tevey/snesToGameCube
  Uses Arduino Nintendo Library(https://github.com/NicoHood/Nintendo) by NicoHood
*/

/*
====SNES PINOUT====
TYPE  | ORIGINAL| EXTENSION CABLE
5v    | white   | green
clock | yellow  | yellow
latch | orange  | white
data  | red     | red
gnd   | brown   | black

====GC PINOUT====
TYPE  | ORIGINAL| EXTENSION CABLE
5v    | yellow  | white
3.3v  | blue    | green
data  | red     | red
gnd   | black   | black
gnd   | white   | yellow
gnd   | green   | blue (some extension cables have 2 ground wires instead of 3)
*/
int R_analog;
int L_analog;
// SNES button mapping
const int BUTTON_A = 8;
const int BUTTON_B = 0;
const int BUTTON_X = 9;
const int BUTTON_Y = 1;
const int BUTTON_SELECT = 2;
const int BUTTON_START = 3;
const int BUTTON_R = 11;
const int BUTTON_L = 10;
const int DPAD_UP = 4;
const int DPAD_DOWN = 5;
const int DPAD_LEFT = 6;
const int DPAD_RIGHT = 7;

// SNES pins
const int SNES_DATA_SERIAL_PIN = 3;
const int SNES_DATA_CLOCK_PIN = 4;
const int SNES_DATA_LATCH_PIN = 5;

int lastVal = 0;

// Array holding SNES buttons states
int buttons[12];

int GC_DATA_PIN = 2;
// This makes the controller bidirection data line on digital pin 2 (d2)
CGamecubeConsole GamecubeConsole(GC_DATA_PIN); // Defines a "Gamecube Console" sending data to the console on pin 8
Gamecube_Data_t d = defaultGamecubeData;       // Structure for data to be sent to console

// This is needed but you don't need a controller on pin 7
CGamecubeController GamecubeController1(7);

void setup()
{
  // Set SNES_DATA_CLOCK_PIN normally HIGH
  pinMode(SNES_DATA_CLOCK_PIN, OUTPUT);
  digitalWrite(SNES_DATA_CLOCK_PIN, HIGH);

  // Set SNES_DATA_LATCH_PIN normally LOW
  pinMode(SNES_DATA_LATCH_PIN, OUTPUT);
  digitalWrite(SNES_DATA_LATCH_PIN, LOW);

  // Set SNES_DATA_SERIAL_PIN normally HIGH
  pinMode(SNES_DATA_SERIAL_PIN, OUTPUT);
  digitalWrite(SNES_DATA_SERIAL_PIN, HIGH);
  pinMode(SNES_DATA_SERIAL_PIN, INPUT);

  // This is needed to run the code.
  GamecubeController1.read();
}

void loop()
{
  readSNES();
  writeGC();
}

void readSNES()
{
  // Latch for 12us
  digitalWrite(SNES_DATA_LATCH_PIN, HIGH);
  delayMicroseconds(12);
  digitalWrite(SNES_DATA_LATCH_PIN, LOW);
  delayMicroseconds(6);

  // Retrieve button presses from shift register by pulling the clock high for 6us
  for (int i = 0; i < 16; i++)
  {
    digitalWrite(SNES_DATA_CLOCK_PIN, LOW);
    delayMicroseconds(6);
    if (i <= 11)
    {
      buttons[i] = !digitalRead(SNES_DATA_SERIAL_PIN);
    }
    digitalWrite(SNES_DATA_CLOCK_PIN, HIGH);
    delayMicroseconds(6);
  }
}

void writeGC()
{
  // sends the complied data to console when console polls for the input
  switch (buttons[BUTTON_R])
  {

  case 0:
    R_analog = 0;
    break;
  case 1:
    R_analog = 255;
    break;
  }
  switch (buttons[BUTTON_L])
  {

  case 0:
    L_analog = 0;
    break;
  case 1:
    L_analog = 255;
    break;
  }
  d.report.a = buttons[BUTTON_A];
  d.report.b = buttons[BUTTON_B];
  d.report.x = buttons[BUTTON_X];
  d.report.y = buttons[BUTTON_Y];
  d.report.z = buttons[BUTTON_SELECT];
  d.report.start = buttons[BUTTON_START];
  d.report.r = buttons[BUTTON_R];
  d.report.right = R_analog;
  d.report.l = buttons[BUTTON_L];
  d.report.left = L_analog;
  d.report.dleft = buttons[DPAD_LEFT];
  d.report.dup = buttons[DPAD_UP];
  d.report.dright = buttons[DPAD_RIGHT];
  d.report.ddown = buttons[DPAD_DOWN];
  GamecubeConsole.write(d);
}