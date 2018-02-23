#include <TimerOne.h>
// pin white system detection
const int pinFR=3;
const int pinFL=2;
const int pinRR=5;
const int pinRL=4;

int lineDetected=0;

//motor pins
const int pinENA = 6;
const int pinIN1 = 7;
const int pinIN2 = 8;
const int pinIN3 = 9;
const int pinIN4 = 10;
const int pinENB = 11;

const int speedAttack = 255; 
const int speedRadar=120;  
const int speedLine=255;


const int pinMotorR[3] = { pinENA, pinIN1, pinIN2 };
const int pinMotorL[3] = { pinENB, pinIN3, pinIN4 };

 //oponent
const int pinOponent=A0;
volatile int TimerOponentLoc=0;

//values range of raw sharp data
const long rangeF=120; //far
const long rangeC=680; //close
const long rangePush=675;

int TurnRadar=0;

void setup()
{
  //white line sensor
  pinMode(pinFR,INPUT);
  pinMode(pinFL,INPUT);
  pinMode(pinRR,INPUT);
  pinMode(pinRL,INPUT);
  

//motors
  pinMode(pinIN1, OUTPUT);
  pinMode(pinIN2, OUTPUT);
  pinMode(pinENA, OUTPUT);
  pinMode(pinIN3, OUTPUT);
  pinMode(pinIN4, OUTPUT);
  pinMode(pinENB, OUTPUT);

  Timer1.initialize(10000);       // timer for opponent detection
  Timer1.attachInterrupt(ISR_Sharp); 
  Serial.begin(9600);
  pinMode(13,OUTPUT);
}

void loop()
{
    delay(1000); // start the round
    
  while (1)
  {
    int detectFR=digitalRead(pinFR);
    int detectFL=digitalRead(pinFL);
    int detectRR=digitalRead(pinRR);
    int detectRL=digitalRead(pinRL);
    lineDetected=0;
    int OponentLoc=0;

   noInterrupts();               // disable interruptions
   OponentLoc= TimerOponentLoc;
   interrupts();                 // enable interruptions

    // Check line sensors
    // LOW-detected line and no oponent: edge of dohyo
    
    if (detectFR==LOW && OponentLoc==0){
      Serial.println("DETECTED FR");
      lineDetected=1;
      moveBackward(pinMotorL, speedLine);
      
    }
    if (detectFL==LOW && OponentLoc==0){
      Serial.println("detected FL");
      lineDetected=1;
      moveBackward(pinMotorR, speedLine);
      
    }
    if (detectRR==LOW && OponentLoc==0){
      Serial.println("detected RR");
      lineDetected=1;
      moveForward(pinMotorR, speedLine);
      
    }
    if (detectRL==LOW && OponentLoc==0){
      Serial.println("detected RL");
      lineDetected=1;
      moveForward(pinMotorL, speedLine);
      
    }
        
    if (lineDetected==0 && OponentLoc==0)
      {
            //Radar State
            
            if(TurnRadar==1){ //levo
              moveForward(pinMotorR, speedRadar);
              moveBackward(pinMotorL, speedRadar);
              Serial.println("radar 1 levo");
             
            }
            if (TurnRadar==0){ //dex
              moveForward(pinMotorL, speedRadar);
              moveBackward(pinMotorR, speedRadar);
              Serial.println("radar 0 dex");
           
            }
            
      }

    if (lineDetected==0 && OponentLoc==1)
      {
            //Estado ataque
            Serial.println("attack state");
            if (TurnRadar==0){ //dex
                TurnRadar=1;
                moveForward(pinMotorR, speedAttack);
                moveForward(pinMotorL, speedAttack);

              } else { //levo
                TurnRadar=0;

                moveForward(pinMotorR, speedAttack);
                moveForward(pinMotorL, speedAttack);

               }
            
       }
      delay(10);
  }
}


void ISR_Sharp()
{

 TimerOponentLoc=GetOponent();

}


void stopAll()
{
  fullStop(pinMotorR);
  fullStop(pinMotorL);
}
void moveForward(const int pinMotor[3], int speed)
{
  digitalWrite(pinMotor[1], HIGH);
  digitalWrite(pinMotor[2], LOW);

  analogWrite(pinMotor[0], speed);
} 

void moveBackward(const int pinMotor[3], int speed)
{
  digitalWrite(pinMotor[1], LOW);
  digitalWrite(pinMotor[2], HIGH);

  analogWrite(pinMotor[0], speed);
}

void fullStop(const int pinMotor[3])
{
  digitalWrite(pinMotor[1], LOW);
  digitalWrite(pinMotor[2], LOW);

  analogWrite(pinMotor[0], 0);
}

int GetOponent()
{

  //long meanSharp = meanFilter.AddValue(valSharp);
  
  int valSharp= analogRead(pinOponent);

  Serial.print("valSharp: ");
  Serial.println(valSharp);  
  
  if (rangeF <= valSharp && valSharp<= rangeC)
  {
      delay(50);
      valSharp=analogRead(pinOponent);
      
  if (rangeF <= valSharp && valSharp <= rangeC)
  {
        digitalWrite(13,HIGH);
        return 1;
  }
  }
  else
  {
        delay(50);
        valSharp=analogRead(pinOponent);
      
  if (rangeF > valSharp || valSharp > rangeC)
  {
        digitalWrite(13,LOW);
            return 0;
  }
    
   } 

}

