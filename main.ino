#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <DHT.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

// config
#define LCD_SDA 0
#define LCD_SCL 1

#define DHT_DAT 2

#define BUZ_OUT 3

#define BTN1_IN 4
#define BTN2_IN 5
#define BTN3_IN 6

#define RTC_CLK 7 // SCLK
#define RTC_DAT 8 // IO
#define RTC_RST 9 // CE

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHT_DAT, DHT11);
ThreeWire tw(RTC_DAT, RTC_CLK, RTC_RST);
RtcDS1302<ThreeWire> rtc(tw);

struct Settings {
  enum  PropertiesIndices {
    MIN_T, MAX_T,
    MIN_H, MAX_H
  };
  int position = 0;
  int properties[4] { 
    [MIN_T] = 20, [MAX_T] = 30, [MIN_H] = 40, [MAX_H] = 60
  };
  int positionDisplay[4][2] {
    [MIN_T] = { 0, 0 },
    [MAX_T] = { 4, 0 },
    [MIN_H] = { 0, 1 },
    [MAX_H] = { 4, 1 }
  };
 
  void operator++(int) {
    if (sizeof(position + 1) > 4)
      tone(BUZ_OUT, 500, 500);
    else {
      properties[position]++;
      rtc.SetMemory(position, properties[position]);
    }
  }

  void operator--(int) {
    if (sizeof(position - 1) > 4)
      tone(BUZ_OUT, 500, 500);
    else {
      properties[position]--;
      rtc.SetMemory(position, properties[position]);
    }
  }

  inline void next() {
    position = (position > 2) ? 0 : ++position;
  }

  inline int* getPositionDisplay() {
    return positionDisplay[position];
  }
} settings;

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

  rtc.Begin();
  if (rtc.GetIsRunning()) {
    settings.properties[settings.MIN_T] = rtc.GetMemory(0x0);
    settings.properties[settings.MAX_T] = rtc.GetMemory(0x1);
    settings.properties[settings.MIN_H] = rtc.GetMemory(0x2);
    settings.properties[settings.MAX_H] = rtc.GetMemory(0x3);
  }
  else
    rtc.SetIsRunning(true);
}

void loop() {
  static enum States { CURRENT, SETTINGS } state = CURRENT;
  static auto last = millis();
  char buf[32];

  lcd.setCursor(0, 0);

  if (!digitalRead(BTN1_IN) || !digitalRead(BTN2_IN) || !digitalRead(BTN3_IN))
    state = SETTINGS;

  switch (state) {
    case CURRENT: {
      auto t = dht.readTemperature(), h = dht.readHumidity();
  
      if (t < settings.MIN_T || t > settings.MAX_T || h < settings.MIN_H || h > settings.MAX_H)
        tone(BUZ_OUT, 1000, 1000);

      lcd.noBlink();
      snprintf(buf, sizeof buf, "%2.2f*C %2.2f%%  ", t, h);
      lcd.print(buf);
      lcd.setCursor(0, 1);
      lcd.print("                ");
      break;
    }

    case SETTINGS:
    if (!digitalRead(BTN1_IN))
      settings.next();

    if (!digitalRead(BTN2_IN))
      settings--;

    if (!digitalRead(BTN3_IN))
      settings++;
    
    snprintf(buf, sizeof buf, "%3d<%3d*C       ", settings.properties[settings.MIN_T], settings.properties[settings.MAX_T]);
    lcd.print(buf);
    lcd.setCursor(0, 1);
    snprintf(buf, sizeof buf, "%3d<%3d%%        ", settings.properties[settings.MIN_H], settings.properties[settings.MAX_H]);
    lcd.print(buf);
    lcd.blink();

    auto tmp = settings.getPositionDisplay();
    lcd.setCursor(tmp[0], tmp[1]);

    if (millis() - last >= 2000) {
      state = CURRENT;
      last = millis();
    }

    break;
  }

  delay(100);
}
