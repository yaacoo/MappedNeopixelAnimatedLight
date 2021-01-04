#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <math.h>

#ifndef STASSID
#define STASSID "yourSSID"
#define STAPSK  "yourPassword"
#endif
//
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#define LED_PIN    D6
#define LED_COUNT 150
int dtime=5;
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Map of the pixels (x,y), x range is 0..168 and y 0..453
int mapMatrix[][2] = { {62, 453},{74, 438},{73, 430},{80, 418},{78, 406},{72, 393},{168, 391},{62, 379},{56, 378},{50, 371},{49, 352},{54, 342},{67, 330},{78, 330},{90, 332},{106, 328},{114, 321},{114, 310},{86, 294},{86, 298},{78, 299},{72, 296},{62, 296},{52, 291},{52, 285},{36, 282},{29, 260},{23, 254},{16, 240},{16, 230},{25, 189},{20, 187},{20, 172},{34, 168},{45, 160},{73, 186},{81, 198},{103, 211},{108, 238},{108, 246},{86, 249},{74, 252},{61, 238},{50, 224},{42, 214},{42, 183},{79, 165},{68, 159},{80, 157},{104, 161},{108, 178},{114, 202},{103, 192},{98, 219},{92, 249},{76, 239},{64, 246},{44, 242},{44, 235},{34, 215},{35, 197},{56, 188},{90, 162},{53, 153},{60, 144},{68, 154},{68, 162},{56, 174},{38, 186},{26, 190},{21, 188},{20, 169},{14, 158},{45, 150},{82, 133},{84, 106},{108, 100},{106, 95},{77, 65},{78, 68},{65, 70},{51, 68},{38, 94},{22, 112},{12, 134},{8, 155},{11, 161},{20, 180},{18, 172},{14, 172},{46, 160},{46, 160},{80, 149},{94, 145},{108, 136},{100, 126},{95, 128},{94, 102},{93, 84},{85, 68},{71, 68},{56, 54},{28, 70},{20, 84},{6, 110},{5, 116},{13, 128},{20, 134},{31, 138},{56, 140},{111, 124},{110, 110},{104, 100},{100, 92},{51, 59},{58, 41},{72, 41},{52, 18},{74, 27},{72, 37},{68, 58},{61, 73},{44, 83},{32, 108},{25, 120},{20, 158},{26, 176},{43, 191},{64, 196},{82, 194},{92, 203},{110, 190},{118, 172},{120, 158},{118, 142},{106, 138},{104, 132},{109, 120},{112, 109},{101, 98},{103, 92},{35, 75},{18, 69},{10, 68},{0, 35},{10, 26},{30, 0},{53, 4},{70, 18},{100, 5} };
const int maxy=453;
const int maxx=168;

//OTA block
const char* ssid = STASSID;
const char* password = STAPSK;

void setup() {
  //OTA block
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
//  pinMode(LED_BUILTIN, OUTPUT);
//
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  strip.begin();       
  strip.show();          
  strip.setBrightness(30);
//
}

void loop() {
  ArduinoOTA.handle(); 
//  digitalWrite(LED_BUILTIN, HIGH); //high is low in this builtinled...
//  strip.clear(); 
  uint32_t colorLiquid=strip.Color(255, 0, 3);
  uint32_t colorTop=strip.Color(0, 0, 0);
  uint32_t colorDrip=strip.Color(255, 0, 3);

  int current_time = millis();
  static int last_time = current_time;
  int millis_passed = current_time - last_time;
  last_time = current_time;
  
  static int wave_tick = 0;
  static int height_tick = 0;
  static int drip_tick=0;
  static const int kMaxWaveMillis = 1000;
  static const int kMaxHeightMillis = 50000;
  static const int kMaxDripMillis= 1700;

  wave_tick += millis_passed;
  if (wave_tick > kMaxWaveMillis) wave_tick -= kMaxWaveMillis;

  height_tick += millis_passed;
  if (height_tick > kMaxHeightMillis) height_tick -= kMaxHeightMillis;

  drip_tick += millis_passed;
  if (drip_tick > kMaxDripMillis) drip_tick -= kMaxDripMillis;


  float wave_fraction = wave_tick / float(kMaxWaveMillis);
  float height_fraction;
  if (height_tick <= 0.5*kMaxHeightMillis) {
    height_fraction = 2.f * height_tick / float(kMaxHeightMillis); }
  else {
    height_fraction = (kMaxHeightMillis - 2.f*(height_tick - 0.5*kMaxHeightMillis) )/ float(kMaxHeightMillis); }
  
  float drip_fraction= drip_tick / float(kMaxDripMillis);
  float const drip_height= 0.06*maxy;
  
  for (int i=0; i<LED_COUNT; i++) {
    float wave_height = sinf(mapMatrix[i][0] / float(maxx) * 2 * M_PI + wave_fraction * 2.f * M_PI) * 20.f;
    float water_h= maxy * height_fraction + wave_height;
    if (mapMatrix[i][1] <= water_h) {
      strip.setPixelColor(i, colorLiquid);
    }
    else if ((mapMatrix[i][1] < maxy- drip_fraction*maxy ) &&
        height_tick <= 0.5*kMaxHeightMillis &&
        (mapMatrix[i][1] > maxy- (drip_fraction*maxy + drip_height) &&
        mapMatrix[i][0] > 0.4*maxx &&
        mapMatrix[i][0] < 0.6*maxx &&
        mapMatrix[i][1] > water_h + 0.25*maxy )) {
      strip.setPixelColor(i, colorDrip);
    }
    else {
      strip.setPixelColor(i, colorTop);
    }
    
  }
  strip.show();
//  delay(dtime);

//
}
