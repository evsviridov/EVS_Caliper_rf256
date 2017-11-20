
#include <EVS_Caliper_rf256.h>
#define SOFT_SERIAL
#define RS485_TX_RX_PIN -1
#define PORT_SPEED ((unsigned long) 9600)
//#define DO_IDN

//#define LIQUID_CRYSTAL
#define TFTLIB

#ifdef LIQUID_CRYSTAL
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3f, 16, 2);
//LiquidCrystal_I2C lcd(0x27, 16, 2);
#endif

#ifdef TFTLIB
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <Fonts/FreeMonoBold24pt7b.h>
// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft;
#endif


#define NUM_OF_CALIPERS 2
uint8_t address[NUM_OF_CALIPERS] = {1, 2};
Caliper_RF256 rf256[NUM_OF_CALIPERS];


#ifndef SOFT_SERIAL
#define HARD_SERIAL
#endif

#ifdef HARD_SERIAL
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
HardwareSerial *p_HardSerial = &Serial3;
#else
#error "Only MEGA board support Hardware Serial"
#endif // ... __AVR_ATmega2560__

#endif // HARD_SERIAL

#ifdef SOFT_SERIAL
#define rxPin 3
#define txPin 2
SoftwareSerial SoftSerial(rxPin, txPin);
SoftwareSerial *p_SoftSerial = &SoftSerial;
#endif // SOFT_SERIAL

String makeString(void)
{
  String str = "";
  for (int i = 0; i < NUM_OF_CALIPERS; ++i)
    str += String(((long)(rf256[i].resultFloat * 1000)) / 1000.0f, 3) + "\t";
  str = str + "AVR uptime: " + String(millis() / 1000) + " sec.";
  return str;
}
void displayBegin()
{
#ifdef LIQUID_CRYSTAL
  lcd.init();
  lcd.backlight();
#endif
#ifdef TFTLIB
  tft.reset();
  uint16_t identifier = tft.readID();

  if (identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if (identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if (identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if (identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if (identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Adafruit 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    return;
  }
  tft.begin(identifier);
  tft.fillScreen(BLACK);
  tft.setRotation(1);
#endif
}

void displayResult(void)
{
  static unsigned long oldTimer = 0;
  static float val, prevVal[NUM_OF_CALIPERS];
  if (micros() < (oldTimer + 200E3)) return;
  oldTimer = micros();
  String str = makeString();
#ifdef LIQUID_CRYSTAL
  lcd.setCursor(0, 1);
  lcd.print(str);
#endif

#ifdef TFTLIB
  //  tft.fillScreen(BLACK);
  char fstr[8];

  for (int i = 0; i < NUM_OF_CALIPERS; ++i)
  {
    val = ((long)(rf256[i].resultFloat * 1000)) / 1000.0;
    if (val != prevVal[i])
    {
      tft.fillRect(0, 70 * i, tft.width(), 70, BLACK);
      tft.setCursor(0, 60 + 70 * i);
      tft.setTextColor(WHITE);
      fstr[0] = (val < 0) ? '-' : ' ';
      dtostrf( fabsf( val ), 6, 3, &fstr[1]);
      tft.println(fstr);
      prevVal[i] = val;
    }
  }
#endif
}

void setup()
{
  String str;
  displayBegin();
  tft.setTextColor(YELLOW);
  tft.setCursor(0, 60);
  tft.println("Caliper initializing...");

  Serial.begin(115200);
  while (!Serial) ;
  delay(5000);
  while(Serial.available()) Serial.read();
  
  
  Serial.println("SSID?");
  str = Serial.readString();
  tft.setCursor(0, 190);
  tft.println(str);

  Serial.println("IP?");
  str = Serial.readString();
  tft.setCursor(0, 210);
  tft.println(str);

  tft.setFont(&FreeMonoBold24pt7b);
  tft.setTextSize(1);

  //  Serial3.begin(115200); //for Arduino MEGA + 8266 board
  //  Serial.println("RF256 Caliper");
#ifdef HARD_SERIAL
  p_HardSerial->begin(PORT_SPEED);
#else
  p_SoftSerial->begin(PORT_SPEED);
#endif

  for (int i = 0; i < NUM_OF_CALIPERS; ++i)
  {
#ifdef HARD_SERIAL
    rf256[i].begin(address[i], p_HardSerial, RS485_TX_RX_PIN, PORT_SPEED);
#endif // HARD_SERIAL
#ifdef SOFT_SERIAL
    rf256[i].begin(address[i], p_SoftSerial, RS485_TX_RX_PIN, PORT_SPEED);
#endif // SOFT_SERIAL
#ifdef DO_IDN
    rf256[i].doIDN();
    rf256[i].printIDN();
#endif // DO_IDN
  }
}

void loop()
{
  static unsigned long delayTime = (unsigned long)100E3, doTime = delayTime;

  if ((unsigned long)micros() < doTime) return;
  doTime += delayTime;
  for (int i = 0; i < NUM_OF_CALIPERS; ++i)
    rf256[i].doMeas();
  Serial.println(makeString());
  //  Serial3.println(makeString());
  displayResult();
}
