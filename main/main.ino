
#define CLK 5
#define RAM_IN 2
#define RAM_OUT 3
#define MAD_IN 4

#define START_PIN 9
//bits go left to right

bool memory[16][8];
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
    
}

void loop() {

  //1 means rising edge, -1 means falling edge
  deltaClk = CLK-strClk;
  strClk = CLK;

  //mad in and ram out creates a special case
  if(RAM_OUT && !MAD_IN){
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
  if(MAD_IN && deltaClk == -1){
    //loading from itself
    if(RAM_OUT){
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
