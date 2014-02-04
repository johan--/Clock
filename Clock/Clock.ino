
#define HOURSPIN 0
#define MINUTESPIN 1
#define SWITCHPIN 1  // read analog port 1, which on the digispark is P2
#define QUAD_A 4  //interupt pin on pin4
#define QUAD_B 5  //interupt pin on pin5

#define SF_SECS .2553
#define SF_TENSECS .0255

#define SF_HOURS 43200000L
#define SF_MINS 3600000L

#define BAND1 85
#define BAND2 256
#define BAND3 426
#define BAND4 597
#define BAND5 768
#define BAND6 938

//analog port 0 = pin5
//analog port 1 = pin2
//analog port 2 = pin4
//analog port 3 = pin3


//<<<   inputs   >>>
int valSwitch=0;

//<<<   outputs   >>>
int hValDisplay=0;
int mValDisplay=0;

//<<<    internal   >>>
long milliStart;
long time = 0;
int maxHRange = 240;
int maxMRange = 240;

volatile long enc_count = 0;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
 // pinMode(HOURSPIN, OUTPUT); //LED on Model B
 // pinMode(minutesPin, OUTPUT); //LED on Model A
 
    attachInterrupt(QUAD_A,encoder_isr,CHANGE);
    attachInterrupt(QUAD_B,encoder_isr,CHANGE);
}

// the loop routine runs over and over again forever:
void loop() {
  
//  valKnob = analogRead(KNOBPIN); //read is 0-1023, while write is 0-255
  valSwitch = analogRead(SWITCHPIN);
  
  if (valSwitch > BAND6){ // this will eventually be a series of if statements for each Switch State
    // this case is the "test mode" which should scroll through the display very quickly.
    
      milliStart = millis();
      hValDisplay = int(float(milliStart % 10000)/10000. * float(maxHRange) );
      mValDisplay = int(float(milliStart % 1000)/1000. * float(maxMRange) );
       
  }
  else if (BAND5 < valSwitch && valSwitch <= BAND6){
    // this mode is to calibate the maximum hours output
    if (enc_count != 0){
      maxHRange = maxHRange + enc_count;
      enc_count=0;
    }
    
    hValDisplay = maxHRange;
    mValDisplay = 0; 
    delay(100);   
  }
  else if (BAND4 < valSwitch && valSwitch <= BAND5){
    // this mode is to calibate the maximum minutes output
     
    if (enc_count != 0){
      maxMRange = maxMRange + enc_count;
      enc_count=0;
    }

    hValDisplay = 0;
    mValDisplay = maxMRange;
    delay(100);   
  }
  else if (valSwitch <= BAND4){
    if (enc_count != 0){
      time = time + 60000 * enc_count;
      enc_count=0;
    }    
    
    delay(100);   

    milliStart = millis() + time;
    
    hValDisplay = int(float(milliStart % SF_HOURS)/43200000. * float(maxHRange) );
    mValDisplay = int(float(milliStart % SF_MINS)/3600000. * float(maxMRange) );
  }
  else{
    hValDisplay = 0;
    mValDisplay = 0; 
  }
    
    
    
  analogWrite(HOURSPIN, hValDisplay);
  analogWrite(MINUTESPIN, mValDisplay);
 
}


void encoder_isr() {
    static int8_t lookup_table[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
    static uint8_t enc_val = 0;
    
    enc_val = enc_val << 1;
    enc_val = enc_val + digitalRead(QUAD_A); 
    enc_val = enc_val << 1;
    enc_val = enc_val + digitalRead(QUAD_B);

    
//    enc_val = enc_val | ((PIND & 0b1100) >> 2) leftover from makeatronics.blogspot.com
 
    enc_count = enc_count + lookup_table[enc_val & 0b1111];
}

