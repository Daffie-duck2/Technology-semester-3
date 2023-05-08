#define SIDLE 0
#define STRANSMIT 1
#define SRECEIVE 2

#define txpin 3
#define rxpin 2

#define BUFLEN 100

int ssstate;

char rxBuf[BUFLEN];
char txBuf[BUFLEN];
int rxBufIndex;
int txBufIndex;
int ssbaudRate;
int ssbits;
int ssparity; //0=n 1=o 2=e
int ssstopBits;
int sstimerPrescaler;
int ssError;//1=parity error;
volatile uint8_t bitReg[16];
volatile uint8_t bitRegIndex=0;
volatile bool sssending=false;



void startTimer(){
  cli();
  TCNT1=0;
  if(sstimerPrescaler==8){
    TCCR1B |= (1<<CS11); //prescaled at 2MHz
  } else {
    TCCR1B |= (1<<CS11); //prescaled at 250Khz
    TCCR1B |= (1<<CS10);
  }
  sei();
}
void haltTimer(){
  cli();
  TCNT1=0;
  TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
  sei();
}

void RX(){ //receive ISR
  ssstate=SRECEIVE;
  detachInterrupt(digitalPinToInterrupt(rxpin));
  bitRegIndex=0;
  //bitReg[0]='\0';
  startTimer();
}

void initTimer(int freq){
    int OCRfactor=((16000000/8)*(1/freq))-1;
    if(OCRfactor>255){
      sstimerPrescaler=64;
      OCRfactor=((16000000/64)*(1/freq))-1;
    }else{
      sstimerPrescaler=8;
    }

    cli();
    TCCR1A=0;
    //TCCR1B=0;
    TCNT1=0;
    TCCR1B |= (1<<WGM12);    //Set the CTC mode
    //OCR1A=OCRfactor;
    OCR1A=207;
    TIMSK1 |= (1<<OCIE1A);   //Set the interrupt request
    TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
    sei();                 //Enable interrupt
}

int ssAvailable(){
  return rxBufIndex;
}
int ssBegin(int baudRate, int bits=8, int parity=0, int stopBits=1){
  ssbaudRate=baudRate;
  ssbits=bits;
  ssparity=parity;
  ssstopBits=stopBits;
  rxBuf[0]='\0';
  txBuf[0]='\0';
  rxBufIndex=0;
  txBufIndex=0;
  attachInterrupt(digitalPinToInterrupt(rxpin), RX, FALLING);
  ssstate=SIDLE;
  initTimer(baudRate);
  pinMode(rxpin,INPUT);
  pinMode(txpin,OUTPUT);
  PORTD |= 1<<txpin;
  pinMode(13,OUTPUT);
  return 0;
}

int ssTransmit(char* data){
  ssstate = STRANSMIT;
  detachInterrupt(digitalPinToInterrupt(rxpin));

  int i=0;
  uint8_t thisBit=0;
  uint8_t parityBitCount=0;
  while(data[i]!='\0'){ //while there's more data
    //preformat bit pattern
    bitReg[bitRegIndex++]=1;
    parityBitCount=0;
    
    for(int j=0;j<ssbits+1;j++){ //build dataBits;
      thisBit=(data[i]&(1<<j))>0?0:1; //if this data masked with desired bit has any data, return 1, else 0
      bitReg[bitRegIndex++]=thisBit;
      if(thisBit)parityBitCount++;
    }
    if(ssparity>0){
      if(ssparity==1){
        if(parityBitCount%2==1){
          bitReg[bitRegIndex++]=1;
        } else {
          bitReg[bitRegIndex++]=0;
        }
      } else {
        if(parityBitCount%2==0){
          bitReg[bitRegIndex++]=1;
        } else {
          bitReg[bitRegIndex++]=0;
        }
      }
    }

    if(ssstopBits==1){
      bitReg[bitRegIndex++]=0;
    } else{
      bitReg[bitRegIndex++]=0;
      bitReg[bitRegIndex++]=0;
    }

    sssending=true;
    bitRegIndex=0;
    startTimer();
    while(sssending){}//hold while sending.
    haltTimer();
    //memset((void*)bitReg, 0, 15);               //COMMENTING THIS LINE REVEALED THAT THE INTERRUPT DOES NOT MODIFY THE BITREG. INT DOESNT RUN?
    //delayMicroseconds(650); //gross but works
    bitRegIndex=0;
    i++;
  }

  attachInterrupt(digitalPinToInterrupt(rxpin), RX, FALLING);
  ssstate = SIDLE;
  return 0;
}

