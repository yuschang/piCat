// 2022.01.11
// Include the AccelStepper Library
#include <Stepper.h>

// define step constant
const int stepsPerRevolution = 64; // 모터별 스탭 수 설정 (28YBJ-48의 경우 64)
Stepper stepper(stepsPerRevolution, 8, 10, 9, 11);

// setup PIR sensor
int PIRled = 4;
int PIRsensor = 2;
int tempPin = 1;

// state of action
int PIR_state  = 0;
int lastPIR_state = 0;
int PIR_LED_state = 0;
int feeder_state = 0;

// cat feeding parameters
int CatFeedingDuration;
unsigned long delayStart = 0; // the time the delay statred
bool delayRunning = false; // true if still waiting for delay to finish
 
// parameters
float temp;

// Serial setup
// String sCommand="";  // Initialised to nothing.


void setup() {
  // setup PIR sensor pin
  pinMode(PIRled, OUTPUT);
  pinMode(PIRsensor, INPUT);
  
  stepper.setSpeed(500); 

  delayStart = millis();
  // delayRunning = true;

  Serial.begin(9600);
}


void feedCat(int CatFeedingDuration ){
    if(delayRunning && ((millis() - delayStart)>=CatFeedingDuration*1000) ) {
       delayRunning = false;
       feeder_state = 0;
       Serial.println("Feeding finished");
    }else{
       stepper.step(-1);  
    }
}

void loop() {

  //state management 
  if(digitalRead(PIRsensor) == HIGH) {
    PIR_state = 1;
  }else{
    PIR_state = 0;
  }
  
  if(PIR_state != lastPIR_state){
    if(PIR_state ==1){
      Serial.println("Motion detected.");
      PIR_LED_state=1;

    }else{
      Serial.println("Motion ended.");
      PIR_LED_state=0;
    }   
    lastPIR_state = PIR_state;
  }

  // serial communication 
  if(Serial.available()){

    String sCommand = Serial.readStringUntil('\n');

    //motor control command
    if(sCommand == "feedCat"){       
      Serial.print("Command ");
      Serial.print(sCommand);
      Serial.println(" Received");
            
      feeder_state = 1;  // this allows the following code to run motor
      delayStart = millis();
      CatFeedingDuration = 3;
      delayRunning = true;
    }
    //temperature read command
   /* if(sCommand == "getTMP"){
        temp = analogRead(tempPin);
        temp = (temp-400)/10;
        Serial.print("T: ");
        Serial.print(temp);
        Serial.print("*C");
        Serial.println();  
    }
   */
    
     sCommand= "";
  }
    
  // motor control
  if(feeder_state==1){
    // stepper.step(-1); 
    // Serial.println("start to feeding the cat");
    // Serial.println(delayStart);
    
    if(delayRunning && ((millis() - delayStart)>=CatFeedingDuration*1000) ) {
       delayRunning = false;
       feeder_state = 0;
       Serial.println("Feeding finished");
    }else{
       stepper.step(-1);  
    } 
  }

  // PIR LED control
  digitalWrite(PIRled, PIR_LED_state);

}
