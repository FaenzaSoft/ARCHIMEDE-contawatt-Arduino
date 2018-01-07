// ARCHIMEDE WiFi Wemos D1 simulatore Ver. 0.1 del 7/1/2018
// Video YouTube HARDWARE: https://youtu.be/gsHxTfW92mU
// Video funzionamento: in preparazione
/*************************************************************
    Occorre caricare nello smartphone l'APP Blynk
    Scheda: Wemos D1 r2 o Wemos D1 (Retired)
    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app
    Vedi anche: http://www.maffucci.it/tag/wemos-d1-mini/
    Vedi anche: http://www.mauroalfieri.it/elettronica/blynk-app-setting.html
*/
// 1) SI PUO' UTILIZZARE LA SCHEDA WEMOS D1 r1 (Retired) OPPURE LA WEMOS D1 r2, OCCORRE PERO' MODIFICARE I PIN DI ALLACCIO COME INDICATO:
// 2) SI PUO' UTILIZZARE QUESTO SOFTWARE IN VERSIONE SIMULATORE PER CONTROLLARE IL CORRETTO FUNZIONAMENTO DELLE USCITE PWM AL VARIARE DI PRODUZIONE E CONSUMI:
// 3) PER AVERE LE USCITE PWM A 5 VOLT OCCORRE INSERIRE SULLA MILLEFORI IL SEMPLICE CIRCUITO ILLUSTRATO NEL VIDEO YOUTUBE (1 BC 237 + 3 RESISTENZE):
// 4) L'USCITA ON/OFF NON E' FRAZIONABILE, E' PRIORITARIA E VIENE USATA PER CARICHI QUALI POMPE DI CALORE E UTENZE A MOTORE;
// 5) L'USCITA PWM (PER CARICHI RESISTIVI)CONSENTE DI FRAZIONARE LA POTENZA, CONSENTE CIOE' DI UTILIZZARE QUALUNQUE WATT DI SURPLUS INSISTE DI MOMENTO IN MOMENTO:
// 6) SULLE USCITE COLLOCARE IN PROVA 2 LED DI CONTROLLO, OPPURE 2 RELE' A STATO SOLIDO CHE HANNO UN LED INCORPORATO (SI ILLUMINA QUANDO E' ATTIVATO).
// 7) IPOTESI DI SIMULAZIONE: PRODUZIONE CON ANDAMENTO LINEARE; CONSUMI_CASA FISSI + VARIABILI + CARICO_ON_OFF + CARICO_PWM:
// 8) VIENE VISUALIZZATO ANCHE IL SURPLUS; QUANDO E' NEGATIVO SIGNIFICA CHE STIAMO PRELEVANDO ENERGIA DALLA RETE:
// 9) QUANDO IL DATO DI SURPLUS E' VICINO ALLO ZERO SIGNIFICA CHE STIAMO UTILIZZANDO IL SURPLUS CON UNO O ENTRAMBI I CARICHI:
// 10) CON SURPLUS POSITIVO SIGNIFICA CHE PUR AVENDO ATTIVATI I CARICHI(AL 100%), RIMANE ALTRA ENERGIA CHE VIENE IMMESSA IN RETE:
// 12) IL PWM SI DEVE ADATTARE IMMEDIATAMENTE ALLE VARIAZIONI DEL SURPLUS E POSIZIONARSI NELLA ZONA CENTRALE:
// 13) CON CARICHI ATTIVATI (RELE' ACCESI) NON SI DEVE AVERE PRELòIEVI DA RETE, CIOE' SURPLUS NEGATIVO:
// 14) SE LA GESTION DEL PWM E' EFFICACE DEVE DARE IL PAREGGIO DI PRODUZIONE E CONSUMI. OVVIAMENTE SE "LAVORA" ALL'INTERNO DEL RANGE.

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
//
// Inserire la  "Auth Token" fornita nella Blynk App:
char auth[] = "fc099d3f40f969652052ac3644e7a53a";   
//
// inserire i dati del modem:
const char* ssid = "TIM-19039013";                             // sostituire con i dati del proprio router/modem:
const char* password = "PcoGvIDWgEViTgpBEOvX1wh1";             // sostituire con la password del proprio router/modem:
//
int pinV1valore = 0;
int pinV2valore = 0;
int pinV3valore = 0;
int pinV4valore = 0;
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
// 
int potenza_watt_consumi = 0;
int potenza_watt_produzione = 1000;           
//int buttonState_consumi = 0;
//int buttonState_produzione = 0;
int ciclopwm1 = 0;
int ciclopwm2 = 0;
int aumento = 0;
unsigned long timer_rele_nuovo = 0;
unsigned long timer_rele_vecchio = 0;
int rele_on_off = 0;          // è uguale a LOW:
unsigned long tempo_adesso = 0;
int conta = 0;
int modifica_rilevazioni = 0;
int random_produzione = 0;
int random_consumi = 0;
int variazione_produzione = + 3;
int watt_consumi_fissi = 50;
int watt_consumi_variabili = 0;
int watt_consumi_ON_OFF = 700;    // POMPA DI CALORE
int watt_consumi_PWM = 1000;      // BOILER
int carico_on_off = 0;
int carico_pwm = 0;
int surplus = 0;
//
BlynkTimer timer;
//
void myTimerEvent()
//
{
  //
  Blynk.virtualWrite(V1, pinV1valore);    // produzione:
  Blynk.virtualWrite(V2, pinV2valore);    // consumi:
  Blynk.virtualWrite(V3, pinV3valore);    // conta:
  Blynk.virtualWrite(V4, pinV4valore);    // surplus:
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
  delay(6);         // serve per rallentare il ciclo:
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
  //
  // si ipotizza che la produzione abbia un andamento lineare:
  random_produzione = random(1, 100);
  if (random_produzione == 33)
  {
    if (potenza_watt_produzione > 3000) variazione_produzione = -3;
    if (potenza_watt_produzione < 0) variazione_produzione = +3;
    potenza_watt_produzione = potenza_watt_produzione + variazione_produzione; 
    modifica_rilevazioni = 1;
  }
  // si ipotizza che i consumi abbiano sbalzi significativi:
  // il consumo totale è dato dalla somma di consumi fissi + consumi variabili + consumi on_off e consumi pwm:
  random_consumi = random(1, 500);
  if (random_consumi == 25)
  {
    delay(50);    // voglòiamo che le variazioni dei consumi abbiano una certa durata:
    watt_consumi_variabili = random(0,2000);  
    if (watt_consumi_variabili > 1000) delay (50); 
    potenza_watt_consumi =  watt_consumi_fissi + watt_consumi_variabili + watt_consumi_ON_OFF + watt_consumi_PWM;  
    if (potenza_watt_consumi > 2500) potenza_watt_consumi = 2500;
    modifica_rilevazioni = 1;
  }
  //
  // *********** fine righe simulatore *******************************************************************
  conta = conta + 10;
  if (conta > 10100) conta = 0;
  //
  tempo_adesso = millis();
  //
  // attivazione relè a stato solido con priorita rele ON OFF ***********POMPA DI CALORE********************************************
  // if (tempo_adesso > timer_rele_vecchio + 300000)     // sono passati 5 minuti:
  if (tempo_adesso > timer_rele_vecchio + 3000)     // sono passati 3 secondi (solo per prove tecniche):
  {
    timer_rele_vecchio = tempo_adesso;
    if (rele_on_off == 0 && potenza_watt_produzione > potenza_watt_consumi + 300)     // il relè ON-OFF si attiva solo quando c'è un surplus di 700 watt:
    {
      rele_on_off = 1;     // HIGH:
      analogWrite(pin_on_off, 1023);     // valido per entrambe le versioni:
    }
    if (potenza_watt_produzione < potenza_watt_consumi)      // mette il rele off:
    {
      rele_on_off = 0;    //LOW:
      analogWrite(pin_on_off, 0);     // valido per entrambe le versioni:
    }
  }
  //
  // attivazione relè a stato solido gestito PWM, cioè con carichi frazionali***BOILER*******************************************:
  // occorre modificare il duty_cycle ogni volta che arriva un impulso da uno dei due contatori digitali:
  if (modifica_rilevazioni == 1)
  {
    if (rele_on_off == 1) carico_on_off = watt_consumi_ON_OFF;
    if (rele_on_off == 0) carico_on_off = 0;
    carico_pwm = watt_consumi_PWM * ciclopwm1 / 1024;   
    potenza_watt_consumi = watt_consumi_fissi + watt_consumi_variabili + carico_on_off + carico_pwm;
    surplus = potenza_watt_produzione - potenza_watt_consumi;
    delay(50);
    if (surplus > 0)        // significa che c'è un surplus:
    {
      aumento = (potenza_watt_produzione - potenza_watt_consumi) * 1024 / watt_consumi_PWM;
      ciclopwm1 = ciclopwm1 + aumento;
    }
    else
    {
      aumento = (potenza_watt_consumi - potenza_watt_produzione) * 1024 / watt_consumi_PWM;
      ciclopwm1 = ciclopwm1 - aumento;
    }
    if (ciclopwm1 > 1023) ciclopwm1 = 1023;
    if (ciclopwm1 < 0) ciclopwm1 = 0;
    analogWrite(pin_pwm, ciclopwm1);     // valido per entrambe le versioni:
    modifica_rilevazioni = 0;
  }
  //
  // fine attivazione uscita PWM per relè a stato solido *************************************************************************:
  //
  pinV1valore = potenza_watt_produzione;    // indica i watt di produzione istantanea:
  pinV2valore = potenza_watt_consumi;       // indica i consumi complessivi, compreso quindi anche i carichi per autoconsumo:
  //
  // pinV3valore = conta / 10;
  pinV3valore = ciclopwm1;     // indica l'utilizzo del carico PWM, in quota:
  pinV4valore = surplus;       // indica i prelievi da rete, se negativo, oppure le immissioni in rete se positivo:
  //
  //
}
//FINE ARCHIMEDE WEMOS D1 autoconsumo








