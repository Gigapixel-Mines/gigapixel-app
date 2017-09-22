#include "AFMotor.h"
#include <Servo.h>

#define INTER_HORI_1 18
#define INTER_HORI_2 19
#define INTER_VERTI_1 20
#define INTER_VERTI_2 21
#define INTER_FOCAL_1 2
#define INTER_FOCAL_2 3

#define MOT_HORI_1 24
#define MOT_HORI_2 26
#define MOT_HORI_3 28
#define MOT_HORI_4 30
#define MOT_VERTI_1 37
#define MOT_VERTI_2 43
#define MOT_VERTI_3 41
#define MOT_VERTI_4 39

#define MOT_FOCAL_1 13 // pas utilisé
#define MOT_FOCAL_2 18 // pas utilisé

#define MOT_SERVO 10

#define PWM_FOCAL 11

#define BUT_HORI_GAUCHE 52
#define BUT_HORI_DROITE 42
#define BUT_VERTI_BAS 46
#define BUT_VERTI_HAUT 40
#define BUT_FOCAL_ARRIERE 50
#define BUT_FOCAL_AVANT 48
#define BUT_STEPPER 49

#define POTAR 14

#define FORWARD_MOT 1     // direction of rotation
#define BACKWARD_MOT 2    // direction of rotation

Servo myservo;
int posServo = 0;

int angleStepper = 0;
int vitesseStepper = 10;
int cransParTourStepper = 200;
boolean enableStepper = false;
AF_DCMotor focalMotor(4, MOTOR12_64KHZ); // 64KHz pwm
AF_Stepper polariseurMotor(cransParTourStepper, 1);

unsigned long lastMilli = 0;      // loop timing
unsigned long lastMilliPrint = 0; // loop timing
long count = 0;                   // rotation counter
long countInit;
long tickNumber = 0;
boolean runFocal = false;         // motor moves
boolean manualMode = false;

int incomingByte = 0;   // port serie
int absoluteVerti = 0;  // valeur absolue du déplacement vertical
int absoluteHori = 0;   // valeur absolue du déplacement horizontal
int relativeHori = 0;   // valeur relative du déplacement horizontal
int relativeVerti = 0;  // valeur relative du déplacement vertical
int potarValue = 0;
int pwmSeuil = 55;
int seuilBackwardFocal = 120;
int seuilForwardFocal = 140;
int maxPwm = 255;

int nbrCransParPasHori = 6903;    // valeur par défaut pour caméra Mako G503
int nbrCransParPasVerti = 8200;   // valeur par défaut pour caméra Mako G503

int coordCransHori = 0;
int coordPasHori = 0;
int coordCransVerti = 0;
int coordPasVerti = 0;

boolean sendSpecPos = false;
int compteurCransMmHori = 0;
int compteurCransMmVerti = 0;
int cransParMmHori = 1274;
int cransParMmVerti = 2018;

int periode = 20000;// période entre chaque début d'impulsion en microsecondes

void setup() {

  myservo.attach(MOT_SERVO);
  disablePolarisation(0);
  
  pinMode(PWM_FOCAL, OUTPUT);
  pinMode(INTER_FOCAL_1, INPUT);
  pinMode(INTER_FOCAL_2, INPUT);
  digitalWrite(INTER_FOCAL_1, HIGH);
  digitalWrite(INTER_FOCAL_2, HIGH);
  attachInterrupt(1, rencoder, FALLING);

  attachInterrupt(digitalPinToInterrupt(INTER_VERTI_2), interruptVerti2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(INTER_VERTI_1), interruptVerti1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(INTER_HORI_1), interruptHori1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(INTER_HORI_2), interruptHori2, CHANGE);
  
  
  // moteurs verti
  pinMode(MOT_VERTI_1,OUTPUT);
  pinMode(MOT_VERTI_2,OUTPUT);
  pinMode(MOT_VERTI_3,OUTPUT);
  pinMode(MOT_VERTI_4,OUTPUT);
  
  // moteurs hori
  pinMode(MOT_HORI_1,OUTPUT); 
  pinMode(MOT_HORI_2,OUTPUT); 
  pinMode(MOT_HORI_3,OUTPUT); 
  pinMode(MOT_HORI_4,OUTPUT); 
  
  // capteur
  pinMode(BUT_VERTI_BAS,INPUT_PULLUP);
  pinMode(BUT_VERTI_HAUT,INPUT_PULLUP);
  pinMode(BUT_FOCAL_ARRIERE,INPUT_PULLUP);
  pinMode(BUT_FOCAL_AVANT,INPUT_PULLUP);
  pinMode(BUT_HORI_GAUCHE,INPUT_PULLUP);
  pinMode(BUT_HORI_DROITE,INPUT_PULLUP);
  pinMode(BUT_STEPPER,INPUT_PULLUP);

  // potentiometre
  pinMode(POTAR, OUTPUT);

  //Initialisation série
  Serial.begin(9600);
}

