#include <Arduino.h>
#include <Wire.h>

// Placeholder ESP32 pins
static const int I2C_SDA_PIN = 21;
static const int I2C_SCL_PIN = 22;

// Try 0x6A first, change to 0x6B if needed
static const uint8_t IMU_ADDR = 0x6B;

// Registers
static const uint8_t REG_WHO_AM_I = 0x0F;
static const uint8_t REG_CTRL2_G  = 0x11;
static const uint8_t REG_CTRL3_C  = 0x12;
static const uint8_t REG_OUTZ_L_G = 0x26;
static const uint8_t REG_OUTZ_H_G = 0x27;

bool writeReg(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(IMU_ADDR);
  Wire.write(reg);
  Wire.write(val);
  return Wire.endTransmission() == 0;
}

bool readReg(uint8_t reg, uint8_t &val) {
  Wire.beginTransmission(IMU_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom((int)IMU_ADDR, 1) != 1) return false;
  val = Wire.read();
  return true;
}

bool readRegs(uint8_t startReg, uint8_t *buf, size_t len) {
  Wire.beginTransmission(IMU_ADDR);
  Wire.write(startReg);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom((int)IMU_ADDR, (int)len) != (int)len) return false;

  for (size_t i = 0; i < len; i++) {
    buf[i] = Wire.read();
  }
  return true;
}

bool initIMU() {
  uint8_t who = 0;
  if (!readReg(REG_WHO_AM_I, who)) {
    Serial.println("WHO_AM_I read failed");
    return false;
  }

  Serial.print("WHO_AM_I = 0x");
  Serial.println(who, HEX);

  // Don't hard-fail here since your board reported 0x6C.
  // We only want minimal bring-up right now.

  // BDU = 1 for safer multi-byte reads
  // CTRL3_C bit6 = BDU
  if (!writeReg(REG_CTRL3_C, 0x40)) {
    Serial.println("CTRL3_C write failed");
    return false;
  }

  // Turn on gyro only:
  // CTRL2_G = 0x40
  // ODR_G = 104 Hz, FS_G = 250 dps on LSM6DS3TR-C table/example style
  if (!writeReg(REG_CTRL2_G, 0x40)) {
    Serial.println("CTRL2_G write failed");
    return false;
  }

  delay(50);
  return true;
}

bool readGyroZRaw(int16_t &gz) {
  uint8_t data[2];
  if (!readRegs(REG_OUTZ_L_G, data, 2)) {
    return false;
  }
  gz = (int16_t)((data[1] << 8) | data[0]);
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(500);

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(400000);

  if (!initIMU()) {
    Serial.println("IMU init failed");
    while (true) delay(1000);
  }

  Serial.println("IMU init OK");
}

void loop() {
  int16_t gz = 0;
  if (readGyroZRaw(gz)) {
    Serial.print("gyro_z_raw = ");
    Serial.println(gz);
  } else {
    Serial.println("gyro read failed");
  }
  delay(100);
}