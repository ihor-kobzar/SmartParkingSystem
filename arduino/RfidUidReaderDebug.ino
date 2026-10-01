#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t SS_PIN = 10;
constexpr uint8_t RST_PIN = 9;

MFRC522 rfid(SS_PIN, RST_PIN);

void setup()
{
    Serial.begin(9600);
    while (!Serial)
    {
        delay(10);
    }

    SPI.begin();
    rfid.PCD_Init();

    Serial.println(F("MFRC522 RFID UID reader"));
    Serial.println(F("Pins: SDA/SS=D10, RST=D9, MOSI=D51, MISO=D50, SCK=D52 on Arduino Mega"));
    Serial.println(F("Power MFRC522 from 3.3V only."));
    Serial.println(F("Bring a card or tag close to the reader."));
    Serial.println();
}

void loop()
{
    if (!rfid.PICC_IsNewCardPresent())
    {
        return;
    }

    if (!rfid.PICC_ReadCardSerial())
    {
        return;
    }

    Serial.print(F("UID="));
    printUidCompact();
    Serial.print(F(" bytes="));
    printUidSpaced();
    Serial.print(F(" size="));
    Serial.print(rfid.uid.size);
    Serial.print(F(" type="));
    Serial.println(rfid.PICC_GetTypeName(rfid.PICC_GetType(rfid.uid.sak)));

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    delay(750);
}

void printUidCompact()
{
    for (byte index = 0; index < rfid.uid.size; index++)
    {
        printHexByte(rfid.uid.uidByte[index]);
    }
}

void printUidSpaced()
{
    for (byte index = 0; index < rfid.uid.size; index++)
    {
        if (index > 0)
        {
            Serial.print(' ');
        }

        printHexByte(rfid.uid.uidByte[index]);
    }
}

void printHexByte(byte value)
{
    if (value < 0x10)
    {
        Serial.print('0');
    }

    Serial.print(value, HEX);
}
