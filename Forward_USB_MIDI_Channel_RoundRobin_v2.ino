/*
  Receive incoming MIDI messages from a device on the
  USB Host port and relay them to the PC via the standard
  USB port. Midi Channels are cycled round robin.
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
  digitalWrite(LED_BUILTIN, LOW);
  channelForNote[note] = midiChannels[i];
  if(i<midiChannelCount-1)i++;
  else i=0;
}

void myNoteOff(byte channel, byte note, byte velocity) {
  int noteOffChannel = channelForNote[note];
  usbMIDI.sendNoteOn(note, 0, noteOffChannel);
  digitalWrite(LED_BUILTIN, HIGH);
}

void myControlChange(byte channel, byte control, byte value) {
  usbMIDI.sendControlChange(control,value,channel);
}

void setup(void)
{
  Serial.begin(115200);
  myusb.begin();
  midi1.setHandleNoteOn(myNoteOn);
  midi1.setHandleNoteOff(myNoteOff);
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
