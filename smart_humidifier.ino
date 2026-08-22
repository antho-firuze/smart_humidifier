/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

#define BLYNK_TEMPLATE_ID "TMPL6L3RFk1YW"
#define BLYNK_TEMPLATE_NAME "Quickstart Device"
#define BLYNK_AUTH_TOKEN "M7fW5gaAIE5_kDRPyCdVWcOHnhmnwqhR"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiManager.h> // Include WiFiManager library
#include <ArduinoOTA.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_AHT10.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "index_page.h"
#include "ota_page.h"

#define SDA_PIN 4 // D2
#define SCL_PIN 5 // D1

String deviceLocation = "";
String version = "1.0.9";

// Local DNS name for OTA updates =======
String localDNS = "smarthumidifier";
void initDNS()
{
    // Get the unique chip ID
    uint32_t chipId = ESP.getChipId();
    localDNS = localDNS + "-" + String(chipId, HEX);

    if (MDNS.begin(localDNS.c_str()))
    {
        Serial.println("Local DNS started: http://" + localDNS + ".local");
    }
    else
    {
        Serial.println("Error setting up MDNS responder!");
    }

    MDNS.addService("http", "tcp", 80);
}
// Local DNS name for OTA updates =======

// LittleFS =============================
const char *CONFIG_PATH = "/config.json";
// Fungsi untuk MENYIMPAN Key-Value
bool setKeyValue(const char *key, const char *value)
{
    StaticJsonDocument<512> doc; // Sesuaikan ukuran byte dengan kebutuhan data Anda

    // 1. Baca file yang sudah ada agar data lama tidak terhapus
    if (LittleFS.exists(CONFIG_PATH))
    {
        File file = LittleFS.open(CONFIG_PATH, "r");
        if (file)
        {
            deserializeJson(doc, file);
            file.close();
        }
    }

    // 2. Tambah atau update pasangan key-value
    doc[key] = value;

    // 3. Tulis kembali data terbaru ke dalam file (Mode "w" akan menimpa file lama)
    File file = LittleFS.open(CONFIG_PATH, "w");
    if (!file)
    {
        return false; // Gagal membuka file
    }

    // serializeJson(doc, file) langsung menulis ke file tanpa memakan banyak RAM
    if (serializeJson(doc, file) == 0)
    {
        file.close();
        return false; // Gagal menulis
    }

    file.close();
    Serial.println("Key-Value pair saved successfully.");
    return true; // Sukses
}
// Fungsi untuk MEMBACA Key-Value
String getKeyValue(const char *key, const char *defaultValue = "")
{
    if (!LittleFS.exists(CONFIG_PATH))
    {
        return defaultValue;
    }

    File file = LittleFS.open(CONFIG_PATH, "r");
    if (!file)
    {
        return defaultValue;
    }

    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error)
    {
        return defaultValue;
    }

    // Jika key tidak ditemukan, kembalikan nilai default
    if (!doc.containsKey(key))
    {
        return defaultValue;
    }

    return doc[key].as<String>();
}
void initLittleFS()
{
    if (!LittleFS.begin())
    {
        Serial.println("Failed to load LittleFS!");
        return;
    }
    Serial.println("LittleFS Successfully Loaded.");

    deviceLocation = getKeyValue("device_location", "");
}
// LittleFS =============================

