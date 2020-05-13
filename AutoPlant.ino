#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

int encoderPosCount = 0; 
int pinALast;  
int aVal;
boolean btn;
int selected = 0;

int hum = 0;
int light = 0;

void setup() { 
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
   
  lcd.begin(16, 2);
  lcd.setCursor(1, 0);
  lcd.print("Fuktighet:");

  lcd.setCursor(1,1);
  lcd.print("Lysmengde:");

  // startverdier
  lcd.setCursor(0,0);
  lcd.print(">");
  lcd.setCursor(12,0);
  lcd.print(0);
  lcd.print("%");
  lcd.setCursor(12,1);
  lcd.print(0);
  lcd.print("t");
   
  pinALast = digitalRead(2);
  Serial.begin (9600);
 } 

void loop() {
  aVal = digitalRead(2);
  btn = digitalRead(4);

  // her velger man og printer riktig valg
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
      if (digitalRead(3) != aVal && hum < 100) {
      hum ++;
      } else if(digitalRead(3) == aVal && hum > 0) {
      hum--;
      }
    lcd.setCursor(12,0);
    lcd.print(hum);
    lcd.print("%  ");
    } 
    pinALast = aVal;

    
  } else if(selected == 1) {
    if (aVal != pinALast){
      if (digitalRead(3) != aVal && light < 24) {
      light ++;
      } else if(digitalRead(3) == aVal && light > 0) {
      light--;
      }
    lcd.setCursor(12,1);
    lcd.print(light);
    lcd.print("t ");
    } 
    pinALast = aVal;
  }
} 
