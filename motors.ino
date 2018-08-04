int ledPin = 6;
int leftPWMPin = 25;
int rightPWMPin = 24;
int left1Pin = 23;
int left2Pin = 22;
int right1Pin = 21;
int right2Pin = 20;
int leftState; // Phase space -50,50
int rightState; // Phase space -50,50
int timeScale = 10000; // how long the longest move will take (ms)
// observations: motors turn at 12 rpm, wheel is .5 in diameter, we have roughly 6 inches of total travel
// Note that this controls our total range of motion since our phase space is fixed size; the most distance we can cover in a single movement is runDuration * max PWM strength

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);
  pinMode(leftPWMPin, OUTPUT);
  pinMode(rightPWMPin, OUTPUT);
  pinMode(left1Pin, OUTPUT);
  pinMode(left2Pin, OUTPUT);
  pinMode(right1Pin, OUTPUT);
  pinMode(right2Pin, OUTPUT);

  digitalWrite(left1Pin, HIGH);
  digitalWrite(left2Pin, LOW);
  digitalWrite(right1Pin, HIGH);
  digitalWrite(right2Pin, LOW);

  leftState = 0;
  rightState = 0;
  // Initial test run of the extrema
  runMotors(-50, -50, 100);
  runMotors(-50, 50, 100);
  runMotors(50, 50, 100);
  runMotors(50, -50, 100);

  runMotors(0, 0, 100);
  blink(200);
  blink(200);
  blink(200);
  blink(200);
  blink(200);
  // Seed RNG
  randomSeed(analogRead(0));
}

void loop() {
  // 1. Randomly roll a new state
  int newPitch = random(20, 51) * (2 * random(0, 2)) - 1;
  // and a deviation amount
  int deviation = random(-10, 11);

  // convert to phase space
  int newLeftState = newPitch;
  int newRightState = newPitch + deviation;

  // checks to avoid self destruction
  newRightState = min(newRightState, 50);
  newLeftState = min(newLeftState, 50);
  newRightState = max(newRightState, -50);
  newLeftState = max(newLeftState, -50);

  int restDuration = 1000 * random(1, 3);
  runMotors(newLeftState, newRightState, restDuration);
}

void blink(int gap) {
  digitalWrite(ledPin, HIGH);
  delay(gap);
  digitalWrite(ledPin, LOW);
  delay(gap);
}

void runMotors(int newLeftState, int newRightState, int restDuration) {
  // 2. Compute how much we have to adjust left and right for (in phase space)
  int leftDisplacement = newLeftState - leftState; // # from -100 to 100
  int rightDisplacement = newRightState - rightState; // # from -100 to -100
  int leftDispAbs = abs(leftDisplacement);
  int rightDispAbs = abs(rightDisplacement);

  leftState = newLeftState;
  rightState = newRightState;
  // 3. Convert to PWM strength, duration, and set directions
  double leftPWM = 0;
  double rightPWM = 0;
  if (leftDispAbs > rightDispAbs) {
    leftPWM = 255;
    rightPWM = 255 * ((double)rightDispAbs / leftDispAbs);
  } else {
    leftPWM = 255 * ((double)leftDispAbs / rightDispAbs);
    rightPWM = 255;
  }
  double runDuration = max(leftDispAbs, rightDispAbs) * ((double)timeScale / 100);

  if (leftDisplacement > 0) {
    digitalWrite(left1Pin, HIGH);
  } else {
    digitalWrite(left2Pin, HIGH);
  }
  if (rightDisplacement > 0) {
    digitalWrite(right1Pin, HIGH);
  } else {
    digitalWrite(right2Pin, HIGH);
  }
  // 4. Run
  analogWrite(leftPWMPin, (int)leftPWM);
  analogWrite(rightPWMPin, (int)rightPWM);
  digitalWrite(ledPin, HIGH);
  delay((int)runDuration);
  analogWrite(leftPWMPin, 0);
  analogWrite(rightPWMPin, 0);
  // 5. Rest and clear state
  digitalWrite(left1Pin, LOW);
  digitalWrite(left2Pin, LOW);
  digitalWrite(right1Pin, LOW);
  digitalWrite(right2Pin, LOW);
  digitalWrite(ledPin, LOW);
  delay(restDuration);
}

