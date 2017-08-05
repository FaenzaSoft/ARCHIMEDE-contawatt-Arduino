/* PROGRAMMA ARCHIMEDE 1 CONTAWATT FV per contatore digitale. 
 Versione per display 2x16 SERIALE (A4 e A5)
 Allaccio contatori a A1 (produzione Penelope)
 autore SoftPlus Consumo Zero - email: luigi.marchi.faenza@gmail.com  
 Versione software del 13 luglio 2017:
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
 IMPORTANTE: il contatore digitale va collocato sulla linea della produzione fotovoltaica a 230 volt AC.
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
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
// LiquidCrystal_I2C lcd(0x20, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);    // LCM1602 IIC A0 A1 A2:
// LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);    //POSITIVE=accensione retroilluminazione:
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);    // indirizzo LCD Philips PCF8574AT: 
//
int pin_produzione = A1;       // pin dove arrivano gli impulsi dei consumi:
int conta1 = 0;
int conta2 = 0;
unsigned long val_prod = 0;
unsigned long potenza_watt_produzione = 0;      // non modificare unsigned long:
unsigned long vecchio_millis_produzione = 0;     //dato corretto, prima era double:
unsigned long corrente_millis_produzione = 0;    // dato corretto, prima era double:
unsigned long tempo_trascorso_produzione = 0;    // non modificare unsigned long anzichè int:
double tempo_adesso_produzione = 0;
unsigned long contawatt = 0;
int volt_partitore_produzione = 0;
double vecchio_millis_minuti = 0;
double corrente_millis_minuti = 0;
double vecchio_millis_ore = 0;
double corrente_millis_ore = 0;
double vecchio_millis_giorni = 0;
double corrente_millis_giorni = 0;
unsigned long impulsi_produzione = 0;
unsigned long impulsi_produzione_nuovo = 0;
unsigned long impulsi_produzione_vecchio = 0;
unsigned long impulsi_1h = 0;
unsigned long contawatt_1h = 0;
unsigned long impulsi_24h = 0;
unsigned long contawatt_24h = 0;
unsigned long impulsi_30g = 0;
unsigned long contawatt_30g = 0;
int val_prod_vecchia = 0;
int segmento_vecchio = 0;
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
// **********************************************************************************************************
int impulsi_kwh = 3200;                          // dato da modificare in base agli impulsi del contatore:
// **********************************************************************************************************
//
// altre definizioni:
// 
void setup()
{
  pinMode(6, OUTPUT);  
  pinMode(9, OUTPUT);    
  //
  pinMode(A1, INPUT);             //pin sensore digitale consumi:
  //
  lcd.begin(16,2);  // Inizializzo LCD 16 caratteri per 2 linee, SERIALE:
  lcd.print("    SoftPlus    ");       
  lcd.setCursor(0, 1);
  lcd.print("  Consumo Zero  ");
  delay(3000);
  //  
  impulsi_kwh = impulsi_kwh/100;  //per semplificare i calcoli:
}
//
// Inizia ciclo
void loop() {
  //
  //
  // step 1 lettura dei dati digitali:
  //
  tempo_adesso_produzione = millis();
  if (tempo_adesso_produzione > vecchio_millis_produzione + 150)   // bisogna che siano passati almeno 100 millisecondi dall'ultima lettura:
    // (mettere 100 per misure fino a 6 Kw e 200 per misure fino a 3 kw, ma dipende dalla durata dell'impulso). 
  {
    // legge lo stato e percepisce l'impulso sul pin deputato a rilevare i consumi:
    volt_partitore_produzione = analogRead(A1);
    if (volt_partitore_produzione > 200)       // la tensione deve essere maggiore di 1 volt:
    {
      corrente_millis_produzione = millis();
      tempo_trascorso_produzione = corrente_millis_produzione - vecchio_millis_produzione;
      vecchio_millis_produzione = corrente_millis_produzione;
      // calcolo produzione media, in watt, di tempo in tempo:
      // 3200 impulsi ora per 1 Kwh, cioè 1.000 wattora:
      // per ogni 1000 wattora serve 1 impulso ogni 3600/3200 secondi cioè ogni 1,125 secondi:
      // se gli impulsi arrivano ogni 11,25 secondi abbiamo 100 wattora e così via:
      // poi bisogna tradurre in millesimi, laddove 1,125 secondi sono 1.125 millesimi di secondo:
      // potenza_watt_consumi = 1000000 / tempo_trascorso_consumi * 36 / 64;        //valido per contatore da 6400 impulsi         
      // potenza_watt_consumi = 1000000 / tempo_trascorso_consumi * 36 / 32;        //valido per contatore da 3200 impulsi   
      // potenza_watt_consumi = 1000000 / tempo_trascorso_consumi * 36 / 20;        //valido per contatore da 2000 impulsi     
      potenza_watt_produzione = 1000000 / tempo_trascorso_produzione * 36 / impulsi_kwh;     // valido per impostazione iniziale impulsi:      
      val_prod = potenza_watt_produzione;
      impulsi_produzione = impulsi_produzione + 1;
    }
    if (tempo_adesso_produzione > vecchio_millis_produzione + 30000) val_prod = 0;   //dopo 30 secondi senza impulsi:  
    //
    //
    // step 2 elaborazione conteggi:  
    // 
    corrente_millis_minuti = millis();
    if (corrente_millis_minuti > vecchio_millis_minuti + 300000)   // significa che sono passati 5 minuti (60.000 x 5 = 300000):
    {
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
      impulsi_produzione_nuovo = impulsi_produzione;     
      m1 = impulsi_produzione_nuovo - impulsi_produzione_vecchio;
      impulsi_produzione_vecchio = impulsi_produzione_nuovo; 
      impulsi_1h = impulsi_1h + m1 - segmento_vecchio;          // conteggio tipo ruota panoramica:
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
        impulsi_24h = impulsi_24h + h1 - segmento_vecchio;            // conteggio tipo ruota panoramica:
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
          impulsi_30g = impulsi_30g + g1 - segmento_vecchio;                // conteggio tipo ruota panoramica:
        }
        // 
      }
      //
    }
    //
    //
    // step 3 gestione led:  
    //      
    if (corrente_millis_minuti > vecchio_millis_minuti + 3600)   // ogni 36 secondi verifica la produzione:
    { 
      if (val_prod < 10)  
        // produzione inferiore a 10 watt oppure sono passati 2 minuti senza nuovi impulsi:
      {
        digitalWrite(6, HIGH);
        digitalWrite(9, LOW);
        // consente di attivare un led (o un altro carico compatibile) sul pin 13 quando si ha a disposizione 1/2 Kw in più:
        // un eventuale relè che superi i 40 milliamper di consumo deve essere pilotato da un transistor:
      }
      else
      {
        digitalWrite(6, LOW); 
        digitalWrite(9, HIGH);
        // consente di disattivare il led se non si ha a disposizione un 1/2 Kw in più:
      }
      //
    }
    //    
  }
  //
  //
  // step 4 visualizzazione informazioni: 
  // 
  if (conta1 == 15000)
  {
    //    contawatt = impulsi_consumi*1000/3200;   // perchè abbiamo 3200 impulsi per Kwh:  
    contawatt = impulsi_produzione*10/impulsi_kwh;   // abbiamo semplificato il calcolo: 
    if (contawatt > 99999)
    {
      lcd.setCursor(0, 0);
      lcd.print("W_Prod Kwh_Produ");  
      lcd.setCursor(0, 1);
      lcd.print("                "); 
      lcd.setCursor(1, 1);  
      lcd.print(val_prod);
      //      lcd.print(tempo_trascorso_consumi); 
      //     lcd.print(impulsi_consumi);
      lcd.setCursor(10, 1);  
      lcd.print(contawatt/1000); 
    }  
    else 
    {
      lcd.setCursor(0, 0);
      lcd.print("W_prod  Wh_produ");  
      lcd.setCursor(0, 1);
      lcd.print("                "); 
      lcd.setCursor(1, 1);  
      lcd.print(val_prod);
      //      lcd.print(tempo_trascorso_consumi);      
      //      lcd.print(impulsi_consumi);     
      lcd.setCursor(10, 1);  
      lcd.print(contawatt); 
    }   
    //
  } 
  if (conta1 == 25000) 
  {
    //   contawatt_1h = impulsi_1h*1000/impulsi_kwh;   // perchè abbiamo 3200 impulsi per Kwh:  
    //   contawatt_24h = impulsi_24h*1000/impulsi_kwh; 
    //   contawatt_30g = impulsi_30g/impulsi_kwh;        // è espresso in Kwh;
    contawatt_1h = impulsi_1h*10/impulsi_kwh;   // per semplificare il calcolo:  
    contawatt_24h = impulsi_24h*10/impulsi_kwh; 
    contawatt_30g = impulsi_30g/impulsi_kwh/100;        // è espresso in Kwh;   

    lcd.setCursor(0, 0);
    lcd.print("Wh_1h 24h 30ggKw");        
    lcd.setCursor(0, 1);
    lcd.print("                "); 
    lcd.setCursor(0, 1); 
    lcd.print(contawatt_1h); 
    lcd.setCursor(5, 1); 
    lcd.print(contawatt_24h); 
    lcd.setCursor(12, 1); 
    lcd.print(contawatt_30g);    
    conta1 = 0;    
  }    
  //
  conta1 = conta1 + 1;  
  //
}
// FINE listato by SoftPlus Consumo Zero - Faenza.  




































