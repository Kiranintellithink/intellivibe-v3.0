#include <Arduino.h>
#include <SPI.h>

// Define SPI pins
#define SCK_PIN   18
#define MISO_PIN  19
#define MOSI_PIN  23
#define CS_PIN    5

// KX132-1211 registers
#define WHO_AM_I 0x13
#define CNTL1    0x1B
#define ODCNTL   0x21
#define XOUT_L   0x08
#define XOUT_H   0x09
#define YOUT_L   0x0A
#define YOUT_H   0x0B
#define ZOUT_L   0x0C
#define ZOUT_H   0x0D

// Function prototypes
uint8_t readRegister(uint8_t reg);
void writeRegister(uint8_t reg, uint8_t value);
int16_t read16(uint8_t reg);
SPIClass spi(HSPI);

void setup() {
  Serial.begin(115200);

  // Initialize SPI
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);

  // Set CS pin as output
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  SPI.beginTransaction(SPISettings(100000, MSBFIRST,SPI_MODE0 ));
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);

  // Initialize KX132-1211
  writeRegister(CNTL1, 0x00); // Put accelerometer in standby mode
  delay(100);
  writeRegister(CNTL1, 0x80); // Set operating mode and data rate
  writeRegister(ODCNTL,0x06); // Set output data rate to 50 Hz
  delay(100);

}

void loop() {
  // Read accelerometer data
  int16_t x = read16(XOUT_L);
  int16_t y = read16(YOUT_L);
  int16_t z = read16(ZOUT_L );

  Serial.print("x: ");
  Serial.print(x);
  Serial.print(" y: ");
  Serial.print(y);
  Serial.print(" z: ");
  Serial.println(z);

  delay(1000);
}

uint8_t readRegister(uint8_t reg) {
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(reg | 0x80);  // Set MSB for read operation
  uint8_t value = SPI.transfer(0x00);  // Dummy write to read data
  digitalWrite(CS_PIN, HIGH);
  return value;
}

void writeRegister(uint8_t reg, uint8_t value) {
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(reg & 0x7F);  // Clear MSB for write operation
  SPI.transfer(value);
  digitalWrite(CS_PIN, HIGH);
}

int16_t read16(uint8_t reg) {
  uint8_t lowByte, highByte;
  
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(reg | 0x80);  // Set MSB for read operation
  lowByte = SPI.transfer(0x00);
  highByte = SPI.transfer(0x00);
  digitalWrite(CS_PIN, HIGH);
  
  return (highByte << 8) | lowByte;
}
