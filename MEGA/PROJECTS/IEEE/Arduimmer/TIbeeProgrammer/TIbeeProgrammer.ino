


int RESET = 2;
int CLOCK = 3; 
int DATA = 4;
byte data1;
byte data2;



void TIbeesetup() {
  pinMode(RESET, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);

}

void enterDebugMode() {
  digitalWrite(RESET, LOW);
  
  digitalWrite(CLOCK, HIGH);
  digitalWrite(CLOCK, LOW);
  digitalWrite(CLOCK, HIGH);
  digitalWrite(CLOCK, LOW);

  digitalWrite(RESET, HIGH);

}




void sendBit(byte data) {
  digitalWrite(CLOCK, HIGH);
  digitalWrite(DATA, data);
  
  digitalWrite(CLOCK, LOW);
  digitalWrite(DATA, LOW);
}

void sendBits(unsigned int data, int n) {
  for (int i = 0; i < n; i++)
    sendBit(bitRead(data, i));
}


byte receiveByte() {
  
  pinMode(DATA, INPUT);
  while(digitalRead(DATA) == HIGH){
    delay(1);
  }
  
  
  // Read byte from PGD
  pinMode(DATA, INPUT);
  byte data = 0;
  
  for (byte i = 0; i < 8; i++) {
    digitalWrite(CLOCK, HIGH);
    digitalWrite(CLOCK, LOW); 
    bitWrite(data, i, digitalRead(DATA));  

  }  
  
  // Set again PGD as output
  pinMode(DATA, OUTPUT);
  
  return data;
}


// the setup routine runs once when you press reset:
void setup() {                
  TIbeesetup();     
}

// the loop routine runs over and over again forever:
void loop() {
  enterDebugMode();
  
  sendBits(0x01101000, 8);
  
  
  data1 = receiveByte();
  data2 = receiveByte();
  
  
  Serial.print(data1, OCT);  Serial.println(data2, OCT);



  delay(5000);
}