void loop() {

  if (Serial.available()){ 
    incomingByte = Serial.read();
  }

if(manualMode==true) {
  potarValue = analogRead(POTAR)/4;    // read the value from the sensor

  if(potarValue<seuilBackwardFocal) {
    if(digitalRead(BUT_FOCAL_AVANT)==HIGH) {
      if((seuilBackwardFocal-potarValue)*(maxPwm/seuilBackwardFocal)>pwmSeuil) {
        moveMotor(BACKWARD_MOT, (seuilBackwardFocal-potarValue)*(maxPwm/seuilBackwardFocal), 1);
      }
    } else {
      motorBrake();
    }
  } else if(potarValue>seuilForwardFocal){
    if(digitalRead(BUT_FOCAL_ARRIERE)==HIGH) {
      if((potarValue-seuilForwardFocal)*(maxPwm/seuilBackwardFocal)>pwmSeuil) {
        moveMotor(FORWARD_MOT, (potarValue-seuilForwardFocal)*(maxPwm/seuilBackwardFocal), 1);
      }
    } else {
      motorBrake();
    }
  }

}

  switch (incomingByte) {
  
    case 97 : //a
      avanceeMoteurVertiBas(1,0);
      break;
    
    case 98 : //b
      avanceeMoteurVertiHaut(1,0);
      break;
    
    case 99 : //c
      avanceeMoteurHoriGauche(1,0);
      break;
    
    case 100 : //d
      avanceeMoteurHoriDroite(1,0);
      break;
    
    case 101 : //e
      miseAuPointAuto();
      break;
    
    case 102 : //f
      miseAuPointManuelleStart();
      break;
    
    case 103 : //g
      miseAuPointManuelleStop();
      break;
    
    case 104 : //h
      miseAuDepartXY();
      break;

    case 105: //i
      setNbrCransHori();
      break;

    case 106: //j
      setNbrCransVerti();
      break;

    case 107: //k
      goToXY();
      break;

/*
    case 108: //l
      enableSpecPos();
      break;

    case 109: //m
      disableSpecPos();
      break;
*/

    case 110: //n
      enablePolarisation();
      break;

    case 111: //o
      disablePolarisation(1);
      break;

    case 112: //p
      choosePolarisation();
      break;

/*
    case 113: //q
      moveStepperToAngle();
      break;
      
    case 114: //r
      setZeroStepper();
      break;
*/

    case 115: //s
      relativeCransToXY();
      break;
    
    default: 
      break;
  
  }
  incomingByte = 0; //Vider le byte de commande
  
}

void moveMotor(int direction, int PWM_val, long tick)  {
  countInit = count;    // abs(count)
  tickNumber = tick;
  if(direction==FORWARD_MOT){
    motorForward(PWM_val);
  } else if(direction==BACKWARD_MOT){
    motorBackward(PWM_val);
  }
}

