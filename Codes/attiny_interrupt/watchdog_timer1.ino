#include <avr/sleep.h>

const int LED_PIN = 0;
const unsigned long KEEP_RUNNING = 10000;    //milliseconds

void setup(void)
{
    //to minimize power consumption while sleeping, output pins must not source
    //or sink any current. input pins must have a defined level; a good way to
    //ensure this is to enable the internal pullup resistors.

    for (byte i=0; i<11; i++) {    //make all pins inputs with pullups enabled
        pinMode(i, INPUT_PULLUP);
    }
    pinMode(LED_PIN, OUTPUT);      //make the led pin an output
    digitalWrite(LED_PIN, LOW);    //drive it low so it doesn't source current
}

void loop(void)
{
    for (byte i=0; i<5; i++) {     //wake up, flash the LED
        digitalWrite(LED_PIN, HIGH);
        delay(100);
        digitalWrite(LED_PIN, LOW);
        delay(100);
    }

    digitalWrite(LED_PIN, HIGH);   //blink LED once before sleeping
    delay(100);
    digitalWrite(LED_PIN, LOW);
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
