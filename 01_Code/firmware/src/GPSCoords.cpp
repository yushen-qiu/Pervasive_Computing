#include <Arduino.h>

// UART2: GPS.TX -> GPIO16 (RX2), GPS.RX -> GPIO17 (TX2, optional)
HardwareSerial GPS(2);
static const int PIN_RX2 = 16; // ESP32 receive（with GPS  TX）
static const int PIN_TX2 = 17; // ESP32 sending（with GPS  RX, optional）

// NEO series rate
int candidates[] = {9600, 38400, 57600, 115200, 4800};
int currentBaud = -1;

void startGPS(int baud)
{
    GPS.end();
    delay(30);
    GPS.begin(baud, SERIAL_8N1, PIN_RX2, PIN_TX2);
    currentBaud = baud;
    Serial.printf("\n[Probe] Trying GPS baud = %d ...\n", baud);
}

// return if detect NMEA start with '$'
bool hasNMEA(unsigned long ms = 1200)
{
    unsigned long t0 = millis();
    while (millis() - t0 < ms)
    {
        while (GPS.available())
        {
            char c = GPS.read();
            if (c == '$')
                return true;
        }

        delay(1);
    }
    return false;
}

// keep searching if not locked
void lockBaud()
{
    while (true)
    {
        for (size_t i = 0; i < sizeof(candidates) / sizeof(candidates[0]); ++i)
        {
            startGPS(candidates[i]);
            if (hasNMEA())
            {
                Serial.printf("[OK] NMEA detected at %d baud.\n", currentBaud);
                Serial.println("-------- NMEA stream (lines) --------");
                return;
            }
        }
        Serial.println("[Retry] No NMEA yet. Re-trying in 1s...");
        delay(1000);
    }
}

void setup()
{
    Serial.begin(115200); // USB testing port（115200）
    Serial.println("\nAuto-bauding GPS on UART2 (RX2=GPIO16, TX2=GPIO17)...");
    lockBaud();
}

void loop()
{
    static String line;
    static unsigned long lastDollarMs = millis();

    //  GPS data show up
    while (GPS.available())
    {
        char c = GPS.read();
        if (c == '$')
            lastDollarMs = millis(); // record time
        if (c == '\r')
            continue; // filter CR
        if (c == '\n')
        {
            if (line.length())
            {
                Serial.println(line);
                line = "";
            }
        }
        else
        {
            line += c;
            if (line.length() > 120)
            {
                Serial.println(line);
                line = "";
            }
        }
    }

    // over 5 sec not meet '$'，Indicates serial port silence or baud rate mismatch; please retry detection.
    if (millis() - lastDollarMs > 5000)
    {
        Serial.println("\n[Lost] No NMEA for 5s. Re-probing baud...");
        lockBaud();
        lastDollarMs = millis();
    }
}
