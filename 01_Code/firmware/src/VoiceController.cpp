// #include "DFrobot_MSM261.h"
// #include "SPI.h"
// #include <SD.h>
// #include <FS.h>
// #include <WiFi.h>
// #include <HTTPClient.h>
// #include <ArduinoJson.h>
// #include <base64.h>

// // Wi-Fi credentials
// const char* ssid = "";
// const char* password = "";
// const char* apiKey = "";  // Replace with your key

// // I2S and recording definitions
// #define SAMPLE_RATE     8000
// #define I2S_SCK_IO      25
// #define I2S_WS_IO       16
// #define I2S_DI_IO       26
// #define DATA_BIT        16
// #define MODE_PIN        4
// #define RECORD_SECONDS  5
// #define BUFFER_SIZE     256

// const char filename[] = "/sound.wav";

// DFRobot_Microphone mic(I2S_SCK_IO, I2S_WS_IO, I2S_DI_IO);
// File file;
// uint8_t buffer[BUFFER_SIZE];

// const int headerSize = 44;
// byte header[headerSize];

// void connectWiFi() {
//   Serial.print("Connecting to Wi-Fi...");
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\nConnected!");
// }

// String sendToGoogleSTT(const char* wavFilePath) {
//   File wav = SD.open(wavFilePath, FILE_READ);
//   if (!wav) {
//     Serial.println("Failed to open WAV file");
//     return "";
//   }

//   // Skip WAV header
//   wav.seek(44);  // skip 44-byte WAV header
//   size_t wavSize = wav.size() - 44;
//   uint8_t* wavData = new uint8_t[wavSize];
//   wav.read(wavData, wavSize);
//   wav.close();

//   // Encode WAV data to Base64 using standard Arduino base64
//   String enc = base64::encode(wavData, wavSize);
//   enc.replace("\n", "");
//   delete[] wavData;

//   Serial.printf("Base64 length: %d\n", enc.length());

//   String jsonRequest = "{";
//   jsonRequest += "\"config\":{";
//   jsonRequest +=   "\"encoding\":\"LINEAR16\",";
//   jsonRequest +=   "\"sampleRateHertz\":" + String(SAMPLE_RATE) + ",";
//   jsonRequest +=   "\"languageCode\":\"en-US\"";
//   jsonRequest += "},";
//   jsonRequest += "\"audio\":{";
//   jsonRequest +=   "\"content\":\"" + enc + "\"";
//   jsonRequest += "}";
//   jsonRequest += "}";

//   Serial.println(enc);        // Verify it's not empty
//   Serial.println(jsonRequest); // Final JSON payload

//   // Send HTTP POST request to Google Speech-to-Text
//   HTTPClient http;
//   String url = String("https://speech.googleapis.com/v1/speech:recognize?key=") + apiKey;
//   http.begin(url);
//   http.addHeader("Content-Type", "application/json");

//   int httpResponseCode = http.POST(jsonRequest);
//   String response = "";
//   if (httpResponseCode > 0) {
//     response = http.getString();
//   } else {
//     Serial.printf("HTTP POST failed: %d\n", httpResponseCode);
//   }
//   http.end();
//   return response;
// }

// void setup() {
//   Serial.begin(115200);
//   pinMode(MODE_PIN, OUTPUT);
//   digitalWrite(MODE_PIN, LOW);

//   connectWiFi();

//   // Initialize microphone
//   while (mic.begin(SAMPLE_RATE, DATA_BIT) != 0) {
//     Serial.println("I2S init failed");
//     delay(500);
//   }
//   Serial.println("I2S init success");

//   // Initialize SD card
//   if (!SD.begin(2)) {
//     Serial.println("SD begin failed");
//     while (1);
//   }

//   SD.remove(filename);
//   file = SD.open(filename, FILE_WRITE);
//   if (!file) {
//     Serial.println("Failed to create file");
//     while (1);
//   }

//   int numChannels = 1;
//   int bytesPerSample = DATA_BIT / 8;
//   int totalSamples = SAMPLE_RATE * RECORD_SECONDS;
//   int dataSize = totalSamples * bytesPerSample;
//   int byteRate = SAMPLE_RATE * numChannels * bytesPerSample;

//   mic.createWavHeader(header, dataSize, SAMPLE_RATE, numChannels, byteRate, bytesPerSample);
//   file.write(header, headerSize);

//   Serial.println("Recording...");
//   int bytesWritten = 0;
//   while (bytesWritten < dataSize) {
//     int bytesRead = mic.read((char*)buffer, BUFFER_SIZE);
//     for (int i = 0; i < bytesRead; i += 4) {
//       if (bytesWritten < dataSize) {
//         file.write(buffer + i, 2);
//         bytesWritten += 2;
//       }
//     }
//   }
//   file.close();
//   Serial.println("Recording finished!");

//   // Send recording to Google Speech-to-Text
//   Serial.println("Sending to Google Speech-to-Text...");
//   String sttResponse = sendToGoogleSTT(filename);
//   Serial.println("Response:");
//   Serial.println(sttResponse);
// }

// void loop() {
//   // Nothing here
// }