// WIFI MANAGER =========================
String apName = "SmartHumidifier";
WiFiManager wifiManager;
bool isConnected = false;
unsigned long lastWiFiCheckTime = 0;
// unsigned long portalStartTime = 0;
const unsigned long TIMEOUT_MS = 60000; // 60 seconds timeout before restart
void initConnection()
{
    wifiManager.setConnectTimeout(15);

    // Enable non-blocking mode
    wifiManager.setConfigPortalBlocking(false);

    // Start the asynchronous connection attempt
    apName = apName + "-" + String(ESP.getChipId(), HEX);
    wifiManager.autoConnect(apName.c_str(), "");

    // Mark when we started trying to connect
    // portalStartTime = millis();
}
void checkConnection()
{
    if (!isConnected && millis() - lastWiFiCheckTime >= 1000)
    {
        lastWiFiCheckTime = millis();
        if (WiFi.status() == WL_CONNECTED && WiFi.localIP() == IPAddress(0, 0, 0, 0))
        {
            Serial.println("\n[ERROR] Connected but IP is 0.0.0.0! Restarting...");
            delay(2000);
            ESP.restart();
        }
        if (WiFi.status() == WL_CONNECTED && WiFi.localIP() != IPAddress(0, 0, 0, 0))
        {
            Serial.print("\n[SUCCESS] Connected! IP: ");
            Serial.println(WiFi.localIP());

            // Running Setup()
            Blynk.begin(BLYNK_AUTH_TOKEN, WiFi.SSID().c_str(), WiFi.psk().c_str());
            initWebserver();
            delay(1000);

            isConnected = true;
        }
        // if (WiFi.status() != WL_CONNECTED && millis() - portalStartTime >= TIMEOUT_MS)
        // {
        //     Serial.println("\n[TIMEOUT] Connection failed to establish. Restarting...");
        //     delay(2000);
        //     ESP.restart();
        // }
    }
}
// WIFI MANAGER =========================

// LED INDICATOR ========================
#define LED_INDICATOR_PIN 2 // D4 on NodeMCU, built-in LED
bool ledIndicator = false;
int ledStart = 0;
long ledDuration = 300; // 300ms
long ledDelay = 5000;   // 5s
void initLedIndicator()
{
    pinMode(LED_INDICATOR_PIN, OUTPUT);
    startLedIndicator();
}
void startLedIndicator()
{
    digitalWrite(LED_INDICATOR_PIN, LOW);
    ledIndicator = true;
    ledStart = millis();
}
void stopLedIndicator()
{
    digitalWrite(LED_INDICATOR_PIN, HIGH);
    ledIndicator = false;
    ledStart = millis();
}
void updateLedIndicator()
{
    if (!isConnected)
    {
        if (millis() - ledStart >= 200)
        {
            digitalWrite(LED_INDICATOR_PIN, !digitalRead(LED_INDICATOR_PIN));
            ledStart = millis();
        }
    }
    else
    {
        if (ledIndicator && millis() - ledStart >= ledDuration)
        {
            stopLedIndicator();
        }

        if (!ledIndicator && millis() - ledStart >= ledDelay)
        {
            startLedIndicator();
        }
    }
}
// LED INDICATOR ========================

// TEMP & HUM ===========================
Adafruit_AHT10 aht;
float temperature = 0.0;
float humidity = 0.0;
float offsetTemp = -1;
float offsetHum = -12.5;
unsigned long lastSensorReadTime = 0;
const unsigned long SENSOR_READ_INTERVAL = 5000; // Read sensors every 5 seconds
void initAHTSensor()
{
    if (!aht.begin(&Wire, 0x38))
    {
        Serial.println("Could not find AHT10 sensor!");
        // We don't freeze the program here so the counter can still work without the sensor
    }

    // Initial read to populate screen data immediately
    offsetTemp = getKeyValue("offsetTemp", "-1").toFloat();
    offsetHum = getKeyValue("offsetHum", "-12.5").toFloat();
    readAHTSensor();
}
void readAHTSensor()
{
    sensors_event_t humidityEvent, tempEvent;
    if (aht.getEvent(&humidityEvent, &tempEvent))
    {
        temperature = tempEvent.temperature + offsetTemp;
        humidity = humidityEvent.relative_humidity + offsetHum;

        Serial.print(F("Temperature: "));
        Serial.print(temperature);
        Serial.print(F(" °C Humidity: "));
        Serial.print(humidity);
        Serial.println(F(" %"));
    }
}
void updateAHTSensor()
{
    if (millis() - lastSensorReadTime >= SENSOR_READ_INTERVAL)
    {
        readAHTSensor();
        lastSensorReadTime = millis();
    }
}
// TEMP & HUM ===========================

