#include "Arduboy.h"
void setup() {
  arduboy.begin();
  arduboy.setFrameRate(15);
}

void loop() {
  if (!(arduboy.nextFrame())) return;
  arduboy.clear();
  arduboy.setCursor(4, 9);
  arduboy.print(F("Hello, world!"));
  arduboy.display();
}
