#include <NewPing.h>
#include <Servo.h>

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 500 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

int buzzer_pin = 2;
int relay_pin = 10;
int servo_pin = 9;
int sensitivity_pin = A7;

int calibration_iterations = 3;
int accepted_deviation = 400; // deviation to trigger a shot,altered with potentiometer

// Servo settings
Servo servo1;
int low = 5;
int mid = 80;
int high = 175;
int turn_delay = 100;

// Other settings
float distances[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int num_positions = 180;

int current_position = num_positions / 2;
int current_angle = mid;
int direction = true; // true = +

void print_distances() {
  for (int i = low; i < high; i = i + 5) {
    Serial.print(distances[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void setAngle(int angle, int wait) {
  Serial.println(angle);
  servo1.attach(servo_pin);
  servo1.write(angle);
  delay(wait);
  servo1.detach();
  current_angle = angle;
}

void beep(int duration) {
  digitalWrite(buzzer_pin, HIGH);
  delay(duration);
  digitalWrite(buzzer_pin, LOW);
}


void fire() {
  digitalWrite(relay_pin, HIGH);
  beep(250);
  // delay(250);
  digitalWrite(relay_pin, LOW);
}

void measureAndShoot() {
  checkSensitivity();
  delay(10);
  float measure = sonar.ping_cm();
  float deviation = abs(measure - distances[current_angle]);
  delay(60);
  
    if (deviation > accepted_deviation) {
      measure = calibrate();
      deviation = abs(measure - distances[current_angle]);
      if (deviation > accepted_deviation) {
        Serial.print("# Angle ");
        Serial.print(current_angle);
        Serial.print(" Measured: ");
        Serial.print(measure);
        Serial.print(" Deviation: ");
        Serial.print(deviation);
        Serial.print(" --> ** FIRE ** ");
        fire();
        Serial.print(" --> Recalibrate");
        distances[current_angle] = calibrate();
        Serial.println();
      } else {
        Serial.print("# Pos ");
        Serial.print(current_angle);
        Serial.print(" Measured: ");
        Serial.print(measure);
        Serial.print(" Deviation: ");
        Serial.print(deviation);
        Serial.print(" --> ** ABORT ** ");
        Serial.println();
      }
    }

}

void printupto3blank(int number) {
  if (number > 99) {
    Serial.print("");
  } else if (number > 9) {
    Serial.print(" ");
  } else {
    Serial.print("  ");
  }
}

void moveto(int moveto_angle) {
  servo1.attach(servo_pin);
  servo1.write(moveto_angle);
  current_angle = moveto_angle;
  delay(turn_delay);
  servo1.detach();
}

float calibrate() {
  float tempRange = 0.0;
  for (int i = 1; i <= calibration_iterations; i++) {
    tempRange = tempRange + sonar.ping_cm();
    delay(100); // Specified in datasheet
  }
  return tempRange / calibration_iterations;
}

void calibrate_distances() {
  beep(400);
  Serial.println("### Calibrating");

  for (int i = high; i > low; i = i - 5) {
    moveto(i);
    delay(10);
    distances[i] = calibrate();
    delay(50);
    Serial.print(i);
    Serial.print(" ");
    Serial.println(distances[i]);
  }
  print_distances();
  Serial.println("### Calibrating complete");
  beep(400);
  delay(200);
  beep(400);
}

void checkSensitivity() {
  int sensitivity = map(analogRead(sensitivity_pin), 0, 1023, 0, 400);
  if (abs(sensitivity - accepted_deviation) > 20) {
    // Do another check just to make sure
    sensitivity = map(analogRead(sensitivity_pin), 0, 1023, 0, 400);
    if (abs(sensitivity - accepted_deviation) > 20) {
      accepted_deviation = sensitivity;
      Serial.print("Accepted deviation: ");
      Serial.println(accepted_deviation);
    }
  }
}

void setup () {
  Serial.begin(9600);
  Serial.println();
  //servo1.attach(servo_pin);
  //servo1.detach();
  pinMode(buzzer_pin, OUTPUT);
  pinMode(relay_pin, OUTPUT);
  pinMode(servo_pin, OUTPUT);
  moveto(high);
  delay(1000);
  calibrate_distances();
}

void loop () {
  for (int i = low; i < high; i = i + 5) {
    moveto(i);
    measureAndShoot();
    delay(60);
  }
  for (int i = high; i > low; i = i - 5) {
    moveto(i);
    measureAndShoot();
    delay(60);
  }
}

