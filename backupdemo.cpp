#include <M5stack.h>


int Amps[5] = {10, 20, 30, 40, 50}; //frequencies and amplitudes for each region
int Freqs[5] = {20, 35, 50, 65, 80};    //global variables



typedef struct K_REGIONS_INFO {             //struct of information about the current region
  int freq;              //what you can use to go to the next state
  int amp;
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

void setup () {
    M5.begin();
    M5.Power.begin();
    Serial1.begin(9600,SERIAL_8N1, 3, 1);
    Serial2.begin(9600,SERIAL_8N1, 16, 17);    

}

loop () {
    
}