// @HeadBoffin, CC0 1.0 Universal (CC0 1.0) Public Domain Dedication, may be OK for toaster, not so much for nuclear powerstations ...


// Setup a variable to hold our time, marking it as volatile so the compiler knows that it is
// used in several places and should ensure the most up to date value is being used
volatile int seconds = 0;

void setup() {
  Serial.begin(57600);

  // Set Timer1 to call a routine once per second

  // The CPU is surrounded by many different peripherals that allows it to interact with the outside world
  // Many of these peripherals access the external pins - Analog to Digital, SPI, I2C, UART and PWM for instance
  // Some peripherals provide facilities for the CPU, either exclusively or alongside the output pins
  // For instance, the millis() function is run by Timer1, which also provides support for the PWM on pins 5 & 6
  // The peripherals are controlled by many many control registers - CPU's are hardware so don't have variables
  // that you can assign values to. The control registers are 8 bit and combine various control functions for the
  // peripheral in one register. So bits 0-2 may set the UART speed whereas bits 4-5 set the flow control.
  // These registers are given abreviated names and consequently the code looks rather cryptic. It is very close
  // to the code you would produce if you were programming in assembly.
  // Long story short, you have to refer to the data sheet, which for an Arudino Uno/Nano is 800+ pages.

  // https://www.microchip.com/wwwproducts/en/ATmega328

  // The Timer is also a Counter depending on how it's configured - it counts pulses and raises it's hand when it reaches a certain value
  // If the pulses are coming from the CPU clock, you can calculate the count it takes to take a period of time
  // Mostly, the timer is actually a counter that enables you to time things. But we like to call it a Timer, mostly


  // We will be using the 16 bit Timer/Counter - starting on page 120
  // We are going to set this counter to count to a particular number and then call our program.

  // TCCR1A & TCCR1B – Timer/Counter1 Control Register A & B
  // Clear all settings so we know where we are starting from
  // See page 140 - 143
  TCCR1A = 0; 
  TCCR1B = 0;


  // 16.9.2 Clear Timer on Compare Match (CTC) Mode - page 131 at the bottom
  // There are two compares available, A & B. We will use the A compare value

  // The CPU clock runs at 16MHz but we can't directly use that as we can't squeeze 16,000,000 in to a 16 bit register (max 65535)
  // But if we could divide the clock to get it under 65535, we're sorted
  // The counter has an filter on it's input, a shift register, that effectively divides the pulses by a power of 2
  // Electrically, it is wired to take counts from divisors of 8, 64, 256 and 1024. This filter / shift-register is called the prescaler
  // If we divide 16,000,0000 by 1024 we get 15625
  // As counts start from 0, we subtract 1 to get 15624

  // A general formula for finding the count is Count = (CPU_Clock / Prescale) / Target_Frequency - 1
  // So if we wanted a fast flash, 5Hz, it would be (16,000,000 / 1024) / 5 - 1 = 3124
  // Adjust the prescaler value to avoid any decimal places if at all possible
  // If you can't, your interrupt routine will have to account for the rounding errors

  // Set the Output Compare Register with our calculated value - this is not a control register so does not have it's own page
  OCR1A = 15624;


  // 16.11.1 TCCR1A – Timer/Counter1 Control Register A & B - starting page 140

  // The compiler provides lots of helper values that refer to the bit position in a control register.
  // This allows us to use a shorthand bit shift technique to turn bits on & off without touching any others
  // ZZZZ |= (1 << BitPos) says, take the ZZZZ register, shift a bit left the number of times in BitPos, perform an OR and put the result back in to ZZZZ
  // As an example for shifting, if BitPos is 4, then we will end up with 00010000, remembering the first bit position on the right is 0
  // An OR turns on all bits that have a 1 in them. It won't turn off anything that's already 1.


  // Turn on Clear Timer on Compare match (CTC) mode which uses the value in the OCR1A register, when it reaches this value it will trigger our interrupt routine
  TCCR1B |= (1 << WGM12);

  // Configure the Clock Select Bits to use the prescaler with a value of 1024
  TCCR1B |= (1 << CS12) | (1 << CS10);  


  // TCNT1 - The 16 bit register that holds the actual count - this is not a control register so does not have it's own page
  TCNT1 = 0;  // Set counter to 0 so we are start from the beginning when we turn on the counter

  // Enable Timer1 interrupts in the Timer Interrupt Mask Register
  // 16.11.8 TIMSK1 – Timer/Counter1 Interrupt Mask Register - see page 144 at the bottom
  TIMSK1 |= (1 << OCIE1A);

  // We now have our Timer/Counter1 setup to count every 1024 CPU pulses and when it gets to 15624, run the interrupt routine which we define next

}

// ISR is Interrupt Service Routine - the TIMER1_COMPA_vect indicates that it is for the Timer1 Compare A
// The compiler puts the address of this function in to the interrupt vectors (lookup) table at 0x00B
// The CPU reads this address and then jumps to the code to run it.

ISR(TIMER1_COMPA_vect) {
  seconds += 1;
  if(seconds >= 60) {
    seconds = 0;  
  }
}


void loop() {
  delay(1000);
  Serial.println(seconds);
}
