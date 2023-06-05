#include "MidiChannel.h"
#include "Display.h"
#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <Bounce.h>

#define SCREEN_ADDRESS 0x3C
#define ONE_V 1241
#define TWO_V 2470
#define THREE_V 3700
#define CLOCK_PIN 12
#define GATE_PIN_OFFS 24

#define BUTTON_LEFT 2
#define BUTTON_RIGHT 3
#define BUTTON_BACK 4
#define BUTTON_EDIT 5
#define NO_MIDI_OPTIONS 4
#define NO_CLOCK_OPTIONS 1

#define UPDATE_INTERVAL 250

enum CLOCK_MULT {  // 24 MIDI clock pulses per 1/4 note
  SIXTEENTH = 6,
  SIXTEENTH_T = 8,
  EIGTH = 12,
  EIGTH_T = 16,
  QUARTER = 24,
  QUARTER_T = 32,
  HALF = 48,
  HALF_T = 64,
  BAR = 96,
  TWO_BAR = 192,
  FOUR_BAR = 384
};

//Adafruit_SSD1306 display(128, 32, &Wire, -1);
Display display(128, 32, &Wire, -1);
Adafruit_MCP4725 dac;

MidiChannel midiChannels[16];
int clockMult = CLOCK_MULT::SIXTEENTH;
int clockCount = 0;
int channelNumber = 1;

elapsedMillis uiTimer;
void buttonLeft_ISR();
void buttonRight_ISR();
void buttonBack_ISR();
void buttonEdit_ISR();
bool buttonLeftFlag = 0;
bool buttonRightFlag = 0;
bool buttonBackFlag = 0;
bool buttonEditFlag = 0;
int selectedChannel = 1;
int selectedLine = 0;

void update_ui();
int note_to_dac(int note);
void MidiNoteOn(byte channel, byte note, byte velocity);
void MidiNoteOff(byte channel, byte note, byte velocity);
void MidiClock();
void MidiStart();

void setup() { 
  Serial.begin(115200);
  Serial.println("Setup Begin");

  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_BACK, INPUT_PULLUP);
  pinMode(BUTTON_EDIT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_LEFT), buttonLeft_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_RIGHT), buttonRight_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_BACK), buttonBack_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_EDIT), buttonEdit_ISR, FALLING);

  for(unsigned int i = 0; i < (sizeof(midiChannels)/sizeof(*midiChannels)); i++)
  {
    midiChannels[i].set_channel_number(i + 1);
    midiChannels[i].set_gate_pin(i + GATE_PIN_OFFS);
    Serial.println(midiChannels[i].get_gate_pin());
  }

  dac.begin(0x62);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 Allocation Failed"));
    for(;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();
  
  delay(1000);
  display.clearDisplay();
  display.display();
  Serial.println("Setup Complete");

  usbMIDI.setHandleNoteOn(MidiNoteOn);
  usbMIDI.setHandleNoteOff(MidiNoteOff);
  usbMIDI.setHandleClock(MidiClock);
  usbMIDI.setHandleStart(MidiStart);
  
}

void loop() {
  if (usbMIDI.read()) {}
  for(MidiChannel m : midiChannels) 
  {
    if(m.gateMode && (m.gateTimer > m.gateMillis))
    {
      m.set_gate(0);
    }
  }
  update_ui();
}

void update_ui()
{
  if(uiTimer > UPDATE_INTERVAL) {
    // Flag handling here
    if(buttonLeftFlag)
    {

      switch(selectedLine)
      {
        default:
        {
          if(selectedChannel > 0) {
            selectedChannel -= 1;
          }
          else {
            selectedChannel = 16;
          }
          break;
        }
      }
      
    }
    if(buttonRightFlag) 
    {
      switch(selectedLine)
      {
        default:
        {
          if(selectedChannel < 16) {
            selectedChannel += 1;
          }
          else {
            selectedChannel = 0;
          }
          break;
        }
      }
    }
    if(buttonBackFlag)
    {
      if(selectedLine > 0)
      {
        selectedLine -= 1;
      }
    }
    if(buttonEditFlag)
    {
      if(selectedLine < NO_MIDI_OPTIONS)
      {
        selectedLine += 1;
      }
    }
    switch(selectedChannel)
    {
      case 0:
      {
        display.display_clock(clockMult, selectedLine);
        break;
      }
      default:
      {
        display.display_channel(midiChannels[selectedChannel - 1], selectedLine);
        break;
      }
    }
    // Reset Flags
    uiTimer = 0;
    buttonLeftFlag = 0;
    buttonRightFlag = 0;
    buttonBackFlag = 0;
    buttonEditFlag = 0;
  }
}

int note_to_dac(int note)
{
  return map(note, 60, 96, 0, THREE_V);
}

void buttonLeft_ISR()
{
  buttonLeftFlag = 1;
}
void buttonRight_ISR()
{
  buttonRightFlag = 1;
}
void buttonBack_ISR()
{
  buttonBackFlag = 1;
}
void buttonEdit_ISR()
{
  buttonEditFlag = 1;
}

void MidiNoteOn(byte channel, byte note, byte velocity)
{
  midiChannels[channel - 1].set_note(note);
  midiChannels[channel - 1].set_velocity(velocity);
  midiChannels[channel - 1].set_gate(1);

  dac.setVoltage(note_to_dac(midiChannels[channel - 1].get_output_note()), false);

  Serial.printf("Note ON, Channel: %i, Note: %i, Velocity: %i, Gate: %i \n", channel, midiChannels[channel - 1].get_output_note(), midiChannels[channel - 1].get_velocity(), midiChannels[channel - 1].get_gate());
  Serial.printf("DAC Value: %i \n", note_to_dac(midiChannels[channel - 1].get_output_note()));
  Serial.printf("Gate Pin: %i \n", midiChannels[channel-1].get_gate_pin());

}

void MidiNoteOff(byte channel, byte note, byte velocity)
{
  midiChannels[channel - 1].set_gate(0);

  Serial.printf("Note ON, Channel: %i, Note: %i, Velocity: %i, Gate: %i \n", channel, midiChannels[channel - 1].get_output_note(), midiChannels[channel - 1].get_velocity(), midiChannels[channel - 1].get_gate());
  
}

void MidiClock()
{
  clockCount++;
  //Serial.println(clockCount, DEC);
  if(clockCount == 1) {
    //Serial.println("CLK");
    digitalWrite(CLOCK_PIN, HIGH);
    delay(50);
    digitalWrite(CLOCK_PIN, LOW);
  }
  else if(clockCount >= clockMult) {
    clockCount = 0;
  }
}

void MidiStart()
{
  clockCount = 0;
  //Serial.println("START");
}


