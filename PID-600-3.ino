#include<QTRSensors.h>
#include <AFMotor.h>
 
AF_DCMotor leftMotor(3, MOTOR12_8KHZ ); // PIN 11 - create motor #1 pwm
AF_DCMotor rightMotor(4, MOTOR12_8KHZ ); // PIN 3 - create motor #2 pwm


// Change the values below to suit your robot's motors, weight, wheel type, etc.
#define KP 0
#define KI 0
#define KD 0
#define M1_DEFAULT_SPEED 230
#define M2_DEFAULT_SPEED 230
#define M1_MAX_SPEED 255
#define M2_MAX_SPEED 255
#define NUM_SENSORS  6     // number of sensors used
#define TIMEOUT       2500  // waits for 2500 us for sensor outputs to go low
#define DEBUG 0// set to 1 if serial debug output needed
#define EMITTER_PIN 30
#define led  36
// create an object for three QTR-xA sensors on analog inputs 0, 2, and 6
//QTRSensorsAnalog qtra((unsigned char[]) {0, 2, 6}, 3);
QTRSensorsRC qtr ((unsigned char[]) {A8,A9,A10,A11,A12,A13} ,NUM_SENSORS, TIMEOUT);

unsigned int sensorValues[NUM_SENSORS]; 

int position;
//int found = 0;
int rightValue;
int centerValue;
 int leftValue;
int left = 0;
int right = 0;
int i;


void setup()
{
  pinMode(led,OUTPUT);
  delay(500);
  manual_calibration(); 
  set_motors(0,0);
  Serial.begin(9600);
}

int lastError = 0;
int  last_proportional = 0;
int integral = 0;

void readsensors(){
  for (int i = 0; i <NUM_SENSORS; i++)
  {
   qtr.read(sensorValues);
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }
  Serial.println();
}

  void observation()
  {
  position = qtra.readLine(sensorValues);
  leftValue = ((sensorValues[0]+sensorValues[1])/2);
  centerValue = ((sensorValues[2]+sensorValues[3])/2);
  rightValue = (sensorValues[4]+sensorValues[5])/2;
  //Serial.println(position);
 
}
 
 
void loop()
{
   
    
   readsensors();
   observation();
     
   if(rightValue>leftValue)
   {
      
    turnrightside();
     
   }
   else if(leftValue>rightValue)
   { 
    
     
    turnleftside();
     
     
   }
    
   
   else if(position==0)
   {
    stop();
    //forward();
   }
    else if(centerValue>leftValue&&centerValue>rightValue)
    {
      straight();
    }
    
    /*else if(leftValue==2500&&centerValue==2500&&rightValue==2500)
    {
       stop();
      delay(500);
       
    } 
 */
}

 //for turn left
 void turnleftside()
 {  
 
   for(int i=0;i<50;i++)
 
   
  {
    if(!position==0)
    {
    position= qtra.readLine(sensorValues);  
    gofront();
   
    }
    else if(position==0)
    {
      break;
    }
    
  }
  
     
    
    position=0;
    //centerValue=16;
    while(position==0)
     //for(int i=0;i<150;i++)
    {  
        //observation();
        position=qtra.readLine(sensorValues);
         
         if(!(leftValue<centerValue))    
          
          {
            qtra.readLine(sensorValues);
            turnLeft();
          }
          else
          {
            position=qtra.readLine(sensorValues);
            break;
          }
    }
   /* position=0;
    
     for(int i=0;i<100;i++)
     {
      position=qtra.readLine(sensorValues);
         if(!leftValue<centerValue)
         {
            qtra.readLine(sensorValues);
            turnLeft();
         }
         
          else
          {
             position=qtra.readLine(sensorValues);
            break;
          }
        
       

     
    }
    */       
      
   }



 //for turn right
void turnrightside()
{
 
  for(int i=0;i<100;i++)
  {
    if(!position==0)
    {
    gofront();
   
    }
    else if(position==0)
    {
      break;
    }
    
  }
  
    
   position=0;
   while(position==0)
    
    {
      //Serial.print(position);
    //Serial.print('\t');
    //Serial.println();
     position= qtra.readLine(sensorValues);
      if(!(rightValue<centerValue))
      {
         qtra.readLine(sensorValues);
         turnRight();
      }
      else  
      {
         
        break;
      }
       
    }
   
     
    
     
} 

    
 
  
   
 void gofront()
 {
    leftMotor.run(FORWARD);
  rightMotor.run(FORWARD);
 }
 
 void forward()
 {
   leftMotor.run(FORWARD);
  rightMotor.run(FORWARD);
   
 }
 
void turnRight()
{
  leftMotor.run(FORWARD);
  rightMotor.run(BACKWARD);
  
}
void backward()
{
  leftMotor.run(BACKWARD);
  rightMotor.run(BACKWARD);
  
}
void turnLeft()
{
  leftMotor.run(BACKWARD);
  rightMotor.run(FORWARD);
  
}
void stop()
{
  leftMotor.run(RELEASE);
  rightMotor.run(RELEASE);
}
 

void set_motors(int leftMotorSpeed, int rightMotorSpeed)
{
  if (leftMotorSpeed> M1_MAX_SPEED )leftMotorSpeed = M1_MAX_SPEED; // limit top speed
  if (rightMotorSpeed > M2_MAX_SPEED )rightMotorSpeed = M2_MAX_SPEED; // limit top speed
  if (leftMotorSpeed< 0) leftMotorSpeed = 0; // keep motor speed above 0
  if (rightMotorSpeed< 0) rightMotorSpeed = 0; // keep motor speed above 0
  leftMotor.setSpeed(leftMotorSpeed);     // set motor speed
  rightMotor.setSpeed(rightMotorSpeed);     // set motor speed
  leftMotor.run(FORWARD);  
  rightMotor.run(FORWARD);
}

void straight()
{
    position = qtra.readLine(sensorValues,QTR_EMITTERS_ON);
    //false for Black in White,//true for  White in Black
  int error = position-2500;

  int motorSpeed = KP * error + KD * (error - lastError);
  lastError = error;

  int leftMotorSpeed = M1_DEFAULT_SPEED + motorSpeed;
  int rightMotorSpeed = M2_DEFAULT_SPEED - motorSpeed;

  // set motor speeds using the two motor speed variables above
  set_motors(leftMotorSpeed, rightMotorSpeed);
}

void manual_calibration() {

   
  // if true, generate sensor dats via serial output
    Serial.begin(9600);
    for (int i = 0; i <NUM_SENSORS; i++)
    {
      Serial.print(qtra.calibratedMinimumOn[i]);
      Serial.print(' ');
    }
    Serial.println();

    for (int i = 0; i <NUM_SENSORS; i++)
    {
      Serial.print(qtra.calibratedMaximumOn[i]);
      Serial.print(' ');
    }
    Serial.println();
    Serial.println();
  }
