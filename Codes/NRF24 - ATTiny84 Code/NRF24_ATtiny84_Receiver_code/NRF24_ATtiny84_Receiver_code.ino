#include <avr/sleep.h>
#include <nRF24L01.h>
#include <RF24.h>

#define motor 7

//user inputs
uint16_t calibration = 2700;
uint16_t barrel = 225;
uint8_t percentage = 100;
//Run time calculation
float pump_rate = 20.834; // 1.25L
float flow_rate = (calibration/ barrel)*percentage;
float run_time_day = (flow_rate/pump_rate)/30;
//uint32_t period = run_time_day* 60000;

bool buttonState = 0;
uint32_t receiver[1];
uint32_t input[1];

const int LED_PIN = 10;

RF24 radio(2, 3); // define CE, CSN Pins for the NRF24L01
const byte addresses[][6] = {"00001", "00002"};

void setup(void)
{
    pinMode(8, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);      
    digitalWrite(LED_PIN, LOW);    //drive it low so it doesn't source current

    pinMode(motor, OUTPUT);
    digitalWrite(motor,LOW);
    
    radio.begin();
    radio.openWritingPipe(addresses[0]);// Writing to Pipe 00001
    radio.openReadingPipe(1, addresses[1]); // Reading from Pipe 00002
    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_250KBPS);
}

void loop(void)
{
    for (byte i=0; i<10; i++) {     //wake up, flash the LED
         // Receiving //
      delay(5);                                                 // set the delay to 5 milliseconds for stability
      radio.startListening();                                   // Stop Receiving and Start receiving
      radio.powerUp();
      if (radio.available()) {                                  // check if there is communication available
          radio.read(&receiver, sizeof(receiver));              // Receiving the input from the receiver
          //analogWrite(motor,receiver[0]);
          analogWrite(motor,180);
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
    delay(1000);           //opportunity to measure active supply current 
    digitalWrite(LED_PIN, HIGH);   //blink LED once before sleeping
    delay(100);
    digitalWrite(LED_PIN, LOW);
    radio.powerDown();
    goToSleep();
}

void goToSleep(void)
{
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    MCUCR &= ~(_BV(ISC01) | _BV(ISC00));      //INT0 on low level
    GIMSK |= _BV(INT0);                       //enable INT0
    byte adcsra = ADCSRA;                     //save ADCSRA
    ADCSRA &= ~_BV(ADEN);                     //disable ADC
    cli();                                    //stop interrupts to ensure the BOD timed sequence executes as required
    byte mcucr1 = MCUCR | _BV(BODS) | _BV(BODSE);  //turn off the brown-out detector
    byte mcucr2 = mcucr1 & ~_BV(BODSE);
    MCUCR = mcucr1;
    MCUCR = mcucr2;
    sei();                                    //ensure interrupts enabled so we can wake up again
    sleep_cpu();                              //go to sleep
    sleep_disable();                          //wake up here
    ADCSRA = adcsra;                          //restore ADCSRA
}

//external interrupt 0 wakes the MCU
ISR(INT0_vect)
{
    GIMSK = 0;                     //disable external interrupts (only need one to wake up)
}

