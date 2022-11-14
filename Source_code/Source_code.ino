#include <MeMCore.h>
#define TURNING_TIME_MS 410 // The time duration (ms) for turning
#define RUN_TIME_MS 750
#define LDR A0
#define IR A1
MeLineFollower lineFinder(PORT_2); // for sensing black line
MeDCMotor leftMotor(M1); // assigning leftMotor to port M1
MeDCMotor rightMotor(M2); // assigning RightMotor to port M2
MeUltrasonicSensor ultrasonic(PORT_3);
MeBuzzer buzzer; // create the buzzer object
uint8_t motorSpeed = 255; // speed that Mbot runs at (max speed)
int minval=30000, mincolour= 0;
int red, green, blue, colour;
int ambient, IRreading;
// 2D array for color x RGB values
int Val[6][3] = { {560,320,780},
                  {550,280,780},
                  {520,275,890},
                  {570,300,920},
                  {530,270,850},
                  {500,260,800},
};
// color for each row:
String Col[6] = { "orange", "red", "blue", "white", "purple", "green" };

void playtune(){
  buzzer.tone(330, 400);
  buzzer.tone(330, 400);
  buzzer.tone(349, 400);
  buzzer.tone(392, 400);
  
  buzzer.tone(392, 400);
  buzzer.tone(349, 400);
  buzzer.tone(330, 400);
  buzzer.tone(294, 400);
  
  buzzer.tone(262, 400);
  buzzer.tone(262, 400);
  buzzer.tone(294, 400);
  buzzer.tone(330, 400);
  
  buzzer.tone(330, 600);
  buzzer.tone(294, 200);
  buzzer.tone(294, 800);
}

// functions to shine lights 
int LIGHT_DELAY = 100;
void shineRedLight() { 
 digitalWrite(11, HIGH); digitalWrite(12, HIGH); // red
 delay(LIGHT_DELAY); 
} 
void shineBlueLight() { 
 digitalWrite(11, LOW); digitalWrite(12, HIGH); // blue
 delay(LIGHT_DELAY);  
} 
void shineGreenLight() { 
  digitalWrite(11, HIGH); digitalWrite(12, LOW); // green
  delay(LIGHT_DELAY); 
} 
void allLightsOff() { 
  digitalWrite(11, LOW); digitalWrite(12, LOW); // IR 
} 

// function to read RGB values of a colour
void ReadValues(){ 
   shineRedLight(); 
   red=(analogRead(LDR));
//   Serial.print("Red=");
//   Serial.println(red);

   shineGreenLight(); 
   green=(analogRead(LDR)); 
//   Serial.print("Green=");
//   Serial.println(green);
   
   shineBlueLight(); 
   blue=(analogRead(LDR)); 
//   Serial.print("blue=");
//   Serial.println(blue);
}

// function to determine which color is being sensed using the Difference of values method
void SenseColours(){
   ReadValues();
   for (int i=0; i<6; i++){
     int temp = abs(red-Val[i][0])+abs(green-Val[i][1])+abs(blue-Val[i][2]);
     if( temp < minval ){
        minval=temp;
        mincolour = i+1;
        //Serial.println(Col[i]);
     }
   }
  colour=mincolour;
  Serial.println(Col[colour-1]);
}

// function for the Mbot to turn
void Turn(bool a){
     if(a){ //true = turn left
       leftMotor.run(motorSpeed); 
       rightMotor.run(motorSpeed); 
       delay(TURNING_TIME_MS); // time taken to turn 90 degrees
     }
     else{ //false = turn right
       leftMotor.run(-motorSpeed); 
       rightMotor.run(-motorSpeed); 
       delay(TURNING_TIME_MS); // time taken to turn 90 degrees
     }
}

// function for the Mbot to run one square
void OneSquare(){
     leftMotor.run(-motorSpeed); 
     rightMotor.run(motorSpeed); 
     delay(RUN_TIME_MS); //time taken to run 1 square
}

//function that combines colour sensing and movement
//translates colour sensed to according movement
void Execute(){ 
  if (colour==1){ // orange
    Turn(true);
    Turn(true);
  }
  else if(colour==2){ //red
    Turn(true);
  }
  else if(colour==3){ // blue
    Turn(false);
    OneSquare();
    Turn(false);
  }
  else if(colour==4){ //white
     leftMotor.run(0); 
     rightMotor.run(0); 
     playtune();
  }
  else if(colour==5){ //purple
    Turn(true);
    OneSquare();
    Turn(true);
  }
  else if(colour==6){ //green
    Turn(false);
  }
}



void setup() {
  pinMode(11,OUTPUT); //pin A0, mapped to S1 on RJ25 adapter at Port 1
  pinMode(12,OUTPUT); //pin A0, mapped to S2 on RJ25 adapter at Port 1
}

void loop() {
  int sensorState = lineFinder.readSensors(); 
  if (sensorState == S1_IN_S2_IN) { // when a black line is reached, stop, sense colours and move accordingly
    //stops
    leftMotor.run(0);
    rightMotor.run(0);
    // set minval to a high dummy value and detect the colour with the smallest difference
    minval=30000;
    SenseColours(); 
    //repeat for orange and red for greater accuracy
    if(colour == 1 || colour == 2) SenseColours(),SenseColours();
    Execute();
  }
  else{ // run without bumping into wall
    //run straight
    rightMotor.run(motorSpeed); // Positive: wheel turns clockwise
    leftMotor.run(-motorSpeed); // Negative: wheel turns anti-clockwise
    if(ultrasonic.distanceCm()<=5){
        Turn(true);
    }
    else if((ultrasonic.distanceCm()>=12) && (ultrasonic.distanceCm()<=16)){
        Turn(false);
    }
  }
}
