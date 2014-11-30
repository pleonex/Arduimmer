int RESET = 2;
int CLOCK = 3; 
int DATA = 4;

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
  for (int i = 7; i >= 0; i--)
    sendBit(bitRead(data, i));
}


byte receiveByte() {
  
  pinMode(DATA, INPUT);
  while(digitalRead(DATA) == HIGH){
    Serial.println("CHIP not ready...");
    for (byte i = 0; i < 8; i++) {
      digitalWrite(CLOCK, HIGH);
      digitalWrite(CLOCK, LOW); 
    }
    delay(1000);
  }
  
  
  // Read byte from PGD
  byte data = 0;
  
  for (byte i = 7; i >= 0; i--) {
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
  delay(10000);
  Serial.print("hola");
  
  enterDebugMode();
  
  sendBits(B01101000, 8);
  
  
 byte  data1 = receiveByte();
  byte data2 = receiveByte();
  
  
  Serial.print(data1, HEX);  Serial.println(data2, HEX);



  delay(5000);
  while (true) { }
}
