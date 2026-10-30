// ESP32-C3 Zero: 2-motor crawler gait (BEAM style), full power short strokes
// Diagonal crawl, repeating:
//   1. FRONT motor, direction 1   (front right foot down)
//   2. BACK  motor, direction 1   (back left foot kick)
//   3. FRONT motor, direction 2   (front left foot down)
//   4. BACK  motor, direction 2   (back right foot kick)
// Each step starts OVERLAP_MS before the previous one ends.
// Non-blocking: speeds are computed from time, so there are no delay() stalls.
//
// Strategy: run at full duty (255) for a short stroke, then pause.
// Full torque on every stroke; PAUSE_MS sets how slow the gait feels.
// At 45 rpm (270 deg/s), a 100 ms stroke at 255 is roughly 27 degrees of output shaft.
//
// Requires ESP32 Arduino core 3.x (ledcAttach/ledcWrite take a pin)
//
// Driver: two RZ7888 chips soldered freeform (one per motor), or one DRV8833 board.
//   Both take two PWM inputs per motor; both inputs low = coast.
//   DRV8833: FRONT_A/B -> AIN1/AIN2, BACK_A/B -> BIN1/BIN2.
//
// Wiring (strapping pins GPIO2/8/9 are NOT used):
//   FRONT motor: IN A -> GPIO4, IN B -> GPIO5
//   BACK  motor: IN A -> GPIO6, IN B -> GPIO7

const int FRONT_A = 4;
const int FRONT_B = 5;
const int BACK_A  = 6;
const int BACK_B  = 7;

// ---- Tunables ----
const int SPEED      = 105;  // 0-255, motor speed (PWM duty). 255 = full power
const int STEP_MS    = 200;  // stroke length: how long each motor runs per step
                             // longer = bigger stroke angle (~0.27 deg/ms at 45 rpm)
const int PAUSE_MS   = 200;  // gap added between step starts: sets the cadence
const int OVERLAP_MS = 30;   // next step starts this long before the current one ends
                             // keep <= STEP_MS / 2 so the same motor never overlaps itself
const int RAMP_MS    = 40;   // soft-start time per step; 0 = jump straight to SPEED

const int PWM_FREQ = 20000;  // 20 kHz, above audible range
const int PWM_RES  = 8;      // 8-bit: 0-255

// ---- Gait table ----
const int NUM_STEPS = 4;
const bool STEP_IS_FRONT[NUM_STEPS] = { true, false, true, false };
const int  STEP_DIR[NUM_STEPS]      = {    1,     1,   -1,    -1 };

// Time between the start of one step and the start of the next
const long STEP_PERIOD = STEP_MS - OVERLAP_MS + PAUSE_MS;
const long CYCLE_MS    = NUM_STEPS * STEP_PERIOD;

// speed: -255 (direction 2) to +255 (direction 1), 0 = coast
void setMotor(int pinA, int pinB, int speed) {
  speed = constrain(speed, -255, 255);
  if (speed > 0) {
    ledcWrite(pinA, speed);
    ledcWrite(pinB, 0);
  } else if (speed < 0) {
    ledcWrite(pinA, 0);
    ledcWrite(pinB, -speed);
  } else {
    ledcWrite(pinA, 0);
    ledcWrite(pinB, 0);
  }
}

void setFront(int speed) { setMotor(FRONT_A, FRONT_B, speed); }
void setBack(int speed)  { setMotor(BACK_A,  BACK_B,  speed); }

// Time since step i started, or -1 if step i is not running at cycle time t.
// Checks t and t + CYCLE_MS so a step can run past the end of the cycle.
long stepElapsed(int i, long t) {
  long start = (long)i * STEP_PERIOD;
  long e = t - start;
  if (e >= 0 && e < STEP_MS) return e;
  e = t + CYCLE_MS - start;
  if (e >= 0 && e < STEP_MS) return e;
  return -1;
}

void setup() {
  ledcAttach(FRONT_A, PWM_FREQ, PWM_RES);
  ledcAttach(FRONT_B, PWM_FREQ, PWM_RES);
  ledcAttach(BACK_A,  PWM_FREQ, PWM_RES);
  ledcAttach(BACK_B,  PWM_FREQ, PWM_RES);
  setFront(0);
  setBack(0);
  delay(1000);  // short settle time after power-up
}

void loop() {
  long t = (long)(millis() % (unsigned long)CYCLE_MS);
  int frontSpeed = 0;
  int backSpeed  = 0;

  for (int i = 0; i < NUM_STEPS; i++) {
    long e = stepElapsed(i, t);
    if (e < 0) continue;

    int spd = SPEED;
    if (RAMP_MS > 0 && e < RAMP_MS) spd = (int)(SPEED * e / RAMP_MS);
    spd *= STEP_DIR[i];

    if (STEP_IS_FRONT[i]) frontSpeed = spd;
    else                  backSpeed  = spd;
  }

  setFront(frontSpeed);
  setBack(backSpeed);
  delay(2);
}
