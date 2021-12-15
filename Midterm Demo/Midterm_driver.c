#include <M5Stack.h>    //frequency 1 kHz, 0.8 A current, 3V

#define RXD1 16
#define TXD1 1
#define RXD2 17
#define TXD2 3


int incomingByte = 0;
int freq_in = 0;
int amp_in = 0;
int freq_duty = 0;
int amp_duty = 0;

int freq = 1000;
int ledChannel1 = 0;
int resolution = 8;
int pin = 2;
int max_duty = 255;
int max_freq = 80;
int max_amp = 50;

void setup(){
  M5.begin();
  Serial1.begin(9600, SERIAL_8N1, RXD1, TXD1);    //setup both serial ports for reading
  Serial2.begin(9600, SERIAL_8N1, RXD1, TXD2);

  ledcSetup( ledChannel1, freq, resolution_bits)
  ledcAttachPin(pin, chan);    //TODO: Specify pins 
  }


void loop() {
  while (Serial1.available()) {
    freq_in = int(Serial1.read());
    M5.Lcd.print("Sending Frequency: ");    //Read from Crying submodule an integer
    M5.Lcd.print(freq_in);
    freq_duty = (freq_in/max_freq)*max_duty;    //duty cycle is out of 255, fraction of max frequency, same for amp
    ledcWrite(ledChannel1, freq_duty);
    
  }
  while (Serial2.available()) {
    M5.Lcd.print("Sending Amplitude: ");    //Read from Crying submodule an integer
    M5.Lcd.print(int(Serial2.read()));
    amp_in = int(Serial1.read());
    amp_duty = (amp_in/max_amp)*max_duty;
    ledcWrite(ledChannel1, amp_duty);
  }  
  delay(10);
  
  M5.Lcd.clear(BLACK);  //reset screen
  M5.Lcd.setCursor(0,0);
}