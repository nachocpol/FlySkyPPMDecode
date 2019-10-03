/*
  ==============================================================
  Receiver PPM protocol decoder using hw interrups (for flysky).
  Made by nachocpol@gmail.com
  ==============================================================
    FlySky´s PPM frame repeats every 20ms, the frame contains 6 pulses (one for each channel)
     lasting from 0.6ms to 1.6ms. The separation between the pulses is always ~0.4ms.
     MIN padding = 20 - ((0.4 * 6) + (1.6 * 6)) = 8ms
     MAX padding = 20 - ((0.4 * 6) + (0.6 * 6)) = 14ms
*/

#define PPM_PIN 2

uint8_t CurChannel = 0;
volatile float ChannelData[6];
unsigned long PulseStart = 0;

void RXCallback()
{
  if(digitalRead(PPM_PIN) == HIGH)
  {
    PulseStart = micros();
  }
  else
  {
    unsigned long elapsed = micros() - PulseStart;
    if(elapsed > 7000) // Just finished padding pulse, get ready for channels.
    {
      CurChannel = 0;
    }
    else
    {
      ChannelData[CurChannel++] = (float)elapsed / 1000.0f;    
    }
  }
}

void setup()
{
  Serial.begin(9600);  

  // Setup the pin and the callback:
  attachInterrupt(
    digitalPinToInterrupt(PPM_PIN), 
    RXCallback,CHANGE
  );
  pinMode(PPM_PIN, INPUT);
}

void loop()
{
  // Copy the data:
  float curChannels[6];
  noInterrupts();
  memcpy(curChannels, ChannelData, sizeof(ChannelData));
  interrupts();

  // Do something with it :´)
  for(int i=0; i<6; i++)
  {
    Serial.print(curChannels[i]); 
    if(i<5)
    {
      Serial.print(",");
    }
  }
  Serial.println();
}
