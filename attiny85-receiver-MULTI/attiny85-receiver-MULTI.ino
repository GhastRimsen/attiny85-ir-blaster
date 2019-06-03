// RF Receiver and IR blaster


#include <VirtualWire.h>
//#define rxLed 2  //use if debugging, wastes battery otherwise
#define IRled 4
//void blinkLed(int pin, int amount, int wait);  //use for blinking rxLED if debugging

//Sony IR protocol: 1 = IR() for 1.2ms, 0 = IR for .6ms, .5ms gap between bits. Begin with 2.4ms IR(), 26ms delay between commands. 
void ir0Sony(); 
void ir1Sony();
void irBeginSony();
void irEndSony();
//NEC IR protocol: 1 = delay for 1.6ms, 0 = delay for .6ms, .5ms IR() between bits. Begin with 9ms IR() followed by 4.5ms delay, 40ms delay between commands. 
void ir0NEC();
void ir1NEC();
void irBeginNEC();
void irEndNEC();

void setup(){

    // Initialise the IO and ISR
    vw_setup(1000);	 // Bits per sec
    vw_set_rx_pin(3);
    //pinMode(rxLed, OUTPUT);  //uncomment for debugging
    //digitalWrite(rxLed, LOW);
    pinMode(IRled, OUTPUT);
    digitalWrite(IRled, LOW);
    vw_rx_start();       // Start the receiver PLL running
}

void loop()
{
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;
    char array[36] = "";
  
    if (vw_get_message(buf, &buflen))
    {
        vw_rx_stop();
        cli();
        //blinkLed(rxLed, 1, 1000); // Flash a light to show received good message
    	          
    	  // Message with a good checksum received, dump it
        int r;
        int i;
        int mode = char(buf[0]) - '0';  //check first char of command. 0 = NEC, 1 = Sony protocol
        
        for (i = 1; i < 36; i++)  //write array with actual command, skipping first char
        {
            array[i] = char(buf[i]);
        }
        if (mode == 0){ // NEC
            for (r = 0; r < 3; r++){  //repeat message 3 times, maybe not necessary every time but this will be implemented later for volume up and such
                irBeginNEC();
                for (i = 0; i<=36; i++){
                    int rx = array[i] - '0';
                    if (rx == 0){
                        ir0NEC();
                    } else if (rx == 1){
                        ir1NEC();
                    }
                }
                irEndNEC();
            }
        } else if (mode == 1) //Sony              
          for (r = 0; r < 3; r++){
              irBeginSony();
              for (i = 0; i<=11; i++){
                  int rx = array[i] - '0';
                  if (rx == 0){
                      ir0Sony();
                  } else if (rx == 1){
                      ir1Sony();
                  }
              }
              irEndSony();
          }
        sei();
        delay(80000);
        vw_rx_start();
    }
}

void blinkLed(int pin, int amount, int wait){
    int i;
    for (i = 1; i <= amount; i++){
        digitalWrite(pin, HIGH);
        delay(wait);
        digitalWrite(pin, LOW);
        delay(wait);
    }
}

//Clock rates are not exact. These seem to work best with the chips I have. Should probably get external clock.
  
void ir0Sony(){
    IR(510);
    delayMicroseconds(555);
}

void ir0NEC(){
    IR(450);
    delayMicroseconds(600);
}

void ir1Sony(){
    IR(975);
    delayMicroseconds(555);
}

void ir1NEC(){
    IR(440);
    delayMicroseconds(1725);
}

void irBeginSony(){
    IR(2100);
    delayMicroseconds(555);
}

void irBeginNEC(){
    IR(7700);
    delayMicroseconds(4500);
}

void irEndSony(){
    delayMicroseconds(555);
    IR(510);    
    delayMicroseconds(4500);  //delay microseconds doesn't work well in large amounts. virtualwire takes "delay()" timer so I had to make due. 
    delayMicroseconds(4500);
    delayMicroseconds(4500);
    delayMicroseconds(4500);
    delayMicroseconds(4500);
    delayMicroseconds(2000);
    delayMicroseconds(1400);
}

void irEndNEC(){
    delayMicroseconds(4500);
    delayMicroseconds(4500);
    delayMicroseconds(4500);
    delayMicroseconds(4500);
    delayMicroseconds(4500);
    delayMicroseconds(4500);
    delayMicroseconds(4500);
    delayMicroseconds(4500);
    delayMicroseconds(4500);
    IR(7700);
    delayMicroseconds(2400);
    IR(440);
}

void IR(long microsecs) {  //pulse IR led at .026ms rate. This makes it about 38khz. 
    while (microsecs > 0) {
        digitalWrite(IRled, HIGH);
        delayMicroseconds(3);
        digitalWrite(IRled, LOW);
        delayMicroseconds(3);
        microsecs -= 26;
    }
}
