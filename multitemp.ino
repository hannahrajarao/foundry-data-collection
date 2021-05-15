#include <Wire.h> 
#include <SPI.h>

int CSpin = 6; //clock signal
int SOpins[] = {2, 4, 9, 11, 29};
int SCKpins[] = {3, 5, 10, 12, 30};
int sensorNum = 5;
int fanPin = 28;
int temp;

/*
 * # Conversion to temperature: R_therm = 10k*exp(K*(1/T-1/T0)
        # and ADC = R/(R+10k)*(2^12-1)
        T0 = 25+273.15 # K
        K = 3950 # K
        self.data['Temp'] = 1.0/(1.0/T0+np.log(self.data['C']/((2**12-1)-self.data['C']))/K)-273.15 # oC
 */



unsigned long currentTime = 0;
unsigned long prevTime = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(CSpin, OUTPUT);
  for(int i = 0; i < sensorNum; i++) {
    pinMode(SOpins[i], INPUT);
    pinMode(SCKpins[i], OUTPUT);
  }

  pinMode(fanPin, OUTPUT);
  
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  currentTime = millis();
  
  if(currentTime-prevTime >= 1000) {
    Serial.print(currentTime/1000);
    Serial.print(", ");
    
    for(int i=0; i<sensorNum; i++) {
      temp = readThermocouple(i);
      Serial.print(temp);
      Serial.print(", ");
    }
    Serial.println();
    prevTime = currentTime;
  }

  analogWrite(fanPin, 1*255);
}

//double ambientTemp() {
//  return analogRead(16);
//}

double readThermocouple(int i) {

  uint16_t v;
  
  digitalWrite(CSpin, LOW);
  delay(1);

  // Read in 16 bits,
  //  15    = 0 always
  //  14..2 = 0.25 degree counts MSB First
  //  2     = 1 if thermocouple is open circuit  
  //  1..0  = uninteresting status
  
  v = shiftIn(SOpins[i], SCKpins[i], MSBFIRST);
  v <<= 8;
  v |= shiftIn(SOpins[i], SCKpins[i], MSBFIRST);
  
  digitalWrite(CSpin, HIGH);
  if (v & 0x4) 
  {    
    // Bit 2 indicates if the thermocouple is disconnected
    return NAN;     
  }

  // The lower three bits (0,1,2) are discarded status bits
  v >>= 3;

  // The remaining bits are the number of 0.25 degree (C) counts
  return v*0.25;
}
