
// Transmitter
//#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define led 1
#define button 0

//0 --> off
//180 -->low
//220 -->meduim
//255 -->high

uint32_t motor_speed = 255;
bool buttonState = 0;
uint32_t receiver[1];
uint32_t input[1];

RF24 radio(2, 3); // define CE, CSN Pins for the NRF24L01
const byte addresses[][6] = {"00001", "00002"};

void setup() {
  //Serial.begin(9600);
  pinMode(led, OUTPUT);
  digitalWrite(led,LOW);
  pinMode(button, INPUT_PULLUP);

  radio.begin();
  radio.openReadingPipe(1, addresses[0]); //Reading From Pipe 00001
  radio.openWritingPipe(addresses[1]); // Writing To Pipe 00002
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);

}
void loop() {
  // Sedning //
  delay(5);                                                 // set the delay to 5 milliseconds for stability
  //radio.powerUp();
  radio.stopListening();                                    // Stop listening and start receiving
  input[0] = motor_speed;                           // Assigning led status to input[0] 
  //Serial.println(input[0]);                                 // Print input[0] to the serial for debugging
  radio.write(&input, sizeof(input));                       // Send input[0] to the Receiver
  //radio.powerDown();
  
  // Receiving //
 /* delay(5);                                                 // set the 5 milliseconds for stability
  radio.startListening();                                   // Stop Receiving and Start receiving
  if (radio.available()) {                                  // check if there is communication available
      radio.read(&receiver, sizeof(receiver));              // Receiving the input from the receiver
      //Serial.println(receiver[0]);                          // Print whatever is received to the serial for debugging
      if (receiver[0] == 0) {digitalWrite(led, HIGH);}      // check the received if it's 0 then turn on the LED
     else if (receiver[0] == 1) {digitalWrite(led, LOW);}   // check the received if it's 1 then turn off the LED
  } */
}
