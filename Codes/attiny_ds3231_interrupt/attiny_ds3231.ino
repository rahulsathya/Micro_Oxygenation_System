#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <Streaming.h>  
#include <DS3232RTC.h>

DS3232RTC rtc;
const uint8_t led_pin = 0;
const uint8_t int_pin = 8;

void setup(void)
{
  pinMode(int_pin, INPUT_PULLUP);
  pinMode(led_pin,OUTPUT);
  digitalWrite(led_pin, LOW);

  //ATtiny84 interrupt
  //Falling edge of INT0 generates interrupt
  MCUCR |= (1 << ISC01);
  MCUCR &= -(1 << ISC00);
  //Enable interrupts for INT0
  GIMSK|= (1 << INT0);
  sei();

  //DS3231 time setup
  setTime(0, 0, 0, 1, 1, 2001);   //set the system time to 23h31m30s on 13Feb2009
  RTC.set(now()); 
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  
  RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
  RTC.alarm(ALARM_1);
  RTC.alarmInterrupt(ALARM_1, true);
  RTC.squareWave(SQWAVE_NONE);  
  RTC.set(compileTime());
  RTC.setAlarm(ALM1_MATCH_SECONDS, 5, 0, 0, 1);
  RTC.alarm(ALARM_1);
}

void loop(void)
{/*
  for (byte i=0; i<4; i++) {     //wake up, flash the LED
        digitalWrite(led_pin, HIGH);
        delay(1000);
        digitalWrite(led_pin, LOW);
        delay(100);
    }   */
  if ( RTC.alarm(ALARM_1) )    // check alarm flag, clear it if set
    {
     for (byte i=0; i<4; i++) 
     {     
        digitalWrite(led_pin, HIGH);
        delay(1000);
        digitalWrite(led_pin, LOW);
        delay(100);
      }
    }  
    delay(100);
  sleep_fn();
}

//Interrupt Service Routine
ISR (INT0_vect)
{
  GIMSK = 0;                     //disabling external interrupts
}

void sleep_fn(void)
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

// function to return the compile date and time as a time_t value
time_t compileTime()
{
    const time_t FUDGE(10);    //fudge factor to allow for upload time, etc. (seconds, YMMV)
    const char *compDate = __DATE__, *compTime = __TIME__, *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
    char compMon[4], *m;

    strncpy(compMon, compDate, 3);
    compMon[3] = '\0';
    m = strstr(months, compMon);

    tmElements_t tm;
    tm.Month = ((m - months) / 3 + 1);
    tm.Day = atoi(compDate + 4);
    tm.Year = atoi(compDate + 7) - 1970;
    tm.Hour = atoi(compTime);
    tm.Minute = atoi(compTime + 3);
    tm.Second = atoi(compTime + 6);

    time_t t = makeTime(tm);
    return t + FUDGE;        //add fudge factor to allow for compile time
}
