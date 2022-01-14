#include <M5Stack.h>
//#include <esptool>

#define STRESS_LENGTH 50
#define STRESS_JUMP 25   //TODO: test
#define ZERO_RANGE_POS 50
#define ZERO_RANGE_NEG -50

int stresses[STRESS_LENGTH] = { '\0' };        //array of stress levels, to be filled out (currently a list of heartbeats)
int differences[STRESS_LENGTH-1] = { '\0' };   //change in stress levels between each element
int curr_arr = 0;                   //current array to be filled
int dS_avg = 0;                     //average change in stress
int clear_arr=0;
int Amps[5] = {10, 20, 30, 40, 50}; //frequencies and amplitudes for each region
int Freqs[5] = {20, 35, 50, 65, 80};    //global variables
int counter = 0;      //how many differences were calculated



typedef struct K_REGIONS_INFO {             //struct of information about the current region
  int amp;              //what you can use to go to the next state
  int freq;
  struct K_REGIONS_INFO *next_state;  //pointer to next state
} K_REGIONS;

  K_REGIONS K1 = {Amps[0], Freqs[0], &K1};
  K_REGIONS K2 = {Amps[0], Freqs[1], &K1};
  K_REGIONS K3 = {Amps[0], Freqs[2], &K2};  
  K_REGIONS K4 = {Amps[1], Freqs[2], &K3};  
  K_REGIONS K5 = {Amps[2], Freqs[2], &K4};
  K_REGIONS K6 = {Amps[2], Freqs[3], &K5};
  K_REGIONS K7 = {Amps[3], Freqs[3], &K6};  
  K_REGIONS K8 = {Amps[4], Freqs[3], &K7};
  K_REGIONS K9 = {Amps[4], Freqs[4], &K8};

static K_REGIONS *state_machine[] = {&K9, &K8, &K7, &K6, &K5, &K4, &K3, &K2, &K1};

K_REGIONS *curr_state = state_machine[0];

int run(K_REGIONS_INFO *curr_state, K_REGIONS_INFO *state_machine[], int dS_avg) {
  if(dS_avg >= STRESS_JUMP) {       //if jump is bigger than a threshold, go to K9
    curr_state = state_machine[0];
    return 1;
  }
  else if(dS_avg < ZERO_RANGE_POS || dS_avg > ZERO_RANGE_NEG) {  //if in between thresholds go to next state
    curr_state = curr_state -> next_state;
    return 1;
  }
  else {
    return 0;
  }  
}


void setup() {
  M5.begin();
  M5.Power.begin();           //basic setup
  Serial1.begin(9600,SERIAL_8N1, 3, 1);
  Serial2.begin(9600,SERIAL_8N1, 16, 17);
  M5.Lcd.print("Test"); 

    M5.Lcd.setTextSize(3);
  //code.work = true;
  
}


void loop() {
  if (Serial2.available()) {
    int sum = 0;
    int Cry = Serial1.read();
    int Heart = Serial2.read();
    M5.Lcd.clear(BLACK);         //basic communication
    M5.Lcd.setCursor(0,0);
    M5.Lcd.print("Recieved (C,H): \n(");
    M5.Lcd.print(Cry);
    M5.Lcd.print(",");
    M5.Lcd.print(Heart);
    M5.Lcd.println(")\n");

    stresses[curr_arr%STRESS_LENGTH] = Heart; 
    if(curr_arr != 0) {                                                                                                  //calculates the stresslevel differences
      differences[(curr_arr-1)%STRESS_LENGTH] = stresses[curr_arr%STRESS_LENGTH]-stresses[(curr_arr-1)%STRESS_LENGTH];   //adds new difference to array
    }

    for (int i = 0; i < STRESS_LENGTH; i++)     
    {
      sum += differences[i]; 
    }

    
    M5.Lcd.println("dS_avg: ");
    if(curr_arr == 0) {
        M5.Lcd.println("_NA_");
    }
    else if (curr_arr <= STRESS_LENGTH) {
      dS_avg = sum/curr_arr;     //calculate average change in stress (first parameter)
      M5.Lcd.println(dS_avg);
    }
    else if(curr_arr > STRESS_LENGTH) {
      dS_avg = sum/STRESS_LENGTH;
      M5.Lcd.println(dS_avg);
    }

    if (dS_avg != 0) {  //only run the next_state check if we have a measured change in stress
      clear_arr = run(curr_state, state_machine, dS_avg);    //main calculations
    }
    
    if (clear_arr==1) {
      for (int i = 0; i < STRESS_LENGTH; i++) //if 1 is returned from run, clear array 
      {                                       //this allows for accurate movements
        stresses[i] = 0;
      }
      
    }

    Serial1.write(curr_state->freq);
    Serial2.write(curr_state->amp);
    M5.Lcd.print("Sending (F,A):\n(");
    M5.Lcd.print(curr_state->freq);
    M5.Lcd.print(",");
    M5.Lcd.print(curr_state->amp);
    M5.Lcd.println(")");

    curr_arr++;
    
    
    delay(10); 
  }
}
