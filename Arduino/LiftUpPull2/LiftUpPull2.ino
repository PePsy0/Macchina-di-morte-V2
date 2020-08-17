#include "hx711.h"

boolean debug =true;

////// Load Cell Variables
float gainValue = -875.7 * (1 - 0.001); //CALIBRATION FACTOR
float measuringIntervall = 2;       //Measuring interval when IDLE
float measuringIntervallTest = .5;  //Measuring interval during SLOW test
float measuringIntervallTestFast = .15; ///Measuring interval during FAST test

long tareValue;

Hx711 loadCell(A0, A1);

////// PIN
int upButton = 7 ;  //pulsante per spostare il carro in su
int downButton =8; //pulsante per spostare il carro in giù 
int dataButton =9;  //pulsante per inviare gli ultimi dati raccolti
int dataLed    =10; //led acceso: salvo i dati
int delaySelector=A0;  //potenziometro per regolare il delay
int directionPin = 3;
int stepPin = 2;
int ledPin = 13;     //posso utilizzare anche il led integrato sulla scheda

/////  stato pulsanti
boolean upStatus = false;
boolean downStatus = false;
boolean dataStatus = false;

////// Stepper Variables
int pulseLength = 20;
float stepsPerMM = 200 * 2 * (26.85) / 5; // Steps per rev * Microstepping * Gear reduction ratio / Pitch
float stepsPerSecond = stepsPerMM / 60; //1mm/min
//int slowSpeedDelay = 3000;    //Time delay between steps for jogging slowly
//int fastSpeedDelay = 300;     //Time delay between steps for jogging fast
int manualSpeedDelay = 300;     //Time delay between steps **MANUAL**
boolean dir = 0;

//modalità di selezione del delay dal potenziometro 
#define RANGE_MODE  
//elencare i delay desiderati e il loro numero 
int del1=300;
int del2=1000;
int del3=1700;
int del4=2400;
int del5=3000;


/*****************************
         Data
*****************************/
long durata=0;//Tempo da inizio prova
//Voltaggio da cella di carico
//Voltaggio estensimetro di riferimento
//Voltaggio estensimetro su provino
//Voltaggio calcolato secondo il ponte di Wheatstone
//Distanza percorsa dal carro
int counter =0;
//TODO



/*****************************
         SETUP
*****************************/
void setup() {
   //Up Button
   pinMode(upButton, INPUT);
   
   //Down Button
   pinMode(downButton,INPUT);

   //Data Button
   pinMode(dataButton, INPUT);   

   //Led
   pinMode(ledPin, OUTPUT);
   digitalWrite(ledPin, LOW);

   pinMode(dataLed, OUTPUT);
   digitalWrite(dataLed, LOW);

   //Seriale
   Serial.begin(9600);
   
   // Load Cell
   tareValue = loadCell.averageValue(32);

   // Stepper
   pinMode(directionPin, OUTPUT);
   pinMode(stepPin, OUTPUT);
   digitalWrite(directionPin, dir);
   digitalWrite(stepPin, LOW);
}
/*****************************
         LOOP
*****************************/
void loop() {
 if(!dataStatus && digitalRead(dataButton)) dataStatus=true; //i dati della prossima prova vengono salvati
 if(dataStatus && digitalRead(dataButton)) dataStatus =false;
 if(dataStatus) digitalWrite(dataLed,HIGH);
 else digitalWrite(dataLed, LOW);
 upStatus = digitalRead(upButton);  //up premuto? sì-> modifico stato
 if(upStatus) upRoutine(); //ciclo up 
 downStatus = digitalRead(downButton); //down premuto? sì-> modifico stato 
 if(downStatus) downRoutine();  //ciclo down

}


/*****************************
         FUNCTIONS
         DECLARATIONS
*****************************/

int  getDelay(){ 
  return 500;
  int selectorValue = analogRead(delaySelector);
  #ifdef RANGEMODE
    if(selectorValue<204) return del1;
    else if(selectorValue<408) return del2;
    else if(selectorValue<613) return del3;
    else if(selectorValue<818) return del4;
    else if(selectorValue<=1023) return del5;
  #else
     return map(selectorValue,0,1023, 300,3000);
  #endif
    
}

/*
 * CIclo di salita. Può salvare dati 
 */
void upRoutine(){
  blinking(10, 100);  //il led integrato lampeggia rapidamente 10 volte: inizio routine up
  if(debug) Serial.println("UP");
  digitalWrite(directionPin, LOW);
  long startTime=millis();
  while(upStatus && !digitalRead(upButton)){//premere up per terminare il ciclo
    manualSpeedDelay=getDelay();
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(pulseLength);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(manualSpeedDelay);
    if(debug){
      Serial.println("Selected Delay:");
      Serial.print(manualSpeedDelay);
    }
  }
  long currentTime=millis();
  durata= currentTime-startTime;
  if(dataStatus){
    //inviare i dati raccolti 
    //TODO
    Serial.println("");
    Serial.println(""); 
    Serial.println("Campionamenti prova n");
    Serial.println(++counter);
    Serial.println("- durata della prova: ");
    Serial.print(durata);
  }
  upStatus=false; //ripristino gli stati
  downStatus=false;
  blinking(2,500);  //il led lampeggia 2 volte in un secondo: uscita
}
/*
 * Ciclo di discesa. Non salva dati
 */
void downRoutine(){ 
   blinking(5,100); //il led integrato lampeggia rapidamente 5 volte: inizio routine down
   if(debug) Serial.println("DOWN");
   digitalWrite(directionPin, HIGH);
   while(downStatus && !digitalRead(downButton)){//premere down per terminare il ciclo
    manualSpeedDelay=getDelay();
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(pulseLength);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(manualSpeedDelay);
    if(debug){
      Serial.println("Selected Delay:");
      Serial.print(manualSpeedDelay);
    }
  }  
   upStatus=false; //ripristino gli stati
   downStatus=false;
   blinking(2,500); //il led lampeggia 2 volte in un secondo: uscita
}


/*
 * Il led lampeggia n volte con pause di ledDel ms per mandare avvisi
*/
void blinking(int n, int ledDel){
  for(int i=0; i<n; i++){
    digitalWrite(ledPin,HIGH);
    delay(ledDel);
    digitalWrite(ledPin,LOW);
  }
}


/////TODO modalità automatiche
