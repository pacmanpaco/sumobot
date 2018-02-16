#include <TimerOne.h>
// pin white system detection
const int pinFR=3;
const int pinFL=2;
const int pinRR=5;
const int pinRL=4;

int lineDetected=0;

//pines para motores
const int pinENA = 6;
const int pinIN1 = 7;
const int pinIN2 = 8;
const int pinIN3 = 9;
const int pinIN4 = 10;
const int pinENB = 11;

const int speedAttack = 255; 
const int speedRadar=180;  
const int speedLine=255;


const int pinMotorR[3] = { pinENA, pinIN1, pinIN2 };
const int pinMotorL[3] = { pinENB, pinIN3, pinIN4 };

 //variables para oponente
const int pinOponent=A0;
volatile int TimerOponentLoc=0;

//rango de valores raw del Sharp
const long rangeF=120; //far
const long rangeC=680; //close
const long rangePush=675;

int TurnRadar=0;

void setup()
{
  //sensores linea
  pinMode(pinFR,INPUT);
  pinMode(pinFL,INPUT);
  pinMode(pinRR,INPUT);
  pinMode(pinRL,INPUT);
  

//motores
  pinMode(pinIN1, OUTPUT);
  pinMode(pinIN2, OUTPUT);
  pinMode(pinENA, OUTPUT);
  pinMode(pinIN3, OUTPUT);
  pinMode(pinIN4, OUTPUT);
  pinMode(pinENB, OUTPUT);

  Timer1.initialize(10000);     	// Dispara cada 10 ms
  Timer1.attachInterrupt(ISR_SHARP); // Activa la interrupcion y la asocia a ISR_SHARP
  Serial.begin(9600);
  pinMode(13,OUTPUT);
}

void loop()
{
    delay(1000); // delay(5100); //contar 5 s antes de iniciar el asalto.
    
  while (1)
  {
    int detectFR=digitalRead(pinFR);
    int detectFL=digitalRead(pinFL);
    int detectRR=digitalRead(pinRR);
    int detectRL=digitalRead(pinRL);
    lineDetected=0;
    int OponentLoc=0;

   noInterrupts();               // Suspende las interrupciones
   OponentLoc= TimerOponentLoc;
   interrupts();                 // Autoriza las interrupciones

    // Se comprueban los sensores de linea
    // detector LOW-linea detectada y no oponente: borde del dojo
    
    if (detectFR==LOW && OponentLoc==0){
      Serial.println("DETECTED FR");
      lineDetected=1;
      moveBackward(pinMotorL, speedLine);
      
    }
    if (detectFL==LOW && OponentLoc==0){
      Serial.println("detectado DI");
      lineDetected=1;
      moveBackward(pinMotorR, speedLine);
      
    }
    if (detectRR==LOW && OponentLoc==0){
      Serial.println("detectado AD");
      lineDetected=1;
      moveForward(pinMotorR, speedLine);
      
    }
    if (detectRL==LOW && OponentLoc==0){
      Serial.println("detectado AI");
      lineDetected=1;
      moveForward(pinMotorL, speedLine);
      
    }
        
    // si no se detecta linea blanca, o si se detecta pero esta el oponente delante
    if (lineDetected==0 && OponentLoc==0)
      {
            //Estado Radar
            
            if(turnRadar==1){ //levo
              moveForward(pinMotorR, speedRadar);
              moveBackward(pinMotorL, speedRadar);
              Serial.println("radar 1 levo");
             
            }
            if (turnRadar==0){ //dex
              moveForward(pinMotorL, speedRadar);
              moveBackward(pinMotorR, speedRadar);
              Serial.println("radar 0 dex");
           
            }
            
      }

    if (lineDetected==0 && OponentLoc==1)
      {
            //Estado ataque
            Serial.println("modo ataque");
            if (turnRadar==0){ //dex
                turnRadar=1;
                moveForward(pinMotorD, speedAttack);
                moveForward(pinMotorI, speedAttack);

              } else { //levo
                giroRadar=0;

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
  
  if (rangeL <= valSharp && valSharp<= rangeC)
  {
      delay(50);
      valSharp=analogRead(pinOponent);
      
 	if (rangeL <= valSharp && valSharp <= rangeC)
	{
     		digitalWrite(13,HIGH);
     		return 1;
	}
  }
  else
  {
       	delay(50);
        valSharp=analogRead(pinOponente);
      
 	if (rangoInf > valSharp || valSharp > rangoSup)
	{
     		digitalWrite(13,LOW);
    	     	return 0;
	}
    
   } 

}


