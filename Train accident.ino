#include <SoftwareSerial.h>
SoftwareSerial mySerial(3, 2);  //Arduino's (rx,tx)
int PIRsensor = 7;              //PIR Sensor Pin
int speaker = A5;               //speaker Pin
int MotionRead, Motion_alert_val, recheckMotion;
int pir_Status, Motion_sms_count = 0, PIR_Motion_Status;
void setup() {
  pinMode(PIRsensor, INPUT);
  pinMode(speaker, OUTPUT);
  pinMode(5, OUTPUT);  // for motor p
  pinMode(6, OUTPUT);  // for motor n
  Serial.begin(9600);

  mySerial.begin(9600);  // Sim Baud rate
  Serial.println("Initializing...");
  delay(1000);
  mySerial.println("AT");  //Once the handshake test is successful, it will back to OK
  updateSerial();

  digitalWrite(speaker, LOW);  // speaker off
  analogWrite(5, 225);
  analogWrite(6, 0);  // motor on
}
void loop() {
  CheckMotion();
  CheckNoMotion();
}
void CheckMotion() {
  Serial.println("Motion Scan - ON");
  Motion_alert_val = ScanMotion();
  //D7 Pin becomes HIGH when Motion detected
  if (Motion_alert_val == HIGH) {
    MotionAlert();  // Function to send SMS Alerts
    MotorDriver();  // motor drive speed control
  }
}
int ScanMotion() {
  MotionRead = digitalRead(PIRsensor);
  return MotionRead;
}
void MotorDriver() {
  analogWrite(5, 225);
  analogWrite(6, 0);
  delay(3000);
  analogWrite(5, 170);
  analogWrite(6, 0);
  delay(2000);
  analogWrite(5, 140);
  analogWrite(6, 0);
  delay(2000);
  analogWrite(5, 100);
  analogWrite(6, 0);
  delay(2000);
  analogWrite(5, 80);
  analogWrite(6, 0);
  delay(2000);
  analogWrite(5, 70);
  analogWrite(6, 0);
  delay(2000);
  analogWrite(5, 0);
  analogWrite(6, 0);
  delay(900000);
}
void MotionAlert() {
  digitalWrite(speaker, HIGH);  // speaker on
  while (Motion_sms_count < 1)  //Number of SMS Alerts to be sent
  {
    mySerial.println("AT+CMGF=1");  // Configuring TEXT mode
    updateSerial();
    mySerial.println("AT+CMGS=\"+916384775109\"");  // phone number to sms
    updateSerial();
    mySerial.print("Animal detected, Slow down the train ");  //text content
    delay(1000);
    updateSerial();
    mySerial.write(26);

    Motion_sms_count++;
  }
  PIR_Motion_Status = 1;
  Serial.println("Motion Detected! SMS Sent!");
  delay(100);
}
void CheckNoMotion() {
  if (PIR_Motion_Status == 1) {
    recheckMotion = ScanMotion();
    if (recheckMotion == LOW)  //D0 Pin becomes HIGH when No Gas
    {
      Serial.println("No Motion Detected");
      digitalWrite(speaker, LOW);
      analogWrite(5,225);
      analogWrite(6,0);
      Motion_sms_count = 0;  //Reset count for next alert triggers
      PIR_Motion_Status = 0;
    }
  }
}
void updateSerial() {
  delay(500);
  while (Serial.available()) {
    mySerial.write(Serial.read());  //Forward what Serial received to Software Serial Port
  }
  while (mySerial.available()) {
    Serial.write(mySerial.read());  //Forward what Software Serial received to Serial Port
  }
}