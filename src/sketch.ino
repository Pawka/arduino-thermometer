#include <dht11.h>
#include <EEPROM.h>

dht11 DHT11;

int EEPROM_ADDR = 0; //Current EEPROM address for write.
int EEPROM_END_VALUE = 255; //Describes end of data sequence.
int EEPROM_SIZE = 10;
int PIN = 13; //Info pin id.

void setup()
{
    DHT11.attach(2);
    pinMode(PIN, OUTPUT);
    Serial.begin(9600);
    EEPROM_ADDR = getDataSize();
    Serial.println(EEPROM_ADDR);
}

void loop()
{
    int temp = getTemperature();
    storeTemperature(temp);
    delay(1000);
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
        } else if (input == '3') {
            commandToggleMonitoring();
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
    //Write until memory is full. Leave last byte for end value.
    if (EEPROM_ADDR < EEPROM_SIZE - 1) {
        digitalWrite(PIN, LOW);
        EEPROM.write(EEPROM_ADDR, value);
        EEPROM_ADDR++;

        EEPROM.write(EEPROM_ADDR, EEPROM_END_VALUE);
    } else {
        //Turn on pin when memory is full.
        digitalWrite(PIN, HIGH);
    }
}

/**
 * Look for data stored on EEPROM size. It is possible to write the value on
 * some EEPROM address but reading is chosen to avoid unnecesasry writes and
 * extend EEPROM lifetime.
 */
int getDataSize()
{
    for (int i = 0; i < EEPROM_SIZE; i++) {
        if (EEPROM.read(i) == EEPROM_END_VALUE) {
            return i - 1;
        }
    }

    return 0;
}

/**
 * Reset program status.
 */
void commandReset()
{
    EEPROM_ADDR = 0;

    //Write end value to start of memory.
    EEPROM.write(0, EEPROM_END_VALUE);
}

/**
 * Write EEPROM data to serial port.
 */
void commandReadData()
{
    Serial.print("Length: ");
    Serial.println(EEPROM_ADDR);
    for (int i = 0; i <= getDataSize(); i++) {
        Serial.println(EEPROM.read(i));
    }
}

/**
 * Start/stop temperature monitoring.
 */
void commandToggleMonitoring()
{
    //@todo implement.
}
