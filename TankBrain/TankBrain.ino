#include <AFMotor.h>

#define DATA_MAX_LEN     25
char data[DATA_MAX_LEN] = {0};

AF_DCMotor motor1(1);
AF_DCMotor motor2(2);

int ReadUntilChar(char *buf, int len, char c) {
  int readed = 0;
  
  while(readed < len) {
    if(Serial.available()) {
      char cur = Serial.read();
      
      buf[readed] = cur;
      readed++;
      
      if(cur == c)
        break;
    }
  }
  
  return readed;
}
void SkipWhileNotChar(char c) {
  while(1)
    if(Serial.available()) {
      if(c == Serial.peek())
        break;
        
       Serial.read();
    }
}
int WaitResponse() {
  int ret = -1;
  char buffer[DATA_MAX_LEN];
  
  SkipWhileNotChar('[');
  int readed = ReadUntilChar(buffer, DATA_MAX_LEN, ']');
  
  if(!memcmp(buffer, "[OK]", 4)) {
    ret = 1;
  }
  else if(!memcmp(buffer, "[ERROR", 6)) {
    ret = 0;
  }
  
  return ret;
}
int SendCommand(const char* cmd) {
  int ret;
  Serial.println(cmd);
  ret = WaitResponse();
  delay(100);
  return ret;
}

int FromHex(char c) {
  switch(c) {
    case '0': return 0x0;
    case '1': return 0x1;
    case '2': return 0x2;
    case '3': return 0x3;
    case '4': return 0x4;
    case '5': return 0x5;
    case '6': return 0x6;
    case '7': return 0x7;
    case '8': return 0x8;
    case '9': return 0x9;
    case 'a': return 0xa;
    case 'b': return 0xb;
    case 'c': return 0xc;
    case 'd': return 0xd;
    case 'e': return 0xe;
    case 'f': return 0xf;
  }
}

void setup() {
  Serial.begin(115200);
  WaitResponse();
  SendCommand("AT");
  SendCommand("AT+WWEP1=f2c7bb35b9");
  SendCommand("AT+NDHCP=1");
  SendCommand("AT+WA=SweetHome3");
  SendCommand("AT+NAUTO=0,0,1.1.1.1,12345");
  SendCommand("ATA2");
  
    SkipWhileNotChar('[');
    ReadUntilChar(data,  DATA_MAX_LEN, ']');
   
  delay(1000);
  Serial.print("+");
  delay(200);
  Serial.print("+");
  delay(200);
  Serial.print("+");
  delay(3000);
  
  SendCommand("AT");
  SendCommand("AT+NCLOSEALL");
  
  data[strlen(data)-1] = 0x00;
  Serial.print("AT+NAUTO=0,1,");
  Serial.print(&data[1]);
  SendCommand(",34543");
  SendCommand("ATA2");
  
  motor1.setSpeed(1);
  motor1.run(RELEASE);
  motor2.setSpeed(1);
  motor2.run(RELEASE);
}

void loop() {
  SkipWhileNotChar('[');
  int readed = ReadUntilChar(data,  DATA_MAX_LEN, ']');
  
  int directionLeft    = FromHex(data[1]);
  int speedLeft        = 0x10 * FromHex(data[2]) + FromHex(data[3]);
  int directionRight   = FromHex(data[4]);
  int speedRight       = 0x10 * FromHex(data[5]) + FromHex(data[6]);
  
  motor1.setSpeed(speedLeft);
  motor1.run(directionLeft);
  motor2.setSpeed(speedRight);
  motor2.run(directionRight);
}
