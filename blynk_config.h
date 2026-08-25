#define BLYNK_TEMPLATE_ID "TMPL6L3RFk1YW"
#define BLYNK_TEMPLATE_NAME "Quickstart Device"
#define BLYNK_AUTH_TOKEN "M7fW5gaAIE5_kDRPyCdVWcOHnhmnwqhR"

#define BLYNK_PRINT Serial

#include <BlynkSimpleEsp8266.h>

void initBlynk()
{
    
    Blynk.begin(BLYNK_AUTH_TOKEN, WiFi.SSID().c_str(), WiFi.psk().c_str());
}
void runBlynk()
{
    Blynk.run();
}