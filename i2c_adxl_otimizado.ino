#include <Wire.h>
#define device 0x53
#define sample_freq 800 //hz
long time=0;
void setup() {
  
  Serial.begin(230400);
  Wire.begin();


  Wire.beginTransmission(device);
  Wire.write(0x2C);//BW_Rate
  Wire.write(0x0D);// 0x0D = 00001101 ->800HZ
  Wire.endTransmission();

  Wire.beginTransmission(device);
  Wire.write(0x2D);//Power_ctrl
  Wire.write(0x08);//0x08 = 00001000 -> Measure
  Wire.endTransmission();

  Wire.beginTransmission(device);
  Wire.write(0x31);//Data_format
  Wire.write(0x0B);//0x0B = 1011 ->FullRes(13bit) 16g| g= 2,4,8,16 ->0x08,0x09,0x0A,0x0B
  Wire.endTransmission();
}

void loop() {
  
  Wire.beginTransmission(device);
  Wire.write(0x36); //primeiro byte de Z
  Wire.endTransmission();
  Wire.requestFrom(device,1);
  byte z0 = Wire.read();


  Wire.beginTransmission(device);
  Wire.write(0x37); //segundo byte de Z
  Wire.endTransmission();
  Wire.requestFrom(device,1);
  byte z1 = Wire.read(); //recebe apenas 5 bits
  z1 = z1 & 0x1F; // enche os outros bits de 0

  uint16_t z = (z1<<8) + z0; //inteiro nao assinalado
  int16_t zf = z;
  if (zf > 4095)
  {
    zf = zf - 8192; //criar os negativos
  }
  //zf tem o extremos de -4095 e 4095

  
  Serial.println(zf);
  delayMicroseconds(1e6/sample_freq-micros()+time);
  time = micros();
}
