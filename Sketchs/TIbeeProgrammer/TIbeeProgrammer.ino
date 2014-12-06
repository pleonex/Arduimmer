// Pin 
int RESET = 2; // Connect to RESET through TTL 3.3V converter
int CLOCK = 3; // Connect to P2.2 (RSV) through TTL 3.3V converter
int DATA = 4;  // Connect to P2.1 (PWM) through TTL 3.3V converter

/**
 * Configure the pin to use.
 */
void TIbeesetup() {
  pinMode(RESET, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);
}

/**
 * Enter into Debug Mode.
 */
void enterDebugMode() {
  digitalWrite(RESET, LOW);
  
  digitalWrite(CLOCK, HIGH);
  digitalWrite(CLOCK, LOW);
  digitalWrite(CLOCK, HIGH);
  digitalWrite(CLOCK, LOW);

  digitalWrite(RESET, HIGH);

}

/**
 * Send a bit to the processor.
 */
void sendBit(byte data) {
  // The bit is transmitted in
  // while the clock is high.
  digitalWrite(CLOCK, HIGH);
  digitalWrite(DATA, data);
  
  digitalWrite(CLOCK, LOW);
  digitalWrite(DATA, LOW);
}

/**
 * Send a bit stream to the processor.
 */
void sendBits(unsigned int data, int n) {
  for (int i = n - 1; i >= 0; i--)
    sendBit(bitRead(data, i));
}

/**
 * Receive a bit stream from the processor.
 */
unsigned int receiveBits(int n) {
  // Wait for the chip to be ready
  // indicated by setting DATA to high.
  pinMode(DATA, INPUT);
  while(digitalRead(DATA) == HIGH){
    // If chip is still not ready...
    // send 8 clock pulses.
    for (int i = 0; i < 8; i++) {
      digitalWrite(CLOCK, HIGH);
      digitalWrite(CLOCK, LOW); 
    }
  }
  
  // Now, it's ready, start to receive data...
  unsigned int data = 0;
  for (int i = n - 1; i >= 0; i--) {
    // Send a clock pulse to get next bit.
    digitalWrite(CLOCK, HIGH);
    digitalWrite(CLOCK, LOW); 
    bitWrite(data, i, digitalRead(DATA));  
  }  
    
  // Set again DATA as output (default)
  pinMode(DATA, OUTPUT);
  
  return data;
}

/**
 * Show Chip ID and Version
 */
void showChipInfo() {
  // Send "get chip command"
  sendBits(B01101000, 8);
  
  // Receive output
  unsigned int data = receiveBits(16);
  Serial.print("CHIP ID: 0x");
  Serial.println(data >> 8, HEX);
  Serial.print("CHIP VERSION: 0x");
  Serial.println(data & 0xFF, HEX);
}

/**
 * Setup configuration
 */
void setup() {   
  // Setup pins  
  TIbeesetup(); 

  // Enter Debug Mode
  enterDebugMode();
  
  // Print Chip ID
  delay(5000);
  showChipInfo();
}

/**
 * Main code
 */
void loop() {
}