void rencoder() { // pulse and direction, direct port reading to save cycles
  if(manualMode==false) {
    if(digitalRead(INTER_FOCAL_1)==HIGH){
      count++;
    } else{
      count--;
    }
    
    if(runFocal){
      if((abs(abs(count)-abs(countInit))) >= tickNumber){
        motorBrake();
      }
    }
  }
}

void motorForward(int PWM_val)  {
  focalMotor.setSpeed(PWM_val);
  focalMotor.run(FORWARD);
  runFocal = true;
}

void motorBackward(int PWM_val)  {
  focalMotor.setSpeed(PWM_val);
  focalMotor.run(BACKWARD);
  runFocal = true;
}

void motorBrake()  {
  focalMotor.setSpeed(0);
  focalMotor.run(RELEASE);
  runFocal = false;
}

// Programme de comptage tour moteur vertical
void interruptVerti2(){
  if( digitalRead(INTER_VERTI_2) == HIGH and digitalRead(INTER_VERTI_1) == HIGH){
    absoluteVerti++;
    relativeVerti++;
    //compteurCransMmVerti++;
  }
  else if(digitalRead(INTER_VERTI_2) == HIGH and digitalRead(INTER_VERTI_1) == LOW){
    absoluteVerti--;
    relativeVerti--;
    //compteurCransMmVerti--;
  }
  else if( digitalRead(INTER_VERTI_2) == LOW and digitalRead(INTER_VERTI_1) == LOW){
    absoluteVerti++;
    relativeVerti++;
    //compteurCransMmVerti++;
  }
  else if(digitalRead(INTER_VERTI_2) == LOW and digitalRead(INTER_VERTI_1) == HIGH){
    absoluteVerti--;
    relativeVerti--;
    //compteurCransMmVerti--;
  }
  /*
  if(sendSpecPos) {
    if(compteurCransMmVerti>=cransParMmVerti) {
      compteurCransMmVerti = 0;
      Serial.flush();
      Serial.print("x");  
    } else if(compteurCransMmVerti<=0) {
      compteurCransMmVerti = cransParMmVerti-1;
      Serial.flush();
      Serial.print("x"); 
    }
  }
  */
  
}
void interruptVerti1(){
  if( digitalRead(INTER_VERTI_2) == LOW and digitalRead(INTER_VERTI_1) == HIGH){
    absoluteVerti++;
    relativeVerti++;
    //compteurCransMmVerti++;
  }
  else if(digitalRead(INTER_VERTI_2) == HIGH and digitalRead(INTER_VERTI_1) == HIGH){
    absoluteVerti--;
    relativeVerti--;
    //compteurCransMmVerti--;
  }
  else if( digitalRead(INTER_VERTI_2) == HIGH and digitalRead(INTER_VERTI_1) == LOW){
    absoluteVerti++;
    relativeVerti++;
    //compteurCransMmVerti++;
  }
  else if(digitalRead(INTER_VERTI_2) == LOW and digitalRead(INTER_VERTI_1) == LOW){
    absoluteVerti--;
    relativeVerti--;
    //compteurCransMmVerti--;
  }

/*
  if(sendSpecPos) {
    if(compteurCransMmVerti>=cransParMmVerti) {
      compteurCransMmVerti = 0;
      Serial.flush();
      Serial.print("x");  
    } else if(compteurCransMmVerti<=0) {
      compteurCransMmVerti = cransParMmVerti-1;
      Serial.flush();
      Serial.print("x"); 
    }
  }
  */
  
}

