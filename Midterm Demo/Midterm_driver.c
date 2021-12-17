#include <M5Stack.h>    //frequency 1 kHz, 0.8 A current, 3V

int incomingByte = 0;
int freq_in = 0;
int amp_in = 0;
int freq_duty = 0;
int amp_duty = 0;

int freq = 1000;
int ledChannel1 = 0;
int ledChannel2 = 1;
int resolution = 8;
int pin1 = 2;
int pin2 = 5;
int max_duty = 255;
double max_freq = 80.0;
double max_amp = 50.0;


void setup(){
  M5.begin();
  Serial1.begin(9600, SERIAL_8N1, 16, 17);    //setup both serial ports for reading
  Serial2.begin(9600, SERIAL_8N1, 3, 1);

  M5.Lcd.setTextSize(3);

  M5.Lcd.print("Setup...\n");
  ledcSetup(ledChannel1, freq, resolution);
  ledcSetup(ledChannel2, freq, resolution);
  ledcAttachPin(pin1, ledChannel1);    //frequency PIN 2
  ledcAttachPin(pin2, ledChannel2);    //amplitude PIN 5
  }


void loop() {
  M5.Lcd.clear(BLACK);  //reset screen
  M5.Lcd.setCursor(0,0);
  if (Serial1.available()) {
    freq_in = int(Serial1.read());
    M5.Lcd.print("Sending Frequency: ");
    M5.Lcd.println(freq_in);
    

    switch (freq_in){     //INVERTED OUTPUT, when duty cycle is 40%, we need 60%
      case 20:
        ledcWrite(ledChannel1, 255);      //100%
        M5.Lcd.print("Duty cycle: ");
        M5.Lcd.println(255);
        break;
      case 35:
        ledcWrite(ledChannel1, 204);
        M5.Lcd.print("Duty cycle: ");    
        M5.Lcd.println(204);
        break;
      case 50:
        ledcWrite(ledChannel1, 153);
        M5.Lcd.print("Duty cycle: ");    
        M5.Lcd.println(153);
        break;
      case 65:
        ledcWrite(ledChannel1, 102);
        M5.Lcd.print("Duty cycle: ");    
        M5.Lcd.println(102);
        break;
      case 80:
        ledcWrite(ledChannel1, 51);
        M5.Lcd.print("Duty cycle: ");
        M5.Lcd.println(51);
        break;
      default:
        M5.Lcd.print("No value sent\n");
        delay(500);
    }   
  }
  else {
    M5.Lcd.print("Serial1 unavailable\n");
  }
  if (Serial2.available()) {
    amp_in = int(Serial2.read());
    M5.Lcd.print("Sending Amplitude: ");    //Read from Crying submodule an integer
    M5.Lcd.println(amp_in);
   
    
    switch (amp_in){
      case 10:
        ledcWrite(ledChannel2, 255);
        M5.Lcd.print("Duty cycle: ");    //Read from Crying submodule an integer
        M5.Lcd.println(255);
        break;
      case 20:
        ledcWrite(ledChannel2, 204);
        M5.Lcd.print("Duty cycle: ");    //Read from Crying submodule an integer
        M5.Lcd.println(204);
        break;
      case 30:
        ledcWrite(ledChannel2, 153);
        M5.Lcd.print("Duty cycle: ");    //Read from Crying submodule an integer
        M5.Lcd.println(153);
        break;
      case 40:
        ledcWrite(ledChannel2, 102);
        M5.Lcd.print("Duty cycle: ");    //Read from Crying submodule an integer
        M5.Lcd.println(102);
        break;
      case 50:
        ledcWrite(ledChannel2, 51);
        M5.Lcd.print("Duty cycle: ");    //Read from Crying submodule an integer
        M5.Lcd.println(51);
        break;
      default:
        M5.Lcd.print("No value sent\n");
        delay(500);
    }  
  }
  else {
      M5.Lcd.print("Serial2 unavailable\n");
    }
}