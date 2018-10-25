/*
  Design and created by Blascarr
  EncoderMe
  Name    : Blascarr
  Description: EncoderMenu.h
  version : 1.0
 
    Blascarr invests time and resources providing this open source code like some other libraries, please
    respect the job and support open-source software.
    
    Written by Adrian for Blascarr
*/

#ifndef ENCODERMENU_H
#define ENCODERMENU_H 

  //For Arduino UNO Interruption PinOUT
  //PinA = SCK -- Interrupt 2
  //PinB = DT --  Interrupt 3
  //PinSW = SW -- Switch

  #if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif
  #include <limits.h>

  // Button configuration (values for 1ms timer service calls)
  
  #define ENC_DEBOUNCEDELAY    50  // check button every x milliseconds, also debouce time
  #define ENC_DOUBLECLICKTIME  600  // second click within 600ms
  #define ENC_HOLDTIME 1200 // report held button after 1.2s

  class EncoderMenu {
  public:

    //Switch and counter Evaluation
    long _counter = 0, _lastValue = 0;
    uint8_t lastEncoded = 10;
    uint8_t sum = 0, encoder_steps = 1;
    int8_t _inc = 1;

    //Function Pointers
    typedef void (*_func)();

    _func f_CW ;
    _func f_CCW ;
    _func _f ;
    _func f_press ;

    #if defined(ENCODER_USE_INTERRUPTS)

      EncoderMenu (uint8_t pinA, uint8_t pinB, uint8_t pinSW, bool clockwise = true, bool pullup = false)
        : _pinA(pinA), _pinB(pinB), _pinSW(pinSW), _state(LOW), _lastState(LOW),
          _lastMillis(0),
          _lastDebounceTime(0) {
        
        pinMode(_pinA, INPUT);
        pinMode(_pinB, INPUT);

        uint8_t configType = (pullup == true) ? INPUT_PULLUP : INPUT;
        pinMode(_pinSW, configType );

        _inc = (clockwise) ? 1 : -1;
        encoder_steps = 4;

        digitalWrite(_pinSW, HIGH);

      }

      EncoderMenu (uint8_t pinA, uint8_t pinB, uint8_t pinSW, void (*f)(), bool clockwise = true, bool pullup = false)
        : _pinA(pinA), _pinB(pinB), _pinSW(pinSW), _state(LOW), _lastState(LOW),
          _lastMillis(0),
          _lastDebounceTime(0) {
        
        EncoderMenu(pinA, pinB, pinSW, clockwise, pullup);

        attach( f );
      }

      void attach( void (*f)() ){
        attachInterrupt( digitalPinToInterrupt(_pinA), f, CHANGE);
        attachInterrupt( digitalPinToInterrupt(_pinB), f, CHANGE);
      }
    #else
      EncoderMenu (uint8_t pinA,uint8_t pinB, uint8_t pinSW, bool clockwise = true, bool pullup = false)
        : _pinA(pinA), _pinB(pinB), _pinSW(pinSW), _state(LOW), _lastState(LOW),
          _lastMillis(0),
          _lastDebounceTime(0) {
        
        pinMode(_pinA, INPUT);
        pinMode(_pinB, INPUT);
        _inc = (clockwise) ? 1 : -1;

        uint8_t configType = (pullup == true) ? INPUT_PULLUP : INPUT;
        pinMode(_pinSW, configType );

        digitalWrite(_pinSW, HIGH);
      }
    #endif
     
    #if defined(ENCODER_USE_INTERRUPTS)
      bool read(  ){
        int MSB = digitalRead(_pinA); //MSB = most significant bit
        int LSB = digitalRead(_pinB); //LSB = least significant bit
        int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
        bool dir;

        sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value  
        lastEncoded = encoded; //store this value for next time

        if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {_counter = _counter - _inc ; dir = true; }
        if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {_counter = _counter + _inc ; dir = false; }
        this->update();
        return dir;
      }
    #else
      bool read(  ){
        byte DialPos = (digitalRead(_pinA) << 1) | digitalRead(_pinB); bool dir;
        if (DialPos == 3 && _Last_DialPos == 1) {
          _counter = _counter + _inc;
          dir = false;
        }
        if (DialPos == 3 && _Last_DialPos == 2) {
           _counter= _counter - _inc;
           dir = true;
        }
        _Last_DialPos = DialPos; 
        this->update();
        return dir;

      } // Debounces the button and returns the state if it was just changed.
    #endif
    
    long readCounter( void (*f)() = NULL, void (*g)() = NULL, void (*h)() = NULL  ){
      int value = _counter / encoder_steps;
      if(_limited){
        if (_cLimits){
          _counter = (value > _maxL) ? _minL*encoder_steps : _counter;
          _counter = (value < _minL) ? _maxL*encoder_steps : _counter;
        }else{
          _counter = (value > _maxL) ? _maxL*encoder_steps : _counter;
          _counter = (value < _minL) ? _minL*encoder_steps : _counter;
        }
      }

      if ( value != _lastValue) {
        if(_lastValue < value){
          this->f_CW();
        }else{
          this->f_CCW();
        }
        _lastValue = value;
        this->_f();
      }
      return (value);
    }

    void update( ){
      this->pressed();
      this->readCounter();
    }

    int counter( ){
      return this->_lastValue;
      
    }

    /*long readCounter( void (*f)() = NULL  ){
      readCounter( void (*f)()  )
    }*/
    
    /*long readCounter( void (*g)() = NULL, void (*h)() = NULL   ){
      readCounter( NULL, g , h  )
    }*/

    void setEncoderSteps( uint8_t steps){
      encoder_steps = steps;
    }

    void setLimits( int max_limit, int min_limit, bool circular = false){
      _limited = true;
      _maxL = max_limit;
      _minL = min_limit;
      _cLimits = circular;
    }
    
    void nolimits( ){
      _limited = false;
    }

    void set_f_CW( void (*fCW)()  ){
      f_CW = fCW;
    }

    void set_f_CCW( void (*fCCW)()  ){
      f_CCW = fCCW;
    }

    void set_f_C( void (*fC)()  ){
      _f = fC;
    }

    /*void set_g_C( void (fC)()){
      _f = fC;
    }*/

    void set_f_rotary( void (*f)() = NULL, void (*g)() = NULL, void (*h)() = NULL ){
      this->set_f_C(f);
      this->set_f_CW(g);
      this->set_f_CCW(h);
    }

    void set_f_press( void (*f)() ){
      f_press = f;
    }

    void pressed( bool triggerState = LOW ) {
        _state = digitalRead(_pinSW); // Checks if the buttons has changed state      
        if (_state != _lastState) {
            
            _lastState = _state;
            if (_lastState == triggerState){
             this->f_press();
          }
        }
    }
   
  private:
    const uint8_t _pinA, _pinB, _pinSW;
    bool _state, _lastState;
    bool _cLimits = false, _limited = false ;
    byte _Last_DialPos;
    uint32_t _lastMillis;
    uint16_t _debounceDelay = ENC_DEBOUNCEDELAY;
    uint32_t _lastDebounceTime;
    int _minL = INT_MIN, _maxL = INT_MAX;

  };

#endif