// Programme de comptage tour moteur horizontal
void interruptHori1(){
  if( digitalRead(INTER_HORI_1) == HIGH and digitalRead(INTER_HORI_2) == HIGH){
    absoluteHori++;
    relativeHori++;
    //compteurCransMmHori++;
  }
  else if(digitalRead(INTER_HORI_1) == HIGH and digitalRead(INTER_HORI_2) == LOW){
    absoluteHori--;
    relativeHori--;
    //compteurCransMmHori--;
    }
  else if( digitalRead(INTER_HORI_1) == LOW and digitalRead(INTER_HORI_2) == LOW){
    absoluteHori++;
    relativeHori++;
    //compteurCransMmHori++;
  }
  else if(digitalRead(INTER_HORI_1) == LOW and digitalRead(INTER_HORI_2) == HIGH){
    absoluteHori--;
    relativeHori--;
    //compteurCransMmHori--;
  }

/*
  if(sendSpecPos) {
    if(compteurCransMmHori>=cransParMmHori) {
      compteurCransMmHori = 0;
      Serial.flush();
      Serial.print("x");  
    } else if(compteurCransMmHori<=0) {
      compteurCransMmHori = cransParMmHori-1;
      Serial.flush();
      Serial.print("x"); 
    }
  }
  */
    
}

void interruptHori2(){
  if( digitalRead(INTER_HORI_1) == LOW and digitalRead(INTER_HORI_2) == HIGH){
    absoluteHori++;
    relativeHori++;
    //compteurCransMmHori++;
  }
  else if(digitalRead(INTER_HORI_1) == HIGH and digitalRead(INTER_HORI_2) == HIGH){
    absoluteHori--;
    relativeHori--;
    //compteurCransMmHori--;
  }
  else if( digitalRead(INTER_HORI_1) == HIGH and digitalRead(INTER_HORI_2) == LOW){
    absoluteHori++;
    relativeHori++;
    //compteurCransMmHori++;
  }
  else if(digitalRead(INTER_HORI_1) == LOW and digitalRead(INTER_HORI_2) == LOW){
    absoluteHori--;
    relativeHori--;
    //compteurCransMmHori--;
  }

/*
  if(sendSpecPos) {
    if(compteurCransMmHori>=cransParMmHori) {
      compteurCransMmHori = 0;
      Serial.flush();
      Serial.print("x");  
    } else if(compteurCransMmHori<=0) {
      compteurCransMmHori = cransParMmHori-1;
      Serial.flush();
      Serial.print("x"); 
    }
  }
  */
    
}

// Programme avancée des moteurs pour translation
void avanceeMoteurVertiBas(int nbrPas, int nbrCrans){
    incomingByte = 0;
    //Effectuer les pas
    for (int compteur = 0; compteur < nbrPas; ++compteur)
    {
      relativeVerti=0;
      while (relativeVerti < nbrCransParPasVerti and digitalRead(BUT_VERTI_BAS)==HIGH) {
        digitalWrite(MOT_VERTI_3,HIGH);
        digitalWrite(MOT_VERTI_4,HIGH);       
      }
      coordPasVerti--;
    }
      
    //Effectuer les crans supplémentaires
    relativeVerti=0;
    while (relativeVerti < nbrCrans and digitalRead(BUT_VERTI_BAS)==HIGH) {
      digitalWrite(MOT_VERTI_3,HIGH);
      digitalWrite(MOT_VERTI_4,HIGH);       
    }    
    digitalWrite(MOT_VERTI_3,LOW);
    digitalWrite(MOT_VERTI_4,LOW);

    coordCransVerti -= nbrCrans;
    if (coordCransVerti < 0)
    {
      --coordPasVerti;
      coordCransVerti = coordCransVerti % nbrCransParPasVerti;
    }

    /*
    Serial.print("coordPasHori : ");
    Serial.println(coordPasHori);
    Serial.print("coordCranssHori : ");
    Serial.println(coordCransHori);
    Serial.print("coordPasVerti : ");
    Serial.println(coordPasVerti);
    Serial.print("coordCranssVerti : ");
    Serial.println(coordCransVerti);
    */
    
    Serial.flush();
    Serial.print("z");   
}

