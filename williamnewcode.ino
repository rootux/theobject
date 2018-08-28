#define M1F 7 //FORWARD   MOTOR 1
#define M1B 8 //BARCKWARD MOTOR 1
#define M2F 2 // FORWARD  MOTOR 2
#define M2B 4 // BACKWARD MOTOR 2
#define M3F 12 // FORWARD  MOTOR 3
#define M3B 13 // BACKWARD MOTOR 3
#define M4F 99 // FORWARD  MOTOR 4
#define M4B 99 // BACKWARD MOTOR 4
#define M5F 99 // FORWARD  MOTOR 5
#define M5B 99 // BACKWARD MOTOR 5
#define M6F 99 // FORWARD  MOTOR 6
#define M6B 99 // BACKWARD MOTOR 6
#define M7F 99 // FORWARD  MOTOR 7
#define M7B 99 // BACKWARD MOTOR 7
#define M8F 99 // FORWARD  MOTOR 8
#define M8B 99 // BACKWARD MOTOR 8
#define M9F 99 // FORWARD  MOTOR 9
#define M9B 99 // BACKWARD MOTOR 9

#define FORWARD  1
#define BACKWARD 2
#define RELEASE  0

// End stop sensor
#define SENSOR1 2 
#define SENSOR2 99 
#define SENSOR3 99 
#define SENSOR4 99 
#define SENSOR5 99 
#define SENSOR6 99 
#define SENSOR7 99 
#define SENSOR8 99 
#define SENSOR9 99 

int sensors[9]   = {SENSOR1, SENSOR2, SENSOR3, SENSOR4, SENSOR5, SENSOR6, SENSOR7, SENSOR8, SENSOR9};
int sensorState[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
int sensorStatePrev[9] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};

int motorForward[9]   = {M1F, M2F, M3F, M4F, M5F, M6F, M7F, M8F, M9F};
int motorBackward[9]  = {M1B, M2B, M3B, M4B, M5B, M6B, M7B, M8B, M9B};
int motorDirection[9]  = {RELEASE, RELEASE, RELEASE, RELEASE, RELEASE, RELEASE, RELEASE, RELEASE, RELEASE};

int motorCurrentPosition[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
int motorMoveToPosition[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

int motorPattern[4][9] = {
 {100, 0, 0, 0, 0, 0, 0, 0, 0},
 {50, 100, 0, 0, 0, 0, 0, 0, 0},
 {0, 50, 100, 0, 0, 0, 0, 0, 0},
 {0, 0, 50, 0, 0, 0, 0, 0, 0}
};
int currentPattern = -1;
int motorPattern2[9] = {0, 100, 0, 0, 0, 0, 0, 0, 0};

unsigned long currentTime;
unsigned long patternStartedTime;
unsigned long showPatternInterval = 5000; //5 seconds


void setup() {
  initMotors();
  changePattern(); // set an initial pattern for the motors
}

void loop() { 
  demoPattern();  // to move the motors as a demo 
  readSensors();
  controlMotors();
}

// Initialize motors: define pinmode
// maybe we also need to move the motors to an initial position, because the code might have stopped with winded wires
void initMotors(){
  for(int i=0;i<9;i++){
    pinMode(motorForward[i], OUTPUT);
    pinMode(motorBackward[i], OUTPUT);
  }
}

// As a demo periodically change the pattern for the motorpositions based on showPatternInterval
void demoPattern(){
  currentTime = millis();  
  if(currentTime - patternStartedTime > showPatternInterval) {
    changePattern();
  }
}

// Change the pattern to the next pattern in the motorPatternArray
void changePattern(){
  patternStartedTime = millis();
  currentPattern++;
  if (currentPattern > sizeof motorPattern){
    currentPattern = 0;
  } 
  for(int i=0;i<9;i++){
    motorMoveToPosition[i] = motorPattern[currentPattern,i];
  }
}
  
// Readsensors: read the endstop sensor, if it is broken, the state is LOW
// if a sensorbeam is broken change the currentposition 
// if motordirection is forward add 1 to its currentposition 
// if motordirection is backward subtract 1 from 1 its currentposition
void readSensors(){
  for(int i=0;i<9;i++){
    // only read sensor for motors that are moving
    if(motorDirection[i] != RELEASE){
      sensorState[i] = digitalRead(sensors[i]); 
      if (sensorState[i] == LOW && sensorStatePrev == HIGH){ // change in the state of the endstop sensor, to prevent a double reading of a LOW-position  
        if(motorDirection[i] == FORWARD){
          motorCurrentPosition[i]++; 
        } else if(motorDirection[i] == BACKWARD){
          motorCurrentPosition[i]--; 
        }
      }
      sensorStatePrev[i] = sensorState[i];
    }
  }
}

// Check whether the motors need to start or stop moving
void controlMotors(){
  for(int i=0;i<9;i++){
    // If the motor is not moving, check whether it should move
    if(motorDirection[i] == RELEASE ) {  //RELEASE means the motor is doing nothing
      if (motorMoveToPosition[i] > motorCurrentPosition[i])  {
        moveMotorForward(i);
      } else if (motorMoveToPosition[i] < motorCurrentPosition[i]){
        moveMotorBackward(i);
      }
    } 
   // If the motor is going forward, check for reaching its end position
    else if(motorDirection[i] == FORWARD){
      if (motorCurrentPosition[i] >= motorMoveToPosition[i]){
        stopMotor(i);
      }
    } 
   // If the motor is going backward, check for reaching its end position
    else if(motorDirection[i] == BACKWARD){
      if (motorCurrentPosition[i] <= motorMoveToPosition[i]){
        stopMotor(i);
      }
    }
  }
}

void moveMotorForward(int x){
  motorDirection[x] = FORWARD;
  digitalWrite(motorForward[x], HIGH); 
}

void moveMotorBackward(int x){
  motorDirection[x] = BACKWARD;
  digitalWrite(motorBackward[x], HIGH); 
}

void stopMotor(int x){
  if (motorDirection[x] == FORWARD){
    digitalWrite(motorForward[x], LOW);
  } else if (motorDirection[x] == BACKWARD){
    digitalWrite(motorBackward[x], LOW);
  }
  motorDirection[x] = RELEASE;
  sensorStatePrev[x] = -1; 
}

void showDebug(char txt[]){
  Serial.println(txt);
}

void showDebug(char txt[], int waarde){
  Serial.print(txt);
  Serial.println(waarde);
}
