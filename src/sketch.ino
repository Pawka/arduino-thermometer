#include <dht11.h>
#include <EEPROM.h>

dht11 DHT11;

int EEPROM_ADDR = 0; //Current EEPROM address for write.
int EEPROM_END_VALUE = 255; //Describes end of data sequence.
int EEPROM_SIZE = 1024;
int DELAY_MINS = 10;

int LED_PIN = 13; //Info pin id.
int BUTTON_RESET_PIN = 8;
int BUTTON_RUN_PIN = 9;

bool RUN = false; //Stop monitoring by default.
bool DEBUG = true;

int resetStatus = 0; //Button status
int runStatus = 0; //Button status
long int _loop = 0;

void setup()
{
    DHT11.attach(2);
    Serial.begin(9600);
    EEPROM_ADDR = getDataSize();

    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_RESET_PIN, INPUT);
    pinMode(BUTTON_RUN_PIN, INPUT);
}

void loop()
{
    handleResetButton();
    handleToggleButton();

    if (true == RUN) {
        digitalWrite(LED_PIN, LOW);

        if (_loop >= 60 * DELAY_MINS) {
            int temp = getTemperature();
            storeTemperature(temp);
            _loop = 0;
        }
    } else {
        //Turn on LED when stopped.
        digitalWrite(LED_PIN, HIGH);
    }
    delay(1000);
    _loop++;
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
        EEPROM.write(EEPROM_ADDR, value);
        EEPROM_ADDR++;

        EEPROM.write(EEPROM_ADDR, EEPROM_END_VALUE);
    } else {
        RUN = false;
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

    if (DEBUG) {
        Serial.println("> RESET");
    }
}

/**
 * Write EEPROM data to serial port.
 */
void commandReadData()
{
    if (DEBUG) {
        Serial.println("> READ");
        Serial.print("Length: ");
        Serial.println(EEPROM_ADDR);
    }
    for (int i = 0; i <= getDataSize(); i++) {
        Serial.println(EEPROM.read(i));
    }
}

/**
 * Start/stop temperature monitoring.
 */
void commandToggleMonitoring()
{
    if (DEBUG) {
        Serial.println("> TOGGLE RUN");
    }
    if (RUN == true) {
        RUN = false;
    } else {
        RUN = true;
    }
}

/**
 * Handle reset button
 */
void handleResetButton() {
    int status = digitalRead(BUTTON_RESET_PIN);
    if (status != resetStatus) {
        resetStatus = status;

        if (status == 1) {
            commandReset();
        }
    }
}

/**
 * Handle toggle run button
 */
void handleToggleButton() {
    int status = digitalRead(BUTTON_RUN_PIN);
    if (status != runStatus) {
        runStatus = status;

        if (status == 1) {
            commandToggleMonitoring();
        }
    }
}
