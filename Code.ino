#include <Servo.h>

#define S0 2
#define S1 1
#define S2 3
#define S3 4
#define sensorOut 5
#define solenoid1 7
#define solenoid2 8
#define solenoid3 9
#define solenoid4 10
#define solenoid5 11
#define servo 6

Servo myservo;

float r = 0;
float g = 0;
float b = 0;
float c = 0;

float yellow_rgb[] = {0, 0, 0};
float purple_rgb[] = {0, 0, 0};
float green_rgb[] = {0, 0, 0};
float red_rgb[] = {0, 0, 0};
float orange_rgb[] = {0, 0, 0};
float brown_rgb[] = {0, 0, 0};

float calib_rgb = 0;

int history = 0;

void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  pinMode(solenoid1, OUTPUT);
  pinMode(solenoid2, OUTPUT);
  pinMode(solenoid3, OUTPUT);
  pinMode(solenoid4, OUTPUT);
  pinMode(solenoid5, OUTPUT);
  
  myservo.write(90);
  myservo.attach(servo);
  
  digitalWrite(S0,LOW);
  digitalWrite(S1,HIGH);

  Serial.begin(9600);
  Serial.println("");

  unstuck();
  calibrate_wheel();
  delay(500);
  rotate();
  delay(5000);
  rotate();
  calibrate();
}

void loop() {
  int color = measure();
  solenoid_color(history);
  history = color;
}

void rotate() {
  myservo.write(40);
  delay(200);

  float rgb = 0;

  while (rgb < calib_rgb || rgb == 0) {
    rgb = read_red();
  }

  myservo.write(90);
}

void unstuck() {
  Serial.println("unstucking");
  myservo.write(130);
  delay(100);
  myservo.write(40);
  delay(300);
}

void solenoid_color(int color) {

  if (color == 0) {
    return;
  }

  if (color == 1) {
    delay(500);
  }

  if (color == 2) {
    delay(200);
  }

  if (color == 3) {
    delay(150);
  }

  if (color == 4) {
    delay(100);
  }
  digitalWrite(color+6, HIGH);
  delay(1000);
  digitalWrite(color+6, LOW);
}

void calibrate_wheel() {
  calib_rgb = 0;
  myservo.write(40);
  for (int i = 0; i < 500; i++) {
    calib_rgb = calib_rgb + read_red();
    delay(10); 
  }
  calib_rgb = calib_rgb/500;
  myservo.write(90);
  Serial.println("calibrated");
  Serial.println(calib_rgb);
}

int measure() {
  rotate();
  delay(500);
  return read_color();
}

void calibrate() {
  Serial.println("Yellow calibration, type 1 when ready.");
  while(Serial.available() == 0){}
  Serial.readStringUntil('\n');
  rotate();
  calibrate_color(yellow_rgb);
  print_rgb(yellow_rgb[0], yellow_rgb[1], yellow_rgb[2]);

  Serial.println("Purple calibration, type 1 when ready.");
  while(Serial.available() == 0){}
  Serial.readStringUntil('\n');
  rotate();
  calibrate_color(purple_rgb);
  print_rgb(purple_rgb[0], purple_rgb[1], purple_rgb[2]);

  Serial.println("Green calibration, type 1 when ready.");
  while(Serial.available() == 0){}
  Serial.readStringUntil('\n');
  rotate();
  calibrate_color(green_rgb);
  print_rgb(green_rgb[0], green_rgb[1], green_rgb[2]);

  Serial.println("Red calibration, type 1 when ready.");
  while(Serial.available() == 0){}
  Serial.readStringUntil('\n');
  rotate();
  calibrate_color(red_rgb);
  print_rgb(red_rgb[0], red_rgb[1], red_rgb[2]);

  Serial.println("Orange calibration, type 1 when ready.");
  while(Serial.available() == 0){}
  Serial.readStringUntil('\n');
  rotate();
  calibrate_color(orange_rgb);
  print_rgb(orange_rgb[0], orange_rgb[1], orange_rgb[2]);

  rotate();
  Serial.println("Brown calibration");
  calibrate_color(brown_rgb);
  print_rgb(brown_rgb[0], brown_rgb[1], brown_rgb[2]);
}

void calibrate_color(float color_rgb[]) {
  for (int i = 0; i < 20; i++) {
    color_rgb[0] = color_rgb[0] + read_red();
    color_rgb[1] = color_rgb[1] + read_green();
    color_rgb[2] = color_rgb[2] + read_blue();
  }
  color_rgb[0] = color_rgb[0] / 20;
  color_rgb[1] = color_rgb[1] / 20;
  color_rgb[2] = color_rgb[2] / 20;
}

// REQUIRED CALIBRATION
int read_color() {

  r = 0;
  g = 0;
  b = 0;

  for (int i = 0; i < 50; i++) {
    r = r + read_red();
    g = g + read_green();
    b = b + read_blue();
  }
  r = r / 50;
  g = g / 50;
  b = b / 50;
  print_rgb(r,g,b);

  int color = 1;
  float color_dist = color_distance(r, g, b, yellow_rgb);

  if (color_dist > color_distance(r, g, b, purple_rgb)) {
    color = 2;
    color_dist = color_distance(r, g, b, purple_rgb);
  }

  if (color_dist > color_distance(r, g, b, green_rgb)) {
    color = 3;
    color_dist = color_distance(r, g, b, green_rgb);
  }

  if (color_dist > color_distance(r, g, b, red_rgb)) {
    color = 4;
    color_dist = color_distance(r, g, b, red_rgb);
  }

  if (color_dist > color_distance(r, g, b, orange_rgb)) {
    color = 5;
    color_dist = color_distance(r, g, b, orange_rgb);
  }

  if (color_dist > color_distance(r, g, b, brown_rgb)) {
    color = 0;
    color_dist = color_distance(r, g, b, brown_rgb);
  }

  print_color(color);
  return color;
}

float color_distance(float r, float g, float b, float color_rgb[]) {
  return (r-color_rgb[0])*(r-color_rgb[0]) + (g-color_rgb[1])*(g-color_rgb[1]) + (b-color_rgb[2])*(b-color_rgb[2]);
}

int read_red() {
  // Setting red filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  // Reading the output frequency
  return pulseIn(sensorOut, LOW);
}

int read_green() {
  // Setting green filtered photodiodes to be read
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  return pulseIn(sensorOut, LOW);
}

int read_clear() {
  // Setting clear filtered photodiodes to be read
  digitalWrite(S2,HIGH);
  digitalWrite(S3,LOW);
  // Reading the output frequency
  return pulseIn(sensorOut, LOW);
}

int read_blue() {
  // Setting blue filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  return pulseIn(sensorOut, LOW);
}

void print_rgb(float r, float g, float b) {
  // Printing the value on the serial monitor
  Serial.print("R= ");//printing name
  Serial.print(r);//printing RED color frequency
  Serial.print("  ");

  // Printing the value on the serial monitor
  Serial.print("G= ");//printing name
  Serial.print(g);//printing RED color frequency
  Serial.print("  ");

  // Printing the value on the serial monitor
  Serial.print("B= ");//printing name
  Serial.print(b);//printing RED color frequency
  Serial.print(" ");
  Serial.println(" ");
}

void print_color(int color) {
  if (color == 1) {
    Serial.println("Yellow");
  }

  if (color == 2) {
    Serial.println("Purple");
  }

  if (color == 3) {
    Serial.println("Green");
  }

  if (color == 4) {
    Serial.println("Red");
  }

  if (color == 5) {
    Serial.println("Orange");
  }
}