void avanceeMoteurVertiHaut(int nbrPas, int nbrCrans){
    incomingByte = 0;
    //Effectuer les pas
    for (int compteur = 0; compteur < nbrPas; ++compteur)
    {
      relativeVerti=0;
      while (relativeVerti > -nbrCransParPasVerti and digitalRead(BUT_VERTI_HAUT)==HIGH) {
        digitalWrite(MOT_VERTI_1,HIGH);
        digitalWrite(MOT_VERTI_2,HIGH); 
      }
      coordPasVerti++; 
    }
    //Effectuer les crans supplémentaires
    relativeVerti=0;
    while (relativeVerti > -nbrCrans and digitalRead(BUT_VERTI_HAUT)==HIGH) {
      digitalWrite(MOT_VERTI_1,HIGH);
      digitalWrite(MOT_VERTI_2,HIGH); 
    }
    digitalWrite(MOT_VERTI_1,LOW);
    digitalWrite(MOT_VERTI_2,LOW);

    coordCransVerti += nbrCrans;
    if (coordCransVerti > nbrCransParPasVerti)
    {
      ++coordPasVerti;
      coordCransVerti = coordCransVerti % nbrCransParPasVerti;
    }

    /*
    Serial.print("coordPasHori : ");
    Serial.println(coordPasHori);
    Serial.print("coordCranssHori : ");
    Serial.println(coordCransHori);
    Serial.print("coordPasVerti : ");
    Serial.println(coordPasVerti);
    Serial.print("coordCranssVerti : ");
    Serial.println(coordCransVerti);
    */
    
    Serial.flush();
    Serial.print("z");  
}

void avanceeMoteurHoriGauche(int nbrPas, int nbrCrans){
    incomingByte = 0;
    //Effectuer les pas
    for (int compteur = 0; compteur < nbrPas; ++compteur)
    {
      relativeHori=0;
      while (relativeHori < nbrCransParPasHori and digitalRead(BUT_HORI_GAUCHE)==HIGH) {
        digitalWrite(MOT_HORI_3,HIGH);
        digitalWrite(MOT_HORI_4,HIGH);       
      }
      coordPasHori--;
    }
    //Effectuer les crans supplémentaires
    relativeHori=0;
    while (relativeHori < nbrCrans and digitalRead(BUT_HORI_GAUCHE)==HIGH) {
      digitalWrite(MOT_HORI_3,HIGH);
      digitalWrite(MOT_HORI_4,HIGH);       
    }    
    digitalWrite(MOT_HORI_3,LOW);
    digitalWrite(MOT_HORI_4,LOW);

    coordCransHori -= nbrCrans;
    if (coordCransHori < 0)
    {
      --coordPasHori;
      coordCransHori = coordCransHori % nbrCransParPasHori;
    }

    /*
    Serial.print("coordPasHori : ");
    Serial.println(coordPasHori);
    Serial.print("coordCranssHori : ");
    Serial.println(coordCransHori);
    Serial.print("coordPasVerti : ");
    Serial.println(coordPasVerti);
    Serial.print("coordCranssVerti : ");
    Serial.println(coordCransVerti);
    */
    
    Serial.flush();
    Serial.print("z");    
}

void avanceeMoteurHoriDroite(int nbrPas, int nbrCrans){
    incomingByte = 0;
    //Effectuer les pas
    for (int compteur = 0; compteur < nbrPas; ++compteur)
    {
      relativeHori=0;
      while (relativeHori > -nbrCransParPasHori and digitalRead(BUT_HORI_DROITE)==HIGH) {
        digitalWrite(MOT_HORI_1,HIGH);
        digitalWrite(MOT_HORI_2,HIGH);       
      }
      coordPasHori++; 
    }
    //Effectuer les crans supplémentaires
    relativeHori=0;
    while (relativeHori > -nbrCrans and digitalRead(BUT_HORI_DROITE)==HIGH) {
      digitalWrite(MOT_HORI_1,HIGH);
      digitalWrite(MOT_HORI_2,HIGH);       
    }    
    digitalWrite(MOT_HORI_1,LOW);
    digitalWrite(MOT_HORI_2,LOW);

    coordCransHori += nbrCrans;
    if (coordCransHori > nbrCransParPasHori)
    {
      ++coordPasHori;
      coordCransHori = coordCransHori % nbrCransParPasHori;
    }

    /*
    Serial.print("coordPasHori : ");
    Serial.println(coordPasHori);
    Serial.print("coordCranssHori : ");
    Serial.println(coordCransHori);
    Serial.print("coordPasVerti : ");
    Serial.println(coordPasVerti);
    Serial.print("coordCranssVerti : ");
    Serial.println(coordCransVerti);
    */
    
    Serial.flush();
    Serial.print("z");     
}

