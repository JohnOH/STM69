// Listener - print sender messages
// From Tnode/Tsender @ anarduino.com
// 2014 - anarduino.com
//
#include <RFM69.h>
#include <SPI.h>

#define GATEWAY_ID    1     // this is ME, TGateway
#define NETWORKID     101   //the same on all nodes that talk to each other

// Uncomment only one of the following three to match radio frequency
//#define FREQUENCY     RF69_433MHZ    
//#define FREQUENCY     RF69_868MHZ
#define FREQUENCY     RF69_915MHZ

//#define IS_RFM69HW   //NOTE: uncomment this ONLY for RFM69HW or RFM69HCW
#define ENCRYPT_KEY    "EncryptKey123456"  // use same 16byte encryption key for all devices on net
#define ACK_TIME       50                  // max msec for ACK wait
#define LED            9                   // Anardino miniWireless has LEDs on D9
#define SERIAL_BAUD    115200
#define VERSION  "1.0"

#define MSGBUFSIZE 64   // message buffersize, but for this demo we only use: 
                        // 1-byte NODEID + 4-bytes for time + 1-byte for temp in C + 2-bytes for vcc(mV)
uint8_t msgBuf[MSGBUFSIZE];
InterruptIn mybutton(USER_BUTTON);

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
  uint8_t theNodeID;
  tmr.start();

   pc.baud(SERIAL_BAUD);
  pc.printf("\r\nListener %s startup at %d Mhz...\r\n",VERSION,(FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915));
  wait(1);
  radio.initialize(FREQUENCY, GATEWAY_ID, NETWORKID);
  radio.encrypt(0);
  radio.promiscuous(promiscuousMode);
//  radio.setPowerLevel(5);
#ifdef IS_RFM69HW
  radio.setHighPower(); //uncomment #define ONLY if radio is of type: RFM69HW or RFM69HCW 
#endif

while(1) {
  if (radio.receiveDone()) {
     pc.printf("Received from TNODE: %d ",radio.SENDERID);
     pc.printf((char*)radio.DATA);
     if (radio.ACKRequested()){
        theNodeID = radio.SENDERID;
        radio.sendACK();
        pc.printf(" - ACK sent. Receive RSSI: %d\r\n",radio.RSSI);
     } else pc.printf("Receive RSSI: %d\r\n",radio.RSSI);
  }
         myled = !myled; 
 }
}