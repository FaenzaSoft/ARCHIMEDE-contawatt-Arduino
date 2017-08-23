/* PROGRAMMA: "ARCHIMEDE 2 contawatt SD"
 MONITORAGGIO CONSUMI, FV e AUTOCONSUMO ZERO CON 2 SENSORI DIGITALI, "PENELOPE" E CON PIU' USCITE PWM:
 Versione per display 2x16 SERIALE (A4 e A5)
 Allaccio contatori a A1 e A2
 Uscite PWM: D6, D9 e D10
 Autore SoftPlus Consumo Zero - email: luigi.marchi.faenza@gmail.com  
 Versione 1.0 del 12 agosto 2017:
 Video Youtube: https://www.youtube.com/watch?v=puKxWG5Dx2o
 //
 Registrazione dati su memoria SD, con i seguenti file:
 1) "produzione_5m.txt" - registra la produzione cumulata, in wattora, ogni 5 minuti, rapportata ad 1 ora
 2) "consumi_5m.txt" - registra i consumi cumulati, in wattora, ogni 5 minuti, rapportati ad 1 ora
 3) "sbilancio_5m.txt" - registra il differenziale relativo alle 2 registrazioni precedenti con segno + o meno
 4) "produzione_1h.txt" - registra la produzione cumulata, in wattora, ogni ora
 5) "consumi_1h.txt - registra i consumi cumulati, in wattora, ogni ora
 6) "produzione_24h.txt" - registra la produzione cumulata, in wattora, ogni 24 ore
 7) "consumi_24h.txt - registra i consumi cumulati, in wattora, ogni 24 ore 
 //
 ATTENZIONE: se si variano i nomi dei file fare attenzione a lunghezza e caratteri usati
 //
 ATTENZIONE, IMPORTANTE: 
 Arduino funziona a 5 volt e non ha problemi di sorta, circa il rischio di scariche elettriche.                                 
 Circa l'alimentatore da rete è OBBLIGATORIO utilizzare un caricabatteria da telefonino (a norma) con uscita 8, 9 volt DC.    
 Per operare sulle parti hardware di contorno occorre avere le competenze ed esperienze consolidate. 
 Occorre essere ESPERTI e CON PATENTINO per gli interventi SUL QUADRO ELETTRICO e in apparecchiature con tensioni di rete (230 AC)
 e/o tensioni elevate di stringhe di pannelli fotovoltaici. E' pertanto necessario affidarsi a personale qualificato.                                 
 Il produttore del software declina ogni responsabilità per danni elettrici.                                                  
 Quanto ai danni derivante da malfunzionamento del software, il produttore del software declina ogni responsabilità circa:    
 1) danni derivanti da utilizzo di componentistica di contorno;                                                               
 2) danni derivanti da modifica del software originario;                                                                      
 3) danni derivanti da scarsa esperienza e scarsa capacità professionale dell'installatore.                                                
 //
 //
 IMPORTANTE: i due sensori da quadro vanno collocati: uno sulla linea dei consumi domestici, l'altro sulla linea che va all'inverter
 il relè a stato solido per il carico (boiler, stufette, scaldabagni) va inserito sulla linea dei consumi, perchè, per equilibrare
 produzione e consumi, si deve misurare, fra i consumi, anche quelli del carico aggiuntivo. 
 //
 IMPORTANTE: i sensori digitali producono degli impulsi la cui distanza, l'uno dall'altro, è inversamente proporzionale alla potenza:
 ciò comporta una certa lentezza nella valutazione dei piccoli carichi,la contropartita è una misurazione "fiscale" della massima
 precisione, come i contatori Enel; i sensori utilizzati (reperibili presso PARSIC Italia - Cervia) richiedono 3200 impulsi 
 ora per un KWora.
 //
 Basetta di interfaccia, tra contatori e Arduino: 2 resistenze da 39 Kohm lato massa e 2 da 100 ohm lato 5 volt positivi.
 Relativamente al numero di impulsi, va utilizzato il software relativo, oppure modificare le istruzioni che fanno riferimento 
 agli impulsi (3200, 2000, 1000, ecc.) 
 Non ci sono trimmer di taratura, neppure software.
 //
 */