void miseAuPointAuto(){
  incomingByte = 0;
  Serial.flush();
  Serial.println("e");
}

void miseAuPointManuelleStart(){
  incomingByte = 0;

  if(manualMode==false) {
    //potarValue = analogRead(POTAR)/4;
    
    //if(potarValue>seuilBackwardFocal && potarValue<seuilForwardFocal) {
      manualMode = true;
      Serial.flush();
      Serial.println("f");
      /*
    } else {
      Serial.flush();
      Serial.println("x");
    }
    */
  }

}

void miseAuPointManuelleStop(){
  incomingByte = 0;

  if(manualMode==true) {
    //potarValue = analogRead(POTAR)/4;
    //if(potarValue>seuilBackwardFocal && potarValue<seuilForwardFocal) {
      manualMode = false;
      motorBrake();
      Serial.flush();
      Serial.println("g");
      /*
    } else {
      Serial.flush();
      Serial.println("x");
    }
    */
    
  }

}

void miseAuDepartXY(){
  compteurCransMmHori = 0;
  compteurCransMmVerti = 0;
  incomingByte = 0;
  while(digitalRead(BUT_HORI_GAUCHE)==HIGH){
    digitalWrite(MOT_HORI_3,HIGH);
    digitalWrite(MOT_HORI_4,HIGH);
  }
  digitalWrite(MOT_HORI_3,LOW);
  digitalWrite(MOT_HORI_4,LOW);
     
  while(digitalRead(BUT_VERTI_BAS)==HIGH){
    digitalWrite(MOT_VERTI_3,HIGH);
    digitalWrite(MOT_VERTI_4,HIGH);
  }
  digitalWrite(MOT_VERTI_3,LOW);
  digitalWrite(MOT_VERTI_4,LOW); 

  coordCransHori = 0;
  coordCransVerti = 0;
  coordPasHori = 0;
  coordPasVerti = 0;
  
  Serial.flush();
  Serial.print("h");
}

void setNbrCransHori() {
  int strongByte = 0;
  int weakByte = 0;
    
  Serial.flush();
  Serial.print("i");
  while (Serial.available()==0) {}
  strongByte = Serial.read();
  Serial.flush();
  Serial.print("i");
  while (Serial.available()==0) {}
  weakByte = Serial.read();

  nbrCransParPasHori = strongByte*256 + weakByte; // https://stackoverflow.com/a/13907166
  Serial.flush();
  Serial.print("i");
}

void setNbrCransVerti() {
  int strongByte = 0;
  int weakByte = 0; 
    
  Serial.flush();
  Serial.print("j");
  while (Serial.available()==0) {}
  strongByte = Serial.read();
  Serial.flush();
  Serial.print("j");
  while (Serial.available()==0) {}
  weakByte = Serial.read();

  nbrCransParPasVerti = strongByte*256 + weakByte; // https://stackoverflow.com/a/13907166
  Serial.flush();
  Serial.print("j");
}

