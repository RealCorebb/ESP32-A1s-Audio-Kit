#include <arduinoFFT.h>
#include <driver/i2s.h>
#include "AC101.h"  
#define IIC_CLK                     32
#define IIC_DATA                    33
#include <driver/i2s.h>
#define I2S_WS 26
#define I2S_SD 35
#define I2S_SCK 27
#include <arduinoFFT.h>
arduinoFFT FFT = arduinoFFT(); /* Create FFT object */
const i2s_port_t I2S_PORT = I2S_NUM_0;
static AC101 ac;

const int BLOCK_SIZE = 1024;
const double samplingFrequency = 44100;
double vReal[BLOCK_SIZE];
double vImag[BLOCK_SIZE];
const int i2s_num = 0;
int retStat = 0;
int32_t sampleIn = 0;

void setup() {
    Serial.begin(115200);
  Serial.println("Configuring I2S...");
  esp_err_t err;
    Serial.printf("Connect to AC101 codec... ");
  while (not ac.begin(IIC_DATA, IIC_CLK))
  {
    Serial.printf("Failed!\n");
    delay(1000);
  }
  Serial.println("Setup I2S ...");
  delay(1000);
  i2s_install();
  i2s_setpin();
  i2s_start(I2S_PORT);
    //This pulls in a bunch of samples and does nothing, its just used to settle the mics output
  for (retStat = 0; retStat < BLOCK_SIZE * 2; retStat++)
  {
    i2s_pop_sample((i2s_port_t)i2s_num, (char*)&sampleIn, portMAX_DELAY);
    delay(1);
  }
}
void loop() {
  sampleIn = 0;
  for (uint16_t i = 0; i < BLOCK_SIZE; i++)
  {
    //this reads 32bits as 4 chars into a 32bit INT variable
    i2s_pop_sample((i2s_port_t)i2s_num, (char*)&sampleIn, portMAX_DELAY);
    //this pushes out all the unwanted bits as we only need right channel data.
    sampleIn >>= 14;
    vReal[i] = sampleIn;
    vImag[i] = 0.0; //Imaginary part must be zeroed in case of looping to avoid wrong calculations and overflows
  }
  FFT.Windowing(vReal, BLOCK_SIZE, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, BLOCK_SIZE, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, BLOCK_SIZE);
  double x;
  double v;
  FFT.MajorPeak(vReal, BLOCK_SIZE, samplingFrequency, &x, &v);
  Serial.print(x, 6);
  Serial.print(", ");
  Serial.println(v, 6);
}

void i2s_install(){
  const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 44100,
    .bits_per_sample = i2s_bits_per_sample_t(32),
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = 0, // default interrupt priority
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}

void i2s_setpin(){
  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };

  i2s_set_pin(I2S_PORT, &pin_config);
}
