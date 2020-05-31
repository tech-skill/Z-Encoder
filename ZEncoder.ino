//
// Code for converting chinese linear scale serial protocol to A/B encoder signal for DRO
//
// PD2 is CLK signal
// PD5 is the DATA signal 
// there are 21 bits in 2.5ms then a pause of 3.5ms
// bits are LSB first
//
// Partly from Yuriy's Toys
// https://www.yuriystoys.com/2012/01/reading-gtizzly-igaging-scales-with.html

#define sign(a) ( ( (a) < 0 )  ?  -1   : ( (a) > 0 ) )


// Pins
int const clockPin = 2;
int const yDataPin = 3;
int const A_pin = 5;
int const B_pin = 4;

// Globals
long yCoord = 0;
long prevYCoord = 0;
long count = 0;

bool A[4] = {false, true, true, false};
bool B[4] = {false, false, true, true};



void setup() {

  //A,B and clock pin should be set as output 
  pinMode(clockPin, OUTPUT);
  pinMode(A_pin, OUTPUT);
  pinMode(B_pin, OUTPUT);

  //data pins should be set as inputs
  pinMode(yDataPin, INPUT); 

  Serial.begin(115200);
  
  // measure once...
  measurement();
  delay(150);
  prevYCoord = yCoord;

}


void loop() {
  
  measurement();
  count = prevYCoord-yCoord;
  delay(50);
  if (count){
//    Serial.println(count);
    encoderAB( count);
  }
  prevYCoord = yCoord;

}



void measurement(){
  int bitOffset;
  yCoord= 0;
  //read the first 20 bits
  for(bitOffset = 0; bitOffset<20; bitOffset++)
  {
    //tick
    digitalWrite(clockPin, HIGH);

    //give the scales a few microseconds to think about it
    __asm__("nop\n\t"); 
    __asm__("nop\n\t");
    __asm__("nop\n\t"); 

    //tock
    digitalWrite(clockPin, LOW);

    //read the pin state and shif it into the appropriate variables
    yCoord |= (digitalRead(yDataPin)<<bitOffset);
  }
  
  //tick
  digitalWrite(clockPin, HIGH);

  //give the scales a few microseconds to think about it
  __asm__("nop\n\t");
  __asm__("nop\n\t");
  __asm__("nop\n\t"); 

  //tock
  digitalWrite(clockPin, LOW);
  //read the last bit (signified the sign)
  //if it's high, fill 11 leftmost bits with "1"s

  if(digitalRead(yDataPin)==HIGH)
    yCoord |= (0x7ff << 21);
  
}


void encoderAB( long count){
  static uint8_t index = 0;
  int8_t dir = sign(count);

  // send AB signals
  for (long counter = 0; counter < abs(count); counter++){
    index+=dir;
    index &= 0x3;
    digitalWrite(A_pin,A[index]?HIGH:LOW);
    digitalWrite(B_pin,B[index]?HIGH:LOW);
//    Serial.print('.');
  }
//  Serial.print('\n');
}
