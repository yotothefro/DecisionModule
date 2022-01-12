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
int Amps[5] = {10, 20, 30, 40, 50};  // frequencies and amplitudes for each region
int Freqs[5] = {20, 35, 50, 65, 80}; // global variables
int counter = 0;                     // how many differences were calculated
int state_int = 9;                   //current state
int jump_bool = 0; //boolean to track if there are any jumps in heartrate and if those jumps are secretly errors
int panic_bool = 0;

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

void runHeart(K_REGIONS_INFO *curr_state, K_REGIONS_INFO *state_machine[])
{
  curr_state = curr_state->next_state;
  state_int = state_int - 1;
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
        h_differences[(curr_arr) % STRESS_LENGTH] = heart_rates[(curr_arr - 1) % STRESS_LENGTH] - heart_rates[(curr_arr) % STRESS_LENGTH]; // adds new difference to array
      }
      //if we measure zeros, return to next loop
      else      
      {
        curr_arr = curr_arr + 1; 
        return;
      }

      M5.Lcd.print("jump_bool: ");
      M5.Lcd.print(jump_bool);
      M5.Lcd.print("\n");

      M5.Lcd.print("(");
      for (int i = 0; i < STRESS_LENGTH;i++)
      {
        M5.Lcd.print(h_differences[i]);
        M5.Lcd.print(", ");
      }
      M5.Lcd.print("}\n");

      //stress jump check
      if (h_differences[(curr_arr) % STRESS_LENGTH] > 15 && jump_bool == 0)
      {
        if (panic_bool == 1)
        {
          curr_arr++;
          panic_bool = 0;
          return;
        }
        jump_bool = 1;
        curr_arr = curr_arr + 1;
        return;
      }

      //panic check
      if (h_differences[(curr_arr) % STRESS_LENGTH] < -15 && panic_bool == 0) 
      {
        panic_bool = 1;
        curr_arr = curr_arr + 1;
        return;
      }

      if (panic_bool ==1 )
      {
        goPanic(curr_state, state_machine);
        panic_bool = 0;
        jump_bool = 0;
        curr_arr++;
        return;
      }

      //jump_bool check
      if (jump_bool >= 1) 
      {
        if (h_differences[(curr_arr) % STRESS_LENGTH] <= 5 && h_differences[(curr_arr) % STRESS_LENGTH] >=-5) 
        {
          if (jump_bool == 3)
          {
            runHeart(curr_state, state_machine);
            jump_bool = 0;
          }
          else 
          {
            curr_arr++;
            jump_bool++;
            return; 
          }
        } 
        else
        {
          jump_bool = 0;
          curr_arr ++;
          return;
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

      delay(500);
    }

//    else // measure from crying
//    {
//      crying_amp[curr_arr] = Cry;
//      // if heart rate is greater than, go to panic state
//      if (Heart >= HEART_PANIC)
//      { 
//        goPanic(curr_state, state_machine);
//        state_int = 9; 
//      }
//
//      if (crying_amp[curr_arr] != 0 && crying_amp[curr_arr - 1] != 0)
//      {                                                                                                                                                                                                                                   //calculates the stresslevel differences
//        c_differences[(curr_arr - 1) % STRESS_LENGTH] = 
//        crying_amp[curr_arr % STRESS_LENGTH] - crying_amp[(curr_arr - 1) % STRESS_LENGTH];
//      }
//      else      
//      {
//        curr_arr = curr_arr + 1;
//        return;
//      }
//
//      M5.Lcd.println("dC_avg: ");
//
//      if (curr_arr <= STRESS_LENGTH)
//      {
//        int sum = 0;
//        for (int i = 0; i < curr_arr; i++)    
//        {
//          sum = sum + c_differences[i];
//        }
//        dC_avg = sum / curr_arr;
//        M5.Lcd.println(dC_avg);
//      }
//      else if (curr_arr > STRESS_LENGTH)
//      {
//        int sum = 0;
//        for (int i = 0; i < STRESS_LENGTH; i++)    
//        {
//          sum = sum + c_differences[i];
//        }
//        dC_avg = sum / STRESS_LENGTH;
//        M5.Lcd.println(dC_avg);
//      }
//
//      if (dC_avg != 0)
//      {                                                          // only run the next_state check if we have a measured change in stress
//        clear_arr = runHeart(curr_state, state_machine, dC_avg); // main calculations
//      }
//
//      if (clear_arr == 1)
//      {
//          // if 1 is returned from runHeart, clear array
//          // this allows for accurate movements
//        for (int i = 0; i < STRESS_LENGTH; i++) 
//        {                                       
//          crying_amp[i] = 0;
//        }
//        curr_arr = 1;
//        dC_avg = 0;
//        clear_arr = 0;
//      }
//
//      Serial1.write(curr_state->freq);
//      Serial2.write(curr_state->amp);
//      M5.Lcd.print("Sending (F,A):\n(");
//      M5.Lcd.print(curr_state->freq);
//      M5.Lcd.print(",");
//      M5.Lcd.print(curr_state->amp);
//      M5.Lcd.println(")");
//
//      curr_arr++;
//
//      delay2500);
//    }
  }
}
