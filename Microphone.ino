#include <arduinoFFT.h>
#include <driver/i2s.h>
#include "AC101.h"  
#define IIC_CLK                     32
#define IIC_DATA                    33
#include <driver/i2s.h>
#define I2S_WS 26
#define I2S_SD 35
#define I2S_SCK 27
const i2s_port_t I2S_PORT = I2S_NUM_0;
static AC101 ac;

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
}
void loop() {
  int32_t sample = 0;
  int bytes = i2s_pop_sample(I2S_PORT, (char*)&sample, portMAX_DELAY);
  if(bytes > 0){
    Serial.println(sample);
  }
}

void i2s_install(){
  const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 44100,
    .bits_per_sample = i2s_bits_per_sample_t(16),
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
