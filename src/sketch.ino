#include <dht11.h>

dht11 DHT11;

void setup()
{
    DHT11.attach(2);
    Serial.begin(9600);
}

void loop()
{
    Serial.println("\n");

    int chk = DHT11.read();

    Serial.print("Read sensor: ");
    switch (chk)
    {
        case 0: Serial.println("OK"); break;
        case -1: Serial.println("Checksum error"); break;
        case -2: Serial.println("Time out error"); break;
        default: Serial.println("Unknown error"); break;
    }

    Serial.print("Temperature (°C): ");
    Serial.println((float)DHT11.temperature, DEC);
    delay(2000);
}
