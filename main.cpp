/****
Applicant Name: Bradley Beroz Date: 04/25/2025
Interview: BitBang 1
Digitize inc
****/

#define CS 2
#define SK 3
#define DI 4
#define DO 5


void cmdout(int cmd){
// Implement sending the functions operation to the EEPROM here

// Enable chip select
digitalWrite(CS, HIGH);

// Send command bit by bit, MSB first
for (int i = 7; i >= 0; i--)
{
// Put bit on DI line
digitalWrite(DI, (cmd >> i) & 1);

// Clock high
digitalWrite(SK, HIGH);

// Small delay
delayMicroseconds(1);

// Clock low
digitalWrite(SK, LOW);

// Small delay
delayMicroseconds(1);
}
}

int dataInput(byte size){
  int data = 0;
  // Don't change DI's mode - it should remain OUTPUT
  
  for (int i = size - 1; i >= 0; i--) {
    digitalWrite(SK, HIGH);
    delayMicroseconds(1);
    // Read bit from DO pin (not DI)
    data |= (digitalRead(DO) << i);
    digitalWrite(SK, LOW);
    delayMicroseconds(1);
  }
  return data;
}

void dataOutput(int data, byte cnt){
// Implement write data to EEPROM

  // Send data bit by bit, MSB first
  for (int i = cnt - 1; i >= 0; i--)
  {
    // Put bit on DI line
    digitalWrite(DI, (data >> i) & 1);

    digitalWrite(SK, HIGH);

    delayMicroseconds(1);

    digitalWrite(SK, LOW);

    delayMicroseconds(1);
  }
}

void EEPROM_wral(byte data){
// Implement write all function that takes in a character to popualte the whole EEPROM 
  // WRAL instruction
  cmdout(0x10);

  // Write 8 bits of data to all addresses
  dataOutput(data, 8);

  // Disable chip select
  digitalWrite(CS, LOW);

  // Wait for write to all to complete
  delay(50);
}

void EEPROM_ewds(){
  // Implement write and erase disble
  cmdout(0x00); // EWDS instruction

  // Disable chip select
  digitalWrite(CS, LOW);
}

void EEPROM_ewen(){
// Implement enable write and erase
cmdout(0x30); // EWEN instruction

// Enable chip select
digitalWrite(CS, HIGH);
delay(10);

}

int EEPROM_read(byte addr){

  digitalWrite(CS, HIGH);

  // Send READ command (10 bits)
  uint16_t cmd = (0b1 << 9) | (0b10 << 7) | (addr & 0x7F);
  for(int i=9; i>=0; i--) {
    digitalWrite(DI, (cmd >> i) & 1); // Send command bits
    digitalWrite(SK, HIGH);
    digitalWrite(SK, LOW);
  }


  // Read 8-bit data MSB-first
  int data = 0;
  for(int i=7; i>=0; i--){
    digitalWrite(SK, HIGH);
    data |= (digitalRead(DO) << i); // Capture bit from DO
    digitalWrite(SK, LOW);
  }

  return data;
}

void EEPROM_write(byte addr, byte data)
{
  digitalWrite(CS, HIGH);
  delay(10);

  // Write command: 1 01 XXXXXXX (10 bits)
  uint16_t cmd = (0b1 << 9) | (0b01 << 7) | (addr & 0x7F); // mask address

  for(int i=9; i>=0; i--) {
    digitalWrite(DI, (cmd >> i) & 1);
    digitalWrite(SK, HIGH);
    digitalWrite(SK, LOW);
  }

  // Send 8 data bits (MSB first)
  for(int i=7; i>=0; i--) {
    digitalWrite(DI, (data >> i) & 1); // Send data bits
    digitalWrite(SK, HIGH);
    digitalWrite(SK, LOW);
  }
  
  digitalWrite(CS, LOW);
  delay(10);
}

void setup() {
    pinMode(CS, OUTPUT);
    pinMode(SK, OUTPUT);
    pinMode(DI, OUTPUT);
    pinMode(DO, INPUT_PULLUP);
    Serial.begin(9600);
    digitalWrite(CS, LOW);
    digitalWrite(SK, LOW);
    digitalWrite(DI, LOW);
}



void loop() {
  int ch;
  Serial.println("\n93c46 Test Menu");
  Serial.println("1 - Dump NVram");
  Serial.println("2 - EWEN Write Enable");
  Serial.println("3 - EWDS Write DISable");
  Serial.println("4 - WRAL 0xA5");
  Serial.println("5 - ERAL Erase ALL");
  Serial.println("6 - Write  0:12,34,56,78,9A,BC,DE,FA");
  Serial.println("7 - Write  0:22,44,66,88,AA,BB,CC,DD");
  Serial.println("8 - Erase  0, 3,4, 7");
  Serial.println("9 - Write Sequential numbers");

  Serial.println("CDM: ");
  while(Serial.available()==0);  // wait for input
  ch= Serial.read();   // Note - Handle CR LF on input

switch (ch) {

case '1':
Serial.println("1 - Dump NVram");
// Implement code to read all the address and print the result
// Read all addresses and print values
//for (int i = 0; i < 128; i++)
//{
//  Serial.println(EEPROM_read(i), HEX);
//  digitalWrite(CS, LOW);
//}

Serial.println(EEPROM_read(4), HEX);
digitalWrite(CS, LOW);



break;

case '2':
Serial.print("EWEN Write Enable");
EEPROM_ewen();
Serial.println(" - Done");
break;

case '3':
Serial.print("EWDS Write DISable");
EEPROM_ewds();
Serial.println(" - Done");
break;

case '4':
Serial.print("WRAL Write ALL = 0xA5");
EEPROM_wral(0xA5);
Serial.println(" - Done");
break;

case '5':
Serial.print("ERAL Erase ALL");
//EEPROM_eral();
Serial.println(" - Done");
break;

case '6':
Serial.print("Write 0:12,34,56,78,9A,BC,DE,FA");

EEPROM_write(4,0x9A);

Serial.println(" - Done");
break;

case '7':
Serial.print("Write 0:22,44,66,88,AA,BB,CC,DD");
EEPROM_write(0,0x22);
EEPROM_write(1,0x44);
EEPROM_write(2,0x66);
EEPROM_write(3,0x88);
EEPROM_write(4,0xAA);
EEPROM_write(5,0xBB);
EEPROM_write(6,0xCC);
EEPROM_write(7,0xDD);
Serial.println(" - Done");
break;
case '8':
Serial.print("Erase 0, 3,4, 7");
//EEPROM_erase(0);
//EEPROM_erase(3);
//EEPROM_erase(4);
//EEPROM_erase(7);
Serial.println(" - Done");
break;

case '9':
Serial.println("9 - Write Sequential numbers");

// Write first 128 ASCII characters
for (int i = 0; i < 128; i++)
{
EEPROM_write(i, i);
}
Serial.print("Sequential numbers written");
break;

default:
break;

}
}