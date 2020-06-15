#include <Arduino.h>
#include <WiFi.h>
#include "AudioOutputI2S.h"
#include "AC101.h"
#include "esp32_bt_music_receiver.h"
BlootoothA2DSink a2d_sink;

#define IIS_SCLK                    27
#define IIS_LCLK                    26
#define IIS_DSIN                    25

#define IIC_CLK                     32
#define IIC_DATA                    33

#define GPIO_PA_EN                  GPIO_NUM_21
#define GPIO_SEL_PA_EN              GPIO_SEL_21

#define PIN_PLAY                    (23)      // KEY 4
#define PIN_VOL_UP                  (18)      // KEY 5
#define PIN_VOL_DOWN                (5)       // KEY 6

static AC101 ac;
static uint8_t volume = 50;
const uint8_t volume_step = 2;


// Randomly picked URL
const char *URL="";


AudioOutputI2S *out;


//--------------------------------

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("Connecting to WiFi");

  Serial.printf("Connect to AC101 codec... ");
  while (not ac.begin(IIC_DATA, IIC_CLK))
  {
    Serial.printf("Failed!\n");
    delay(1000);
  }
  Serial.printf("OK\n");
    // Enable amplifier
  pinMode(GPIO_PA_EN, OUTPUT);
  digitalWrite(GPIO_PA_EN, HIGH);

  // Configure keys on ESP32 Audio Kit board
  pinMode(PIN_PLAY, INPUT_PULLUP);
  pinMode(PIN_VOL_UP, INPUT_PULLUP);
  pinMode(PIN_VOL_DOWN, INPUT_PULLUP);

  ac.SetVolumeSpeaker(volume);
  ac.SetVolumeHeadphone(volume);
   Serial.println("ac work done");
  audioLogger = &Serial;
  out = new AudioOutputI2S();
  out->SetPinout(IIS_SCLK /*bclkPin*/, IIS_LCLK /*wclkPin*/, IIS_DSIN /*doutPin*/);
  Serial.println("audio work done");
  a2d_sink.start("bbClockV2Music"); 
}

bool pressed( const int pin )
{
  if (digitalRead(pin) == LOW)
  {
    delay(500);
    return true;
  }
  return false;
}
void loop()
{
  bool updateVolume = false;
  static int lastms = 0;
  if (pressed(PIN_VOL_UP))
  {
    if (volume <= (63-volume_step))
    {
      // Increase volume
      volume += volume_step;
      updateVolume = true;
    } 
  }
  if (pressed(PIN_VOL_DOWN))
  {
    if (volume >= volume_step)
    {
      // Decrease volume
      volume -= volume_step;
      updateVolume = true;
    } 
  }
  if (updateVolume)
  {
    // Volume change requested
    Serial.printf("Volume %d\n", volume);
    ac.SetVolumeSpeaker(volume);
    ac.SetVolumeHeadphone(volume);
  }
}
