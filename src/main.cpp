#include <Arduino.h>

#define UART_TX_PIN 16   // UART0 TX. We only send, so RX is left unused.
#define BAUD_RATE   9600 // FN-M16P default UART speed

#define BUSY_PIN    11
#define LED_PIN     13
#define BUTTON_PIN  15


uint8_t counter = 1;

int playCount = 1;



bool lastState = true;   // pulled-up = true (not pressed)
bool ledState  = false;

// Play track 1 command: 7E FF 06 03 00 00 01 [checksum_H checksum_L] EF
static uint8_t playAudio[8] = {
    0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x01, 0xEF
};

//int numberArray[24] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int littleArray[6] = {0,0,0,0,0,0};

void playMp3String(uint8_t trackNumber ) {
  playAudio[6] = trackNumber;
  Serial1.write(playAudio, sizeof(playAudio));
  delay(300);
}

int getTrackNum(){
  int trackNo = 0;
  do{
    trackNo = random(0, 6);
  }while( littleArray[trackNo] != 0);

  littleArray[trackNo] = 1;
  return trackNo + 1 ; // MP3 Players tracks start from 001
}
  

void setup() {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    pinMode(BUTTON_PIN, INPUT_PULLUP);

    pinMode(BUSY_PIN, INPUT);

    Serial1.setTX(UART_TX_PIN);
    Serial1.begin(BAUD_RATE);

    delay(2000); // let the FN-M16P finish booting / mounting the SD card
}

void loop() {
    bool currentState = digitalRead(BUTTON_PIN);

    // Falling edge only: fires once per press, not on release too
    if (lastState && !currentState) {

    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);

    playCount = 1;
    while (playCount <4){
  
      bool dupa = digitalRead(BUSY_PIN);
      if(dupa == HIGH) {      
      playMp3String(getTrackNum());
      playCount++ ;
      counter++ ;
      
      }
     }
    }
    
    if (counter > 6) {
      counter = 1;
      for (int i= 0; i <6; i++){
       littleArray[i] = 0;
      }
    }
    lastState = currentState;
    delay(20); // simple debounce
}

