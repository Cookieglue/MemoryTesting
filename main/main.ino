
#define CLK 2 //inverted
#define RAM_IN 3
#define RAM_OUT 4 //inverted
#define MAD_IN 13

#define START_PIN 5
//bits go left to right (5 to 12)

bool memory[16][8] ={
  {0,1,0,1, 0,0,0,1}, //LDI 1
  {0,1,0,0, 1,1,1,0}, //STA Y
  {0,1,0,1, 0,0,0,0}, //LDI 0
  {1,1,1,0, 0,0,0,0}, //OUT
  {0,0,1,0, 1,1,1,0}, //ADD Y
  {0,1,0,0, 1,1,1,1}, //STA Z 
  {0,0,0,1, 1,1,1,0}, //LDA Y //cache new y into old y
  {0,1,0,0, 1,1,0,1}, //STA X
  {0,0,0,1, 1,1,1,1}, //LDA Z
  {0,1,0,0, 1,1,1,0}, //STA Y
  {0,0,0,1, 1,1,0,1}, //LDA X
  {0,1,1,1, 0,0,0,0}, //JC 0
  {0,1,1,0, 0,0,1,1}, //JMP 3
  {0,0,0,0, 0,0,0,0}, //X
  {0,0,0,0, 0,0,0,0}, //Y
  {0,0,0,0, 0,0,0,0}  //Z
};
int pointer = 0;
bool strClk = 0;
int deltaClk = 0;
int busSplit =  3;


void setup() {
// put your setup code here, to run once:
    pinMode(CLK, INPUT);
    pinMode(RAM_IN, INPUT);
    pinMode(RAM_OUT, INPUT);
    pinMode(MAD_IN, INPUT);
    Serial.print(memory[0][0]);

    for (int i = 0 ; i < 8 ; i++){
      pinMode(START_PIN + i , INPUT);
    }

    Serial.begin(9600);
    
}

void loop() {

  //read and invert pins
  bool clock = 1-digitalRead(CLK);
  bool ramOut = 1-digitalRead(RAM_OUT);
  bool ramIn = digitalRead(RAM_IN);
  bool madIn = 1-digitalRead(MAD_IN);

  if (millis()%2000 == 0){
    Serial.println(millis()/2000);
    Serial.print("Clock: ");
    Serial.println(clock);
    Serial.print("RAM OUT: ");
    Serial.println(ramOut);
    Serial.print("RAM IN: ");
    Serial.println(ramIn);
    Serial.print("MAD IN: ");
    Serial.println(madIn);
    Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  }
  //1 means rising edge, -1 means falling edge
  deltaClk = clock-strClk;
  strClk = clock;

  
  //mad in and ram out creates a special case
  if(ramOut && !madIn){
    //allow outputting to the bus
    for (int i = 0 ; i < 8 ; i++){
      pinMode(START_PIN + i , OUTPUT);
    }
    //write to the bus
    for (int i = 0 ; i < 8 ; i++){
      digitalWrite(START_PIN + i, memory[pointer][i]);
    }

  }
  else{
    //put pins back in high impedence to prevent short circuits
    for (int i = 0 ; i < 8 ; i++){
      pinMode(START_PIN + i , INPUT);
    }
  }

  //MAD IN and RAM IN should never be called at the same time, so no error  detection (we ball)
  

  //set mad in on falling edge
  if(madIn && deltaClk == -1){
    //loading from itself
    if(ramOut){
      int buffer;
      for (int i = busSplit ; i < 8 ; i++){
        //shift bits and add new bit from array
        buffer << 1;
        buffer += memory[pointer][i];
      }
    }
    //standard loading
    else{
      for (int i = 0 ; i < 8 ; i++){
        pinMode(START_PIN + i , INPUT);
      }
      int buffer = 0;
      //read from left to right, starting from the bus split. 
      for (int i = busSplit ; i < 8 ; i++){
        //shift bits and add new bit from bus
        buffer << 1;
        buffer += digitalRead(START_PIN + i);
      }
      pointer = buffer;
    }
  }

}   
