#include <Wire.h>  
#include <LiquidCrystal_I2C.h>
//#define ENCODER_USE_INTERRUPTS
#include "EncoderMenu.h"
#include <LiquidMenu.h>

LiquidCrystal_I2C lcd(0x3f,16,2);
EncoderMenu rotary(2,3,4, false , true);

char* playermenu[] = {"1 Play", "2 Next", "3 Previous", "4 Stop", "5 Resume"};
int npointer = 0;
int* pointermenu = &npointer;

char uptext[] = "Play  ";
char *uptr = playermenu[0] ;

char downtext[] = "Next   ";
char *dptr = playermenu[1];

String txt = "Time";

LiquidLine upline (3, 0, uptr );
LiquidLine downline (3, 1, dptr );

LiquidScreen screenplayer( upline, downline );

LiquidMenu menu(lcd);

byte rFocus[8] = {
  0b11000,
  0b11100,
  0b00110,
  0b00011,
  0b00011,
  0b00110,
  0b11100,
  0b11000
};

void menuUpdate(){
  Serial.println("MENU UPDATE");
  menu.update();  
}

void line_fn(){
  Serial.println("SWITCH LINE ");
  //menu.switch_focus( true);
}

void ifPressed(){
  Serial.println("Button Pressed");
  //npointer= npointer+1;
  Serial.println(npointer);
  Serial.print("Ptr: ");
  Serial.println( *pointermenu );

  Serial.print("Size: ");
  Serial.println( sizeof(playermenu)/sizeof(playermenu[0]) );

   menu.update(); 
}

void ifCWS(){
  
  Serial.print("CLOCKWISE: ");
  //menu.switch_focus( false);
  if(npointer > 0){
    npointer= npointer-1;
    uptr=playermenu[npointer];
    dptr=playermenu[npointer+1];
    //changetxtvar( playermenu[npointer], uptr );
    Serial.print("Txt: ");Serial.println(playermenu[npointer]);
    //changetxtvar( playermenu[npointer+1], dptr );
    Serial.print("Txt: ");Serial.println(playermenu[npointer+1]);
  }
  
  Serial.println(npointer);
  
}

void ifCCWS(){
  Serial.print("ANTICLOCKWISE: ");
  //menu.switch_focus(true);
  // Minus 2 because size of lcd is for two columns
  if(npointer < sizeof(playermenu)/sizeof(playermenu[0])-2){
    npointer= npointer+1;
    uptr=playermenu[npointer];
    dptr=playermenu[npointer+1];
  }

  Serial.println(npointer);
  
}

void setup() {
  Serial.begin(9600); 
  Serial.println("Goodnight Moon");
  
  lcd.init(); 
  lcd.backlight();

  menu.add_screen( screenplayer );
  
  upline.attach_function(1, line_fn );
  downline.attach_function(1, line_fn);
  menu.set_focusSymbol(Position::LEFT, rFocus);
  menu.set_focusPosition(Position::LEFT);
  rotary.setLimits(10,-10, true);
  rotary.set_f_press( ifPressed );
  rotary.set_f_rotary( menuUpdate, ifCWS, ifCCWS );
  menu.update(); 
}

void loop() {
  rotary.read();
}
