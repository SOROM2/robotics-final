#include <ZumoShield.h>
#include <NewPing.h>

#define SONAR_RANGE 200
#define SONAR_TRIG 2
#define SONAR_ECHO 3

#define LIGHT_L A1
#define LIGHT_R A0

NewPing sonar(SONAR_TRIG, SONAR_ECHO, SONAR_RANGE);

Pushbutton button(ZUMO_BUTTON);
ZumoMotors motors;

// motor l and r speeds
int l_speed;
int r_speed;

// PD variables
int last_error;
const float kp = 0.4f;
const float kd = 6.0f;

void setup()
{
    Serial.begin(9600);

    button.waitForButton(); // wait for zumo button press
}

void loop()
{
    // reset motor speed
    l_speed = 0;
    r_speed = 0;

    int ping = sonar.ping_cm();         // get distance from sonar

    // read light readings from photoresistor pair
    int light_l = analogRead(LIGHT_L);
    int light_r = analogRead(LIGHT_R);

    // use a PD controller to apply a vector to the motor to center the brightest light
    PD_motorvec(light_l, light_r);

    // print l and r speed
    Serial.print(l_speed);
    Serial.print(" ");
    Serial.println(r_speed);

    // drive here
    // check for sonar collision
    // if will collide with something
        // override photoresistor PD controller with sonar controller

    // set motor speeds
    motors.setSpeeds(l_speed, r_speed);
}

/**
 * offsets global l_speed and r_speed based on the orientation of the light to the photoresistors
 */
void PD_motorvec(int light_l, int light_r)
{
    int error = light_l - light_r;
    int speed_diff = (error / kp) + kd * (error - last_error);
    last_error = error;
    l_speed -= speed_diff;
    r_speed += speed_diff;
}

