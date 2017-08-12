/* PROGRAMMA CONTAWATT SD per contatore digitale e memoria SD su hardware PENELOPE e Data logging Shield. 
  Versione per display 2x16 SERIALE (A4 e A5)
 Allaccio contatori a A2 (consumi) della centralina PENELOPE, ingressi 3 (negativo da SO-) e 4 (positivo da SO+)
 autore SoftPlus Consumo Zero - email: luigi.marchi.faenza@gmail.com  
 Versione software 1.0 del 20 luglio 2017:
 //
 //
 ATTENZIONE, IMPORTANTE: 
 Arduino funziona a 5 volt e non ha problemi di sorta, circa il rischio di scariche elettriche.                                 
 Circa l'alimentatore da rete è OBBLIGATORIO utilizzare un caricabatteria da telefonino (a norma) con uscita 8, 9 volt DC.    
 Per operare sulle parti hardware di contorno occorre avere le competenze ed esperienze consolidate. 
 Occorre essere ESPERTI e CON PATENTINO per gli interventi SUL QUADRO ELETTRICO e in apparecchiature con tensioni di rete (230 AC).
 E' pertanto necessario affidarsi a personale qualificato.                                 
 Il produttore del software declina ogni responsabilità per danni elettrici.                                                  
 Quanto ai danni derivante da malfunzionamento del software, il produttore del software declina ogni responsabilità circa:    
 1) danni derivanti da utilizzo di componentistica di contorno;                                                               
 2) danni derivanti da modifica del software originario;                                                                      
 3) danni derivanti da scarsa esperienza e scarsa capacità professionale dell'installatore.                                                
 //
 //
 IMPORTANTE: il contatore digitale va collocato sulla linea dei consumi domestici.
 //
 IMPORTANTE: i sensori digitali producono degli impulsi la cui distanza, l'uno dall'altro, è inversamente proporzionale alla potenza:
 ciò comporta una certa lentezza nella valutazione dei piccoli carichi,la contropartita è una misurazione "fiscale" della massima
 precisione, come i contatori Enel; i sensori utilizzati (reperibili presso PARSIC ITALIA - Cervia) richiedono 3200 impulsi ora per un KWora.
 //
 Basetta di interfaccia, tra contatori e Arduino: 1 resistenze da 39 Kohm lato massa e 1 da 100 ohm lato 5 volt.
 Relativamente al numero di impulsi, va utilizzato il software relativo, oppure modificare le istruzioni che fanno riferimento 
 agli impulsi. 
 Non ci sono trimmer di taratura, neppure software:
 //
 */
