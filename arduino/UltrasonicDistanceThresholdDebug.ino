// Temporary HC-SR04 diagnostics sketch.
// Prints live distances for parking, entry, and exit sensors.

constexpr unsigned long SERIAL_BAUD_RATE = 9600;
constexpr unsigned long REFRESH_INTERVAL_MS = 500;
constexpr uint16_t MAX_VALID_DISTANCE_CM = 400;

constexpr uint8_t SENSOR_COUNT = 5;
const char *sensorNames[SENSOR_COUNT] = {
    "P1 slot",
    "P2 slot",
    "P3 slot",
    "Exit/gate passage",
    "Entry/front access"
};
const uint8_t trigPins[SENSOR_COUNT] = {7, 4, A0, A2, 25};
const uint8_t echoPins[SENSOR_COUNT] = {8, 6, A1, A3, 26};

long readDistanceCm(uint8_t trigPin, uint8_t echoPin)
{
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH, 25000);
    if (duration == 0)
    {
        return -1;
    }

    long distanceCm = duration * 0.0343 / 2;
    if (distanceCm < 0 || distanceCm > MAX_VALID_DISTANCE_CM)
    {
        return -1;
    }

    return distanceCm;
}

void clearConsole()
{
    Serial.write(27);       // ESC
    Serial.print("[2J");    // clear screen
    Serial.write(27);       // ESC
    Serial.print("[H");     // cursor home
}

void printSensorLine(uint8_t index, long distanceCm)
{
    Serial.print(sensorNames[index]);
    Serial.print(": ");

    if (distanceCm == -1)
    {
        Serial.print("N/A");
    }
    else
    {
        Serial.print(distanceCm);
        Serial.print(" cm");
    }
    Serial.println();
}

void setup()
{
    Serial.begin(SERIAL_BAUD_RATE);

    for (uint8_t i = 0; i < SENSOR_COUNT; i++)
    {
        pinMode(trigPins[i], OUTPUT);
        pinMode(echoPins[i], INPUT);
        digitalWrite(trigPins[i], LOW);
    }
}

void loop()
{
    clearConsole();

    Serial.println("Smart Parking ultrasonic debug");
    Serial.println("Distances");
    Serial.println();

    for (uint8_t i = 0; i < SENSOR_COUNT; i++)
    {
        long distanceCm = readDistanceCm(trigPins[i], echoPins[i]);
        printSensorLine(i, distanceCm);
        delay(60);
    }

    delay(REFRESH_INTERVAL_MS);
}
