#include <M5Stack.h>

struct regions {
    int K;
    int A;
    int F;
    int s_opt;
    struct regions *next;
}
typedef struct regions regions;

void setup() {
  M5.begin();
  M5.Power.begin();
  Serial1.begin(9600,SERIAL_8N1, 3, 1);
  Serial2.begin(9600,SERIAL_8N1, 16, 17);
  M5.Lcd.print("Test");

  k_init = 9;
}

void loop() {
   while (Serial1.available()&&Serial2.available()) {
    int Cry = Serial1.read();
    int Heart = Serial2.read();
    M5.Lcd.clear(BLACK);
    M5.Lcd.setCursor(0,0);
    M5.Lcd.print("Recieved: \n");
    M5.Lcd.print(Cry);
    M5.Lcd.print("\n");
    M5.Lcd.print(Heart);
    int sum = Cry+Heart;
    M5.Lcd.print("\n");
    M5.Lcd.print("Sending value:\n");
    M5.Lcd.print(sum);
    Serial1.write(sum);
    delay(10);
  }
}