#include <HCSR04.h>
#include <Servo.h>

UltraSonicDistanceSensor distanceSensor(12, 11);  // Initialize sensor that uses digital pins 13 and 12.
int buzzer_pin = 2;
int relay_pin = 10;
int servo_pin = 9;
int sensitivity_pin = A7;

int calibration_iterations = 10;
int accepted_deviation = 400; // deviation to trigger a shot,altered with potentiometer


// Servo settings
Servo servo1;
int low = 24;
int mid = 80;
int high = 166;
int turn_delay = 50;

// Other settings
float distances[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
int positions[]  = {24, 38, 52, 66, 80, 102, 123, 145, 166};
int num_positions = 9;
int current_position = num_positions / 2;
int current_angle = mid;
int direction = true; // true = +

void setupPositions() {
  for (int i = 0; i < num_positions; i++) {
    positions[i] = low + i * (high - low) / (num_positions - 1);
  }
}


void print_array(int in_array[]) {
  for (int i = 0; i < num_positions; i++) {
    Serial.print(in_array[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void print_array(float in_array[]) {
  for (int i = 0; i < num_positions; i++) {
    Serial.print(in_array[i]);
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

void pulse_beep(int duration, int pulses) {
  for (int i = 1; i <= pulses; i++) {
    digitalWrite(buzzer_pin, HIGH);
    delay(duration);
    digitalWrite(buzzer_pin, LOW);
  }
}


void fire() {
  digitalWrite(relay_pin, HIGH);
  beep(250);
  // delay(250);
  digitalWrite(relay_pin, LOW);
}

float calibrate(int msec) {
  float tempRange = 0.0;
  for (int i = 1; i <= calibration_iterations; i++) {
    tempRange = tempRange + distanceSensor.measureDistanceCm();
    delay(msec);
  }
  return tempRange / calibration_iterations;
}

void measureAndShoot(int position) {
  checkSensitivity();
  delay(10);
  float measure = distanceSensor.measureDistanceCm();
  float deviation = abs(measure - distances[position]);
  delay(10);


  // 1. Check if there might be a deviation.
  // 2. Doublecheck by measuring several times
  // 3. Trigger fire or abort

  if (deviation > accepted_deviation) {
    measure = calibrate(50);
    deviation = abs(measure - distances[position]);
    if (deviation > accepted_deviation) {
      Serial.print("# Pos ");
      Serial.print(position);
      Serial.print(" Measured: ");
      Serial.print(measure);
      Serial.print(" Deviation: ");
      Serial.print(deviation);
      Serial.print(" --> ** FIRE ** ");
      fire();
      Serial.print(" --> Recalibrate");
      distances[position] = calibrate(100);
      Serial.println();
    } else {
      Serial.print("# Pos ");
      Serial.print(position);
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

void movetoposition(int new_position) {
  moveto(positions[new_position]);
  current_position = new_position;
}

void moveto(int moveto_angle) {
  servo1.attach(servo_pin);
  // printupto3blank(moveto_angle);
  //  Serial.print(moveto_angle);
  //  Serial.print(" ");

  if (moveto_angle < current_angle) {
    for (int i = current_angle; i > moveto_angle; i--) {
      servo1.write(i);
      current_angle = i;
      delay(turn_delay);
      //Serial.print(" ");
      //printupto3blank(current_angle);
      //Serial.print(current_angle);
      //      Serial.print(">");
    }
  } else if (moveto_angle > current_angle) {
    for (int i = current_angle; i < moveto_angle; i++) {
      servo1.write(i);
      current_angle = i;
      delay(turn_delay);
      //Serial.print(" ");
      //printupto3blank(current_angle);
      //Serial.print(current_angle);
      //      Serial.print(">");
    }
  }
  servo1.detach();
  current_angle = moveto_angle;
  // Serial.println();
}

void calibrate_distances() {
  Serial.println("### Calibrating");
  for (int i = num_positions - 1; i >= 0; i--) {
    Serial.print(i);
    Serial.print(" ");
    movetoposition(i);
    distances[current_position] = calibrate(100);
  }
  Serial.println();
  print_array(distances);
  Serial.println("### Calibrating complete");
}

void setup () {
  Serial.begin(9600);
  Serial.println();
  setupPositions();

  pinMode(buzzer_pin, OUTPUT);
  pinMode(relay_pin, OUTPUT);
  pinMode(servo_pin, OUTPUT);

  calibrate_distances();
}

void checkSensitivity() {
  int sensitivity = map(analogRead(sensitivity_pin), 0, 1023, 0, 400);
  if (sensitivity != accepted_deviation) {
    accepted_deviation = sensitivity;
    Serial.print("Accepted deviation: ");
    Serial.println(accepted_deviation);
  }
}

void loop () {

  for (int i = 0; i < num_positions; i++) {
    movetoposition(i);
    delay(1000);
    measureAndShoot(i);
  }
  for (int i = num_positions - 2; i > 0; i--) {
    movetoposition(i);
    delay(1000);
    measureAndShoot(i);
  }

}

