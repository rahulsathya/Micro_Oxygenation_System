//Receiver
//#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define motor 7
#define button 0

bool buttonState = 0;
uint32_t receiver[1];
uint32_t input[1];

RF24 radio(2, 3); // define CE, CSN Pins for the NRF24L01
const byte addresses[][6] = {"00001", "00002"};

void setup() {
  //Serial.begin(9600);
  pinMode(motor, OUTPUT);
  digitalWrite(motor,LOW);
  pinMode(button, INPUT_PULLUP);
  
  radio.begin();
  radio.openWritingPipe(addresses[0]);// Writing to Pipe 00001
  radio.openReadingPipe(1, addresses[1]); // Reading from Pipe 00002
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  
}
void loop() {
  // Receiving //
  delay(5);                                                 // set the delay to 5 milliseconds for stability
  radio.startListening();                                   // Stop Receiving and Start receiving
  if (radio.available()) {                                  // check if there is communication available
      radio.read(&receiver, sizeof(receiver));              // Receiving the input from the receiver
      //Serial.println(receiver[0]);                          // Print whatever is received to the serial for debugging
      //if (receiver[0] == 0) {analogWrite(led,127);}      // check the received if it's 0 then turn on the LED
     //else if (receiver[0] == 1) {analogWrite(led,255);}   // check the received if it's 1 then turn off the LED
      analogWrite(motor,receiver[0]);
      
  }
  /*
  // Sending //
  delay(5);                                                 // set the 5 milliseconds for stability
  radio.powerUp();
  radio.stopListening();                                    // Stop listening and start receiving
  input[0] = digitalRead(button);                           // Assigning led status to input[0] 
  //Serial.println(input[0]);                                 // Print input[0] to the serial for debugging
  radio.write(&input, sizeof(input));                       // Send input[0] to the Receiver
  radio.powerDown();
  */
}
