#include <SPFD5408_Adafruit_GFX.h>
#include "Cave.h"

Cave::Cave(void) {
  _gfx = 0;
}

void Cave::initCave(
 Adafruit_GFX *gfx, int16_t x, int16_t y)
{
  _x            = x;
  _y            = y;
  _gfx          = gfx;
}

void Cave::drawCave() {
    _gfx->fillCircle(_x, _y, CAVE_RADIUS, RED);
    _gfx->fillCircle(_x, _y, CAVE_RADIUS_INNER, YELLOW);
}

boolean Cave::contains(int16_t x, int16_t y) {
  if ((x < (_x - CAVE_RADIUS / 2)) || (x > (_x + CAVE_RADIUS / 2))) return false;
  if ((y < (_y - CAVE_RADIUS / 2)) || (y > (_y + CAVE_RADIUS / 2))) return false;
  return true;
}

int Cave::enter() {
    if (isEntered)
    {
      return 0;
    }

    _gfx->fillRect(0, 0, 200, 70, BLACK);
    _gfx->setCursor(0, 10);
    _gfx->setTextSize(2);
    _gfx->setTextColor(RED);
    isEntered = true;
    _gfx->fillCircle(_x, _y, CAVE_RADIUS_INNER, GREEN);
    if (state == WUMPUS)
    {
      _gfx->println("Eaten by wumpus");
      _gfx->fillCircle(_x, _y, CAVE_RADIUS_INNER - 5, RED);
      return 1;
    }
    if (state == PIT)
    {
      _gfx->println("Fell in a\nbottomless pit");
      _gfx->fillCircle(_x, _y, CAVE_RADIUS_INNER - 5, RED);
      return 1;
    }

    if (state == BATS)
    {
      _gfx->println("Carried away by\nsuperbats");
      delay(1000);
      return 0;
    }
    
    _gfx->setTextColor(GREEN);
    if (cave1)
    {
      cave1->becomeNeighbour();
    }
    if (cave2)
    {
      cave2->becomeNeighbour();
    }
    if (cave3)
    {
      cave3->becomeNeighbour();
    }

    return 0;
}

void Cave::becomeNeighbour() {
    _gfx->drawCircle(_x, _y, CAVE_RADIUS_INNER - 5, BLUE);
    _gfx->drawCircle(_x, _y, CAVE_RADIUS_INNER - 10, BLUE);
    if (state == WUMPUS)
    {
      _gfx->println("I smell a wumpus");
    }
    if (state == PIT)
    {
      _gfx->println("I feel a draft");
    }
    if (state == BATS)
    {
      _gfx->println("Bats nearby");
    }
}

int Cave::shoot() {
    if (state == WUMPUS)
    {
      writeShootResult(1);
      return 1;
    }

    writeShootResult(0);
    delay(100);
    return 0;
}

void Cave::reset() {
    _gfx->fillCircle(_x, _y, CAVE_RADIUS_INNER, YELLOW);
}

void Cave::leave() {
    isEntered = false;
    reset();
    if (cave1)
    {
      cave1->reset();
    }
    if (cave2)
    {
      cave2->reset();
    }
    if (cave3)
    {
      cave3->reset();
    }
}

void Cave::setState(int16_t newState)
{
  state = newState;
}

int16_t Cave::getState()
{
  return state;
}


boolean Cave::isPressed() { return currstate; }
boolean Cave::justPressed() { return (currstate && !laststate); }
boolean Cave::justReleased() { return (!currstate && laststate); }

