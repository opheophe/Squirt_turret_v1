#include <NewPing.h>
#include <Servo.h>

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define USPOWER_PIN  4
#define MAX_DISTANCE 500 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

int buzzer_pin = 2;
int relay_pin = 10;
int servo_pin = 9;
int sensitivity_pin = A7;
int led_red_pin = 6;
int led_green_pin = 5;

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
int current_angle = mid;

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

int distances[] = {
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400
};

int distances_2[] = {
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400,
  -400, -400, -400, -400, -400, -400, -400, -400, -400, -400
};

int num_wrong[] = {
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

int num_wrong_2[] = {
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

void setup() {
  Serial.begin(9600);
  pinMode(buzzer_pin, OUTPUT);
  pinMode(led_red_pin, OUTPUT);
  pinMode(relay_pin, OUTPUT);
  pinMode(led_green_pin, OUTPUT);
  pinMode(USPOWER_PIN, OUTPUT);
  pinMode(relay_pin, OUTPUT);
  
  beep(250);
  delay(1000);
  digitalWrite(USPOWER_PIN, HIGH);
  beep(250);
}

void moveto(int moveto_angle) {
  servo1.attach(servo_pin);
  servo1.write(moveto_angle);
  //Serial.println(moveto_angle);
  current_angle = moveto_angle;
  delay(turn_delay);
  servo1.detach();
}

void fill(int in) {
  if (in < 0) {

  } else {
    Serial.print(" ");
  }

  if (abs(in) > 999) {
    // Do nothing
  } else if (abs(in) > 99) {
    Serial.print(" ");
  } else if (abs(in) > 9) {
    Serial.print("  ");
  } else {
    Serial.print("   ");
  }
  Serial.print(in);


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

void beep(int duration) {
  digitalWrite(buzzer_pin, HIGH);
  delay(duration);
  digitalWrite(buzzer_pin, LOW);
}

void fire() {
  digitalWrite(relay_pin, HIGH);
<<<<<<< HEAD
  beep(250);
 //  delay(250);
=======
  //beep(250);
  delay(250);
>>>>>>> simplified
  digitalWrite(relay_pin, LOW);
}

void measure(int distance_array) {
  digitalWrite(led_red_pin, LOW);
  digitalWrite(led_green_pin, LOW);


  checkSensitivity();
  int distance;
  int wrong;
  if (distance_array == 1) {
    distance = distances[current_angle];
    wrong = num_wrong[current_angle];
  } else {
    distance = distances_2[current_angle];
    wrong = num_wrong_2[current_angle];
  }
  Serial.print("Angle: ");
  fill(current_angle);

  int cm = sonar.ping_cm();
  if (cm == 0 || cm > 500) {
    wrong = wrong + 1;
    // No proper reading
    digitalWrite(led_red_pin, HIGH);
    Serial.print(" Current: ");
    fill(cm);
    Serial.print(" Last: ");
    fill(distance);
    Serial.print(" Accepted deviation: ");
    fill(accepted_deviation);
    Serial.print(" Wrong: ");
    fill(wrong);


    if (wrong > 4) {
      digitalWrite(led_red_pin, HIGH);
      digitalWrite(led_green_pin, HIGH);
      Serial.print(" ¤¤ WRONG SET : 400 ¤¤ ");
      if (distance_array == 1) {
        distances[current_angle] = 400;
      } else {
        distances_2[current_angle] = 400;
      }
    }
    
    if (distance_array == 1) {
      num_wrong[current_angle] = wrong;
    } else {
      num_wrong_2[current_angle] = wrong;
    }



    Serial.println();
    delay(50);
  } else {
    // Proper reading
    if (distance_array == 1) {
      num_wrong[current_angle] = 0;
    } else {
      num_wrong_2[current_angle] = 0;
    }
    Serial.print(" Current: ");
    fill(cm);
    Serial.print(" Last: ");
    fill(distance);
    Serial.print(" Accepted deviation: ");
    fill(accepted_deviation);
    Serial.print(" --> ");
    fill(distance - cm);

    if (distance - cm > accepted_deviation) {
      digitalWrite(led_green_pin, HIGH);
      Serial.print(" ## FIRE ## ");
      fire();
    }

    Serial.println();

    if (distance_array == 1) {
      distances[current_angle] = cm;
    } else {
      distances_2[current_angle] = cm;
    }

    delay(50);
  }
}


void printbox(int numbox) {
  for (int i = 0; i < numbox; i++) {
    Serial.print("#");
  }
  Serial.println();
}

void loop() {

  for (int i = low; i < high; i = i + turn) {
    moveto(i);
    delay(turn_delay);
    measure(1);

  }
  for (int i = high; i > low; i = i - turn) {
    moveto(i);
    delay(turn_delay);
    measure(2);
  }



}