#include <Arduino.h> //inclusione della libreria base:
// includere altre librerie:
#include <SPI.h>    
// ensure to install the SdFat Library from: https://github.com/greiman/SdFat
//#include <SD.h>    //Loose this in favour of SdFat Library for timestamp 
#include <SdFat.h>   //New SdFat library è compresa nella cartella SD-master.zip:
SdFat SD;            //Aggiungere per rendere compatibile il codice: 
#include <RTClib.h>   // compresa nella cartella RTClib-master.zip:
#include <Wire.h>     // serve per il funzionamento del display seriale, è inclusa nella libreria base:
#include <LiquidCrystal_I2C.h>
// LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);    //POSITIVE=accensione retroilluminazione:
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);    // indirizzo LCD Philips PCF8574AT: 
//
//
File file;
//
// definizione delle variabili e dei pin di INPUT e OUTPUT:
int pin_carico1 = 6;        // carico analogico PWM1:
int pin_carico2 = 9;        // carico analogico PWM2:
// int pin_carico3 = 10;        // carico analogico PWM3:      // disattivato, non si può usare perchè questo pin è impegnato per la SD:
int pin_consumi = A2;       // pin dove arrivano gli impulsi dei consumi:
int pin_produzione = A1;    // pin dove arrivano gli impulsi della produzione fotovoltaica:
int long ciclopwm = 0;       // va bene:
int ciclopwm1 = 0;                    // questo va bene:
int ciclopwm2 = 0;                    // questo va bene:
int ciclopwm3 = 0;                    // questo va bene:
unsigned long conta1 = 0;
unsigned long conta2 = 0;
unsigned long conta3 = 0;
unsigned long conta4 = 0;
unsigned long val_prod = 0;
unsigned long val_cons = 0;
int aumento = 0;
int potenza_watt_consumi = 0;
int potenza_watt_produzione = 0;
int buttonState = 0;
int modificaOK = 0;
int aumentoOK = 0;
unsigned long vecchio_millis_consumi = 0;
unsigned long corrente_millis_consumi = 0;
unsigned long tempo_trascorso_consumi = 0;
unsigned long vecchio_millis_produzione = 0;
unsigned long corrente_millis_produzione = 0;
unsigned long tempo_trascorso_produzione = 0;
unsigned long tempo_trascorso = 0;
unsigned long stop_tempo = 0;
unsigned long val;
unsigned long val1;
unsigned long val2;
unsigned int cicli = 0 ;
unsigned long sommaFV = 0;
unsigned long sommaCons = 0;
unsigned long millesimi = 0;
unsigned long mill_produzione = 0;
unsigned long mill_consumi = 0;
unsigned long tempo_adesso_consumi = 0;
unsigned long tempo_adesso_produzione = 0;
int volt_partitore_consumi = 0;
int volt_partitore_produzione = 0;
int modifica_rilevazioni = 0;
int long impulsi_consumi = 0;
int long impulsi_produzione = 0;
int long Kwh_FV = 0;
int long Kwh_Consu = 0;
int long Kwh_immissioni = 0;
int long Kwh_prelievi = 0;
int long tot_Kwh_immissioni = 0;
int long tot_Kwh_prelievi = 0;
unsigned long vecchio_millis_minuti = 0;
unsigned long corrente_millis_minuti = 0;
unsigned long vecchio_millis_ore = 30000;    // per una registrazione delle ore che non interferisca con quella dei 5 minuti:
unsigned long corrente_millis_ore = 0;
unsigned long vecchio_millis_giorni = 60000;  // per una registrazione dei giorni sfasata rispetto alle altre registrazioni:
unsigned long corrente_millis_giorni = 0;
unsigned long impulsi_produzione_vecchio5m = 0;
unsigned long impulsi_produzione_nuovo5m = 0;
unsigned long impulsi_consumi_vecchio5m = 0;
unsigned long impulsi_consumi_nuovo5m = 0;
//int sbilancio_5m = 0;
long sbilancio_5m = 0;
unsigned long impulsi_produzione_vecchio1h = 0;
unsigned long impulsi_produzione_nuovo1h = 0;
unsigned long impulsi_consumi_vecchio1h = 0;
unsigned long impulsi_consumi_nuovo1h = 0;
unsigned long impulsi_produzione_vecchio24h = 0;
unsigned long impulsi_produzione_nuovo24h = 0;
unsigned long impulsi_consumi_vecchio24h = 0;
unsigned long impulsi_consumi_nuovo24h = 0;
//int contawatt_m5p = 0;           
//int contawatt_m5c = 0;
long contawatt_m5p = 0;           
long contawatt_m5c = 0;
int contawatt_1hp = 0;           
int contawatt_1hc = 0;
long contawatt_sbilancio_5m = 0;
unsigned long impulsi_1hp = 0;       // 1 ora produzione:
unsigned long impulsi_1hc = 0;       // 1 ora consumi:
unsigned long contawatt_1h = 0;
unsigned long impulsi_24hp = 0;      // 24 ore produzione:
unsigned long impulsi_24hc = 0;      // 24 ore consumi:
unsigned long contawatt_24hp = 0;
unsigned long contawatt_24hc = 0;
unsigned long impulsi_30g = 0;
long vecchio_millis = 0;
int segmento_vecchio = 0;
int stampaok = 0;
//
unsigned long m5p = 0;   //minuti 5 produzione:
unsigned long m5c = 0;   //minuti 5 consumi:
//
//unsigned long h1p = 0;   // ora 1 produzione:
//unsigned long h1c = 0;   // ora 1 consumi:
//
//unsigned long g1p = 0;   // giorno 1 produzione:
//unsigned long g1c = 0;   // giorno 1 consumi:
//
// **********************************************************************************************************
int impulsi_kwh = 3200;                          // dato da modificare in base agli impulsi:
// int impulsi_kwh = 2000;
// int impulsi_kwh = 1600;
// int impulsi_kwh = 1000; 
// **********************************************************************************************************

