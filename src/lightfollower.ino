#include <NewPing.h>

#define SONAR_RANGE 200
#define SONAR_TRIG 2
#define SONAR_ECHO 3
#define LIGHT_L A1
#define LIGHT_R A0

NewPing sonar(SONAR_TRIG, SONAR_ECHO, SONAR_RANGE);

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    int ping = sonar.ping_cm();
    int light_l = analogRead(LIGHT_L);
    int light_r = analogRead(LIGHT_R);
    Serial.println(ping);
    Serial.print(light_l);
    Serial.print(" ");
    Serial.println(light_r);
    delay(200);
}
