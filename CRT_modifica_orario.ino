/* CRT_modifica_orario. Aggiornato da: SoftPlus Consumo Zero.
 * Programma standard, modificato, con visualizzazione a video su display 2X16 seriale.
 * Lo sketch permette di modificare l'ora dell'orologio shield Arduino, tipo Deek-Robot Data logging shield V1.0:
 * modificare giorno,mese, anno, ora e minuti:
 * La modifica dell'orario va fatta prima di caricare su Arduino il software finale.
 * Ovviamente occorre che nel data logger sia inserita la batteria (carica).
 * Può essere necessario modifica la libreria "Liquicrystal_I2C", tra quelle indicate,
 * in funzione del modello di seriale utilizzata (dipende dal tipo di integrato). 
 */
#include <Arduino.h> //inclusione della libreria base:
// includere altre librerie:
//#include <SPI.h> 
//#include <SD.h>    //Loose this in favour of SdFat Library for timestamp 
#include <SdFat.h>   //New SdFat library 
SdFat SD;            //Add this line for backwards compatability with coding 
#include <RTClib.h> 
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
// LiquidCrystal_I2C lcd(0x20, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);    // LCM1602 IIC A0 A1 A2:
// LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);    // LCM1602 IIC V1:
 LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);   
//
//
void setup()
{
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
  //
  //attivo la comunicazione con il DS1307
  //l'indirizzo dell'RTC è 0x68
  Wire.beginTransmission(0x68);
  //il primo byte stabilisce il registro
  //iniziale da scivere
  Wire.write((byte)0x00);
  //specifico il tempo e la data
  Wire.write((byte)0x00); //1° byte SECONDI da 0x00 a 0x59
  Wire.write((byte)0x32); //2° byte MINUTI da 0x00 a 0x59
  Wire.write((byte)0x19); //3° byte ORE da 0x00 a 0x23
  Wire.write((byte)0x04); //4° byte GIORNO della settimana da 0x01 a 0x07
  Wire.write((byte)0x00 | 0x15 ); //5° byte GIORNO del mese da 0x01 a 0x31
  Wire.write((byte)0x00 | 0x8); //6° byte MESE da 0x01 a 0x12
  Wire.write((byte)0x17); //7° byte ANNO 0x00 a 0x99
  Wire.endTransmission();  
  //  
  //
}
void loop()
{
  //inizzializza la trasmissione partendo
  //dall'indirizzo 0x00
  Wire.beginTransmission(0x68);
  Wire.write((byte)0x00);
  Wire.endTransmission();

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
  delay(2000);
  //
  //
  //
  lcd.setCursor(0, 0);
  //    lcd.print("W_Consumi  W_FV ");
  lcd.print("Orario    data  ");
  lcd.setCursor(0, 1); 
  lcd.print("                "); 
  lcd.setCursor(0, 1);   
  lcd.print(ora, HEX);
  lcd.print(":");
  lcd.setCursor(3, 1);  
  lcd.print(minuti, HEX);
  lcd.print("   ");
  lcd.setCursor(6, 1);    
  //  lcd.println(secondi, HEX);
  //  lcd.setCursor(8, 1);    
  //  lcd.print("  ");  

  //  Serial.print("Giorno della settimana: ");
  //  Serial.println(giorno_sett, HEX);

  //  Serial.print("Data corrente: ");
  lcd.setCursor(9, 1);    
  lcd.print(giorno_mese, HEX);
  lcd.print("/");
  lcd.setCursor(12, 1);   
  lcd.print(mese, HEX);
  lcd.print("/");
  lcd.setCursor(14, 1);   
  lcd.println(anno, HEX);
  delay(500);  
}
//FINE