void goToXY() {
  
  compteurCransMmHori = 0;
  compteurCransMmVerti = 0;  
  int weakByte = 0;
  int strongByte = 0;
  int nbrPasHoriToGo = 0;
  int nbrPasVertiToGo = 0;
  int cranHoriToGo = 0;
  int cranVertiToGo = 0;

  int diffPasHori = 0;
  int diffPasVerti = 0;
  int diffCransHori = 0;
  int diffCransVerti = 0;
  
  
  Serial.flush();
  Serial.print("k");
  while (Serial.available()==0) {}
  nbrPasHoriToGo = Serial.read();
  Serial.flush();
  Serial.print("k");
  while (Serial.available()==0) {}
  strongByte = Serial.read();
  Serial.flush();
  Serial.print("k");
  while (Serial.available()==0) {}
  weakByte = Serial.read();
  cranHoriToGo = strongByte * 256 + weakByte;
  Serial.flush();
  Serial.print("k");
  while (Serial.available()==0) {}
  nbrPasVertiToGo = Serial.read();
  Serial.flush();
  Serial.print("k");
  while (Serial.available()==0) {}
  strongByte = Serial.read();
  Serial.flush();
  Serial.print("k");
  while (Serial.available()==0) {}
  weakByte = Serial.read();
  cranVertiToGo = strongByte * 256 + weakByte;
  Serial.flush();
  Serial.print("k");

  diffPasHori =  nbrPasHoriToGo - coordPasHori;
  diffPasVerti = nbrPasVertiToGo - coordPasVerti;
  diffCransHori = cranHoriToGo - coordCransHori;
  diffCransVerti = cranVertiToGo - coordCransVerti;

  delay(1000);

  if(diffPasHori>0) {
    if(diffCransHori>=0) { // aller a droite de diffPasHori et de diffCransHori
      avanceeMoteurHoriDroite(diffPasHori,diffCransHori);
    }
    else { // aller a droite de diffPasHori-1 et de nbrCransParPasHori+diffCransHori car diffCransHori < 0 et bougeràdroite prend un paramètre > 0
      avanceeMoteurHoriDroite(diffPasHori-1,nbrCransParPasHori+diffCransHori);
    }
    
  }
  else if(diffPasHori==0) {
    if(diffCransHori>=0) { // aller a droite de diffCransHori
      avanceeMoteurHoriDroite(0,diffCransHori);
    }
    else { // aller a gauche de -diffCransHori car diffCransHori < 0 et bougeràgauche prend un paramètre > 0
      avanceeMoteurHoriGauche(0,-diffCransHori);
    }
  }
  else {
    if(diffCransHori<=0) { // aller a gauche de -diffPasHori et de nbrCransParPasHori+diffCransHori
      avanceeMoteurHoriGauche(-diffPasHori,nbrCransParPasHori+diffCransHori);
    }
    else { // aller a gauche de -diffPasHori et de diffCransHori
      avanceeMoteurHoriGauche(-diffPasHori,diffCransHori);
    }
  }

  delay(1000);
  
  if(diffPasVerti>0) {
    if(diffCransVerti>=0) { // aller a Haut de diffPasVerti et de diffCransVerti
      avanceeMoteurVertiHaut(diffPasVerti,diffCransVerti);
    }
    else { // aller a Haut de diffPasVerti-1 et de nbrCransParPasVerti+diffCransVerti car diffCransVerti < 0 et bougeràHaut prend un paramètre > 0
      avanceeMoteurVertiHaut(diffPasVerti-1,nbrCransParPasVerti+diffCransVerti);
    }
    
  }
  else if(diffPasVerti==0) {
    if(diffCransVerti>=0) { // aller a Haut de diffCransVerti
      avanceeMoteurVertiHaut(0,diffCransVerti);
    }
    else { // aller a Bas de -diffCransVerti car diffCransVerti < 0 et bougeràBas prend un paramètre > 0
      avanceeMoteurVertiBas(0,-diffCransVerti);
    }
  }
  else {
    if(diffCransVerti<=0) { // aller a Bas de -diffPasVerti et de nbrCransParPasVerti+diffCransVerti
      avanceeMoteurVertiBas(-diffPasVerti,nbrCransParPasVerti+diffCransVerti);
    }
    else { // aller a Bas de -diffPasVerti et de diffCransVerti
      avanceeMoteurVertiBas(-diffPasVerti,diffCransVerti);
    }
  }

  delay(1000);

  Serial.flush();
  Serial.print("k");

  //Serial.println(coordPasHori); // debug
  //Serial.println(coordCransHori);

}

