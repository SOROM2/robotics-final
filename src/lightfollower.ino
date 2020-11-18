#include <ZumoShield.h>
#include <NewPing.h>

#define SONAR_RANGE 200
#define SONAR_TRIG 2
#define SONAR_ECHO 3
#define SONAR_THRESHOLD 23

#define LIGHT_L A1
#define LIGHT_R A0

#define DRIVE_SPEED 280
#define DODGE_DELAY 400
#define REVERSE_THRESHOLD 8

NewPing sonar(SONAR_TRIG, SONAR_ECHO, SONAR_RANGE);

Pushbutton button(ZUMO_BUTTON);
ZumoMotors motors;

// light l and r speeds
int l_speed;
int r_speed;
int dodge_delay;

// PD light variables
int last_light_error;
const float kp_light = 0.8f;
const float kd_light = 1.0f;

// PD ping variables
int last_ping_error;
const float kp_ping = 0.6f;
const float kd_ping = 1.0f;

// states
enum class ZumoState {
    DRIVING,
    DODGING
} state = ZumoState::DRIVING;

void setup()
{
    Serial.begin(9600);

    button.waitForButton(); // wait for zumo button press
}

void loop()
{
    // reset light speed
    l_speed = DRIVE_SPEED;
    r_speed = DRIVE_SPEED;
    dodge_delay = DODGE_DELAY;

    int ping = sonar.ping_cm();         // get distance from sonar

    // read light readings from photoresistor pair
    int light_l = analogRead(LIGHT_L);
    int light_r = analogRead(LIGHT_R);


    // change state
    if (ping > SONAR_THRESHOLD || ping == 0) {
        state = ZumoState::DRIVING;
    } else {
        state = ZumoState::DODGING;
    }

    // perform state action
    switch (state) {
        case ZumoState::DODGING:
            if (ping < REVERSE_THRESHOLD) {
                l_speed = 0-200;
                r_speed = 0-200;
                dodge_delay = DODGE_DELAY * 2;
            } else {
                PD_sonarvec(ping, light_l - light_r);
            }
            break;
        case ZumoState::DRIVING:
            PD_lightvec(light_l, light_r);
            break;
    }

    // print l and r speed
    Serial.print(state == ZumoState::DRIVING ? "DRIVING" : "DODGING");
    Serial.print(" ");
    Serial.print(l_speed);
    Serial.print(" ");
    Serial.print(r_speed);
    Serial.print(" ");
    Serial.println(ping);


    // set motor speeds
    motors.setSpeeds(l_speed, r_speed);

    if (state == ZumoState::DODGING) {
        delay(dodge_delay);
    }
}

/**
 * offsets global l_speed and r_speed based on the orientation of the light to the photoresistors
 */
void PD_lightvec(int light_l, int light_r)
{
    int error = light_l - light_r;
    int speed_diff = (error / kp_light) + kd_light * (error - last_light_error);
    last_light_error = error;
    l_speed -= speed_diff;
    r_speed += speed_diff;
}

/**
 * offsets global l and r speed based on the ping input values
 */
void PD_sonarvec(int ping, int light_diff)
{
    int error = SONAR_RANGE - ping;
    int speed_diff = (error / kp_ping) + kd_ping * (error - last_ping_error);
    last_ping_error = error;
    if (light_diff < 0) {
        l_speed -= speed_diff;
        r_speed += speed_diff;
    } else {
        l_speed += speed_diff;
        r_speed -= speed_diff;
    }
}
