/* 
 PENELOPE_2_PLUS
 PROGRAMMA FV AUTOCONSUMO ZERO CON 2 SENSORI DIGITALI, "PENELOPE" E CON PIU' USCITE PWM:
 Versione per display 2x16 SERIALE (A4 e A5)
 Allaccio contatori a A1 e A2
 Uscite PWM: D6, D9 e D10
 autore SoftPlus Consumo Zero - email: luigi.marchi.faenza@gmail.com  
 Versione 2 PLUS per PLC Forum - aggiornamento software del 7 maggio 2016:
 //
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
 precisione, come i contatori Enel; i sensori utilizzati (reperibili presso ELCOTRONIC Srl) richiedono 3200 impulsi ora per un KWora.
 //
 Basetta di interfaccia, tra contatori e Arduino: 2 resistenze da 39 Kohm lato massa e 2 da 100 ohm lato 5 volt.
 Relativamente al numero di impulsi, va utilizzato il software relativo, oppure modificare le istruzioni che fanno riferimento 
 agli impulsi (3200, 2000, 1000, ecc.) 
 
 Modificare l'impostazione del display seriale (LiquiCrystal_I2C) in relazione all'hardware utilizzato:
 Non ci sono trimmer di taratura, neppure software:
 //
 */
#include <Arduino.h> //inclusione della libreria base:
// includere altre librerie:
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
// *******************************DISPLAY SERIALE **************************************************
// LiquidCrystal_I2C lcd(0x20, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);    // LCM1602 IIC A0 A1 A2:
// LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);    // LCM1602 IIC V1:
 LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);    // indirizzo LCD Philips PCF8574AT: 