// SPRAYING =============================
#define SPRAY_PIN 16 // D0 on NodeMCU
bool spraying = false;
bool isAutoSpray = true;
unsigned long minHumidityStart = 42;
unsigned long maxHumidityStop = 50;
void initSpray()
{
    pinMode(SPRAY_PIN, OUTPUT);
    isAutoSpray = getKeyValue("isAutoSpray", "1").toInt() == 1;
    minHumidityStart = getKeyValue("minHumidityStart", "42").toInt();
    maxHumidityStop = getKeyValue("maxHumidityStop", "50").toInt();
}
void startSpray()
{
    digitalWrite(SPRAY_PIN, HIGH);
    spraying = true;

    Serial.println("ACTION: Spraying...");
}
void stopSpray()
{
    digitalWrite(SPRAY_PIN, LOW);
    spraying = false;
}
void updateSpray()
{
    if (!isAutoSpray)
    {
        return; // Skip automatic spraying if auto mode is disabled
    }
    if (humidity < minHumidityStart && !spraying)
    {
        startSpray();
    }
    else if (humidity > maxHumidityStop && spraying)
    {
        stopSpray();
    }
}
// SPRAYING =============================

// OLED =================================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 OLED(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
unsigned long lastCheck = 0;
const unsigned long OLED_INTERVAL = 2000; // Baca & update data setiap 2000ms
float lastTemp = -1;
float lastHum = -1;
bool lastSpraying;
bool lastAutoSpray;
void displayOLED(std::vector<std::string> segment = {}, bool isfirst = true)
{
    OLED.clearDisplay();
    // Header Text
    OLED.setTextSize(1);
    OLED.setTextColor(SSD1306_WHITE);
    OLED.setCursor(0, 0);
    OLED.println("SMART HUMIDIFIER");
    OLED.drawLine(0, 10, 128, 10, SSD1306_WHITE); // Horizontal dividing line
    OLED.display();
    // --- 2. TEMPERATURE (Middle-Top) ---
    OLED.setTextSize(2);
    OLED.setCursor(0, 20);
    OLED.setTextColor(SSD1306_WHITE);
    OLED.print(temperature, 1); // "1" limits to one decimal place
    OLED.setTextSize(1);
    OLED.print(" C");
    // --- 3. HUMIDITY (Middle-Bottom) ---
    OLED.setTextSize(2);
    OLED.setCursor(65, 20);
    OLED.setTextColor(SSD1306_WHITE);
    OLED.print(humidity, 1); // "1" limits to one decimal place
    OLED.setTextSize(1);
    OLED.print(" %");
    OLED.drawLine(0, 50, 128, 50, SSD1306_WHITE); // Horizontal dividing line
    // Bottom Status Text
    OLED.setTextSize(1);
    OLED.setCursor(0, 55);
    OLED.setTextColor(SSD1306_WHITE);
    OLED.print(isAutoSpray ? "AUTO" : "MANUAL");
    // Bottom Status Text
    OLED.setTextSize(1);
    OLED.setCursor(55, 55);
    OLED.setTextColor(SSD1306_WHITE);
    OLED.print("| spray: " + String(spraying ? "ON" : "OFF"));
    OLED.display();
    Serial.println("OLED Updated: Temp=" + String(temperature, 1) + "C, Hum=" + String(humidity, 1) + "%, AutoSpray=" + String(isAutoSpray ? "ON" : "OFF") + ", Spraying=" + String(spraying ? "ON" : "OFF"));

    // if (isfirst)
    // {
    //     OLED.clearDisplay();
    //     // Header Text
    //     OLED.setTextSize(1);
    //     OLED.setTextColor(SSD1306_WHITE);
    //     OLED.setCursor(0, 0);
    //     OLED.println("SMART HUMIDIFIER");
    //     OLED.drawLine(0, 10, 128, 10, SSD1306_WHITE); // Horizontal dividing line
    //     OLED.drawLine(0, 50, 128, 50, SSD1306_WHITE);
    //     OLED.display();
    // }

    // if (segment.empty() || std::find(segment.begin(), segment.end(), "temp") != segment.end())
    // {
    //     OLED.fillRect(0, 20, 128, 30, SSD1306_BLACK); // Clear the area where temperature is displayed
    //     // --- 2. TEMPERATURE (Middle-Top) ---
    //     OLED.setTextSize(2);
    //     OLED.setCursor(0, 20);
    //     OLED.setTextColor(SSD1306_WHITE);
    //     OLED.print(temperature, 1); // "1" limits to one decimal place
    //     OLED.setTextSize(1);
    //     OLED.print(" C");
    // }
    //  if (segment.empty() || std::find(segment.begin(), segment.end(), "hum") != segment.end())
    // {
    //     OLED.fillRect(65, 20, 63, 30, SSD1306_BLACK); // Clear the area where humidity is displayed
    //     // --- 3. HUMIDITY (Middle-Bottom) ---
    //     OLED.setTextSize(2);
    //     OLED.setCursor(65, 20);
    //     OLED.setTextColor(SSD1306_WHITE);
    //     OLED.print(humidity, 1); // "1" limits to one decimal place
    //     OLED.setTextSize(1);
    //     OLED.print(" %");
    // }
    //  if (segment.empty() || std::find(segment.begin(), segment.end(), "isAutoSpray") != segment.end())
    // {
    //     OLED.fillRect(0, 55, 50, 14, SSD1306_BLACK);
    //     // Bottom Status Text
    //     OLED.setTextSize(1);
    //     OLED.setCursor(0, 55);
    //     OLED.setTextColor(SSD1306_WHITE);
    //     OLED.print(isAutoSpray ? "AUTO" : "MANUAL");
    // }
    // if (segment.empty() || std::find(segment.begin(), segment.end(), "spraying") != segment.end())
    // {
    //     OLED.fillRect(55, 55, 70, 14, SSD1306_BLACK);
    //     // Bottom Status Text
    //     OLED.setTextSize(1);
    //     OLED.setCursor(55, 55);
    //     OLED.setTextColor(SSD1306_WHITE);
    //     OLED.print("| spray: " + String(spraying ? "ON" : "OFF"));
    // }

    // OLED.display();
}
void initOLED()
{
    if (!OLED.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
        while (1)
            ;
    }
    // displayOLED({"temp", "hum", "isAutoSpray", "spraying"}, true);
    OLED.clearDisplay();
}
void updateOLED()
{
    if (millis() - lastCheck >= OLED_INTERVAL)
    {
        displayOLED({}, false);
        lastCheck = millis();
        // std::vector<std::string> updatedSegments;

        // if (lastTemp != temperature)
        // {
        //     lastTemp = temperature;
        //     updatedSegments.push_back("temp");
        // }
        // if (lastHum != humidity)
        // {
        //     lastHum = humidity;
        //     updatedSegments.push_back("hum");
        // }
        // if (lastAutoSpray != isAutoSpray)
        // {
        //     lastAutoSpray = isAutoSpray;
        //     updatedSegments.push_back("isAutoSpray");
        // }
        // if (lastSpraying != spraying )
        // {
        //     lastSpraying = spraying;
        //     updatedSegments.push_back("spraying");
        // }
        // if (updatedSegments.size() > 0)
        // {
        //     displayOLED(updatedSegments, false);
        // }
    }
}
// OLED =================================

