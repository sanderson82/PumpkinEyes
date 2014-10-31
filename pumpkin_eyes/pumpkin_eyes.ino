/**
 * A simple arduino app that uses 2 servos, 6 leds(red/white/green), and an HC04 to 
 * control a set of eys mounted into a pumpkin.
 *
 * The pumpkin uses 2 plastic hollow eyes and 3 leds mounted inside of each of them.
 *
 * The ultrasonic sensor measure the distance from the front of the pumpkin to the nearest object
 *
 * The eyes are mounted on top of a servo and scan the surrounding for objects.  As the object nears the 
 * eyes range of scan is reduced until the eyes are stuck in the middle.
 *
 * The eyes also adjust their color as the object approaches.  The eyes change from 
 * white, to green to red and also adjust brightness based on distance.
 * 
 *
 */
#include <Servo.h>

// HC04 Pins 
#define trigPin 13
#define echoPin 12

// LED Pins.  
#define redled = 3;       
#define greenled = 6;
#define whiteled1 = 7;
#define whiteled2 = 8;

// Servo control pins
#define leftServoPin = 9;
#define rightServoPin = 10;

Servo leftServo;
Servo rightServo;

// Keeps track of the last servo position
int lastServoPos = 90;

// Defines the servo movement amount
int movementAmount = 15;

void setup() {
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(whiteled1, OUTPUT);
  pinMode(whiteled2, OUTPUT);
  pinMode(greenled, OUTPUT);
  pinMode(redled, OUTPUT);
  
  leftServo.attach(leftServoPin);
  rightServo.attach(rightServoPin);
  
  leftServo.write(lastServoPos);
  rightServo.write(lastServoPos);
}

void loop() {
  distance = getDistance();
  Serial.print(distance);
  Serial.println(" cm");
  updateLED(distance);
  updateServo(distance);
  delay(200);
}

/**
 * Gets the distance from ultrasonic sensor
 * @return Returns the distance in cm
 */
long getDistance()
{
  digitalWrite(trigPin, LOW);  
  delayMicroseconds(2); 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  return (duration/2) / 29.1;
}

/**
 * Update the LEDs
 * @param distance The distance in cm from the nearest object
 */
void updateLED(long distance)
{
  int val = distance/100;
  int per = distance%100;
  int brightness = ((100-per)*255)/100;
  Serial.print("val=");
  Serial.println(val);
  Serial.print("per=");
  Serial.println(per);
  Serial.print("bright=");
  Serial.println(brightness);
  
  switch(val)
  {
    case 0:
    analogWrite(redled, brightness); 
    digitalWrite(whiteled1, LOW); 
    digitalWrite(whiteled2, LOW); 
    analogWrite(greenled, 0); 
    break;
    case 1:
    analogWrite(greenled, brightness); 
    digitalWrite(whiteled1, LOW); 
    digitalWrite(whiteled2, LOW); 
    analogWrite(redled, 0); 
    break;
    default:
    digitalWrite(whiteled1, HIGH); 
    digitalWrite(whiteled2, HIGH); 
    analogWrite(greenled, 0); 
    analogWrite(redled, 0); 
    break;
  }
}

/**
 * Update the servo position based on distance and last position.
 * If there are no objects within 3m the servos should scan between 45 and 135.  
 *
 * @param distance The distance in cm from the nearest object
 *
 * distance >3m 45-135
 * 2m<distance<=3m 60-120
 * 1m<distance<=2m 75-105
 * distance<=1 hold 90
 */
void updateServo(long distance)
{
  int val = distance/100;
  lastServoPos = lastServoPos + movementAmount;
  switch(val)
  {
     case 0:
       lastServoPos = 90;
       break;
     case 1:
       if(lastServoPos<75 || lastServoPos>105)
         movementAmount = movementAmount*-1;
       break;
     case 2: 
       if(lastServoPos<60 || lastServoPos>120)
         movementAmount = movementAmount*-1;
       break;
     default:
       if(lastServoPos<45 || lastServoPos>135)
         movementAmount = movementAmount*-1;
       break;

  }
  
  leftServo.write(lastServoPos);
  rightServo.write(lastServoPos);
}
