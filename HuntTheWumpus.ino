#include <SPFD5408_Adafruit_GFX.h>    // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library
#include <SPFD5408_TouchScreen.h>
#include "Cave.h"

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

// Calibrate values
#define TS_MINX 108
#define TS_MINY 99
#define TS_MAXX 835
#define TS_MAXY 842

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define BOXSIZE 40
#define PENRADIUS 3
int oldcolor, currentcolor;
int gameOver = 0;
int mode = MOVE;
int arrows = 2;

int16_t height = tft.height() - 1;
int16_t width = tft.width() - 1;
Cave *activeCave;

Cave caves[CAVES];
Adafruit_GFX_Button shootButton;
Adafruit_GFX_Button moveButton;

void setup(void) {
  randomSeed(analogRead(A0));
  tft.reset();
  tft.begin(0x9341); // SDFP5408
  tft.setRotation(0); // Need for the Mega, please changed for your choice or rotation initial

  Cave::drawMap(&tft, caves);
  shootButton.initButton(&tft,                           // TFT object
                210, 95,                            // x, y,
                50, 20, WHITE, BLUE, WHITE,         // w, h, outline, fill, 
                "Shoot", 1);                           // text
  moveButton.initButton(&tft,                           // TFT object
                210, 95,                            // x, y,
                50, 20, WHITE, BLUE, WHITE,         // w, h, outline, fill, 
                "Move", 1);                           // text
  int index;

  // Set WUMPUS
  int count = 1;
  do
  {
    index = (int)random(20);
    if (!caves[index].getState())
    {
      caves[index].setState(WUMPUS);
      count--;
    }
  } while (count);

  // Set pits
  count = 2;
  do
  {
    index = (int)random(20);
    if (!caves[index].getState())
    {
      caves[index].setState(PIT);
      count--;
    }
  } while (count);

  // Set BATS
  count = 2;
  do
  {
    index = (int)random(20);
    if (!caves[index].getState())
    {
      caves[index].setState(BATS);
      count--;
    }
  } while (count);

  count = 1;
  do
  {
    index = (int)random(20);
    if (!caves[index].getState())
    {
      caves[index].enter();
      activeCave = &caves[index];
      count--;
    }
  } while (count);
  shootButton.drawButton();
  drawArrows();
  tft.setCursor (15, 20);
  tft.setTextSize (2);
  tft.setTextColor(CYAN);
  pinMode(13, OUTPUT);
  Serial.begin(9600);
}

#define MINPRESSURE 10
#define MAXPRESSURE 5000

void loop()
{
  if (gameOver)
  {
    return;
  }
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
    Serial.println(p.z);
    // Check buttons
    if (shootButton.contains(p.x, p.y)) {
      mode = mode == SHOOT ? MOVE : SHOOT;
      if (mode == SHOOT) {
        moveButton.drawButton();
      } else {
        shootButton.drawButton();
      }
      delay(100);      
    } 
    
    // Check caves
    if (activeCave->cave1->contains(p.x, p.y)) {
      if (mode == SHOOT)
      {
        gameOver = activeCave->cave1->shoot() || --arrows == 0;
      } else {
        activeCave->leave();
        activeCave = activeCave->cave1;
        gameOver = activeCave->enter(); 
      }
    } 
    else if (activeCave->cave2->contains(p.x, p.y)) {
      if (mode == SHOOT)
      {
        gameOver = activeCave->cave2->shoot() || --arrows == 0;
        drawArrows();
      } else {
        activeCave->leave();
        activeCave = activeCave->cave2;
        gameOver = activeCave->enter(); 
      }
    } 
    else if (activeCave->cave3->contains(p.x, p.y)) {
      if (mode == SHOOT)
      {
        gameOver = activeCave->cave3->shoot() || --arrows == 0;
      } else {
        activeCave->leave();
        activeCave = activeCave->cave3;
        gameOver = activeCave->enter(); 
      }
    }

    if (activeCave->getState() == BATS)
    {
      activeCave->leave();
      int index = (int)random(20);
      activeCave = &caves[index];      
      gameOver = activeCave->enter();
    }
  }
}

void drawArrows() {
    // write number of arrowa
    tft.setCursor(0, 85);
    tft.setTextSize(2);
    tft.setTextColor(RED);
    tft.print(arrows);  
}