void relativeCransToXY() {
  
  int weakByte = 0;
  int strongByte = 0;
  int directionHori = 0; //0 = gauche, 1 = droite
  int directionVerti = 0; //0 = bas, 1 = haut
  int cranHoriToGo = 0;
  int cranVertiToGo = 0;
  
  Serial.flush();
  Serial.print("s");
  while (Serial.available()==0) {}
  directionHori = Serial.read();
  
  Serial.flush();
  Serial.print("s");
  while (Serial.available()==0) {}
  strongByte = Serial.read();
  
  Serial.flush();
  Serial.print("s");
  while (Serial.available()==0) {}
  weakByte = Serial.read();

  cranHoriToGo = strongByte * 256 + weakByte;

  Serial.flush();
  Serial.print("s");
  while (Serial.available()==0) {}
  directionVerti = Serial.read();
  
  Serial.flush();
  Serial.print("s");
  while (Serial.available()==0) {}
  strongByte = Serial.read();
  
  Serial.flush();
  Serial.print("s");
  while (Serial.available()==0) {}
  weakByte = Serial.read();
  
  cranVertiToGo = strongByte * 256 + weakByte;
  Serial.flush();
  Serial.print("s");

  delay(1000);

  if(directionHori==0) {
    avanceeMoteurHoriGauche(0,cranHoriToGo);
  } else {
    avanceeMoteurHoriDroite(0,cranHoriToGo);
  }

  delay(1000);

  if(directionVerti==0) {
    avanceeMoteurVertiBas(0,cranVertiToGo);
  } else {
    avanceeMoteurVertiHaut(0,cranVertiToGo);
  }


  delay(1000);

  Serial.flush();
  Serial.print("s");

  //Serial.println(coordPasHori); // debug
  //Serial.println(coordCransHori);

}

void enableSpecPos() {
  sendSpecPos = true;
  compteurCransMmHori = 0;
  compteurCransMmVerti = 0;
  Serial.flush();
  Serial.print("l");
}

void disableSpecPos() {
  sendSpecPos = false;
  compteurCransMmHori = 0;
  compteurCransMmVerti = 0;
  Serial.flush();
  Serial.print("m");
}


void choosePolarisation() {
  while (Serial.available()==0) {}
  int polarisation = Serial.read();
  setZeroStepper();
  angleStepper = (polarisation-1)*45;
  moveStepperToAngle();
  Serial.flush();
  Serial.print("p");
}

void moveStepperToAngle () {

  if(enableStepper) {
    int nbrCrans = floor(((angleStepper*1.0)/360.0)*cransParTourStepper*4.0);
    polariseurMotor.setSpeed(vitesseStepper);
    polariseurMotor.step(nbrCrans, BACKWARD, DOUBLE);
    delay(500);
    polariseurMotor.setSpeed(0);
    polariseurMotor.step(0, BACKWARD, DOUBLE);
    polariseurMotor.release();
  }

  /*
  Serial.flush();
  Serial.print("q");

  */
}

void setZeroStepper() {

  if(enableStepper) {
  
    while(digitalRead(BUT_STEPPER)==HIGH) {
      polariseurMotor.setSpeed(vitesseStepper);
      polariseurMotor.step(1, FORWARD, DOUBLE);
    }
    
    polariseurMotor.setSpeed(1);
    polariseurMotor.step(2, BACKWARD, DOUBLE);
    polariseurMotor.release();

  }
/*
  Serial.flush();
  Serial.print("r");
  */
}

void enablePolarisation() {

  for (posServo = 90; posServo >= 0; posServo -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(posServo);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  enableStepper = true;

  Serial.flush();
  Serial.print("n");
}

void disablePolarisation(int sendText) {
  
  for (posServo = 90; posServo <= 180; posServo += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(posServo);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  enableStepper = false;

  if(sendText==1) {
    Serial.flush();
    Serial.print("o");
  }
  
}

