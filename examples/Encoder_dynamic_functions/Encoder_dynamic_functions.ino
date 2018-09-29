//#define ENCODER_USE_INTERRUPTS
#include "EncoderMenu.h"

EncoderMenu rotary(2,3,4, false , true);
float counter = 0;
bool mode = false;

void ifPressed(){
  Serial.println("Button Pressed");

  if ( mode ){
    rotary.set_f_CW( plus );
    rotary.set_f_CCW( minus );
    mode = false;
  }else{
    rotary.set_f_CW( multiply );
    rotary.set_f_CCW( divide );
    mode = true;
  }
}


void plus(){
  Serial.println("Plus");
  counter = counter +1;
}

void minus(){
  Serial.println("Minus");
  counter = counter -1;
}

void multiply(){
  Serial.println("Multiply");
  counter = counter*2;
}

void divide(){
  Serial.println("Divide");
  counter = counter / 2;
}

void printcounter(){
  Serial.println(counter);
}

void setup() {
  Serial.begin(9600); 

  rotary.setLimits(10,-10, true);
  rotary.set_f_CW( plus );
  rotary.set_f_CCW( minus );
  rotary.set_f_C( printcounter );
  rotary.set_f_press( ifPressed );

}

void loop() {
 
  rotary.read();
}
