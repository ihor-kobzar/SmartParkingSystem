// Temporary HC-05 Bluetooth echo/debug sketch for Arduino Mega 2560.
//
// IMPORTANT:
// On Arduino Mega, SoftwareSerial cannot receive on D2.
// Use hardware Serial1 instead:
//   HC-05 TXD -> Arduino Mega D19 RX1
//   HC-05 RXD -> Arduino Mega D18 TX1, preferably through a voltage divider
//   HC-05 GND -> Arduino GND
//   HC-05 VCC -> 5V
//
// Open Arduino IDE Serial Monitor at 9600 baud.
// Send text from the Bluetooth phone terminal.
// Every received Bluetooth byte is printed here and echoed back to the phone.

#include <ctype.h>

constexpr long USB_BAUD_RATE = 9600;
constexpr long BT_BAUD_RATE = 9600;
unsigned long lastHeartbeatAt = 0;

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

void setup()
{
    Serial.begin(USB_BAUD_RATE);
    Serial1.begin(BT_BAUD_RATE);

    Serial.println("HC-05 Mega Serial1 echo debug");
    Serial.println("USB Serial Monitor: 9600 baud");
    Serial.println("Bluetooth Serial1: D19 RX1, D18 TX1, 9600 baud");
    Serial.println("Send HELLO SPS from the phone.");
}

void loop()
{
    while (Serial1.available() > 0)
    {
        uint8_t value = static_cast<uint8_t>(Serial1.read());
        printByte("BT -> Mega", value);
        Serial1.write(value);
    }

    while (Serial.available() > 0)
    {
        uint8_t value = static_cast<uint8_t>(Serial.read());
        printByte("USB -> BT", value);
        Serial1.write(value);
    }

    if (millis() - lastHeartbeatAt >= 3000)
    {
        lastHeartbeatAt = millis();
        Serial.println("waiting for Bluetooth bytes on Serial1 D19/D18...");
    }
}
