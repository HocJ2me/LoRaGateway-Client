
#include "LoRa.h"
#include <fstream>

const long frequency = 433E6;  // LoRa Frequency

const int csPin = 10;   //   7    // LoRa radio chip select
const int resetPin = 3; //   22   // LoRa radio reset
const int irqPin = 22;  //   6    // change for your board; must be a hardware interrupt pin

string buff_img = "";
fstream file;
string url_picture = "cp /home/pi/LoRaGateway/test.jpg /home/pi/FTP/files";
string rename_picture = "mv /home/pi/folder/old_name.jpg /home/pi/folder/new_name.jpg";

char buff[4];

int picture_number = 1;
int count = 1;
unsigned long long cnt_buff = 0;

void LoRa_rxMode(){
  LoRa.disableInvertIQ();               // normal mode
  LoRa.receive();                       // set receive mode
}

void LoRa_txMode(){
  LoRa.idle();                          // set standby mode
  LoRa.enableInvertIQ();                // active invert I and Q signals
}

void LoRa_sendMessage(String message) {
  LoRa_txMode();                        // set tx mode
  LoRa.beginPacket();                   // start packet
  LoRa.print(message);                  // add payload
  LoRa.endPacket(true);                 // finish packet and send it
}

void onReceive(int packetSize) {
  string message = "";
  
  while (LoRa.available()) {
    message += (char)LoRa.read();   // doc data truyen ve 
  }
  cout << message << endl;          // in ra terminal 
  if(message == "send successfully") // check chuoi ket thuc
  {
      file.open("test.jpg");
      file << buff_img;            // luu file test.jpg o thu muc loraGateway
      file.close();
      sprintf(buff, "%d", picture_number);
      string str(buff);
      string rename_picture = "mv /home/pi/FTP/files/test.jpg //home/pi/FTP/files/test" + str + ".jpg";
      system(url_picture.c_str());      // di chuyen file test den folder FTP
      system(rename_picture.c_str());   //  doi ten file test trong folder FTP
      cout << "save file test";
      cout << picture_number << endl;
      picture_number ++;
      buff_img = "";
  }
  else
  {
      buff_img += message;         // luu data vao buffer
  }
}

void onTxDone() {
  printf("TxDone\n");
  LoRa_rxMode();
}


bool runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}

void setup() {

  LoRa.setPins(csPin, resetPin, irqPin);

  if (!LoRa.begin(frequency)) {
     printf("LoRa init failed. Check your connections.\n");
    while (true);                       // if failed, do nothing
  }

  printf("LoRa init succeeded.\n\n");
  printf("LoRa Simple Gateway\n");
  printf("Only receive messages from nodes\n");
  printf("Tx: invertIQ enable\n");
  printf("Rx: invertIQ disable\n\n");
  
  LoRa.onReceive(onReceive);
  LoRa.onTxDone(onTxDone);
  LoRa_rxMode();
}


void loop() {
  if (runEvery(5000)) { // repeat every 5000 millis

    String message = "HeLoRa World! ";
    message += "I'm a Gateway! ";
    message += millis();
    cout << "Message to send : " << message << endl;

    LoRa_sendMessage(message); // send a message

    printf("Send Message!\n");
  }
}


int  main(void) {
   setup();
   while(1)
   {
   }
}
