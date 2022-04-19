// Flipo Project - https://Flipo.io
// https://flipo.io/project/pulse-current-power-supply/
// https://github.com/marcinsaj/Flipo-Pulse-Current-Power-Supply
//
// Basic example of Flipo#3 module control - flip on/off 3 discs
//
// Hardware:
// Pulse Current Power Supply Module for flip discs - https://flipo.io/project/pulse-current-power-supply/
// Flipo#3 Module for 3 flip discs - https://flipo.io/project/controller-flip-disc-3/
// Arduino UNO


// Declaration of the controller control inputs
#define DIN_PIN     12    // A - pcb markings
#define EN_PIN      11    // B
#define CLK_PIN     10    // C

// Declaration of the PCPS module control input/output
#define EN_VF       A0    // Turn ON/OFF charging PCPS module
#define FB_VF       A1    // Measurement output for checking if the current pulse is ready

int value;

// Bit notation for Flipo#3 controller - set flip-discs
// Always active only two bits corresponding to control outputs 
// Check controller documentation and schematic
uint8_t setDisc[]=
{
  0b10000010,
  0b10010000,
  0b10100000  
};

// Bit notation for Flipo#3 controller - reset flip-discs
uint8_t resetDisc[]=
{
  0b00000101,
  0b01000100,
  0b00001100
};

void setup() 
{
  pinMode(DIN_PIN, OUTPUT);
  digitalWrite(DIN_PIN, LOW);
 
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);
 
  pinMode(CLK_PIN, OUTPUT);
  digitalWrite(CLK_PIN, LOW);

  pinMode(EN_VF, OUTPUT);
  digitalWrite(EN_VF, LOW); 

  ShiftOutData(0);            // Clear Flipo #3 Controller

  PrepareCurrentPulse();      // Prepare current pulse
}

void loop()
{
  // Reset all 3 flip discs
  for(int i = 0; i < 3; i++)
  {
    ResetDisc(i);
    delay(500); 
  } 

  // Set all 3 flip discs
  for(int i = 0; i < 3; i++)
  {
    SetDisc(i);
    delay(500);   
  }
}

// First charging - setup call
void PrepareCurrentPulse(void)
{
  CurrentPulse();
}

void CurrentPulse()
{
  digitalWrite(EN_VF, HIGH);            // Turn ON PCPS module- charging begin

  do {value = analogRead(FB_VF);}       // Measure the voltage of the accumulated charge
  while (value < 500);                  // ~2.5V this voltage means that the current pulse is ready
 
  digitalWrite(EN_VF, LOW);             // Turn ON PCPS module- charging complete 
}

void SetDisc(uint8_t discNumber)
{
  CurrentPulse();                       // Prepare current pulse - charging begin
  ShiftOutData(setDisc[discNumber]);    // Turn on flip-disc controller corrsponding outputs
  delay(1);                             // Flip-disc required 1ms current pulse to flip

  ShiftOutData(0);                      // Absolutely required!
                                        // This function here turns off the current pulse 
                                        // and cleans the controller outputs
}

void ResetDisc(uint8_t discNumber)
{
  CurrentPulse();                       // Prepare current pulse - charging begin
  ShiftOutData(resetDisc[discNumber]);  // Turn on flip-disc controller corrsponding outputs
  delay(1);                             // Flip-disc required 1ms current pulse to flip

  ShiftOutData(0);                      // Absolutely required!
                                        // This function here turns off the current pulse 
                                        // and cleans the controller outputs
}

void ShiftOutData(uint8_t discNumber)
{
  digitalWrite(EN_PIN, LOW);            // Transfer data begin
  shiftOut(DIN_PIN, CLK_PIN, MSBFIRST, discNumber);   // Send data to the controller
  digitalWrite(EN_PIN, HIGH);           // Transfer data complete
}
