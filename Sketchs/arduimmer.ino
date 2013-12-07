const int CMD_LENGTH  4
char command[CMD_LENGTH + 1];

void setup() { 
  Serial.begin(9600);
  while (!Serial) ;   // Leonardo compability
}

void loop() {
  serialCommands();
}

void serialCommands() {
  if (Serial.available() < CMD_LENGTH)
    return;
  
   Serial.readBytes(command, CMD_LENGTH);   
   if (strcmp(command, "ping") == 0)
     ping();
}

void ping() {
  Serial.println("pong");
}
