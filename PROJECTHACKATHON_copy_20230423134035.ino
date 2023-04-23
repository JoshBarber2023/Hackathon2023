#include <LiquidCrystal.h>
#include <Servo.h>

// Initialisation

#define BLUE 6
#define RED 5

Servo myServo;
int servoMax = 179;
int servoMin = 1; //*
int servoPos = 90; 

int button = 2; // Turn ON
int sensor_one = A1;
int sensor_two = A0;
int sensor_three = A2;
int led = 4;
float threshold = 60; 
int counter = 0;
volatile int alert_times = 0;
float ambient_cal1 = 0;
float ambient_cal2 = 0;
float ambient_cal3 = 0;
float ambience = 0;

// For turn on
volatile int turnItOn = 0;

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);


// Set up IO

void setup() {
  Serial.begin(9600);
  Serial.println("Servo Online");
  myServo.attach(3);
  myServo.write(90);



  // clear console on startup
  for (int i = 0; i < 12; i++) {
    Serial.println(" ");
  }
  pinMode(button, INPUT); 
  //pinMode(button2, INPUT);  
  pinMode(sensor_one, INPUT);
  pinMode(sensor_two, INPUT);
  pinMode(sensor_three, INPUT);
  pinMode(led, OUTPUT);


  // startup low ouput
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  digitalWrite(RED, LOW);
  digitalWrite(BLUE, LOW);

  // Set up interrupts:
  attachInterrupt(digitalPinToInterrupt(2), turnOn, HIGH);
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("prss bttn:");
}

// Main loop
void loop() {

  while(1){
    if(turnItOn == 1){
      turnItOn = 0;
      main_loop();
      break;
    }
  }
}

int main_loop() {
  Serial.println(turnItOn);
  Serial.println("Main loop has begun!");
  myServo.write(90);

  
    // stage 1: Setup
  CalculateAmbience();
    // stage 2: 
 

    // stage 4: Go into sleep mode? (Retrieve ALERTS and display them, 
    // whilst turning off loop)


}


float CalculateAmbience() { 
  Serial.println("Calculating Ambience...");
  lcd.setCursor(0,0);
  lcd.print("Calc Ambience");

  for (int i = 0; i < 29; i++) {
    delay(333);
    ambient_cal1 += analogRead(sensor_one)*32;
    ambient_cal2 += analogRead(sensor_two)*32;
    ambient_cal3 += analogRead(sensor_three)*32;
  }

  ambient_cal1 = ambient_cal1 / 30.0;
  ambient_cal2 = ambient_cal2 / 30.0;
  ambient_cal3 = ambient_cal3 / 30.0;

  ambience = (ambient_cal1 + ambient_cal2 + ambient_cal3)/3.0;

  // display ambient noise
  //Serial.print("Ambient Noise Average: "); ***********************
  //Serial.println(ambience); *******************

  // flick LED on to indicate that the calibration is done
  digitalWrite(led, HIGH);
  delay(1000);
  digitalWrite(led,LOW);
  delay(100);
  Serial.println("Ambience:");
  Serial.println(ambience);

  idle(ambience);
  return;
}


// Checking sensor values, if there's any outliers, send an alert
int idle(float value) {

  Serial.println("IDLE MODE ON...");
  lcd.setCursor(0,0);
  lcd.print("Alarm On:    ");
  while(1) {
      
    float s1 = analogRead(sensor_one)*32;
    float s2 = analogRead(sensor_two)*32;
    float s3 = analogRead(sensor_three)*32;
    Serial.println("test");
    Serial.println(s1);
    Serial.println(s2);
    Serial.println(s3);
    Serial.println(" ");
    delay(100);

    LEDBlue();
    float alert = (value + (threshold)); 


  
    // If any of the microphones get outliers, track that as an alert
    if (s1 > alert || s2 > alert || s3 > alert ) {

      Serial.println("Values");

      Serial.println(s1);
      Serial.println(s2);
      Serial.println(s3);

      alert_noise(s1, s2, s3);

    }



    counter += 1;
    if (counter == 50) { 
      CalculateAmbience();
    }

  }
  return;
}

float alert_noise(float one, float two, float three) {
  Serial.println("Noise Detected");

  LEDRED();
  alert_times += 1;

  lcd.setCursor(0,0);
  lcd.print("ALERT!ALERT!");
  Serial.print("ALERT TIMES:");
  Serial.println(alert_times);
  lcd.setCursor(0,1);

  lcd.print(alert_times);

  // if ((one > two) & (one > three))
  // if ((two > one) & (two > three))
  // if ((three > one) & (three > two))

  if (one > (((two + three)/2))+(threshold/4)) {
    myServo.write(servoMax);
  }  if (two > (((one + three)/2)+(threshold/4))) {
    myServo.write(servoPos);
  }  if (three > (((two + one)/2)+(threshold/4))) {
    myServo.write(servoMin);
  } if (three == two) {
    myServo.write(40);
  } if (two == one) {
    myServo.write(140);
  }
  delay(5000); 

  for (int i = 1; i < 88; i++) {
    myServo.write(2 * i);
    delay(50);
  }

  myServo.write(90);

  CalculateAmbience();
  return;
}





int endAlert(){
  lcd.setCursor(0,0);
  lcd.print("ALERT!ALERT!");
  Serial.print("ALERT TIMES:");
  Serial.println(alert_times);
  lcd.setCursor(0,14);
  lcd.print(alert_times);
  alert_times = 0;
  return;
}


int turnOn() {
  //Serial.println("Button 1 has been pressed");
  turnItOn = 1;
  return;
}

// Turn RGB LED to blue
int LEDBlue() {
  analogWrite(BLUE, 255);
  delay(200);
  analogWrite(BLUE, 0);
  analogWrite(RED, 0);
  delay(0);
  return;
}


// Turn RGB LED to red
int LEDRED() {
  analogWrite(BLUE, 0);
  analogWrite(RED, 255);
  delay(1000);
  analogWrite(RED, 0);
  delay(1000);
  return;
}

