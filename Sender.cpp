#include "mbed.h"
 
// Code from TNode/TGateway @ Anarduino.com
// NOTE: code sample was inspired from ideas gained from: https://github.com/aanon4/RFM69
//
// Sender - periodically send time(millis), radio temperature, RSSI to listener...
// 2014 - anarduino.com
//
#include <RFM69.h>

#define GATEWAY_ID    1
#define NODE_ID       9    // node ID
#define NETWORKID     101    //the same on all nodes that talk to each other
#define MSG_INTERVAL  100

// Uncomment only one of the following three to match radio frequency
//#define FREQUENCY     RF69_433MHZ    
//#define FREQUENCY     RF69_868MHZ
#define FREQUENCY     RF69_915MHZ

#define IS_RFM69HW   //NOTE: uncomment this ONLY for RFM69HW or RFM69HCW
#define ENCRYPT_KEY    "EncryptKey123456"  // use same 16byte encryption key for all devices on net
#define ACK_TIME       50                  // max msec for ACK wait
#define LED            9                   // Anardino miniWireless has LEDs on D9
#define SERIAL_BAUD    115200
#define VERSION  "1.0"

#define MSGBUFSIZE 64   // message buffersize, but for this demo we only use: 
                        // 1-byte NODEID + 4-bytes for time + 1-byte for temp in C + 2-bytes for vcc(mV)
char msgBuf[MSGBUFSIZE];

#ifdef TARGET_NUCLEO_F401RE
Serial pc(USBTX, USBRX);
DigitalOut myled(D9);             //"Moteino Half Shield" has LED on D9
//RFM69::RFM69(PinName  PinName mosi, PinName miso, PinName sclk,slaveSelectPin, PinName int)
RFM69 radio(D11,D12,D13,D10,D8);
#elif defined(TARGET_NUCLEO_L152RE)
Serial pc(USBTX, USBRX);
DigitalOut myled(D9);             //"Moteino Half Shield" has LED on D9
//RFM69::RFM69(PinName  PinName mosi, PinName miso, PinName sclk,slaveSelectPin, PinName int)
RFM69 radio(D11,D12,D13,D10,D8);
#elif  defined(TARGET_LPC1114)
Serial pc(USBTX, USBRX);
DigitalOut myled(LED1);
//RFM69::RFM69(PinName  PinName mosi, PinName miso, PinName sclk,slaveSelectPin, PinName int)
RFM69 radio(dp2,dp1,dp6,dp4,dp9);
#elif  defined(TARGET_LPC1768)
Serial pc(USBTX, USBRX);
DigitalOut myled(D9);             //"Moteino Half Shield" has LED on D9
//RFM69::RFM69(PinName  PinName mosi, PinName miso, PinName sclk,slaveSelectPin, PinName int)
RFM69 radio(D11,D12,D13,D10,D8);    // Seeedstudio Arch Pro
//RFM69 radio(p5, p6, p7, p10, p9,p8)  // Mbed 1768 ?
#endif

bool promiscuousMode = false; // set 'true' to sniff all packets on the same network
bool requestACK=false;
Timer tmr;

main() {
  memset(msgBuf,0,sizeof(msgBuf));
  int i=1;
  long l;
  tmr.start();

  pc.baud(SERIAL_BAUD);
  pc.printf("Sender %s startup at %d Mhz...\r\n",VERSION,(FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915));
  wait(2);
  radio.initialize(FREQUENCY, NODE_ID, NETWORKID);
  radio.encrypt(0);
  radio.setPowerLevel(20);
  radio.promiscuous(promiscuousMode);
#ifdef IS_RFM69HW
  radio.setHighPower(); //uncomment #define ONLY if radio is of type: RFM69HW or RFM69HCW 
#endif
  msgBuf[0] = (uint8_t)NODE_ID;  // load NODEID

while(1) {
  uint8_t tempC =  radio.readTemperature(-1); // -1 = user cal factor, adjust for correct ambient
  uint8_t tempF = 1.8 * tempC + 32; // 9/5=1.8
 
  l = tmr.read_ms();  // load time

  sprintf((char*)msgBuf,"#%d, t=%Lu, temp=%dF, RSSI=%d ",i,l,tempF,radio.RSSI); 
   if(radio.sendWithRetry((uint8_t)GATEWAY_ID, msgBuf,strlen(msgBuf),true))
            pc.printf("Packet %d sent, Ack ok!\r\n",i++);
       else pc.printf("Packet %d sent, no Ack!\r\n",i++);
  wait_ms(MSG_INTERVAL);
  myled = !myled;
  }
}