// **************************************************************************************************
//
// ***************************************************************
int impulsi_x_Kw = 3200;                // dato modificabile:
// ***************************************************************
int parametro_modifica = 3000;          // dato modificabile:
// ***************************************************************
// altre definizioni:
//
// definizione delle variabili e dei pin di INPUT e OUTPUT:
int pin_carico1 = 6;        // carico analogico PWM1:
int pin_carico2 = 9;        // carico analogico PWM2:
int pin_carico3 = 10;        // carico analogico PWM3:
int pin_consumi = A2;       // pin dove arrivano gli impulsi dei consumi:
int pin_produzione = A1;    // pin dove arrivano gli impulsi della produzione fotovoltaica:
// il display seriale va collegato ai pin A4 e A5, oltre a 5 volt e GND: 
int long ciclopwm = 0;       // va bene:
//int ciclopwm = 0;                    // questo va bene:
int ciclopwm1 = 0;                    // questo va bene:
int ciclopwm2 = 0;                    // questo va bene:
int ciclopwm3 = 0;                    // questo va bene:
unsigned long conta1 = 0;
unsigned long conta2 = 0;
unsigned long conta3 = 0;
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
// altre definizioni:
void setup()
{
  pinMode(pin_carico1, OUTPUT);   //pin digitale 8 uscita carico PWM:
  pinMode(pin_carico2, OUTPUT);   //pin digitale 9 uscita carico PWM:  
  pinMode(pin_carico3, OUTPUT);   //pin digitale 10 uscita carico PWM:  
  pinMode(13, OUTPUT);           //pin led, o relè:
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
}
//
// Inizia ciclo
void loop() {
  //
  //
  // step 1 lettura dei dati digitali:
  //
  tempo_adesso_consumi = millis();
  if (tempo_adesso_consumi > vecchio_millis_consumi + 100)   // bisogna che siano passati almeno 100 millisecondi dall'ultima lettura:
  {
    // legge lo stato e percepisce l'impulso sul pin deputato a rilevare i consumi:
    //    buttonState = digitalRead(pin_consumi);
    //    if (buttonState == 1) // oppure if (buttonState == HIGH): è indifferente:
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
      //       potenza_watt_consumi = 1000000 / tempo_trascorso_consumi * 36 / 64;        //valido per contatore da 6400 impulsi:          
      potenza_watt_consumi = 1000000 / tempo_trascorso_consumi * 36 / (impulsi_x_Kw / 100);       //valido per qualunque contatore:  
      //       potenza_watt_consumi = 1000000 / tempo_trascorso_consumi * 36 / 20;        //valido per contatore da 2000 impulsi: 
      //       potenza_watt_consumi = 1000000 / tempo_trascorso_consumi * 36 / 10;        //valido per contatore da 1000 impulsi:         
      val_cons = potenza_watt_consumi;
      val2 = val_cons;
      buttonState = 0;
      modifica_rilevazioni = 1;
      //    delay(100);        // ritardo per stoppare la ricerca degli impulsi e evitare disguidi:
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
      //       potenza_watt_produzione = 1000000 / tempo_trascorso_produzione * 36 / 64;       //valido per contatore da 6400 impulsi:        
      //       potenza_watt_produzione = 1000000 / tempo_trascorso_produzione * 36 / 32;       //valido per contatore da 3200 impulsi:
      potenza_watt_produzione = 1000000 / tempo_trascorso_produzione * 36 / (impulsi_x_Kw / 100);       //valido per qualunque contatore:     
      //       potenza_watt_produzione = 1000000 / tempo_trascorso_produzione * 36 / 10;       //valido per contatore da 1000 impulsi:       
      val_prod = potenza_watt_produzione;
      val1 = val_prod;
      modifica_rilevazioni = 1;
      //    buttonState = 0;
      //    aumentoOK = 0;
      //    delay(100);        // ritardo per stoppare la ricerca degli impulsi e evitare disguidi:
    }
  }
  //
  //
  conta1 = (conta1 + 1);
  conta2 = (conta2 + 1);
  conta3 = (conta3 + 1);
  //
  //
  // step 2 modifica del cicloPWM per azzerare il differenziale tra produzione e consumi:
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
        aumento = (val_prod - val_cons) * 254 / parametro_modifica;   
        // aumento = (val_prod - val_cons) * 254 / 3000;          // è un calcolo di tipo progressivo, molto reattivo:
        // aumento = (val_prod - val_cons) * 254 / 1000;    
        // aumento = (val_prod - val_cons) * 254 / 10000;              
        ciclopwm = ciclopwm + aumento;
      }
      else                                // diminuiamo il carico aggiuntivo:
      {
        aumento = (val_cons - val_prod) * 254 / parametro_modifica; 
        // aumento = (val_cons - val_prod) * 254 / 3000;
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
      analogWrite(pin_carico3, ciclopwm3); 

      //      analogWrite(pin_carico, ciclopwm);
      aumentoOK = 1;       //mettiamo a "1" la variazione di aumento per evitare di ripeterla prima delle nuove misure:

      //
      // step 3 gestione del led di segnalazione sul pin 13, all'occorrenza:
      // predisposto per attivare un relè wireless convenzionale, per attivare il carico (boiler, stufetta, ecc.) in alternativa al RSS:
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
  // step 4 gestione del display relativamente a consumi ( in watt) e produzione fotovoltaico (in watt):
  // la visualizzazione dei dati avviene solo dopo un certo numero di cicli di verifiche Arduino:
  //
  if (conta2 > 20000)     // lo faccio funzionare solo ogni 2 secondi circa:  
  {
    conta2 = 0;
    conta1 = 0;
    lcd.clear();      // pulisce lo schermo:
    lcd.setCursor(0, 0);
    lcd.print("W_Consumi  W_FV ");
    lcd.setCursor(1, 1);
    lcd.print(val2);
    lcd.setCursor(11, 1);
    lcd.print(val1);
  }
  //
  //
  // step 5 gestione del display relativamente a prelievi dalla rete (Enel) o immissioni in rete (GSE), l'uno alternativo all'altro:
  // inoltre viene evidenziato il carico extra per pareggiare produzione e consumi(i cicli PWM vanno da 1 a 254),:
  // ma sono espressi in percentuale (da 0% a 100%):
  // con 3 uscite PWM la percentuale può arrivare fino a 314% la priorità è per i RSS collegato al pin D6, ma se il termostato stacca:
  // questo carico non è attivo e il carico utile passa al RSS collegato al PIN D9 e viene visualizzato in percentuale tra 100% e 200%
  // con altissima produzione FV possono essere attivi i primi 2 RSS e anche il terzo (viene evidenziato sul display un carico compreso 
  // tra il 200% e il 300% oltre il 300% significa che i primi 3 RSS sono attivi o anche disattivati.
  //
  if (conta1 > 11000)            // deve visualizzare, a rotazione, solo ogni 2 secondi circa:  
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
}
// FINE listato by SoftPlus Consumo Zero - Faenza. Per Nuovenergie Srl 




















