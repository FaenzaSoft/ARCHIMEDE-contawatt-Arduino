// ARCHIMEDE WiFi Wemos D1 autoconsumo Ver. 0.2 del 5/1/2018
// Video YouTube: https://youtu.be/gsHxTfW92mU
/*************************************************************
    Occorre caricare nello smartphone l'APP Blynk
    Scheda: Wemos D1 r2 o Wemos D1 (Retired)
    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app
    Vedi anche: http://www.maffucci.it/tag/wemos-d1-mini/
 */
// 1) SI PUO' UTILIZZARE LA SCHEDA WEMOS D1 r1 (Retired) OPPURE LA WEMOS D1 r2, OCCORRE PERO' MODIFICARE I PIN DI ALLACCIO COME INDICATO:
// 2) LA SCHEDA VA AGGIORNATA CON LE 4 RESISTENZE DI INTERFACCIA CON I CONTATORI DIGITALI (ESCLUSE NELLA VERSIONE SIMULATORE):
// 3) SI PUO' UTILIZZARE QUESTO SOFTWARE IN VERSIONE SIMULATORE PER CONTROLLARE IL CORRETTO FUNZIONAMENTO DELLE USCITE PWM AL VARIARE DI PRODUZIONE E CONSUMI:
// 4) PER AVERE LE USCITE PWM A 5 VOLT OCCORRE INSERIRE SULLA MILLEFORI IL SEMPLICE CIRCUITO ILLUSTRATO NEL VIDEO YOUTUBE (1 BC 107 + 3 RESISTENZE):
// 5) L'USCITA ON/OFF NON E' FRAZIONABILE, E' PRIORITARIA E VIENE USATA PER CARICHI QUALI POMPE DI CALORE E UTENZE A MOTORE;
// 6) L'USCITA PWM (PER CARICHI RESISTIVI)CONSENTE DI FRAZIONARE LA POTENZA, CONSENTE CIOE' DI UTILIZZARE QUALUNQUE WATT DI SURPLUS INSISTE DI MOMENTO IN MOMENTO:
// 7) SULLE USCITE COLLOCARE 2 LED DI CONTROLLO, OPPURE 2 RELE' A STATO SOLIDO CHE HANNO UN LED INCORPORATO (SI ILLUMINA QUANDO E' ATTIVATO).

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
//
// Inserire la  "Auth Token" fornita nella Blynk App:
char auth[] = "e54498f9f8ea075c3ef477ea7033d59e";            
//
// inserire i dati del modem:
const char* ssid = "TIM-19390013";                             // sostituire con i dati del proprio router/modem:
const char* password = "DWgEvX1wh1PViTgpBEOcoGvI";             // sostituire con la password del proprio router/modem:
//
int pinV1valore = 0;
int pinV2valore = 0;
int pinV3valore = 0;
//
// int pin_consumi = D3;         // valido per WEMOS D1 R1 - GPIO5:
// int pin_produzione = D4;      // valido per WEMOS D1 R1 - GPIO4:
int pin_consumi = D2;         // valido per WEMOS D1 R2 - GPIO04:
int pin_produzione = D5;      // valido per WEMOS D1 R2 - GPIO14:
//
// int pin_on_off = D11;         // valido per WEMOS D1 R1 - GPIO13:
// int pin_pwm = D12;      // valido per WEMOS D1 R1 - GPIO12:
int pin_on_off = D7;         // valido per WEMOS D1 R2 - GPIO13:
int pin_pwm = D6;      // valido per WEMOS D1 R2 - GPIO12:
//
int potenza_watt_consumi = 0;
int potenza_watt_produzione = 0;
int buttonState_consumi = 0;
int buttonState_produzione = 0;
int ciclopwm1 = 0;
int ciclopwm2 = 0;
int aumento = 0;
unsigned long timer_rele_nuovo = 0;
unsigned long timer_rele_vecchio = 0;
int rele_on_off = 0;          // è uguale a LOW:
unsigned long corrente_millis_consumi = 0;
unsigned long vecchio_millis_consumi = 0;
unsigned long tempo_trascorso_consumi = 0;
unsigned long corrente_millis_produzione = 0;
unsigned long vecchio_millis_produzione = 0;
unsigned long tempo_trascorso_produzione = 0;
unsigned long tempo_ultima_lettura_consumi = 0;
unsigned long tempo_ultima_lettura_produzione = 0;
unsigned long tempo_adesso = 0;
int conta = 100;
int modifica_rilevazioni = 0;
//
BlynkTimer timer;
//
void myTimerEvent()
//
{
  //
  Blynk.virtualWrite(V1, pinV1valore);    // produzione
  Blynk.virtualWrite(V2, pinV2valore);    // consumi
  Blynk.virtualWrite(V3, pinV3valore);    // conta
}
//
void setup()
{
  // Debug console
  Serial.begin(9600);
  //
  Blynk.begin(auth, ssid, password);
  timer.setInterval(1000L, myTimerEvent);
  //
  // pinMode(D11, OUTPUT);   // valido per D1 r1 - GPIO13:
  // pinMode(D12, OUTPUT);   // valido per D1 r1 - GPIO12:
  pinMode(D7, OUTPUT);   // valido per D1 r2 - GPIO13 - PIEDINO 10:
  pinMode(D6, OUTPUT);   // valido per D1 r2 - GPIO12 - PIEDINO 9:
  // pinMode(D3, INPUT);   // valido per D1 r1 - GPIO5:
  // pinMode(D4, INPUT);   // valido per D1 r1 - GPIO4:
  pinMode(D5, INPUT);   // valido per D1 r2 - GPIO14 - PIEDINO 8:
  pinMode(D2, INPUT);   // valido per D1 r2 - GPIO4 . PIEDINO 5:
}
void loop()
{
  // delay(100);         //inserire delay(100) solo per simulazione:
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
  //
  // ********* versione simulatore, 4 righe da inserire nel codice per simulare la variazione di produzione e consumi******************
  //  potenza_watt_produzione = potenza_watt_produzione + 3;
  //  if (potenza_watt_produzione > 2000) potenza_watt_produzione = 1;
  //  potenza_watt_consumi =  potenza_watt_consumi + 1;
  //  if (potenza_watt_consumi > 2000) potenza_watt_consumi = 1;
  // *********** fine righe simulatore *******************************************************************
  conta = conta + 10;
  if (conta > 10100) conta = 0;
  //
  tempo_adesso = millis();
  //
  if (tempo_adesso > tempo_ultima_lettura_consumi + 100)         // bisogna che siano passati almeno 100 millisecondi da ultima lettura:
  {
    buttonState_consumi = digitalRead(pin_consumi);
    if (buttonState_consumi == 1)   // cioè HIGH:
    {
      tempo_ultima_lettura_consumi = tempo_adesso;
      corrente_millis_consumi = tempo_adesso;
      tempo_trascorso_consumi = (corrente_millis_consumi - vecchio_millis_consumi);
      vecchio_millis_consumi = corrente_millis_consumi;
      modifica_rilevazioni = 1;
      // riga sottostante da togliere, per versione simulatore:
      potenza_watt_consumi = 1000000 / tempo_trascorso_consumi * 36 / 32;       // da modificare con contatori diversi da 3200 impulsi:
    }
  }
  //
  if (tempo_adesso > tempo_ultima_lettura_produzione + 100)         // bisogna che siano passati almeno 100 millisecondi da ultima lettura:
  {
    buttonState_produzione = digitalRead(pin_produzione);
    if (buttonState_produzione == 1)   // cioè HIGH:
    {
      tempo_ultima_lettura_produzione = tempo_adesso;
      corrente_millis_produzione = tempo_adesso;
      tempo_trascorso_produzione = (corrente_millis_produzione - vecchio_millis_produzione);
      vecchio_millis_produzione = corrente_millis_produzione;
      modifica_rilevazioni = 1;
      // riga sottostante da togliere, per versione simulatore:
      potenza_watt_produzione = 1000000 / tempo_trascorso_produzione * 36 / 32;           // da modificare con contatori diversi da 3200 impulsi:
    }
  }
  //
  // attivazione relè a stato solido con priorita rele ON OFF ***********POMPA DI CALORE********************************************
  // if (tempo_adesso > timer_rele_vecchio + 300000)     // sono passati 5 minuti:
  if (tempo_adesso > timer_rele_vecchio + 6000)     // sono passati 10 secondi:
  {
    timer_rele_vecchio = tempo_adesso;
    if (potenza_watt_produzione > potenza_watt_consumi);
    {
      if (potenza_watt_produzione > potenza_watt_consumi + 300)     // il relè ON-OFF si attiva solo quando c'è un surplus di 300 watt:
      {
        rele_on_off = 1;     // HIGH:
        // analogWrite(D11, 1023);     // valido per r1:
        // analogWrite(D7, 1023);     // valido per r2:
        analogWrite(pin_on_off, 1023);     // valido per entrambe le versioni:
      }
      else
      {
        rele_on_off = 0;     // LOW:
        // analogWrite(D11, 0);        // valido per r1:
        // analogWrite(D7, 0);        // valido per r2:
        analogWrite(pin_on_off, 0);     // valido per entrambe le versioni:
      }
    }
    if (potenza_watt_produzione < potenza_watt_consumi)
    {
      rele_on_off = 0;    //LOW:
      // analogWrite(D11, 0);         // valido per r1:
      // analogWrite(D7, 0);        // valido per r2:
      analogWrite(pin_on_off, 0);     // valido per entrambe le versioni:
    }
  }
  // fine attivazione relè a stato solido ON_OFF ************************ POMPA DI CALORE****************************************:
  //
  // attivazione relè a stato solido gestito PWM, cioè con carichi frazionali***BOILER*******************************************:
  // occorre modificare il duty_cycle ogni volta che arriva un impulso da uno dei due contatori digitali:
  if (modifica_rilevazioni == 1)            // togliere questa riga per simulazione:
  {          // togliere questa parentesi per simulazione:
    if (potenza_watt_produzione > potenza_watt_consumi)        // significa che c'è un surplus:
    {
      aumento = (potenza_watt_produzione - potenza_watt_consumi) * 1024 / 1000;
      ciclopwm1 = ciclopwm1 + aumento;
    }
    else
    {
      aumento = (potenza_watt_consumi - potenza_watt_produzione) * 1024 / 1000;
      ciclopwm1 = ciclopwm1 - aumento;
    }
    if (ciclopwm1 > 1023) ciclopwm1 = 1023;
    if (ciclopwm1 < 0) ciclopwm1 = 0;
    // analogWrite(D12, ciclopwm1);     // valido per r1:
    //analogWrite(D6, ciclopwm1);     // valido per r2:
    analogWrite(pin_pwm, ciclopwm1);     // valido per entrambe le versioni:
    modifica_rilevazioni = 0;
  }      // togliere questa parentesi per simulazione
  //
  // fine attivazione uscita PWM per relè a stato solido *************************************************************************:
  //
  pinV1valore = potenza_watt_produzione;
  pinV2valore = potenza_watt_consumi;
  //
  // pinV3valore = conta / 10;
  pinV3valore = ciclopwm1;
  //
  //
}
//FINE ARCHIMEDE WEMOS D1 autoconsumo








