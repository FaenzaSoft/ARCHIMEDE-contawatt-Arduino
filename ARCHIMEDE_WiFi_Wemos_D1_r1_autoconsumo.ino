// ARCHIMEDE WiFi Wemos D1 r1 autoconsumo 
// Ver. 1.0 del 18/01/2018
// Video YouTube: https://yotu.be/gsHxTfW92mU
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
// 4) PER AVERE LE USCITE PWM A 5 VOLT OCCORRE INSERIRE SULLA MILLEFORI IL SEMPLICE CIRCUITO ILLUSTRATO NEL VIDEO YOUTUBE (1 BC 237 + 3 RESISTENZE):
// 5) L'USCITA ON/OFF NON E' FRAZIONABILE, E' PRIORITARIA E VIENE USATA PER CARICHI QUALI POMPE DI CALORE E UTENZE A MOTORE;
// 6) L'USCITA PWM (PER CARICHI RESISTIVI)CONSENTE DI FRAZIONARE LA POTENZA, CONSENTE CIOE' DI UTILIZZARE QUALUNQUE WATT DI SURPLUS INSISTE DI MOMENTO IN MOMENTO:
// 7) SULLE USCITE COLLOCARE 2 LED DI CONTROLLO, OPPURE 2 RELE' A STATO SOLIDO CHE HANNO UN LED INCORPORATO (SI ILLUMINA QUANDO E' ATTIVATO).

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
//
// Inserire la  "Auth Token" fornita nella Blynk App:
char auth[] = "69652052ac3644e7a53afc099d3f40f9";               // Archimede contawatt:     
//
// inserire i dati del modem:
const char* ssid = "TIM-19093013";                              // sostituire con i dati del proprio router/modem:
const char* password = "DWgEViTgpBEOPcoGvvX1wh1I";              // sostituire con la password del proprio router/modem:
//
int pinV1valore = 0;
int pinV2valore = 0;
int pinV3valore = 0;
int pinV4valore = 0;
//
int pin_consumi = D3;         // valido per WEMOS D1 R1 - GPIO5:
int pin_produzione = D4;      // valido per WEMOS D1 R1 - GPIO4:
//int pin_consumi = D2;         // valido per WEMOS D1 R2 - GPIO04:
//int pin_produzione = D5;      // valido per WEMOS D1 R2 - GPIO14:
//
int pin_on_off = D11;         // valido per WEMOS D1 R1 - GPIO13:
int pin_pwm = D12;      // valido per WEMOS D1 R1 - GPIO12:
// int pin_on_off = D7;         // valido per WEMOS D1 R2 - GPIO13:
// int pin_pwm = D6;      // valido per WEMOS D1 R2 - GPIO12:
// ***************************************************************************
int watt_pdc = 300;         // potenza nominale pompa di calore:
int watt_boiler = 500;      // potenza resistenza di carico variabile:
// ***************************************************************************
unsigned long consumo_boiler = 0;
unsigned long consumo_pdc = 0;
int potenza_watt_consumi = 0;
int consumi_lettura1 = 0;
int consumi_lettura2 = 0;
int consumi_lettura3 = 0;
int potenza_watt_produzione = 0;
int buttonState_consumi = 0;
int buttonState_produzione = 0;
int ciclopwm = 0;
int ciclopwm12 = 0;
int aumento = 0;
int surplus = 0;
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
  Blynk.virtualWrite(V4, pinV4valore);    // surplus
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
  pinMode(D11, OUTPUT);   // valido per D1 r1 - GPIO13:
  pinMode(D12, OUTPUT);   // valido per D1 r1 - GPIO12:
  // pinMode(D7, OUTPUT);   // valido per D1 r2 - GPIO13 - PIEDINO 10:
  // pinMode(D6, OUTPUT);   // valido per D1 r2 - GPIO12 - PIEDINO 9:
  pinMode(D3, INPUT);   // valido per D1 r1 - GPIO5:
  pinMode(D4, INPUT);   // valido per D1 r1 - GPIO4:
  // pinMode(D5, INPUT);   // valido per D1 r2 - GPIO14 - PIEDINO 8:
  // pinMode(D2, INPUT);   // valido per D1 r2 - GPIO4 . PIEDINO 5:
}
void loop()
{
  delay(10);
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
  //
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
      consumi_lettura3 = consumi_lettura2;
      consumi_lettura2 = consumi_lettura1;
      consumi_lettura1 = potenza_watt_consumi;
      potenza_watt_consumi = (consumi_lettura1 + consumi_lettura2 + consumi_lettura3) / 3;  
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
  // if (tempo_adesso > timer_rele_vecchio + 300000)     // sono passati 5 minuti:     va bene:
  if (tempo_adesso > timer_rele_vecchio + 6000)     // sono passati 10 secondi:   usare tempi modesti solo per simulazione, c'è lo spunto del motore:
  { 
    timer_rele_vecchio = tempo_adesso;
    if (rele_on_off == 0 && potenza_watt_produzione > (potenza_watt_consumi + watt_pdc - consumo_boiler))     // il relè ON-OFF si attiva solo quando c'è un surplus di 300 watt:
    {
      rele_on_off = 1;     // HIGH:
      analogWrite(pin_on_off, 1023);     // valido per entrambe le versioni:
      consumo_pdc = watt_pdc;
    }
    if (rele_on_off == 1 && potenza_watt_produzione < potenza_watt_consumi)
    {
      rele_on_off = 0;     // LOW:
      analogWrite(pin_on_off, 0);     // valido per entrambe le versioni:
      consumo_pdc = 0;
    }
  }
  // fine attivazione relè a stato solido ON_OFF ************************ POMPA DI CALORE****************************************:
  //
  // attivazione relè a statosolido gestito PWM, cioè con carichi frazionali***BOILER********************************************:
  if (modifica_rilevazioni == 1)
  {
    if (potenza_watt_produzione > potenza_watt_consumi)
    {
      aumento = (potenza_watt_produzione - potenza_watt_consumi) * 1024 / 3000;        // ridurre il denominatore (3000) per più velocità:
      ciclopwm12 = ciclopwm12 + aumento;
    }
    else
    {
      aumento = (potenza_watt_consumi - potenza_watt_produzione) * 1024 / 3000;        // ridurre il denominatore (3000) per più velocità:
      ciclopwm12 = ciclopwm12 - aumento;
    }
    if (ciclopwm12 > 1023) ciclopwm12 = 1023;
    if (ciclopwm12 < 0) ciclopwm12 = 0;
    // analogWrite(D12, ciclopwm12);    // valido per r1:
    // analogWrite(D6, ciclopwm12);     // valido per r2:
    analogWrite(pin_pwm, ciclopwm12);     // valido per entrambe le versioni:
    modifica_rilevazioni = 0;
  }
  //
  // fine attivazione uscita PWM per relè a stato solido *************************************************************************:
  //
  surplus = potenza_watt_produzione - potenza_watt_consumi;
  //
  pinV1valore = potenza_watt_produzione;
  pinV2valore = potenza_watt_consumi;
  //
  // pinV3valore = conta / 10;
  pinV3valore = ciclopwm12;
  pinV4valore = surplus;
  //
  //
}
//FINE ARCHIMEDE WEMOS D1 autoconsumo