// WEBSERVER ============================
ESP8266HTTPUpdateServer httpUpdater;
ESP8266WebServer server(80);
void handleRoot()
{
    server.send_P(200, "text/html", INDEX_PAGE);
}
void handleData()
{
    String json = "{";
    json += "\"device_location\":\"" + String(deviceLocation) + "\"";
    json += ", \"version\":\"" + String(version) + "\"";
    json += ", \"temperature\":\"" + String(temperature, 1) + "\"";
    json += ", \"humidity\":\"" + String(humidity, 1) + "\"";
    json += ", \"offset_temp\":\"" + String(offsetTemp, 1) + "\"";
    json += ", \"offset_hum\":\"" + String(offsetHum, 1) + "\"";
    json += ", \"is_auto\":\"" + String(isAutoSpray ? "1" : "0") + "\"";
    json += ", \"min_hum_start\":\"" + String(minHumidityStart) + "\"";
    json += ", \"max_hum_stop\":\"" + String(maxHumidityStop) + "\"";
    json += ", \"spraying\":\"" + String(spraying ? "1" : "0") + "\"";
    json += "}";

    server.sendHeader("Cache-Control", "no-cache");
    server.send(200, "application/json", json);
}
void handleSetDeviceLocation()
{
    if (server.hasArg("device_location"))
    {
        deviceLocation = server.arg("device_location");
        setKeyValue("device_location", deviceLocation.c_str());
    }
    server.sendHeader("Location", "/");
    server.send(303);
}
void handleSetOffsetTemp()
{
    if (server.hasArg("offset_temp"))
    {
        offsetTemp = server.arg("offset_temp").toFloat();
        setKeyValue("offsetTemp", String(offsetTemp).c_str());
    }
    server.sendHeader("Location", "/");
    server.send(303);
}
void handleSetOffsetHum()
{
    if (server.hasArg("offset_hum"))
    {
        offsetHum = server.arg("offset_hum").toFloat();
        setKeyValue("offsetHum", String(offsetHum).c_str());
    }
    server.sendHeader("Location", "/");
    server.send(303);
}
void handleSetAutoSpray()
{
    if (server.hasArg("is_auto"))
    {
        isAutoSpray = server.arg("is_auto").toInt() == 1;
        setKeyValue("isAutoSpray", String(isAutoSpray ? 1 : 0).c_str());
    }
    server.sendHeader("Location", "/");
    server.send(303);
}
void handleSetMinHumStart()
{
    if (server.hasArg("min_hum_start"))
    {
        minHumidityStart = server.arg("min_hum_start").toInt();
        setKeyValue("minHumidityStart", String(minHumidityStart).c_str());
    }
    server.sendHeader("Location", "/");
    server.send(303);
}
void handleSetMaxHumStop()
{
    if (server.hasArg("max_hum_stop"))
    {
        maxHumidityStop = server.arg("max_hum_stop").toInt();
        setKeyValue("maxHumidityStop", String(maxHumidityStop).c_str());
    }
    server.sendHeader("Location", "/");
    server.send(303);
}
void handleWebSprayON()
{
    startSpray();
    server.sendHeader("Location", "/");
    server.send(303);
}
void handleWebSprayOFF()
{
    stopSpray();
    server.sendHeader("Location", "/");
    server.send(303);
}
void handleWebResetWiFi()
{
    server.sendHeader("Location", "/");
    server.send(303);
    wifiManager.resetSettings();
    delay(3000);
    ESP.restart(); // Reset and try again
}
void initWebserver()
{
    // Define what happens when you visit the root IP/URL
    server.on("/", handleRoot);
    server.on("/data", handleData);
    server.on("/set_device_location", handleSetDeviceLocation);
    server.on("/set_offset_temp", handleSetOffsetTemp);
    server.on("/set_offset_hum", handleSetOffsetHum);
    server.on("/set_auto_spray", handleSetAutoSpray);
    server.on("/set_min_hum_start", handleSetMinHumStart);
    server.on("/set_max_hum_stop", handleSetMaxHumStop);
    server.on("/spray_on", handleWebSprayON);
    server.on("/spray_off", handleWebSprayOFF);
    server.on("/reset_wifi", handleWebResetWiFi);

    // Define what happens when you visit the OTA update page
    server.on("/server-ota", []()
              { server.send(200, "text/html", OTA_PAGE); });
    //   Setup OTA Update Server
    httpUpdater.setup(&server);

    // Start the web server
    server.begin();

    Serial.println("HTTP server started");
}
// WEBSERVER ============================

void setup()
{
    Serial.begin(115200);

    initLittleFS();

    // Initialize I2C Bus explicitly
    Wire.begin(SDA_PIN, SCL_PIN);

    // initOLED();
    initLedIndicator();
    initSpray();
    initAHTSensor();

    initConnection();

    // Setup mDNS for local network access
    initDNS();
}

void loop()
{
    wifiManager.process();

    // updateOLED();
    updateLedIndicator();
    updateSpray();
    updateAHTSensor();

    checkConnection();
    if (isConnected)
    {
        Blynk.run();
        server.handleClient();
    }
    MDNS.update();
}