#include <Arduino.h> //inclusione della libreria base:
// includere altre librerie:
//#include <SPI.h>     // 
//#include <SD.h>    //Loose this in favour of SdFat Library for timestamp 
#include <SdFat.h>   //New SdFat library è compresa nella cartella SD-master.zip:
SdFat SD;            //Aggiungere per rendere compatibile il codice: 
#include <RTClib.h>   // compresa nella cartella RTClib-master.zip:
#include <Wire.h>     // serve per il funzionamento del display seriale, è inclusa nella libreria base:
#include <LiquidCrystal_I2C.h>      
// LiquidCrystal_I2C lcd(0x20, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);    // LCM1602 IIC A0 A1 A2:
// LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);    //POSITIVE=accensione retroilluminazione:
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);    // indirizzo LCD Philips PCF8574AT: 
//
File file;
//
int pin_consumi = A2;       // pin dove arrivano gli impulsi dei consumi:
int conta1 = 0;
int conta2 = 0;
unsigned long val_cons = 0;
unsigned long potenza_watt_consumi = 0;      // non modificare unsigned long:
unsigned long vecchio_millis_consumi = 0;
unsigned long corrente_millis_consumi = 0;
unsigned long tempo_trascorso_consumi = 0;         // non modificare unsigned long (anzichè int):
unsigned long tempo_adesso_consumi = 0;
unsigned long contawatt = 0;
int volt_partitore_consumi = 0;
unsigned long impulsi_consumi = 0;
unsigned long vecchio_millis_minuti = 0;
unsigned long corrente_millis_minuti = 0;
unsigned long vecchio_millis_ore = 0;
unsigned long corrente_millis_ore = 0;
unsigned long vecchio_millis_giorni = 0;
unsigned long corrente_millis_giorni = 0;
unsigned long impulsi_consumi_vecchio = 0;
unsigned long impulsi_consumi_nuovo = 0;
int contawatt_m1 = 0;
unsigned long impulsi_1h = 0;
unsigned long contawatt_1h = 0;
unsigned long impulsi_24h = 0;
unsigned long contawatt_24h = 0;
unsigned long impulsi_30g = 0;
unsigned long contawatt_30g = 0;
int segmento_vecchio = 0;
int stampaok = 0;
unsigned long m1 = 0;
unsigned long m2 = 0;
unsigned long m3 = 0;
unsigned long m4 = 0;
unsigned long m5 = 0;
unsigned long m6 = 0;
unsigned long m7 = 0;
unsigned long m8 = 0;
unsigned long m9 = 0;
unsigned long m10 = 0;
unsigned long m11 = 0;
unsigned long m12 = 0;
//
unsigned long h1 = 0;
unsigned long h2 = 0;
unsigned long h3 = 0;
unsigned long h4 = 0;
unsigned long h5 = 0;
unsigned long h6 = 0;
unsigned long h7 = 0;
unsigned long h8 = 0;
unsigned long h9 = 0;
unsigned long h10 = 0;
unsigned long h11 = 0;
unsigned long h12 = 0;
unsigned long h13 = 0;
unsigned long h14 = 0;
unsigned long h15 = 0;
unsigned long h16 = 0;
unsigned long h17 = 0;
unsigned long h18 = 0;
unsigned long h19 = 0;
unsigned long h20 = 0;
unsigned long h21 = 0;
unsigned long h22 = 0;
unsigned long h23 = 0;
unsigned long h24 = 0;
//
unsigned long g1 = 0;
unsigned long g2 = 0;
unsigned long g3 = 0;
unsigned long g4 = 0;
unsigned long g5 = 0;
unsigned long g6 = 0;
unsigned long g7 = 0;
unsigned long g8 = 0;
unsigned long g9 = 0;
unsigned long g10 = 0;
unsigned long g11 = 0;
unsigned long g12 = 0;
unsigned long g13 = 0;
unsigned long g14 = 0;
unsigned long g15 = 0;
unsigned long g16 = 0;
unsigned long g17 = 0;
unsigned long g18 = 0;
unsigned long g19 = 0;
unsigned long g20 = 0;
unsigned long g21 = 0;
unsigned long g22 = 0;
unsigned long g23 = 0;
unsigned long g24 = 0;
unsigned long g25 = 0;
unsigned long g26 = 0;
unsigned long g27 = 0;
unsigned long g28 = 0;
unsigned long g29 = 0;
unsigned long g30 = 0;
//
//
// **********************************************************************************************************
int impulsi_kwh = 3200;                          // dato da modificare in base agli impulsi del contatore:
// **********************************************************************************************************
//
// altre definizioni:
void setup()
{
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
// Inizia ciclo
void loop() {
  //
  // step 1 lettura dei dati digitali:
  //
  tempo_adesso_consumi = millis();
  if (tempo_adesso_consumi > vecchio_millis_consumi + 100)   // bisogna che siano passati almeno 100 millisecondi dall'ultima lettura:
  {
    // legge lo stato e percepisce l'impulso sul pin deputato a rilevare i consumi:
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
      // potenza_watt_consumi = 1000000 / tempo_trascorso_consumi * 36 / 64;        //valido per contatore da 6400 impulsi         
      // potenza_watt_consumi = 1000000 / tempo_trascorso_consumi * 36 / 32;        //valido per contatore da 3200 impulsi   
      // potenza_watt_consumi = 1000000 / tempo_trascorso_consumi * 36 / 20;        //valido per contatore da 2000 impulsi     
      potenza_watt_consumi = 1000000 / tempo_trascorso_consumi * 36 / (impulsi_kwh /100);     // valido per impostazione iniziale impulsi:      
      val_cons = potenza_watt_consumi;
      impulsi_consumi = impulsi_consumi + 1;
    }
    //
    // step 2 conteggi e registrazione dei dati:   //
    //
    corrente_millis_minuti = millis();
    if (corrente_millis_minuti > vecchio_millis_minuti + 300000)   // significa che sono passati 5 minuti (60.000 x 5 = 300000):
    {
      stampaok = 1;
      vecchio_millis_minuti = corrente_millis_minuti;         
      segmento_vecchio = m12;
      m12 = m11;     // indicazione delle tranche di 5 minuti (ce sono 12 in un'ora):
      m11 = m10;
      m10 = m9;      
      m9 = m8;
      m8 = m7;
      m7 = m6;
      m6 = m5;
      m5 = m4;      
      m4 = m3;
      m3 = m2;
      m2 = m1;
      impulsi_consumi_nuovo = impulsi_consumi;     
      m1 = impulsi_consumi_nuovo - impulsi_consumi_vecchio;
      impulsi_consumi_vecchio = impulsi_consumi_nuovo; 
      impulsi_1h = impulsi_1h + m1 - segmento_vecchio;          // conteggio tipo ruota panoramica:   
      // impulsi_1h = m1+m2+m3+m4+m5+m6+m7+m8+m9+m10+m11+m12; 
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
      file = SD.open("consumiwatt_5m.txt", FILE_WRITE); //File in scrittura:  
      if (file) //Se il file è stato aperto correttamente:
      { 
        contawatt_m1 = m1*1000*12/impulsi_kwh;       // x 12 per rapportare a 1 ora i consumi di 5 minuti:
        file.println(contawatt_m1);   // Scrivo i watt di consumo ( valido per 5 minuti),  rapportato ad ora:   
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
      //
      corrente_millis_ore = millis();
      if (corrente_millis_ore > vecchio_millis_ore + 3600000)   // 3,6 milioni, cioè un'ora (60.000 x 60 = 3600000):
      { 
        vecchio_millis_ore = corrente_millis_ore;  
        segmento_vecchio = h24;        
        h24 = h23;
        h23 = h22;
        h22 = h21;      
        h21 = h20;
        h20 = h19;
        h19 = h18;
        h18 = h17;
        h17 = h16;      
        h16 = h15;
        h15 = h14;
        h14 = h13; 
        h13 = h12;       
        h12 = h11;
        h11 = h10;
        h10 = h9;      
        h9 = h8;
        h8 = h7;
        h7 = h6;
        h6 = h5;
        h5 = h4;      
        h4 = h3;
        h3 = h2;
        h2 = h1;
        h1 = impulsi_1h;  
        // vecchio_millis_ore = corrente_millis_ore;  
        impulsi_24h = impulsi_24h + h1 - segmento_vecchio;            // conteggio tipo ruota panoramica:        
        // impulsi_24h = h1+h2+h3+h4+h5+h6+h7+h8+h9+h10+h11+h12+h13+h14+h15+h16+h17+h18+h19+h20+h21+h22+h23+h24; 
        //
        file = SD.open("consumi_1h.txt", FILE_WRITE); //File in scrittura:  
        if (file) //Se il file è stato aperto correttamente:
        {    
          contawatt_1h = impulsi_1h*1000/impulsi_kwh; 
          file.println(contawatt_1h);   // Scrivo i watt di consumi ora per ora:   
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
        //
        corrente_millis_giorni = millis();
        if (corrente_millis_giorni > vecchio_millis_giorni + 86400000)   // 24 ore (3,6 milioni x 24 = 86400000): 
        {
          vecchio_millis_giorni = corrente_millis_giorni; 
          segmento_vecchio = g30;          
          g30 = g29;
          g29 = g28;
          g28 = g27;      
          g27 = g26;
          g26 = g25;
          g25 = g24;      
          g24 = g23;
          g23 = g22;
          g22 = g21;      
          g21 = g20;
          g20 = g19;
          g19 = g18;
          g18 = g17;
          g17 = g16;      
          g16 = g15;
          g15 = g14;
          g14 = g13; 
          g13 = g12;       
          g12 = g11;
          g11 = g10;
          g10 = g9;      
          g9 = g8;
          g8 = g7;
          g7 = g6;
          g6 = g5;
          g5 = g4;      
          g4 = g3;
          g3 = g2;
          g2 = g1;
          g1 = impulsi_24h; 
          // vecchio_millis_giorni = corrente_millis_giorni;  
          impulsi_30g = impulsi_30g + g1 - segmento_vecchio;                // conteggio tipo ruota panoramica:          
          //
          file = SD.open("consumi_24h.txt", FILE_WRITE); //File in scrittura:  
          if (file) //Se il file è stato aperto correttamente:
          {    
            contawatt_24h = impulsi_24h*1000/impulsi_kwh; 
            file.println(contawatt_24h);   // Scrivo i watt di consumi ora per ora:   
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
      }
      //
    }
    //    
  }
  //
  //
  // step 3 visualizzazione dati:  
  if (conta1 > 15000)
  {
    contawatt = impulsi_consumi*1000/impulsi_kwh;   // perchè abbiamo 3200 impulsi per Kwh: 
    if (contawatt > 99999)
    {
      lcd.setCursor(0, 0);
      lcd.print("W_Enel Kwh_Consu");  
      lcd.setCursor(0, 1);
      lcd.print("                "); 
      lcd.setCursor(1, 1);  
      lcd.print(val_cons);
      lcd.setCursor(10, 1);  
      lcd.print(contawatt/1000);  
    }  
    else 
    {
      lcd.setCursor(0, 0);
      lcd.print("W_Enel  Wh_Consu");  
      lcd.setCursor(0, 1);
      lcd.print("                "); 
      lcd.setCursor(1, 1);  
      lcd.print(val_cons);
      lcd.setCursor(10, 1);  
      lcd.print(contawatt);  
    }   
    conta1 = 0;
    //
  } 
  if (conta2 > 25000)
  {
    contawatt_1h = impulsi_1h*1000/impulsi_kwh;   // perchè abbiamo 3200 impulsi per Kwh: 
    contawatt_24h = impulsi_24h*1000/impulsi_kwh; 
    contawatt_30g = impulsi_30g/impulsi_kwh;        // è espresso in Kwh;
    lcd.setCursor(0, 0);
    lcd.print("Wh_1h 24h 30ggKw");        
    lcd.setCursor(0, 1);
    lcd.print("                "); 
    lcd.setCursor(0, 1); 
    lcd.print(contawatt_1h);
    lcd.setCursor(6, 1); 
    lcd.print(contawatt_24h);     
    lcd.setCursor(12, 1); 
    lcd.print(contawatt_30g); 
    conta2 = 0; 
    conta1 = 0;    
  }        
  //
  conta1 = conta1 + 1;  
  conta2 = conta2 + 1;      
  //
}
// FINE listato by SoftPlus Consumo Zero - Faenza.  
































