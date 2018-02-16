#include <TimerOne.h>
// pines para sistema linea blanca
const int pinDD=3;
const int pinDI=2;
const int pinAD=5;
const int pinAI=4;

//const int lineaDetectada;
int lineaDetectada=0;

//pines para motores
const int pinENA = 6;
const int pinIN1 = 7;
const int pinIN2 = 8;
const int pinIN3 = 9;
const int pinIN4 = 10;
const int pinENB = 11;

const int speedAttack = 255; 
const int speedRadar=180;  
const int speedLinea=255;
const int speedPush=120;

const int pinMotorD[3] = { pinENA, pinIN1, pinIN2 };
const int pinMotorI[3] = { pinENB, pinIN3, pinIN4 };

 //variables para oponente
const int pinOponente=A0;
volatile int TimerOponenteLoc=0;

//rango de valores raw del Sharp
const long rangoInf=120; //lejos
const long rangoSup=680; //cerca
const long rangoPush=675;
//Para estabilizar fuente alimentacion, condensador de 10uF o mas entre Vcc y Gnd

int giroRadar=0;

void setup()
{
  //sensores linea
  pinMode(pinDD,INPUT);
  pinMode(pinDI,INPUT);
  pinMode(pinAD,INPUT);
  pinMode(pinAI,INPUT);
  

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
    int detectDD=digitalRead(pinDD);
    int detectDI=digitalRead(pinDI);
    int detectAD=digitalRead(pinAD);
    int detectAI=digitalRead(pinAI);
    lineaDetectada=0;
    int OponenteLoc=0;

   noInterrupts();               // Suspende las interrupciones
   OponenteLoc= TimerOponenteLoc;
   interrupts();                 // Autoriza las interrupciones

    // Se comprueban los sensores de linea
    // detector LOW-linea detectada y no oponente: borde del dojo
    
    if (detectDD==LOW && OponenteLoc==0){
      Serial.println("detectado DD");
      lineaDetectada=1;
      moveBackward(pinMotorI, speedLinea);
      
    }
    if (detectDI==LOW && OponenteLoc==0){
      Serial.println("detectado DI");
      lineaDetectada=1;
      moveBackward(pinMotorD, speedLinea);
      
    }
    if (detectAD==LOW && OponenteLoc==0){
      Serial.println("detectado AD");
      lineaDetectada=1;
      moveForward(pinMotorD, speedLinea);
      
    }
    if (detectAI==LOW && OponenteLoc==0){
      Serial.println("detectado AI");
      lineaDetectada=1;
      moveForward(pinMotorI, speedLinea);
      
    }
    
    // detector low y oponente=1: treta del oponente: poner lamina blanca debajo //del robot
    
//    Serial.print("OponenteLoc: ");
//    Serial.print(OponenteLoc);
//    Serial.print("analogRead: ");
//    Serial.print(analogRead(pinOponente));
    //Serial.print("lineadetectada: ");
    //Serial.println(lineaDetectada);
    // si no se detecta linea blanca, o si se detecta pero esta el oponente delante
    if (lineaDetectada==0 && OponenteLoc==0)
      {
            //Estado Radar
            
            if(giroRadar==1){ //levo
              moveForward(pinMotorD, speedRadar);
              moveBackward(pinMotorI, speedRadar);
              Serial.println("modo radar 1 levo");
             
            }
            if (giroRadar==0){ //dex
              moveForward(pinMotorI, speedRadar);
              moveBackward(pinMotorD, speedRadar);
              Serial.println("modo radar 0 dex");
           
            }
            
      }

    if (lineaDetectada==0 && OponenteLoc==1)
      {
            //Estado ataque
            Serial.println("modo ataque");
            if (giroRadar==0){ //dex
                giroRadar=1;
//                moveForward(pinMotorD,speedAttack);
//                delay(130); //compensa deteccion no linea recta
                moveForward(pinMotorD, speedAttack);
                moveForward(pinMotorI, speedAttack);
	    if (  analogRead(pinOponente) >= rangoPush )  Push();
              } else { //levo
                giroRadar=0;
//                moveForward(pinMotorI,speedAttack);
//                delay(130); //compensa deteccion no linea recta
                moveForward(pinMotorD, speedAttack);
                moveForward(pinMotorI, speedAttack);
   if (  analogRead(pinOponente) >= rangoPush )  Push();
               }
            
            // estrategia: varios empujones?
            // sensor giroscopio por si se levanta el morro? detectar choque?
       }
      delay(10);
  }
}
  
void Push()
{	
	
moveForward(pinMotorD, speedPush);
moveForward(pinMotorI, speedPush);
delay(100);
moveForward(pinMotorD, speedAttack);
moveForward(pinMotorI, speedAttack);
delay(100);

}

void ISR_Sharp()
{

 TimerOponenteLoc=GetOponent();

}


void stopAll()
{
  fullStop(pinMotorD);
  fullStop(pinMotorI);
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
  
  int valSharp= analogRead(pinOponente);

  Serial.print("valSharp: ");
  Serial.println(valSharp);  
  
  if (rangoInf <= valSharp && valSharp<= rangoSup)
  {
      delay(50);
      valSharp=analogRead(pinOponente);
      
 	if (rangoInf <= valSharp && valSharp <= rangoSup)
{
     digitalWrite(13,HIGH);
     return 1;
}
  }
  else{
        	delay(50);
            valSharp=analogRead(pinOponente);
      
 	if (rangoInf > valSharp || valSharp > rangoSup)
{
     digitalWrite(13,LOW);
    	     return 0;
}
    
    } 

}


