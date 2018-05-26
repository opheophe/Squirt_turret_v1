#include <NewPing.h>
#include <Servo.h>

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define USPOWER_PIN  4
#define MAX_DISTANCE 2000 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

int buzzer_pin = 2;
int relay_pin = 10;
int servo_pin = 9;
int sensitivity_pin = A7;

int calibration_iterations = 3;
int accepted_deviation = 400; // deviation to trigger a shot,altered with potentiometer

// Servo settings
Servo servo1;
int low = 6;
int mid = 90;
int high = 174;
int turn_delay = 30;
int measure_delay = 30;
int turn = 3;

// Other settings
float distances[] = {
  -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0,
  -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0,
  -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0,
  -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0,
  -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0,
  -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0,
  -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0,
  -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0,
  -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0,
  -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0,
  -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0,
  -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0,
  -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0,
  -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0,
  -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0,
  -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0,
  -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0,
  -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0, -400.0
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

float measure() {
  return measure_recursive(0);
}

float measure_recursive(int count) {
  float temp = sonar.ping_cm();
  delay(measure_delay + count);

  if (temp == 0 && digitalRead(ECHO_PIN) == HIGH) {
    pinMode(ECHO_PIN, OUTPUT);
    digitalWrite(ECHO_PIN, LOW);


    digitalWrite(USPOWER_PIN, LOW);
    delay(100);     // 50 mS is the minimum Off time to get clean restart
    digitalWrite(USPOWER_PIN, HIGH); // Adjust this value if your sensors don't read after reset
    pinMode(ECHO_PIN, INPUT);
    delay(100);     // Some sensors throw out a very short false echo after timeout
    sonar.ping_cm(); // 5cm-10cm value.  If your sensors do not throw out
    delay(100);
    // this false echo, you can get rid of this delay and ping after power HIGH
  }                // Conversely, if you still get a false echo, you may need to increase the delay

  if ((count > 1) && (temp > 5)) {
    Serial.print(" --> ");
    Serial.println(temp);
    return temp;
  } else if (temp > 5) {
    return temp;
  } else if (count == 1) {
    Serial.print("## Measure error: >");
  } else if (count > 10) {
    Serial.println(" --> -400.00");
    return -400.0;
  } else if (count > 1) {
    Serial.print(">");
  }
  return measure_recursive(count + 1);

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

  float measured_distance = sonar.ping_cm();
  float deviation = measured_distance - distances[current_angle];
  delay(60);

  if (measured_distance == 0) {
    Serial.println("No reading");
  } else if (abs(deviation) > accepted_deviation) {
    deviation = measured_distance - distances[current_angle];
    Serial.print("# Angle ");
    Serial.print(current_angle);
    Serial.print(" Measured: ");
    Serial.print(measured_distance);
    Serial.print(" Previous: ");
    Serial.print(distances[current_angle]);

    if (deviation > 0) {
      Serial.print(" --> ** Distance increased ** ");
      Serial.print(" --> Recalibrate");
      distances[current_angle] = measured_distance;
      Serial.println();
    } else if (deviation < 0) {
      Serial.print(" --> ** FIRE ** ");
 //     fire();
      Serial.print(" --> Recalibrate");
      distances[current_angle] = measured_distance;
      Serial.println();
    }
  }
  if (current_angle==105){
    delay(1000);
  }

}


//void measureAndShoot() {
//  checkSensitivity();
//  delay(10);
//
//
//  // 1. Check if we have a possible deviation
//  // 2. If TRUE calibrate
//  // 3. Check if we have a deviation
//  // 4.
//
//  if (sonar.ping_cm() == 0) {
//    Serial.println("Fuck");
//  } else {
//
//    float measured_distance = measure();
//    float deviation = measured_distance - distances[current_angle];
//
//
//    if (abs(deviation) > accepted_deviation) {
//      measured_distance = calibrate();
//      deviation = measured_distance - distances[current_angle];
//
//      if (abs(deviation) > accepted_deviation) {
//        if (deviation > 0) { // Measured distance is further away than previous distance
//          // Do not shoot, recalibrate and continue
//          Serial.print("# Angle ");
//          Serial.print(current_angle);
//          Serial.print(" Measured: ");
//          Serial.print(measured_distance);
//          Serial.print(" Deviation: ");
//          Serial.print(deviation);
//          Serial.print(" --> ** Distance increased ** ");
//          Serial.print(" --> Recalibrate: ");
//          Serial.print(measured_distance);
//          distances[current_angle] = measured_distance;
//          Serial.println();
//        } else if (deviation < 0) {
//          Serial.print("# Angle ");
//          Serial.print(current_angle);
//          Serial.print(" Measured: ");
//          Serial.print(measured_distance);
//          Serial.print(" Deviation: ");
//          Serial.print(deviation);
//          Serial.print(" --> ** FIRE ** ");
//          fire();
//          Serial.print(" --> Recalibrate: ");
//          Serial.print(measured_distance);
//          distances[current_angle] = measured_distance;
//          Serial.println();
//        }
//      } else {
//        Serial.print("# Pos ");
//        Serial.print(current_angle);
//        Serial.print(" Measured: ");
//        Serial.print(measured_distance);
//        Serial.print(" Deviation: ");
//        Serial.print(deviation);
//        Serial.print(" --> ** ABORT ** ");
//        Serial.println();
//      }
//    }
//  }
//}

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
  //Serial.println(moveto_angle);
  current_angle = moveto_angle;
  delay(turn_delay);
  servo1.detach();
}

float calibrate() {
  float tempRange = 0.0;
  for (int i = 1; i <= calibration_iterations; i++) {
    tempRange = tempRange + measure();
  }
  return tempRange / calibration_iterations;
}

void calibrate_distances() {
  beep(400);
  Serial.println("### Calibrating");

  for (int i = high; i >= low; i = i - turn) {
    moveto(i);
    delay(10);
    distances[i] = calibrate();
    delay(measure_delay);
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
  delay(1000);
  pinMode(USPOWER_PIN, OUTPUT);
  digitalWrite(USPOWER_PIN, HIGH);
  Serial.println();
  //servo1.attach(servo_pin);
  //servo1.detach();
  pinMode(buzzer_pin, OUTPUT);
  pinMode(relay_pin, OUTPUT);
  pinMode(servo_pin, OUTPUT);
  moveto(low);
  delay(1000);
  beep(250);
  // calibrate_distances();
}

void loop () {
  for (int i = low; i < high; i = i + turn) {
    moveto(i);
    delay(turn_delay);
    measureAndShoot();

  }
  for (int i = high; i > low; i = i - turn) {
    moveto(i);
    delay(turn_delay);
    measureAndShoot();
  }
}

