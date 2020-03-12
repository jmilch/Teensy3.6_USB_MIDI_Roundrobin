/*
  Receive incoming MIDI messages from a device on the
  USB Host port and relay them to the PC via the standard
  USB port and if connected a Hardware Serial MIDI Port. Midi Channels are cycled round robin.
  On Windows, use MIDI-Ox to monitor the received messages.
  Set up MIDI-Ox using Options|MIDI Devices and make sure
  that "Teensy MIDI" appears as a device in MIDI Inputs.
  
>>To compile this, you must select Serial + MIDI from the "Tools|USB Type" menu
   
   This example code is in the public domain.

   PJRC USB host cable for T3.6 is available at:
   https://www.pjrc.com/store/cable_usb_host_t36.html
*/

// Add USB Host MIDI to handle the 61es keyboard
#include <USBHost_t36.h>
#include <MIDI.h>
//Set up MIDI through Serial Port via Pins
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

//Set up USB Midi
USBHost myusb;
MIDIDevice midi1(myusb);
//Define Midi Channels here
int midiChannels[] = {3, 4, 5, 6};
//Define number of Midi Channels
const int midiChannelCount = 4;
//Set up variables for notes played
byte channelForNote[128];
int i = 0;

// Each of these callback functions handles a
// specific type of MIDI message from the
// USB Host interface and then passes it on to
// the PC thru the normal USB interface.
// You can change what is sent to the PC.
// For example, No matter what channel is used
// by the 61-es keyboard, I send it to the
// PC on channel 2 with:
// usbMIDI.sendNoteOn(note, velocity, 2);
void myNoteOn(byte channel, byte note, byte velocity) {
  usbMIDI.sendNoteOn(note, velocity, midiChannels[i]);
  MIDI.sendNoteOn(note, velocity, midiChannels[i]);
  digitalWrite(LED_BUILTIN, LOW);
  channelForNote[note] = midiChannels[i];
  if(i<midiChannelCount-1)i++;
  else i=0;
}

void myNoteOff(byte channel, byte note, byte velocity) {
  int noteOffChannel = channelForNote[note];
  usbMIDI.sendNoteOn(note, 0, noteOffChannel);
  MIDI.sendNoteOn(note, 0, noteOffChannel);
  digitalWrite(LED_BUILTIN, HIGH);
}

void myAfterTouchPoly(byte channel, byte note, byte pressure) {
  int noteOffChannel = channelForNote[note];
  usbMIDI.sendAfterTouchPoly(note, pressure, noteOffChannel);
//  MIDI.sendPolyPressure(note, pressure, noteOffChannel);
}

void myControlChange(byte channel, byte control, byte value) {
  for(int j = 0; j<midiChannelCount-1; j++) {
    usbMIDI.sendControlChange(control,value,midiChannels[j]);
    MIDI.sendControlChange(control,value,midiChannels[j]);
  }
}

void myProgramChange(byte channel, byte program) {
  for(int j = 0; j<midiChannelCount-1; j++) {
    usbMIDI.sendProgramChange(program, midiChannels[j]);
    MIDI.sendProgramChange(program, midiChannels[j]);
  }
}

void myAfterTouch(byte channel, byte pressure) {
  for(int j = 0; j<midiChannelCount-1; j++) {
    usbMIDI.sendAfterTouch(pressure, midiChannels[j]);
    MIDI.sendAfterTouch(pressure, midiChannels[j]); 
  }
 }

void myPitchChange(byte channel, int value){
  for(int j = 0; j<midiChannelCount-1; j++) {
    usbMIDI.sendPitchBend(value, midiChannels[j]);
    MIDI.sendPitchBend(value, midiChannels[j]);
  }
}

void setup(void)
{
  Serial.begin(115200);
  myusb.begin();
  MIDI.begin();
  midi1.setHandleNoteOn(myNoteOn);
  midi1.setHandleNoteOff(myNoteOff);
  midi1.setHandleAfterTouchPoly(myAfterTouchPoly);
  midi1.setHandleControlChange(myControlChange);
  midi1.setHandleProgramChange(myProgramChange);
  midi1.setHandleAfterTouch(myAfterTouch);
  midi1.setHandlePitchChange(myPitchChange);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  usbMIDI.begin();
  // Testing. Can be removed. If you are
  // monitoring with MIDI-Ox you should see
  // these 5 notes sent to the PC
  Serial.println("Send midi notes to the PC");
/*  for (int i = 0; i < 5; i++) {
    usbMIDI.sendNoteOn(49, 100, 1);
    delay(500);
    usbMIDI.sendNoteOff(49, 0, 1);
    delay(100);
  } */
  Serial.println("Start");
}

void loop(void)
{
  myusb.Task();
  midi1.read();
}
