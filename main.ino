#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <DHT.h>

// config
#define LCD_SDA 0
#define LCD_SCL 1

#define DHT_DAT 2

#define BUZ_OUT 3

#define BTN1_IN 4
#define BTN2_IN 5
#define BTN3_IN 6
// ---

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHT_DAT, DHT11);

enum States { disp, set } state = disp;
int max_tem = 30, min_tem = 20, max_hum = 60, min_hum = 40;
int set_pos = 0;

void setup() {
  Wire.setSDA(LCD_SDA);
  Wire.setSCL(LCD_SCL);
  Wire.begin();

  lcd.init();
  lcd.backlight();

  dht.begin();

  pinMode(BUZ_OUT, OUTPUT);

  pinMode(BTN1_IN, INPUT_PULLUP);
  pinMode(BTN2_IN, INPUT_PULLUP);
  pinMode(BTN3_IN, INPUT_PULLUP);
}

void loop() {
  static unsigned long last = millis();
  char buf[32];
  lcd.setCursor(0, 0);

  if (!digitalRead(BTN1_IN) || !digitalRead(BTN2_IN) || !digitalRead(BTN3_IN))
    state = set;

  switch (state) {
    case disp: {
      float hum = dht.readHumidity(), tem = dht.readTemperature();
  
      if (tem > max_tem || tem < min_tem || hum > max_hum || hum < min_hum) {
        tone(BUZ_OUT, 500, 1000);
      }

      lcd.noBlink();
      snprintf(buf, sizeof buf, "%2.2f%% %2.2f*C  ", hum, tem);
      lcd.print(buf);
      lcd.setCursor(0, 1);
      lcd.print("                ");
      break;
    }

    case set:
    if (!digitalRead(BTN1_IN)) {
      if (set_pos > 2) set_pos = 0;
      else set_pos++;
    }

    if (!digitalRead(BTN2_IN)) {
      switch (set_pos) {
        case 0:
        max_tem--;
        break;

        case 1:
        min_tem--;
        break;

        case 2:
        max_hum--;
        break;

        case 3:
        min_hum--;
        break;
      }
    }

    if (!digitalRead(BTN3_IN)) {
      switch (set_pos) {
        case 0:
        max_tem++;
        break;

        case 1:
        min_tem++;
        break;

        case 2:
        max_hum++;
        break;

        case 3:
        min_hum++;
        break;
      }
    }

    lcd.blink();
    snprintf(buf, sizeof buf, "%3d<%3d*C       ", min_tem, max_tem);
    lcd.print(buf);
    lcd.setCursor(0, 1);
    snprintf(buf, sizeof buf, "%3d<%3d%%        ", min_hum, max_hum);
    lcd.print(buf);

    switch (set_pos) {
      case 0:
      lcd.setCursor(4, 0);
      break;

      case 1:
      lcd.setCursor(0, 0);
      break;

      case 2:
      lcd.setCursor(4, 1);
      break;

      case 3:
      lcd.setCursor(0, 1);
      break;
    }

    if (millis() - last >= 2000) {
      state = disp;
      last = millis();
    }

    break;
  }

  delay(100);
}
