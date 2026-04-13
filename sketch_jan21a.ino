#include <esp_adc/adc_oneshot.h>
#include <driver/i2s_std.h>
#include <math.h>

#define MQ2_PIN   ADC_CHANNEL_6   // GPIO34
#define I2S_WS    25
#define I2S_SD    33
#define I2S_SCK   26
#define SAMPLES   256

adc_oneshot_unit_handle_t adc_handle;
i2s_chan_handle_t rx_handle;

void setup() {
  Serial.begin(115200);

  adc_oneshot_unit_init_cfg_t init_cfg = { .unit_id = ADC_UNIT_1 };
  adc_oneshot_new_unit(&init_cfg, &adc_handle);

  adc_oneshot_chan_cfg_t chan_cfg = {
    .atten    = ADC_ATTEN_DB_11,
    .bitwidth = ADC_BITWIDTH_12,
  };
  adc_oneshot_config_channel(adc_handle, MQ2_PIN, &chan_cfg);

  i2s_chan_config_t chan_cfg_i2s = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
  i2s_new_channel(&chan_cfg_i2s, NULL, &rx_handle);

  i2s_std_config_t std_cfg = {
    .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(16000),
    .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO),
    .gpio_cfg = {
      .mclk = I2S_GPIO_UNUSED,
      .bclk = (gpio_num_t)I2S_SCK,
      .ws   = (gpio_num_t)I2S_WS,
      .dout = I2S_GPIO_UNUSED,
      .din  = (gpio_num_t)I2S_SD,
    },
  };
  i2s_channel_init_std_mode(rx_handle, &std_cfg);
  i2s_channel_enable(rx_handle);
}

void loop() {
  int gasValue = 0;
  adc_oneshot_read(adc_handle, MQ2_PIN, &gasValue);
  float air = constrain(map(gasValue, 150, 3000, 0, 3000), 0, 3000);

  int32_t samples[SAMPLES];
  size_t bytes_read;
  i2s_channel_read(rx_handle, samples, sizeof(samples), &bytes_read, portMAX_DELAY);

  double sum = 0;
  int count = bytes_read / sizeof(int32_t);
  for (int i = 0; i < count; i++) {
    double s = samples[i] / (double)INT32_MAX;
    sum += s * s;
  }
  double rms = sqrt(sum / count);

  float sound = 0;
  if (rms > 0) {
    sound = 20.0 * log10(rms) + 120.0;
  }
  sound = constrain(sound, 0, 120);

  Serial.print("air:");
  Serial.println(air);
  Serial.print("sound:");
  Serial.println(sound);

  delay(1000);
}
