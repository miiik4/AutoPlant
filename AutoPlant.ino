// importer biblioteker
#include <LiquidCrystal_I2C.h>
#include <ThreeWire.h>  
#include <RtcDS1302.h>

ThreeWire myWire(6,7,8);
RtcDS1302<ThreeWire> Rtc(myWire);

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

// definerer alle verdier
int encoderPosCount = 0; 
int pinALast;
int aVal;
boolean btn;
int selected = 0;

unsigned long pumpTime = 0;
unsigned long checkTime = 0;

int currentHour;
int targetHour = 0;
int lightDuration = 0;
int startTime = 8; // kl. 0800
boolean lightStatus = false;

int targetHum = 0;
int failSafePump = 0;

// testverdi
int trueHum = 20;

void setup() { 
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
   
  lcd.begin(16, 2);
  
  lcd.setCursor(1, 0);
  lcd.print("Fuktighet:");
  lcd.setCursor(1,1);
  lcd.print("Lysmengde:");

  // startverdier for UI
  lcd.setCursor(0,0);
  lcd.print(">");
  lcd.setCursor(12,0);
  lcd.print(0);
  lcd.print("%");
  lcd.setCursor(12,1);
  lcd.print(0);
  lcd.print("t");
   
  pinALast = digitalRead(2);

  // henter starttid
  RtcDateTime checkTime = Rtc.GetDateTime();
  currentHour = checkTime.Hour();
    
  Serial.begin (9600);
 } 

void loop() {

  /* Lys */
  
  // sjekker hvilken time det er hvert 10 minutt
  if(millis() >= checkTime + 100000){
    checkTime += 100000;
    RtcDateTime checkTime = Rtc.GetDateTime();
    currentHour = checkTime.Hour();
  }

  // fikser problemet med 24timers klokke
  targetHour = startTime+lightDuration;
  if(currentHour == 0){
    targetHour = targetHour - 24;
  } else if(currentHour == startTime){
    targetHour = targetHour + 24;
  }

  // bytter status på lys basert på tid og status
  if(targetHour > currentHour && lightStatus == false){
    lightStatus = true;
    Serial.println("Lights ON");
  } else if(targetHour <= currentHour && lightStatus == true) {
    lightStatus = false;
    Serial.println("Light Off");
  } /* Slutt lys */

  /* vanning */
  
  // stopper programmet om pumpe kjører mer enn 6 ganger/ 1 minutt
  if(failSafePump > 6){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("FailSafe: Pumpe");
    delay(0xFFFFFFFF);
  }

  // venter 10sek før den kjører pumpe på ny
  if(targetHum > trueHum){
    if(millis() >= pumpTime + 10000){
    pumpTime += 10000;
    Serial.println("run pump...");
    failSafePump++;
    }
  } else {
    failSafePump = 0;
  } /* slutt vanning */

  /* program UI */
  
  aVal = digitalRead(2);
  btn = digitalRead(4);

  // Valg av meny printer riktig valg
  if(btn == LOW){ 
    if(selected == 1){
      selected = 0;
      lcd.setCursor(0,1);
      lcd.print(" ");
      lcd.setCursor(0,0);
      lcd.print(">");
      delay(500);
    } else if (selected == 0) {
      selected = 1;
      lcd.setCursor(0,0);
      lcd.print(" ");
      lcd.setCursor(0,1);
      lcd.print(">");
      delay(500);
    }
  }

  if(selected == 0){
    if (aVal != pinALast){
      if (digitalRead(3) != aVal && targetHum < 100) {
      targetHum ++;
      } else if(digitalRead(3) == aVal && targetHum > 0) {
      targetHum--;
      }
    lcd.setCursor(12,0);
    lcd.print(targetHum);
    lcd.print("%  ");
    } 
    pinALast = aVal;
    
  } else if(selected == 1) {
    if (aVal != pinALast){
      if (digitalRead(3) != aVal && lightDuration < 24) {
      lightDuration ++;
      } else if(digitalRead(3) == aVal && lightDuration > 0) {
      lightDuration--;
      }
    lcd.setCursor(12,1);
    lcd.print(lightDuration);
    lcd.print("t ");
    } 
    pinALast = aVal;
  } /* Slutt program UI */
}
