// import libraries
#include <ThreeWire.h>  
#include <RtcDS1302.h>
#include <LiquidCrystal_I2C.h>

ThreeWire myWire(6,7,8);
RtcDS1302<ThreeWire> Rtc(myWire);
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

// UI and control variables
int encoderPosCount, pinALast, aVal, selected = 0; 
unsigned long pumpTime, checkTime = 0;
boolean btn;

// light variables
int currentHour, targetHour, lightDuration = 0;
boolean lightStatus = false;
int startTime = 8; // kl. 0800

// water variables
const int csmsAirVal = 873; // calibrated 23.09.20
const int csmsWaterVal = 585; // calibrated 23.09.20
int targetHum, failSafePump, csmsVal, csmsPercent = 0;

void setup() { 
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
   
  lcd.begin(16, 2);
  
  lcd.setCursor(1, 0);
  lcd.print("Fuktighet:");
  lcd.setCursor(1,1);
  lcd.print("Lysmengde:");

  // starting values for UI
  lcd.setCursor(0,0);
  lcd.print(">");
  lcd.setCursor(12,0);
  lcd.print(0);
  lcd.print("%");
  lcd.setCursor(12,1);
  lcd.print(0);
  lcd.print("t");
   
  pinALast = digitalRead(2);

  // getting starttime
  RtcDateTime checkTime = Rtc.GetDateTime();
  currentHour = checkTime.Hour();
    
  Serial.begin (9600);
 } 

void loop() {

  /* Light */
  
  // checking what hour it is every 10 minute
  if(millis() >= checkTime + 100000){
    checkTime += 100000;
    RtcDateTime checkTime = Rtc.GetDateTime();
    currentHour = checkTime.Hour();
  }

  // will account for 24 hour clock
  targetHour = startTime+lightDuration;
  if(currentHour == 0){
    targetHour = targetHour - 24;
  } else if(currentHour == startTime){
    targetHour = targetHour + 24;
  }

  // changing light status based on time and status
  if(targetHour > currentHour && lightStatus == false){
    lightStatus = true;
    Serial.println("Lights ON");
  } else if(targetHour <= currentHour && lightStatus == true) {
    lightStatus = false;
    Serial.println("Light Off");
  } /* Light END */

  /* Watering */
  
  // stops program if it runs more than 6 times in a minute
  if(failSafePump > 6){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("FailSafe: Pumpe");
    delay(0xFFFFFFFF);
  }

  csmsVal = analogRead(0);
  csmsPercent = map(csmsVal, csmsAirVal, csmsWaterVal, 0, 100);

  // wait 10 sec before run pump again
  if(targetHum > csmsPercent){
    if(millis() >= pumpTime + 10000){
    pumpTime += 10000;
    Serial.println("run pump...");
    failSafePump++;
    }
  } else {
    failSafePump = 0;
  } /* Watering end */

  /* program UI */
  
  aVal = digitalRead(2);
  btn = digitalRead(4);

  // Menu choise prints correct menu
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
  } /* program UI END */
}