void Cave::drawMap(Adafruit_GFX *tft, Cave caves[]) {
  int16_t height = tft->height() - 1;
  int16_t width = tft->width() - 1;
  tft->fillScreen(BLACK);
  tft->drawRect(0, height - width, width, width, WHITE);
  int16_t centreX = width / 2;
  int16_t centreY = height - width / 2;
  float radius1 = 30;
  float radius2 = 65;
  float radius3 = 100;
  int sides = 5;
  int16_t x;
  int16_t y;
  int16_t x2;
  int16_t y2;
  int caveNo = 0;

  tft->fillCircle(centreX, centreY, radius3 + 4, RED);
  tft->fillCircle(centreX, centreY, radius3 - 2, RED);
  tft->fillCircle(centreX, centreY, radius3 - 4, BLACK);
  tft->fillCircle(centreX, centreY, radius2 + 4, RED);
  tft->fillCircle(centreX, centreY, radius2 - 2, RED);
  tft->fillCircle(centreX, centreY, radius2 - 4, BLACK);
  tft->fillCircle(centreX, centreY, radius1 + 4, RED);
  tft->fillCircle(centreX, centreY, radius1 - 2, RED);
  tft->fillCircle(centreX, centreY, radius1 - 4, BLACK);
  
  float increment = PI / sides;
  float angleCorrect = 0.5 * increment ;
  for (float angle = 0; angle < 2 * PI; angle += increment)
  {
    x = (int16_t)(centreX + cos(angle - angleCorrect) * radius2);
    y = (int16_t)(centreY + sin(angle - angleCorrect) * radius2);
    caves[caveNo].initCave(tft, x, y);
    caves[caveNo].drawCave();
    if (angle > 0)
    {
      caves[caveNo].cave2 = &caves[caveNo - 1];
      caves[caveNo - 1].cave1 = &caves[caveNo];
    }
    caveNo++;
  }
  caves[0].cave2 = &caves[caveNo - 1];
  caves[caveNo - 1].cave1 = &caves[0];

  increment = 2 * PI / sides;
  angleCorrect = 0.25 * increment ;
  for (float angle = 0; angle < 2 * PI; angle += increment)
  {
    x = (int16_t)(centreX + cos(angle + angleCorrect) * radius1);
    y = (int16_t)(centreY + sin(angle + angleCorrect) * radius1);
    caves[caveNo].initCave(tft, x, y);
    caves[caveNo].drawCave();
    caves[caveNo].cave3 = &caves[2 * (caveNo - sides * 2) + 1];
    caves[2 * (caveNo - sides * 2) + 1].cave3 = &caves[caveNo];
    if (angle > 0)
    {
      caves[caveNo].cave2 = &caves[caveNo - 1];
      caves[caveNo - 1].cave1 = &caves[caveNo];
    }

    caveNo++;
    x2 = (int16_t)(centreX + cos(angle + angleCorrect) * radius2);
    y2 = (int16_t)(centreY + sin(angle + angleCorrect) * radius2);
    tft->drawLine(x - 1, y - 1, x2 - 1, y2 - 1, YELLOW);
    tft->drawLine(x, y, x2, y2, YELLOW);
    tft->drawLine(x + 1, y + 1, x2 + 1, y2 + 1, YELLOW);
  }
  caves[caveNo - sides].cave2 = &caves[caveNo - 1];
  caves[caveNo - 1].cave1 = &caves[caveNo - sides];

  angleCorrect = 0.25 * increment;
  for (float angle = 0; angle < 2 * PI; angle += increment)
  {
    x = (int16_t)(centreX + cos(angle - angleCorrect) * radius3);
    y = (int16_t)(centreY + sin(angle - angleCorrect) * radius3);
    caves[caveNo].initCave(tft, x, y);
    caves[caveNo].drawCave();
    caves[caveNo].cave3 = &caves[2 * (caveNo - sides * 3)];
    caves[2 * (caveNo - sides * 3)].cave3 = &caves[caveNo];
    if (angle > 0)
    {
      caves[caveNo].cave2 = &caves[caveNo - 1];
      caves[caveNo - 1].cave1 = &caves[caveNo];
    }

    caveNo++;
    x2 = (int16_t)(centreX + cos(angle - angleCorrect) * radius2);
    y2 = (int16_t)(centreY + sin(angle - angleCorrect) * radius2);
    tft->drawLine(x - 1, y - 1, x2 - 1, y2 - 1, YELLOW);
    tft->drawLine(x, y, x2, y2, YELLOW);
    tft->drawLine(x + 1, y + 1, x2 + 1, y2 + 1, YELLOW);
  }
  caves[caveNo - sides].cave2 = &caves[caveNo - 1];
  caves[caveNo - 1].cave1 = &caves[caveNo - sides];
    
  tft->drawCircle(centreX, centreY, radius1, YELLOW);
  tft->drawCircle(centreX, centreY, radius2, YELLOW);
  tft->drawCircle(centreX, centreY, radius3, YELLOW);
  tft->drawCircle(centreX, centreY, radius1 + 1, YELLOW);
  tft->drawCircle(centreX, centreY, radius2 + 1, YELLOW);
  tft->drawCircle(centreX, centreY, radius3 + 1, YELLOW);
  tft->drawCircle(centreX, centreY, radius1 - 1, YELLOW);
  tft->drawCircle(centreX, centreY, radius2 - 1, YELLOW);
  tft->drawCircle(centreX, centreY, radius3 - 1, YELLOW);
}

void Cave::writeShootResult(int hit) {
    if (hit) {
      // write number of arrowa
      _gfx->setCursor(50, 180);
      _gfx->setTextSize(6);
      _gfx->setTextColor(GREEN, BLACK);
      _gfx->print("HIT!");
      return;       
    }

    _gfx->setCursor(5, 105);
    _gfx->setTextSize(1);
    _gfx->setTextColor(WHITE, BLACK);
    _gfx->print("MISSED!");  
}

