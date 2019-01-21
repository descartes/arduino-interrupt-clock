//interrupt code based on this source: https://www.instructables.com/id/Arduino-Timer-Interrupts/

int clock_hours, clock_mins, clock_seconds = 0;

void setup() {
  Serial.begin(9600);
  
  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set enire TCCR1A register to 0
  TCCR1B = 0;// and for TCCR1B
  TCNT1  = 0;// initialize counter value to 0
  
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
}

ISR(TIMER1_COMPA_vect){//timer1 interrupt 1Hz
  //generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)
  clock_seconds += 1;
  if(clock_seconds >= 60){
    clock_mins += 1;
    clock_seconds = 0;  
  }
  if(clock_mins >= 60){
    clock_hours += 1;
    clock_mins = 0;
  }
  if(clock_hours >= 24){
    clock_hours = 0;
  }

  printTime();
}

void printTime(){ //keeping serial prints out of the interrupt service routine
  Serial.print(clock_hours);
  Serial.print(":");
  Serial.print(clock_mins);
  Serial.print(":");
  Serial.println(clock_seconds);
}

void loop(){
  
}