// altre definizioni:
void setup()
{
  pinMode(pin_carico1, OUTPUT);   //pin digitale 8 uscita carico PWM:
  pinMode(pin_carico2, OUTPUT);   //pin digitale 9 uscita carico PWM:  
  // pinMode(pin_carico3, OUTPUT);   //pin digitale 10 uscita carico PWM:   // non si può usare, interferisce con la Shield DATA:
  pinMode(13, OUTPUT);            //pin led, o relè:
  pinMode(A1, INPUT);             //pin sensore digitale produzione:
  pinMode(A2, INPUT);             //pin sensore digitale consumi:
  //
  lcd.begin(16,2);  // Inizializzo LCD 16 caratteri per 2 linee, SERIALE:
  lcd.print("    SoftPlus    ");       
  lcd.setCursor(0, 1);
  lcd.print("  Consumo Zero  ");
  delay(3000);
  //  
  lcd.begin(16,2); 
  //
  Serial.begin(9600);
  delay(500);
  //
  //  Serial.print("Inizializzazione Card: ");
  if (!SD.begin(10)) //il Pin 10 è collegato a CS
  {
    delay(500);    
    //    Serial.println("FALLITA!");
    lcd.setCursor(0, 1);
    lcd.print("ERRORE inizializ");       
    return;
  }
  lcd.println("ESEGUITO!!");
  delay(1000);
  //
  //
}
//
void loop() {
  //
  //
  // step 1 lettura dei dati digitali:
  //
  tempo_adesso_consumi = millis();
  if (tempo_adesso_consumi > vecchio_millis_consumi + 100)   // bisogna che siano passati almeno 100 millisecondi dall'ultima lettura:
  {
    // legge lo stato e percepisce l'impulso sul pin deputato a rilevare la produzione:    
    volt_partitore_consumi = analogRead(A2);
    if (volt_partitore_consumi > 200)       // la tensione deve essere maggiore di 1 volt:
    {
      corrente_millis_consumi = millis();
      tempo_trascorso_consumi = corrente_millis_consumi - vecchio_millis_consumi;
      vecchio_millis_consumi = corrente_millis_consumi;
      // calcolo produzione media, in watt, di tempo in tempo:
      // 3200 impulsi ora per 1 Kwh, cioè 1.000 wattora:
      // per ogni 1000 wattora serve 1 impulso ogni 3600/3200 secondi cioè ogni 1,125 secondi:
      // se gli impulsi arrivano ogni 11,25 secondi abbiamo 100 wattora e così via:
      // poi bisogna tradurre in millesimi, laddove 1,125 secondi sono 1.125 millesimi di secondo:
      //       potenza_watt_consumi = 1000000 / tempo_trascorso_consumi * 36 / 64;        //valido per contatore da 6400 impulsi         
      //       potenza_watt_consumi = 1000000 / tempo_trascorso_consumi * 36 / 32;        //valido per contatore da 3200 impulsi   
      //      potenza_watt_consumi = 1000000 / tempo_trascorso_consumi * 36 / 20;        //valido per contatore da 2000 impulsi     
      potenza_watt_consumi = 1000000 / tempo_trascorso_consumi * 36 / (impulsi_kwh /100);     // valido per impostazione iniziale impulsi:      
      val_cons = potenza_watt_consumi;
      val2 = val_cons;
      buttonState = 0;
      modifica_rilevazioni = 1;
      impulsi_consumi = impulsi_consumi + 1;
    }
  }
  //
  //
  tempo_adesso_produzione = millis();
  if (tempo_adesso_produzione > vecchio_millis_produzione + 100)   // bisogna che siano passati almeno 100 millisecondi dall'ultima lettura:
  {
    // legge lo stato e percepisce l'impulso sul pin deputato a rilevare la produzione:
    volt_partitore_produzione = analogRead(A1);
    if (volt_partitore_produzione > 200)       // la tensione deve essere maggiore di 1 volt:    
    {
      corrente_millis_produzione = millis();
      tempo_trascorso_produzione = corrente_millis_produzione - vecchio_millis_produzione;
      vecchio_millis_produzione = corrente_millis_produzione;
      // calcolo produzione media, in watt, di tempo in tempo:
      // 3200 impulsi ora per 1 Kwh, cioè 1.000 wattora:
      // cioè per ogni 1000 wattora serve 1 impulso ogni 3600/3200 secondi cioè ogni 1,125 secondi:
      // cioè se gli impulsi arrivano ogni 11,25 secondi abbiamo 100 wattora e così via:
      // poi bisogna tradurre in millesimi, laddove 1,125 secondi sono 1.125 millesimi di secondo:
      //       potenza_watt_produzione = 1000000 / tempo_trascorso_produzione * 36 / 64;       //valido per contatore da 6400 impulsi      
      //       potenza_watt_produzione = 1000000 / tempo_trascorso_produzione * 36 / 32;       //valido per contatore da 3200 impulsi
      //       potenza_watt_produzione = 1000000 / tempo_trascorso_produzione * 36 / 20;       //valido per contatore da 2000 impulsi
      potenza_watt_produzione = 1000000 / tempo_trascorso_produzione * 36 / (impulsi_kwh / 100);       //valido per impostazione iniziale impulsi      
      val_prod = potenza_watt_produzione;
      val1 = val_prod;
      modifica_rilevazioni = 1;
      impulsi_produzione = impulsi_produzione + 1;
    }
  }
  //
  //
  conta1 = (conta1 + 1);
  conta2 = (conta2 + 1);
  conta3 = (conta3 + 1);
  conta4 = (conta4 + 1);  
  //
  //
  // step 2 calcolo cumulo energia immessa in rete e energia prelevata. 
  //
  // Verifica ogni secondo (3600 secondi = 1 ora):
  if (millis() > vecchio_millis + 999)
  {
    if (val_prod > val_cons) Kwh_immissioni = Kwh_immissioni + (val_prod - val_cons); 
    // somma surplus immesso in rete:
    else Kwh_prelievi = Kwh_prelievi + (val_cons - val_prod);        
    // somma deficit prelevato:
    vecchio_millis = vecchio_millis + 1000;
  }
  //
  //
  // step 3 stampa produzione e consumi nei 5 minuti.
  // 
  corrente_millis_minuti = millis();
  if (corrente_millis_minuti > vecchio_millis_minuti + 300000)   // significa che sono passati 5 minuti (60.000 x 5 = 300000):
  {
    stampaok = 1;
    impulsi_produzione_nuovo5m = impulsi_produzione;     
    m5p = impulsi_produzione_nuovo5m - impulsi_produzione_vecchio5m;  
    impulsi_produzione_vecchio5m = impulsi_produzione_nuovo5m;
    impulsi_consumi_nuovo5m = impulsi_consumi;     
    m5c = impulsi_consumi_nuovo5m - impulsi_consumi_vecchio5m;
    impulsi_consumi_vecchio5m = impulsi_consumi_nuovo5m;
    vecchio_millis_minuti = corrente_millis_minuti; 
    sbilancio_5m = m5p-m5c;    
    //
    // aggiorna l'ora ogni 5 minuti su CRT Data Logging Shield;
    //
    //inizzializza la trasmissione partendo
    //dall'indirizzo 0x00
    Wire.beginTransmission(0x68);
    Wire.write((byte)0x00);
    Wire.endTransmission();
    //
    //richiedo 7 byte dal dispositivo con
    //indirizzo 0x68
    Wire.requestFrom(0x68, 7);
    //regupero i 7 byte relativi ai
    //corrispondenti registri
    byte secondi = Wire.read();
    byte minuti = Wire.read();
    byte ora = Wire.read();
    byte giorno_sett = Wire.read();
    byte giorno_mese = Wire.read();
    byte mese = Wire.read();
    byte anno = Wire.read(); 
    // 
    file = SD.open("produzione_5m.txt", FILE_WRITE); //File in scrittura:  
    if (file) //Se il file è stato aperto correttamente:
    { 
      contawatt_m5p = m5p*1000*12/impulsi_kwh;      // x 12 perchè li rapporto ad 1 ora:
      file.println(contawatt_m5p);   // Scrivo i watt di consumo ( valido per 5 minuti),  rapportato ad ora:   
      file.print("                      ora: "); //Scrivo su file l'ora: 
      file.print(ora, HEX); //Scrivo su file l'ora:  
      file.print("-");
      file.print(minuti, HEX); //Scrivo su file l'ora:  
      file.print(" / giorno: ");    
      file.print(giorno_mese, HEX); //Scrivo su file l'ora:  
      file.print("-");
      file.print(mese, HEX); //Scrivo su file l'ora: 
      file.print("-");
      file.print(anno, HEX); //Scrivo su file l'ora:      
      file.println("   "); 
      file.close(); //Chiusura file:  
      stampaok = 0;        
    }
    file = SD.open("consumi_5m.txt", FILE_WRITE); //File in scrittura:  
    if (file) //Se il file è stato aperto correttamente:
    { 
      contawatt_m5c = m5c*1000*12/impulsi_kwh;    // x 12 perchè li rapporto ad 1 ora:
      file.println(contawatt_m5c);   // Scrivo i watt di consumo ( valido per 5 minuti),  rapportato ad ora:   
      file.print("                      ora: "); //Scrivo su file l'ora: 
      file.print(ora, HEX); //Scrivo su file l'ora:  
      file.print("-");
      file.print(minuti, HEX); //Scrivo su file l'ora:  
      file.print(" / giorno: ");    
      file.print(giorno_mese, HEX); //Scrivo su file l'ora:  
      file.print("-");
      file.print(mese, HEX); //Scrivo su file l'ora: 
      file.print("-");
      file.print(anno, HEX); //Scrivo su file l'ora:      
      file.println("   "); 
      file.close(); //Chiusura file:  
      stampaok = 0;       
    }
    //
    file = SD.open("sbilancio_5m.txt", FILE_WRITE); //File in scrittura:  
    if (file) //Se il file è stato aperto correttamente:
    { 
      contawatt_sbilancio_5m = sbilancio_5m*12*1000/impulsi_kwh;   //rapportato a ora:
      file.println(contawatt_sbilancio_5m);   // Scrivo i watt di consumo ( valido per 5 minuti),  rapportato ad ora:   
      file.print("                      ora: "); //Scrivo su file l'ora: 
      file.print(ora, HEX); //Scrivo su file l'ora:  
      file.print("-");
      file.print(minuti, HEX); //Scrivo su file l'ora:  
      file.print(" / giorno: ");    
      file.print(giorno_mese, HEX); //Scrivo su file l'ora:  
      file.print("-");
      file.print(mese, HEX); //Scrivo su file l'ora: 
      file.print("-");
      file.print(anno, HEX); //Scrivo su file l'ora:      
      file.println("   "); 
      file.close(); //Chiusura file:  
      stampaok = 0;       
    }
    //
  }
  //
  corrente_millis_ore = millis();
  if (corrente_millis_ore > vecchio_millis_ore + 3600000)   // 3,6 milioni, cioè un'ora (60.000 x 60 = 3600000):
  { 
    vecchio_millis_ore = corrente_millis_ore;  
    impulsi_produzione_nuovo1h = impulsi_produzione;     
    impulsi_1hp = impulsi_produzione_nuovo1h - impulsi_produzione_vecchio1h;  
    impulsi_produzione_vecchio1h = impulsi_produzione_nuovo1h;
    impulsi_consumi_nuovo1h = impulsi_consumi;     
    impulsi_1hc = impulsi_consumi_nuovo1h - impulsi_consumi_vecchio1h;
    impulsi_consumi_vecchio1h = impulsi_consumi_nuovo1h;
    //
    // aggiorna l'ora su CRT Data Logging Shield;
    //
    //inizzializza la trasmissione partendo
    //dall'indirizzo 0x00
    Wire.beginTransmission(0x68);
    Wire.write((byte)0x00);
    Wire.endTransmission();
    //
    //richiedo 7 byte dal dispositivo con
    //indirizzo 0x68
    Wire.requestFrom(0x68, 7);
    //regupero i 7 byte relativi ai
    //corrispondenti registri
    byte secondi = Wire.read();
    byte minuti = Wire.read();
    byte ora = Wire.read();
    byte giorno_sett = Wire.read();
    byte giorno_mese = Wire.read();
    byte mese = Wire.read();
    byte anno = Wire.read(); 
    // 
    file = SD.open("produzione_1h.txt", FILE_WRITE); //File in scrittura:  
    if (file) //Se il file è stato aperto correttamente:
    {    
      contawatt_1hp = impulsi_1hp*1000/impulsi_kwh; 
      file.println(contawatt_1hp);   // Scrivo i watt di consumi ora per ora:   
      file.print("                      ora: "); //Scrivo su file l'ora: 
      file.print(ora, HEX); //Scrivo su file l'ora:  
      file.print("-");
      file.print(minuti, HEX); //Scrivo su file l'ora:  
      file.print(" / giorno: ");    
      file.print(giorno_mese, HEX); //Scrivo su file l'ora:  
      file.print("-");
      file.print(mese, HEX); //Scrivo su file l'ora: 
      file.print("-");
      file.print(anno, HEX); //Scrivo su file l'ora:      
      file.println("   "); 
      file.close(); //Chiusura file:  
      //stampaok = 0;       
    }
    //
    file = SD.open("consumi_1h.txt", FILE_WRITE); //File in scrittura:  
    if (file) //Se il file è stato aperto correttamente:
    {    
      contawatt_1hc = impulsi_1hc*1000/impulsi_kwh; 
      file.println(contawatt_1hc);   // Scrivo i watt di consumi ora per ora:   
      file.print("                      ora: "); //Scrivo su file l'ora: 
      file.print(ora, HEX); //Scrivo su file l'ora:  
      file.print("-");
      file.print(minuti, HEX); //Scrivo su file l'ora:  
      file.print(" / giorno: ");    
      file.print(giorno_mese, HEX); //Scrivo su file l'ora:  
      file.print("-");
      file.print(mese, HEX); //Scrivo su file l'ora: 
      file.print("-");
      file.print(anno, HEX); //Scrivo su file l'ora:      
      file.println("   "); 
      file.close(); //Chiusura file:  
      //stampaok = 0;       
    }  
    //   
  }
  // 
  corrente_millis_giorni = millis();
  if (corrente_millis_giorni > vecchio_millis_giorni + 86400000)   // 24 ore (3,6 milioni x 24 = 86400000): 
  {  
    vecchio_millis_giorni = corrente_millis_giorni;  
    impulsi_produzione_nuovo24h = impulsi_produzione;     
    impulsi_24hp = impulsi_produzione_nuovo24h - impulsi_produzione_vecchio24h;
    impulsi_produzione_vecchio24h = impulsi_produzione_nuovo24h;
    impulsi_consumi_nuovo24h = impulsi_consumi;     
    impulsi_24hc = impulsi_consumi_nuovo24h - impulsi_consumi_vecchio24h;
    impulsi_consumi_vecchio24h = impulsi_consumi_nuovo24h;
    //
    // aggiorna l'ora su CRT Data Logging Shield;
    //
    //inizzializza la trasmissione partendo
    //dall'indirizzo 0x00
    Wire.beginTransmission(0x68);
    Wire.write((byte)0x00);
    Wire.endTransmission();
    //
    //richiedo 7 byte dal dispositivo con
    //indirizzo 0x68
    Wire.requestFrom(0x68, 7);
    //regupero i 7 byte relativi ai
    //corrispondenti registri
    byte secondi = Wire.read();
    byte minuti = Wire.read();
    byte ora = Wire.read();
    byte giorno_sett = Wire.read();
    byte giorno_mese = Wire.read();
    byte mese = Wire.read();
    byte anno = Wire.read(); 
    // 
    
    file = SD.open("produzione_24h.txt", FILE_WRITE); //File in scrittura, ATTENZIONE: non mettere nomi troppo lunghi:  
    if (file) //Se il file è stato aperto correttamente:
    {    
      contawatt_24hp = impulsi_24hp*10/(impulsi_kwh/100); 
      file.println(contawatt_24hp);   // Scrivo i watt di consumi ora per ora:   
      file.print("                      ora: "); //Scrivo su file l'ora: 
      file.print(ora, HEX); //Scrivo su file l'ora:  
      file.print("-");
      file.print(minuti, HEX); //Scrivo su file l'ora:  
      file.print(" / giorno: ");    
      file.print(giorno_mese, HEX); //Scrivo su file l'ora:  
      file.print("-");
      file.print(mese, HEX); //Scrivo su file l'ora: 
      file.print("-");
      file.print(anno, HEX); //Scrivo su file l'ora:      
      file.println("   "); 
      file.close(); //Chiusura file:  
      //stampaok = 0;       
    }
    //
    file = SD.open("consumi_24h.txt", FILE_WRITE); //File in scrittura:  
    if (file) //Se il file è stato aperto correttamente:
    {    
      contawatt_24hc = impulsi_24hc*10/(impulsi_kwh/100); 
      file.println(contawatt_24hc);   // Scrivo i watt di consumi ora per ora:   
      file.print("                      ora: "); //Scrivo su file l'ora: 
      file.print(ora, HEX); //Scrivo su file l'ora:  
      file.print("-");
      file.print(minuti, HEX); //Scrivo su file l'ora:  
      file.print(" / giorno: ");    
      file.print(giorno_mese, HEX); //Scrivo su file l'ora:  
      file.print("-");
      file.print(mese, HEX); //Scrivo su file l'ora: 
      file.print("-");
      file.print(anno, HEX); //Scrivo su file l'ora:      
      file.println("   "); 
      file.close(); //Chiusura file:  
      //stampaok = 0;       
    }  
    //   
    //}     
    //
    //}
    //
  }
  // step 4 modifica del cicloPWM per azzerare il differenziale tra produzione e consumi:
  //
  // faccio la modifica ogni volta che arriva un nuovo impulso dal contatore dei consumi o da quello dela produzione:
  if (modifica_rilevazioni == 1)
  {
    if (millis() > tempo_trascorso + 500)     // vogliamo che passi un pò di tempo, per il ricalcolo del differenziale:
      // if (millis() > tempo_trascorso + 200)     // vogliamo che passi un pò di tempo, per il ricalcolo:    
    {
      modifica_rilevazioni = 0;
      // facciamo una verifica dello sbilancio e attiviamo il carico aggiuntivo giusto:
      if (val_prod > val_cons)            // alziamo il carico aggiuntivo:
      {
        aumento = (val_prod - val_cons) * 254 / 3000;          // è un calcolo di tipo progressivo, molto reattivo:
        // aumento = (val_prod - val_cons) * 254 / 1000;    
        // aumento = (val_prod - val_cons) * 254 / 10000;              
        ciclopwm = ciclopwm + aumento;
      }
      else                                // diminuiamo il carico aggiuntivo:
      {
        aumento = (val_cons - val_prod) * 254 / 3000;
        // aumento = (val_cons - val_prod) * 254 / 1000;  
        // aumento = (val_cons - val_prod) * 254 / 10000;       
        ciclopwm = ciclopwm - aumento;
      }
      // imposta i valori limite del ciclo_PWM (da 0 a 255) e li cambia:
      if (ciclopwm < 1) ciclopwm = 0;
      if (ciclopwm > 800) ciclopwm = 800;
      ciclopwm1 = ciclopwm;
      ciclopwm2 = 0;
      ciclopwm3 = 0;
      if (ciclopwm > 254)
      {
        ciclopwm1 = 254;
        ciclopwm2 = ciclopwm - 254;
      }
      if (ciclopwm > 508)
      {
        ciclopwm2 = 254;
        ciclopwm3 = (ciclopwm - 508);
      }
      if (ciclopwm > 762)
      {
        ciclopwm3 = 254;
      }
      analogWrite(pin_carico1, ciclopwm1);
      analogWrite(pin_carico2, ciclopwm2);
//      analogWrite(pin_carico3, ciclopwm3);              // disattivato perchè il pin 10 interferisce con la registrazione SD: 
      //
      aumentoOK = 1;       //mettiamo a "1" la variazione di aumento per evitare di ripeterla prima delle nuove misure:
      //
      // step 5 gestione del led di segnalazione sul pin 13, all'occorrenza:
      // predisposto per attivare un relè convenzionale, per attivare il carico (boiler, stufetta, ecc.) in alternativa al RSS:
      //
      if (val_prod > val_cons + 500)
      {
        digitalWrite(13, HIGH);
        // consente di attivare un led (o un altro carico compatibile) sul pin 13 quando si ha a disposizione 1/2 Kw in più:
        // un eventuale relè che superi i 40 milliamper di consumo deve essere pilotato da un transistor:
      }
      else
      {
        digitalWrite(13, LOW);    // consente di disattivare il led se non si ha a disposizione un 1/2 Kw in più:
      }
      modifica_rilevazioni = 0;
      tempo_trascorso = millis();
    }
  }
  //
  //
  // step 5 gestione del display relativamente a consumi ( in watt) e produzione fotovoltaico (in watt):
  // la visualizzazione dei dati avviene solo dopo qualche migliaio di cicli di verifiche Arduino:
  //
  // step 5 gestione del display relativamente a prelievi dalla rete (Enel) o immissioni in rete (GSE), l'uno alternativo all'altro:
  // inoltre viene evidenziato il carico extra per pareggiare produzione e consumi(i cicli PWM vanno da 1 a 254),:
  // ma sono espressi in percentuale (da 0% a 100%):
  // con 3 uscite PWM la percentuale può arrivare fino a 314% la priorità è per i RSS collegato al pin D6, ma se il termostato stacca:
  // questo carico non è attivo e il carico utile passa al RSS collegato al PIN D9 e viene visualizzato in percentuale tra 100% e 200%
  // con altissima produzione FV possono essere attivi i primi 2 RSS e anche il terzo (viene evidenziato sul display un carico compreso 
  // tra il 200% e il 300% oltre il 300% significa che i primi 3 RSS sono attivi o anche disattivati.
  //
  if (conta1 > 10000)            // deve visualizzare, a rotazione, solo ogni 2 secondi circa:  
  {
    conta1 = 0;
    lcd.clear();      // pulisce lo schermo:
    // Visualizzo il messaggio sul display SERIALE con le informazioni:    
    if (val2 >= val1)   //siamo in zona prelievi:
    {
      lcd.print("W_Enel   %carico");
      lcd.setCursor(1, 1);
      lcd.print(val2 - val1);
    }
    if (val2 < val1)   //siamo in zona immissioni:
    {
      lcd.print("W_GSE    %carico");
      lcd.setCursor(1, 1);
      lcd.print(val1 - val2);
    }
    lcd.setCursor(11, 1);
    if (ciclopwm > 1)
      lcd.print(ciclopwm * 100 / 254);
    else
      lcd.print("O");
  }
  //
  //
  if (conta2 > 20000)     // lo faccio funzionare solo ogni 2 secondi circa:  
  {
    conta1 = 0;
    conta2 = 0;    
    lcd.clear();      // pulisce lo schermo:
    lcd.setCursor(0, 0);
    //    lcd.print("W_Consumi  W_FV ");
    lcd.print(" W_FV  W_Consumi");    
    lcd.setCursor(1, 1);
    lcd.print(val1);
    lcd.setCursor(11, 1);
    lcd.print(val2);
  }
  //
  // 
  if (conta3 > 30000)     // lo faccio funzionare solo ogni 2 secondi circa:  
  {
    conta1 = 0;
    conta2 = 0;    
    conta3 = 0;
    lcd.clear();      // pulisce lo schermo:
    lcd.setCursor(0, 0);
    lcd.print("Kwh_FV Kwh_Consu");  
    Kwh_FV = impulsi_produzione * 100 / impulsi_kwh;
    Kwh_Consu = (impulsi_consumi * 100) / impulsi_kwh;
    lcd.setCursor(2, 1);
    if (Kwh_FV < 10) lcd.setCursor(3, 1);  
    lcd.print(Kwh_FV);
    lcd.setCursor(0, 1);
    if (Kwh_FV > 99)lcd.setCursor(1, 1);    
    lcd.print(Kwh_FV/100); 
    lcd.print(",");  
    if (Kwh_FV < 10) lcd.print("0");    
    lcd.setCursor(11, 1);
    if (Kwh_Consu < 10)lcd.setCursor(12, 1);      
    lcd.print(Kwh_Consu);
    lcd.setCursor(9, 1);
    if (Kwh_Consu > 99)lcd.setCursor(10, 1);    
    lcd.print(Kwh_Consu/100); 
    lcd.print(",");  
    if (Kwh_Consu < 10) lcd.print("0");        
    //
  }
  //
  if (conta4 > 40000)     // lo faccio funzionare solo ogni 2 secondi circa:  
  {
    conta1 = 0;
    conta2 = 0;    
    conta3 = 0;
    conta4 = 0;    
    lcd.clear();      // pulisce lo schermo:
    lcd.setCursor(0, 0);
    lcd.print("Kwh_GSE Kwh_Enel");  
    tot_Kwh_immissioni = Kwh_immissioni/36000;
    tot_Kwh_prelievi = Kwh_prelievi/36000;      
    //      tot_Kwh_immissioni = Kwh_immissioni/3600/10;         // diviso 3600 perchè è stato fatto la somma in watt ogni secondo;
    //      tot_Kwh_prelievi = Kwh_prelievi/3600/10;             // diviso anche 100, perchè vogliamo vedere i Kwh con decimali;
    lcd.setCursor(2, 1);
    if (tot_Kwh_immissioni < 10) lcd.setCursor(3, 1);  
    lcd.print(tot_Kwh_immissioni);
    lcd.setCursor(0, 1);
    if (tot_Kwh_immissioni > 99)lcd.setCursor(1, 1);    
    lcd.print(tot_Kwh_immissioni/100); 
    lcd.print(",");  
    if (tot_Kwh_immissioni < 10) lcd.print("0");    
    lcd.setCursor(11, 1);
    if (tot_Kwh_prelievi < 10)lcd.setCursor(12, 1);      
    lcd.print(tot_Kwh_prelievi);
    lcd.setCursor(9, 1);
    if (tot_Kwh_prelievi > 99)lcd.setCursor(10, 1);    
    lcd.print(tot_Kwh_prelievi/100); 
    lcd.print(",");  
    if (tot_Kwh_prelievi < 10) lcd.print("0");        
  }
  //
}
// FINE listato by SoftPlus Consumo Zero - Faenza.  






























