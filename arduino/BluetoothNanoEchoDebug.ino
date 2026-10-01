#include <SoftwareSerial.h>

constexpr long BAUD_RATE = 38400;
constexpr uint8_t BT_RX_PIN = 2;
constexpr uint8_t BT_TX_PIN = 3;

SoftwareSerial bluetoothSerial(BT_RX_PIN, BT_TX_PIN);

void setup()
{
    Serial.begin(BAUD_RATE);
    bluetoothSerial.begin(BAUD_RATE);

    delay(500);
    Serial.println(F("USB ready. Type here to send to Bluetooth."));
    Serial.println(F("Bluetooth pins: HC-05/HC-06 TXD -> D2, RXD -> D3 through voltage divider."));
    bluetoothSerial.println(F("Bluetooth ready. Type here to send to USB."));
}

void loop()
{
    while (Serial.available() > 0)
    {
        char value = static_cast<char>(Serial.read());
        bluetoothSerial.write(value);
        Serial.write(value);
    }

    while (bluetoothSerial.available() > 0)
    {
        char value = static_cast<char>(bluetoothSerial.read());
        Serial.write(value);
        bluetoothSerial.write(value);
    }
}
