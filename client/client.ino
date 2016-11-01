#include <Arduboy.h>
Arduboy arduboy;
#define PLAYER_OWN (0)
#define PLAYER_OPP (1)
#define PLAYERS    (2)
char keymapMP[PLAYERS]={0x00, 0x00};
void setup() {
  arduboy.beginNoLogo();
  arduboy.setFrameRate(15);
  Serial.setTimeout(100);
  Serial.begin(9600);
}
#if 1
  char sss[4]="ABC";
  int ssi=0;
#endif
void loop() {
  //PIN -> latch own key -> cMPKey
  for(int k=2;k<8;k++){
    keymapMP[PLAYER_OWN] |= arduboy.pressed(_BV(k))?0x00:_BV(k);
  }
  // in active frame
  if (!(arduboy.nextFrame())) return;
  keymapMP[PLAYER_OWN]=0;
  
#if 1
  keymapMP[PLAYER_OWN]=sss[ssi];
  ssi++;
  ssi%=3;
#endif
  //Serial -> receive opp key -> cMPKey
  Serial.write(&keymapMP[PLAYER_OWN],1);
  Serial.readBytes(&keymapMP[PLAYER_OPP],1);

  //clear for next
  procAll();
  keymapMP[PLAYER_OWN]=0x00;
}
void procAll(){
  drawAll();
}
void drawAll(){
  arduboy.clear();
  arduboy.setCursor( 0, 0);
  
  arduboy.print("recv:");
  arduboy.print((int)keymapMP[PLAYER_OPP]);
  arduboy.println();
  
  arduboy.print("send:");
  arduboy.print((int)keymapMP[PLAYER_OWN]);
  arduboy.println();
  arduboy.display();
  #if 0
  arduboy.clear();
  arduboy.setCursor( 0, 0);
  arduboy.println("my key:");
  arduboy.setCursor( 8, 8);
  arduboy.println((keymapMP[PLAYER_OWN]&UP_BUTTON)?"":"U");
  arduboy.setCursor( 0,16);
  arduboy.println((keymapMP[PLAYER_OWN]&LEFT_BUTTON)?"":"L");
  arduboy.setCursor(16,16);
  arduboy.println((keymapMP[PLAYER_OWN]&RIGHT_BUTTON)?"":"R");
  arduboy.setCursor(32,16);
  arduboy.println((keymapMP[PLAYER_OWN]&A_BUTTON)?"":"A");
  arduboy.setCursor(40,16);
  arduboy.println((keymapMP[PLAYER_OWN]&B_BUTTON)?"":"B");
  arduboy.setCursor( 8,24);
  arduboy.println((keymapMP[PLAYER_OWN]&DOWN_BUTTON)?"":"D");

  arduboy.setCursor( 0,32);
  arduboy.println("your key:");
  arduboy.setCursor( 8,40);
  arduboy.println((keymapMP[PLAYER_OPP]&UP_BUTTON)?"":"U");
  arduboy.setCursor( 0,48);
  arduboy.println((keymapMP[PLAYER_OPP]&LEFT_BUTTON)?"":"L");
  arduboy.setCursor(16,48);
  arduboy.println((keymapMP[PLAYER_OPP]&RIGHT_BUTTON)?"":"R");
  arduboy.setCursor(32,48);
  arduboy.println((keymapMP[PLAYER_OPP]&A_BUTTON)?"":"A");
  arduboy.setCursor(40,48);
  arduboy.println((keymapMP[PLAYER_OPP]&B_BUTTON)?"":"B");
  arduboy.setCursor( 8,56);
  arduboy.println((keymapMP[PLAYER_OPP]&DOWN_BUTTON)?"":"D");

  arduboy.display();
  #endif
}
