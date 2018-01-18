
// ARCHIMEDE WiFi Wemos D1
/*************************************************************
    Occorre caricare nello smartphone l'APP Blynk
    Scheda: Wemos D1 r2 o Wemos D1 (Retired)
    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app
    Vedi anche: http://www.maffucci.it/tag/wemos-d1-mini/
    Video YoTube: in preparazione
    Versione software: 1.2 del 18/01/2018
*/
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
//
// Inserire la  "Auth Token" fornita nella Blynk App.
char auth[] = "69c099d3f652052ac3644e7a53af40f9";                  // sostituire con quella inviata da Blink:
//
const char* ssid = "Telecom-30277373";                             // sostituire con i dati del proprio router/modem:
const char* password = "iCpV1ZNB9FjcGYNuwu8G4qYH";                 // sostituire con la password del proprio router/modem:
//
int pinV1valore = 0;
int pinV2valore = 0;
int pinV3valore = 0;
int pin_consumi = D6;         // valido per WEMOS D1 R1 e R2;
int pin_produzione = D7;      // valido per WEMOS D1 R1 e R2;
int potenza_watt_consumi = 0;
int potenza_watt_produzione = 0;
int buttonState_consumi = 0;
int buttonState_produzione = 0;
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
//
BlynkTimer timer;
//
void myTimerEvent()
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
}
void loop()
{
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
  //
  conta = conta + 1;
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
      potenza_watt_produzione = 1000000 / tempo_trascorso_produzione * 36 / 32;           // da modificare con contatori diversi da 3200 impulsi:
    }
  }
  //
  pinV1valore = potenza_watt_produzione;
  pinV2valore = potenza_watt_consumi;
  //
  pinV3valore = conta / 10;
  //
}
//FINE ARCHIMEDE WEMOS D1








