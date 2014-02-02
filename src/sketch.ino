#include <dht11.h>
#include <EEPROM.h>

dht11 DHT11;

int EEPROM_ADDR = 0; //Current EEPROM address for write.

int serialData = 0; //Data from serial.

void setup()
{
    DHT11.attach(2);
    Serial.begin(9600);
}

void loop()
{
    int temp = getTemperature();
    storeTemperature(temp);
    delay(2000);
}

/**
 * Event is triggered when data on Serial is available.
 */
void serialEvent()
{
    if (Serial.available()) {
        char input = (char)Serial.read();

        if (input == '1') {
            commandReset();
        } else if (input == '2') {
            commandReadData();
        }
    }
}

int getTemperature()
{
    int chk = DHT11.read();

    /*
    switch (chk)
    {
        case 0: Serial.println("OK"); break;
        case -1: Serial.println("Checksum error"); break;
        case -2: Serial.println("Time out error"); break;
        default: Serial.println("Unknown error"); break;
    }
    */

    return (int)DHT11.temperature;
}

void storeTemperature(int value)
{
    EEPROM.write(EEPROM_ADDR, value);
    EEPROM_ADDR++;
}

/**
 * Reset program status.
 */
void commandReset()
{
    EEPROM_ADDR = 0;
}

/**
 * Write EEPROM data to serial port.
 */
void commandReadData()
{
    Serial.print("Length: ");
    Serial.println(EEPROM_ADDR + 1);
    for (int i = 0; i <= EEPROM_ADDR; i++) {
        Serial.println(EEPROM.read(i));
    }
}

