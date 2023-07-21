
#include "camera.h"
#include <SPI.h>
#include <LoRa.h>

#define SECOND 1000000
#define MINUTE 60*SECOND
#define HOUR   60*MINUTE

#define ss 15
#define rst 16
#define dio0 2

#define sck 14
#define miso 13
#define mosi 12

#define LED 4

SPIClass hspi;

void camera_take_picture(void);

void setup() {
  // init camera
  camera_config_t config = camera_config();
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    return;
  }
  // init LoRa
  hspi.begin(sck, miso, mosi, ss);
  LoRa.setSPI(hspi);
  LoRa.setPins(ss, rst, dio0);
  while (!LoRa.begin(433E6)) {
  }
  // init deepsleep
  esp_sleep_enable_timer_wakeup(2 * MINUTE);
  // take picture and send
  camera_take_picture();
  // deep sleep
  LoRa.sleep();
  esp_deep_sleep_start();
}

void loop() {
}

void camera_take_picture()
{
  String str;
  uint8_t * buff = NULL;
  uint8_t counter = 0;
  uint64_t cnt_buff = 0;
  camera_fb_t * fb = NULL;
  // Take Picture with Camera
  fb = esp_camera_fb_get();
  if (!fb) {
    return;
  }
  esp_camera_fb_return(fb);
  buff = fb -> buf;        // lấy địa chỉ vùng nhớ lưu dữ liệu ảnh
  cnt_buff = fb -> len;    // lấy dung lượng 
  //send data
  while (cnt_buff != 0)
  {
    str = "";
    for (int i = 0; i < 255; i++) 
    {
      str += (char)buff[255 * counter + i];
      cnt_buff --;        // mỗi thêm vào buffer cnt_buff giảm đi 1 đơn vị
      if (cnt_buff == 0)  // khi nào = 0 thì thoát 
      {
        i = 256;   // break
      }
    }
    // truyền 
    LoRa.beginPacket();
    LoRa.print(str);
    LoRa.endPacket();
    counter++;
    delay(200);  // đợi 200ms, dưới 200ms thì bên nhận không kịp nhận
    
    if (cnt_buff == 0)// sau khi truyền xong thì truyền chuỗi kết thúc
    {
      LoRa.beginPacket();
      LoRa.print("send successfully");
      LoRa.endPacket();
    }
  }
}
