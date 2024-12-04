#define DIR_A 4   // Direction control pin for motor A
#define PWM_A 5   // PWM control pin for motor A
#define DIR_B 7   // Direction control pin for motor B
#define PWM_B 6   // PWM control pin for motor B

#define TRIG_PIN 8  // Trig pin for the ultrasonic sensor
#define ECHO_PIN 9  // Echo pin for the ultrasonic sensor

long duration;       // Duration of the pulse
int distance;        // Distance measurement in cm
String command = ""; // Store the received command
bool stopFlag = false; // Flag to track if stop command is active

void setup() {
  Serial.begin(9600);  // Start serial communication for debugging and Bluetooth

  // Set the motor control pins as output
  pinMode(DIR_A, OUTPUT);
  pinMode(PWM_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  pinMode(PWM_B, OUTPUT);

  // Set the ultrasonic sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Initialize motors in the forward direction
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);

  Serial.println("Setup complete. Waiting for voice commands...");
}

void loop() {
  // Trigger the ultrasonic sensor to send a pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read the echo pulse duration
  duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate the distance based on the duration
  distance = duration * 0.0344 / 2; // Convert time to distance in cm

  // Check for new commands
  if (Serial.available()) {
    command = Serial.readStringUntil('\n'); // Read the full command
    command.trim();                         // Remove trailing whitespace
    command.toLowerCase();                  // Convert to lowercase for consistency

    if (command == "stop") {
      stopFlag = true;  // Set the stop flag
      stopMotors();
      Serial.println("Command: Stop. Car stopped.");
      return;
    } else {
      stopFlag = false; // Reset the stop flag for other commands
      executeCommand(command);
    }
  }

  // If the stop flag is active, keep the car stopped
  if (stopFlag) {
    stopMotors();
    return;
  }

  // Check for obstacle
  if (distance < 20) {
    Serial.println("Obstacle detected! Waiting for voice command...");
    stopMotors();

    // Wait for a new command
    if (Serial.available()) {
      command = Serial.readStringUntil('\n'); // Read the full command
      command.trim();                         // Remove trailing whitespace
      command.toLowerCase();                  // Convert to lowercase for consistency
      executeCommand(command);
    }
  } else {
    // No obstacle and no stop command, move forward
    moveForward();
  }

  delay(100); // Small delay for stability
}

void executeCommand(String cmd) {
  if (cmd == "forward") {
    Serial.println("Command: Forward");
    moveForward();
  } else if (cmd == "backward") {
    Serial.println("Command: Backward");
    moveBackward();
  } else if (cmd == "left") {
    Serial.println("Command: Turn Right"); // Left command makes it turn right
    turnRight();
  } else if (cmd == "right") {
    Serial.println("Command: Turn Left"); // Right command makes it turn left
    turnLeft();
  } else {
    Serial.println("Invalid command. Use forward, backward, left, right, stop.");
  }
}

void moveForward() {
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_A, 255);
  analogWrite(PWM_B, 255);
  Serial.println("Moving forward...");
}

void moveBackward() {
  digitalWrite(DIR_A, HIGH);
  digitalWrite(DIR_B, HIGH);
  analogWrite(PWM_A, 255);
  analogWrite(PWM_B, 255);
  Serial.println("Moving backward...");
  delay(1000); // Move backward for 1 second
  stopMotors(); // Stop after moving backward
}

void turnLeft() {
  digitalWrite(DIR_A, HIGH);
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_A, 255);
  analogWrite(PWM_B, 255);
  delay(1000); // Adjust delay for turn duration
  stopMotors();
}

void turnRight() {
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, HIGH);
  analogWrite(PWM_A, 255);
  analogWrite(PWM_B, 255);
  delay(1000); // Adjust delay for turn duration
  stopMotors();
}

void stopMotors() {
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);
  Serial.println("Motors stopped.");
}
