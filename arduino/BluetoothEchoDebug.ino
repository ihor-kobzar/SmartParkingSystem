// Legacy temporary HC-05 Bluetooth echo/debug sketch for Uno/Nano-style SoftwareSerial tests.
//
// This is NOT the wiring used by the current Arduino Mega controller firmware.
// Current Mega firmware uses Serial1:
//   HC-05 TXD -> Arduino Mega D19 RX1
//   HC-05 RXD -> Arduino Mega D18 TX1, preferably through a voltage divider
//
// Legacy SoftwareSerial wiring used only by this diagnostic sketch:
//   HC-05 TXD -> Arduino D2
//   HC-05 RXD -> Arduino D3
//   HC-05 GND -> Arduino GND
//   HC-05 VCC -> 5V
//
// Open Arduino IDE Serial Monitor at 9600 baud.
// Send text from the Bluetooth COM/terminal.
// Every received Bluetooth byte is printed here.
//
// This sketch cycles through common HC-05 baud rates and both likely pin
// directions:
//   normal:  HC-05 TXD -> D2, HC-05 RXD -> D3
//   swapped: HC-05 TXD -> D3, HC-05 RXD -> D2
//
// You can also type text in Arduino Serial Monitor; it will be forwarded to HC-05.

#include <SoftwareSerial.h>
#include <ctype.h>

constexpr long USB_BAUD_RATE = 9600;
constexpr unsigned long PROFILE_DURATION_MS = 7000;

SoftwareSerial normalBtSerial(2, 3);
SoftwareSerial swappedBtSerial(3, 2);

struct BluetoothProfile
{
    const char *name;
    SoftwareSerial *serial;
    long baudRate;
};

BluetoothProfile profiles[] = {
    {"normal D2=RX D3=TX, 9600 baud", &normalBtSerial, 9600},
    {"normal D2=RX D3=TX, 38400 baud", &normalBtSerial, 38400},
    {"normal D2=RX D3=TX, 19200 baud", &normalBtSerial, 19200},
    {"normal D2=RX D3=TX, 57600 baud", &normalBtSerial, 57600},
    {"swapped D3=RX D2=TX, 9600 baud", &swappedBtSerial, 9600},
    {"swapped D3=RX D2=TX, 38400 baud", &swappedBtSerial, 38400},
    {"swapped D3=RX D2=TX, 19200 baud", &swappedBtSerial, 19200},
    {"swapped D3=RX D2=TX, 57600 baud", &swappedBtSerial, 57600}
};

constexpr uint8_t PROFILE_COUNT = sizeof(profiles) / sizeof(profiles[0]);

unsigned long lastHeartbeatAt = 0;
unsigned long currentProfileStartedAt = 0;
uint8_t currentProfileIndex = 0;

void printByte(const char *source, uint8_t value)
{
    Serial.print(source);
    Serial.print(" byte: 0x");
    if (value < 16)
    {
        Serial.print('0');
    }
    Serial.print(value, HEX);
    Serial.print(" '");

    if (value == '\n')
    {
        Serial.print("\\n");
    }
    else if (value == '\r')
    {
        Serial.print("\\r");
    }
    else if (isprint(value))
    {
        Serial.print(static_cast<char>(value));
    }
    else
    {
        Serial.print('?');
    }

    Serial.println("'");
}

BluetoothProfile &currentProfile()
{
    return profiles[currentProfileIndex];
}

void startCurrentProfile()
{
    normalBtSerial.end();
    swappedBtSerial.end();
    delay(50);

    currentProfile().serial->begin(currentProfile().baudRate);
    currentProfile().serial->listen();
    currentProfileStartedAt = millis();

    Serial.println();
    Serial.print("Listening: ");
    Serial.println(currentProfile().name);
}

void switchToNextProfile()
{
    currentProfileIndex = (currentProfileIndex + 1) % PROFILE_COUNT;
    startCurrentProfile();
}

void setup()
{
    Serial.begin(USB_BAUD_RATE);

    Serial.println("HC-05 Bluetooth echo debug");
    Serial.println("USB Serial: 9600 baud");
    Serial.println("Cycling pins/baud. Keep sending HELLO SPS from the phone.");
    Serial.println("Send text over Bluetooth. Received bytes will appear below.");
    Serial.println("Type in this Serial Monitor to forward bytes to Bluetooth.");

    startCurrentProfile();
}

void loop()
{
    if (millis() - currentProfileStartedAt >= PROFILE_DURATION_MS)
    {
        switchToNextProfile();
    }

    SoftwareSerial *activeSerial = currentProfile().serial;

    while (activeSerial->available() > 0)
    {
        uint8_t value = static_cast<uint8_t>(activeSerial->read());
        printByte("BT -> Arduino", value);
        activeSerial->write(value);
    }

    while (Serial.available() > 0)
    {
        uint8_t value = static_cast<uint8_t>(Serial.read());
        printByte("USB -> BT", value);
        activeSerial->write(value);
    }

    if (millis() - lastHeartbeatAt >= 3000)
    {
        lastHeartbeatAt = millis();
        Serial.print("waiting for Bluetooth bytes on ");
        Serial.println(currentProfile().name);
    }
}
