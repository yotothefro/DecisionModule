#include <M5Stack.h>
//#include <esptool>

#define STRESS_LENGTH 5
#define NEXT_STATE_CHECK 15
#define HEART_PANIC 210 // TODO: TEST

int heart_rates[STRESS_LENGTH] = {'\0'}; // array of stress levels, to be filled out (currently a list of heartbeats)
int crying_amp[STRESS_LENGTH] = {'\0'};
int h_differences[STRESS_LENGTH] = {'\0'}; // change in stress levels between each element
int c_differences[STRESS_LENGTH] = {'\0'};
int curr_arr = 1;                              // current array to be filled
int dH_avg = 0;                                // average change in HR
int dC_avg = 0;                                // average change in CA
int clear_arr = 0;
int Amps[5] = {10, 20, 30, 40, 50};  // frequencies and amplitudes for each region
int Freqs[5] = {20, 35, 50, 65, 80}; // global variables
int counter = 0;                     // how many differences were calculated
int state_int = 9;                   //current state

// struct of information about the current region
// what you can use to go to the next state
typedef struct K_REGIONS_INFO
{          
  int amp;
  int freq;
  struct K_REGIONS_INFO *next_state; // pointer to next state
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

int goNext(K_REGIONS_INFO *curr_state, K_REGIONS_INFO *state_machine[], int dH_avg)
{
  // if big jump go to next state
  if (dH_avg < NEXT_STATE_CHECK)
  { 
    curr_state = curr_state->next_state;
    state_int = state_int - 1;
    return 1;
  }
  else
  {
    return 0;
  }
}
void goPanic(K_REGIONS_INFO *curr_state, K_REGIONS_INFO *state_machine[])
{
  curr_state = state_machine[0];
  state_int = 9;
}

void setup()
{
  M5.begin();
  M5.Power.begin(); 
  // basic setup
  Serial1.begin(9600, SERIAL_8N1, 3, 1);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  M5.Lcd.print("Test");
  M5.Lcd.setTextSize(3);
  // code.work = false;
}

void loop()
{
  if (Serial2.available())
  {
    if (state_int == 9) 
    {
      delay(8000);
      goNext(curr_state, state_machine);
      return;
    }
    int Cry = Serial1.read();
    int Heart = Serial2.read();
    M5.Lcd.clear(BLACK); 
    // basic communication
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.print("Received (C,H): \n(");
    M5.Lcd.print(Cry);
    M5.Lcd.print(",");
    M5.Lcd.print(Heart);
    M5.Lcd.println(")\n");

    // if greater than 5 measure from heart rate
    if (state_int >= 5) 
    {
      //only measure heartrate if it's greater than minimum
      if (Heart>=60)
      {
      heart_rates[curr_arr] = Heart;
      }
      // if heart rate is greater than, go to panic state

      if (heart_rates[curr_arr] != 0 && heart_rates[curr_arr - 1] != 0)
      {                                                                                                                                      // if neither is 0 find the difference                                                                                             //calculates the stresslevel differences
        h_differences[(curr_arr) % STRESS_LENGTH] = heart_rates[curr_arr % STRESS_LENGTH] - heart_rates[(curr_arr - 1) % STRESS_LENGTH]; // adds new difference to array
      }
      //if we measure zeros, return to next loop
      else      
      {
        curr_arr = curr_arr + 1; 
        return;
      }

      M5.Lcd.println("dH_avg: ");

      if (heart_rates[curr_arr] == 0)
      {
        M5.Lcd.println("_NA_");
      }
      else if (curr_arr <= STRESS_LENGTH)
      {
        // sum of differences
        int sum = 0;
        for (int i = 0; i < curr_arr; i++) 
        {
          sum = sum + h_differences[i];
        }
        // calculate average change in stress (first parameter)
        dH_avg = sum / curr_arr; 
        M5.Lcd.println(dH_avg);
      }
      //take average of whole array
      else if (curr_arr > STRESS_LENGTH)
      {
        int sum = 0;
        for (int i = 0; i < STRESS_LENGTH; i++)    
        {
          sum = sum + h_differences[i];
        }
        dH_avg = sum / STRESS_LENGTH;
        M5.Lcd.println(dH_avg);
      }
       // only run the next_state check if we have a measured change in stress

      if (Heart >= HEART_PANIC)
      { 
        goPanic(curr_state, state_machine);
        clear_arr = 1;
      }
      else if (dH_avg != 0)
      {                                    
        clear_arr = goNext(curr_state, state_machine, dH_avg); 
      }

      // if 1 is returned from goNext, clear everything (moved to new state so need new calculations)
      if (clear_arr == 1)
      {
        for (int i = 0; i < STRESS_LENGTH; i++) 
        {                                       
          heart_rates[i] = 0;
        }
        curr_arr = 1;
        dH_avg = 0;
        clear_arr = 0;
      }
      //printing and writing to the motor module
      Serial1.write(curr_state->freq);
      Serial2.write(curr_state->amp);
      M5.Lcd.print("Sending (F,A):\n(");
      M5.Lcd.print(curr_state->freq);
      M5.Lcd.print(",");
      M5.Lcd.print(curr_state->amp);
      M5.Lcd.println(")");

      curr_arr++;

      delay(500);
    }

    else // measure from crying
    {
      crying_amp[curr_arr] = Cry;
      // if heart rate is greater than, go to panic state
      if (Heart >= HEART_PANIC)
      { 
        goPanic(curr_state, state_machine);
        state_int = 9; 
      }

      if (crying_amp[curr_arr] != 0 && crying_amp[curr_arr - 1] != 0)
      {                                                                                                                                                                                                                                   //calculates the stresslevel differences
        c_differences[(curr_arr - 1) % STRESS_LENGTH] = crying_amp[curr_arr % STRESS_LENGTH] - crying_amp[(curr_arr - 1) % STRESS_LENGTH];
      }
      else      
      {
        return;
      }

      M5.Lcd.println("dC_avg: ");

      if (curr_arr <= STRESS_LENGTH)
      {
        int sum = 0;
        for (int i = 0; i < curr_arr; i++)    
        {
          sum = sum + c_differences[i];
        }
        dC_avg = sum / curr_arr;
        M5.Lcd.println(dC_avg);
      }
      else if (curr_arr > STRESS_LENGTH)
      {
        int sum = 0;
        for (int i = 0; i < STRESS_LENGTH; i++)    
        {
          sum = sum + c_differences[i];
        }
        dC_avg = sum / STRESS_LENGTH;
        M5.Lcd.println(dC_avg);
      }

      if (dC_avg != 0)
      {                                                          // only run the next_state check if we have a measured change in stress
        clear_arr = goNext(curr_state, state_machine, dC_avg); // main calculations
      }

      if (clear_arr == 1)
      {
        for (int i = 0; i < STRESS_LENGTH; i++) // if 1 is returned from goNext, clear array
        {                                       // this allows for accurate movements
          crying_amp[i] = 0;
        }
        curr_arr = 1;
        dC_avg = 0;
        clear_arr = 0;
      }

      Serial1.write(curr_state->freq);
      Serial2.write(curr_state->amp);
      M5.Lcd.print("Sending (F,A):\n(");
      M5.Lcd.print(curr_state->freq);
      M5.Lcd.print(",");
      M5.Lcd.print(curr_state->amp);
      M5.Lcd.println(")");

      curr_arr++;

      delay2500);
    }
  }
}
