#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define CAVE_RADIUS 18
#define CAVE_RADIUS_INNER 13
#define CAVES 20
#define WUMPUS 1
#define BATS 2
#define PIT 3
#define SHOOT 1
#define MOVE 0

class Cave {

 public:
  Cave(void);
  void initCave(Adafruit_GFX *gfx, int16_t x, int16_t y);
  void drawCave();
  boolean contains(int16_t x, int16_t y);

  void press(boolean p);
  int enter();
  int shoot();
  void reset();
  void leave();
  void becomeNeighbour();
  void writeShootResult(int hit);
  boolean isPressed();
  boolean justPressed();
  boolean justReleased();
  void setState(int16_t newState);
  int16_t getState();
  Cave *cave1 = 0, *cave2 = 0, *cave3 = 0;

  static void drawMap(Adafruit_GFX *tft, Cave caves[]);

 private:
  Adafruit_GFX *_gfx;
  int16_t _x, _y, state = 0;
  boolean currstate, laststate, isEntered;
};

