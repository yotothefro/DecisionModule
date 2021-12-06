#include <M5Stack.h>

typedef enum {                          //list of different states, needs a __LAST_STATE so that the loop can be terminated
  K9, K8, K7, K6, K5, K4, K3, K2, K1, 
  __LAST_STATE
} K_REGIONS;

typedef struct {             //struct of information about the current and next regions
  K_REGIONS state;
  values *tokens;              //what you can use to go to the next state
  K_REGIONS next_state;
} K_REGIONS_INFO;

struct values {
  int F, A;
  int S_opt;    //is the stress value optimal for current region
}; 
typedef struct values values;

int run(K_REGIONS start_state, K_REGIONS_INFO *state_machine, int K_curr) {
  K_REGIONS_INFO *p = state_machine;         //main loop, based on the start state, current state, and the K region (cargo)
  K_REGIONS current_state = start_state;
  K_REGIONS_INFO *found;
  int k = K_curr;

  while (k!=1) {
      found = NULL;
      p = state_machine; 
      while (p->state != __LAST_STATE) {    //check if we are in the last state (end if true)
        if (p->state == current_state) {    //check if the state is an entry for the current state
          if (p->tokens->F == && p->tokens->A == && p->tokens->S_opt == 1)    //main conditions for moving to next state
        }
      }
  }
}


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
    int sum = Cry+Heart;          //read values from previous devices


    M5.Lcd.print("\n");
    M5.Lcd.print("Sending value:\n");
    M5.Lcd.print(sum);
    Serial1.write(sum);
    delay(10);
  }
}