char ssReceive(){
  char c = rxBuf[--rxBufIndex];
  rxBuf[rxBufIndex]='\0';
  return c;
}


void finalizeRX(){
  haltTimer();
  uint8_t bitsInFrame = bitRegIndex;
  bitRegIndex=0;
  
  digitalWrite(13,!digitalRead(13));

  uint8_t dataBuf[8];
  bool parity;
  uint8_t parityBitCount;
  for(uint8_t i=0; i<bitsInFrame-1; i++){
      if(i<ssbits){ //filter data frame
        dataBuf[i]=bitReg[i]>0?1:0;
        if(bitReg[i]>0)parityBitCount++;
      }
      if(parity>0){
        if(i==ssbits){//filter parity bit
          parity=bitReg[i];
        }
      }
  }

  if(parity>0){ //check parity
    if(parity==1){
      if(parityBitCount%2==0){
        ssError=1;
        return;
      }
    } else {
      if(parityBitCount%2==1){
        ssError=1;
        return;
      }
    } //if okay, continue 
  }  

  char c=0;
  for(int i=0; i<ssbits; i++){ //resolve character (rudimentary but works)
    if((dataBuf[i]<<i)>0){c |= 0<<i;}else{c |= 1<<i;}
  }

    rxBuf[rxBufIndex++]=c;
    rxBuf[rxBufIndex]='\0';
    bitReg[0]='\0';
    bitRegIndex=0;

  attachInterrupt(digitalPinToInterrupt(rxpin), RX, FALLING);
}

void finalizeTX(){
  sssending=false;
  haltTimer();
}

ISR(TIMER1_COMPA_vect){    //This is the interrupt request
  if(ssstate==SRECEIVE){
    //uint8_t rx =  //return 1 if PIND bit at rxpin is set. THIS ALREADY INVERTS THE SIGNAL.
    bitReg[bitRegIndex]=(PIND & (1<<rxpin))>0?0:1; //<- hier gaat t mis
    bitRegIndex = bitRegIndex+1;
    if(bitRegIndex>=(1+ssbits+(ssparity>0?1:0)+ssstopBits)){ //check if bitregindex is as long as requested (aka transmission frame is over)
      finalizeRX();
    }
    PORTD ^= (1<<(txpin+1));
    PORTD ^= (1<<(txpin+1));
    PORTD ^= (1<<(txpin+1));
  } else { //if STRANSMIT
    uint8_t mod= PORTD;
    mod &= ~(1<<txpin);
    mod |= (bitReg[bitRegIndex++]==1?0:1<<txpin);
    PORTD=mod;
    
    if(bitRegIndex==1+ssbits+(ssparity>0?1:0)+ssstopBits){
      PORTD |= 1<<txpin; //return to neutral high-level;
      finalizeTX();
    }
  }
}

///--------------------- end of SoftSerial section ---------------------

char buf[80];
char outBuf[80];
int bufIndex;
void setup() {
  pinMode(txpin+1,OUTPUT); //debug clock pin
  ssBegin(9600,8,0);
  delay(1); 
  ssTransmit("UUU");
  ssTransmit("DIT_is_een_TEST");
}

void loop() {
  if(ssAvailable()>0){
    char ch=ssReceive();
    buf[bufIndex++]=ch;
    sprintf(outBuf, "%c", ch);
    ssTransmit(outBuf);

  }
  if(buf[bufIndex==0?bufIndex:bufIndex-1]=='\n' || buf[bufIndex==0?bufIndex:bufIndex-1]=='\r' ){
    ssTransmit("\nTransmission Received!");
    bufIndex=0;
    buf[0]='\0';
  }
  
}