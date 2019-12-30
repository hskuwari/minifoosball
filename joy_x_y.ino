/*
  Joystick example for Educational BoosterPack MK II
 http://boosterpackdepot.info/wiki/index.php?title=Educational_BoosterPack_MK_II

 Move the joystick around in x & y axes (pin 2 & 26) to adjust/mix the Green
 Press straight down on the joystick center button to turn on the Red LED.


 The circuit:
 * Joystick X attached to pin 2
 * Joystick Y attached to pin 26
 * Joystick Sel attached to pin 5
 * Blue LED (analog) attached to pin 37
 * Green LED (analog) attached to pin 38
 * Red LED (digital) attached to pin 39



 Dec 03 2013 for Educational BoosterPack MK II
 by Dung Dang

 This example code is in the public domain.

 */

// constants won't change. They're used here to
// set pin numbers:
const int joystickSel = 5;     // the number of the joystick select pin
const int joystickX = 2;       // the number of the joystick X-axis analog
const int joystickY =  26;     // the number of the joystick Y-axis analog

const int buttonOne = 33;     // the number of the pushbutton pin
const int buttonTwo = 32;     // the number of the pushbutton pin
// variables will change:
int joystickSelState = 0;      // variable for reading the joystick sel status
int joystickXState, joystickYState ;
int prevXState = 0;
int buttonOneState, buttonTwoState;

#define PLAYER 1

void setup() {

  Serial.begin(9600);

  // initialize the pushbutton pin as an input:
  pinMode(buttonOne, INPUT_PULLUP);     
  pinMode(buttonTwo, INPUT_PULLUP);   

}

void loop(){
  // read the analog value of joystick x axis
  joystickXState = analogRead(joystickX);
  // scale the analog input range [0,4096] into the analog write range [0,255]
  joystickXState = map(joystickXState, 0, 4096, 7, 23);

  // read the state of the pushbutton value:
  buttonOneState = digitalRead(buttonOne);

  // read the state of the pushbutton value:
  buttonTwoState = digitalRead(buttonTwo);

//  Serial.println(joystickXState);
  
  // joystick = 0
  // button one = 1
  // button two = 2
  
  // if joystickX == 16: 
  // dont do anything
  // joystickX != 16: 
  // send signal && joystickX != prevX:
  // header + player + ~player + 00 + 01 + joystickX (5 bits) + ~joystickX (5 bits) 

  if (joystickXState != prevXState){
    sendSignals(0, joystickXState);
//    Serial.println(joystickXState);
    prevXState = joystickXState;
    Serial.println("0");
  } 
  
  if (buttonOneState == LOW) {
    sendSignals(1, 0);
    Serial.println("1");
  } 
  
  if (buttonTwoState == LOW) {
    sendSignals(2, 0);
    Serial.println("2");
  }


  
  
//  sendHeader();
////  
//  sendZero();
//  sendOne();
//  sendZero();
//  sendOne();
//  
//  analogWrite(12,HIGH);
//  delay(1000);
//  analogWrite(12, LOW);
  delay(10);

}

  // if joystickX:
  // header + player + ~player + 00 + 11 + joystickX (5 bits) + ~joystickX (5 bits) 

 // else if buttonOne: 
  // send signal: 
  // header + player + ~player + 01 + 10 + 00000 + 11111

  // else if buttonTwo: 
  // send signal: 
  // header + player + ~player + 10 + 01 + 00000 + 11111
void sendSignals(int i, int state) {
  int x = 0;
  //send the header
  sendHeader();
  // there are two players: 
  // send player followed by not(player) : 
  if (PLAYER == 0) {
    sendZero();
    sendOne();
  }

  else if (PLAYER == 1) {
    sendOne();
    sendZero();
  }

  // get either of the three cases: 
  // send the case, followed by not(case)
  // case 0: joystick is moved => 00, 11
  // case 1: button1 is pressed => 01, 10
  // case 2: button2 is pressed => 10, 01
  if (i==0){
    sendZero();
    sendZero();
    sendOne();
    sendOne();

    sendConvertBin(state);
    sendConvertBin((~state)&0x1F);
  }

  else if (i==1){
    sendZero();
    sendOne();
    sendOne();
    sendZero();
    
    // send 5 zeros, followed by 5 ones.
    for (x; x < 5; x++){
      sendZero();
      Serial.print(0);
    }

    x = 0;
    for (x; x < 5; x++){
      sendOne();
      Serial.print(1);
    }
  }

  else if (i==2){
    sendOne();
    sendZero();
    sendZero();
    sendOne();

    for (x; x < 5; x++){
      sendZero();
      Serial.print(0);
    }

    x = 0;
    for (x; x < 5; x++){
      sendOne();
      Serial.print(1);
    }
  }

  for (i; i < 10; i++) {
    analogWrite(12, HIGH);
    delayMicroseconds(10);
    analogWrite(12, LOW);
    delayMicroseconds(10);
  }
  Serial.println("");
  
}


void sendConvertBin(int state) {
  int b = state;
  int i = 0;
  int bin[] = {0,0,0,0,0};
  for (i; i < 5; i++) {
    if (b&1) {
      bin[4-i] = 1;
      Serial.print(1);
    } 
    
    else {
      bin[4-i] = 0;
      Serial.print(0);
    }

    b = b >> 1;
  }
  i = 0;
  for (i; i < 5; i++) {
    if (bin[i]) {
      sendOne();
    } else {
      sendZero();
    }
  }
}
void sendHeader() {
  int i = 0;
  // 13.1625 Microseconds (size of one), full time = 9milliseconds 
  for (i; i < 270; i++) {
    analogWrite(12, HIGH);
    delayMicroseconds(10);
    analogWrite(12, LOW);
    delayMicroseconds(10);
  }
  delayMicroseconds(4500);
}

//560 -> <200
//1200 => 1500

void sendOne() {
// period = 552 us, our period is 560us
  int i =0;
  for (i; i < 18; i++) {
    analogWrite(12, HIGH);
    delayMicroseconds(10);
    analogWrite(12, LOW);
    delayMicroseconds(10);
  }
  delayMicroseconds(1675);

}


void sendZero() {
// period = 552 us, our period is 560us
  int i =0;
  for (i; i < 18; i++) {
    analogWrite(12, HIGH);
    delayMicroseconds(10);
    analogWrite(12, LOW);
    delayMicroseconds(10);
  }
  delayMicroseconds(545);

}
