#include "secrets.h"

#include "wifi.h"
#include "mqtt.h"

#include <FastLED.h>

#define LED_TYPE    WS2811
#define COLOR_ORDER GBR
#define NUM_LEDS    18
CRGB leds[NUM_LEDS];

#define BRIGHTNESS         96
#define FRAMES_PER_SECOND  30

// Arduino UNO
#define DATA_PIN 7 
#define ONBOARD_LED 13

bool all = false;

const char* mqttTopic = "nodemcu/signal";

void registerMqttListener() {
  mqttClient.subscribe(mqttTopic);
//  mqttClient.unsubscribe(mqttTopic);
  mqttClient.onMessage(messageReceived);
}

void setup() {
  delay(3000); // 3 second delay for recovery
  pinMode(DATA_PIN, OUTPUT); 
  pinMode(ONBOARD_LED, OUTPUT); 
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  setupWifiClient(ssid, pass);
//  testWifiConnection();
  
  setupMqttClient(mqttBrokerHost, wifiClient);
  registerMqttListener();
  // Publish a test mqtt event
  mqttClient.publish(mqttTopic, "sls");
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm, red, green, blue, off};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop()
{  
  loopMqttClient();
  if (!mqttClient.connected()) {
    connectMqtt();
    registerMqttListener();
  }
  
  loopWifiClient();
  
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow

  if(all) {
    EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
  }
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming MQTT message: " + topic + " - " + payload);
  digitalWrite(ONBOARD_LED, HIGH);

  all = payload == "all";
  
  if(payload == "rainbow") {
    gCurrentPatternNumber = 0;
  } else if(payload == "glitter") {
    gCurrentPatternNumber = 1;
  } else if(payload == "confetti") {
    gCurrentPatternNumber = 2;
  } else if(payload == "sinelon") {
    gCurrentPatternNumber = 3;
  } else if(payload == "juggle") {
    gCurrentPatternNumber = 4;
  } else if(payload == "bpm") {
    gCurrentPatternNumber = 5;
  } else if(payload == "red") {
    gCurrentPatternNumber = 6;
  } else if(payload == "green") {
    gCurrentPatternNumber = 7;
  } else if(payload == "blue") {
    gCurrentPatternNumber = 8;
  } else {
    gCurrentPatternNumber = 9;
  }
  
  digitalWrite(ONBOARD_LED, LOW); 
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % (ARRAY_SIZE( gPatterns) - 4);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void red()
{
//  fadeUsingColor(leds, NUM_LEDS, CRGB::Red);
//  fadeToBlackBy( leds, NUM_LEDS, 20);
  for(int i=0; i<NUM_LEDS; i++)
    leds[i] = CRGB::Red;
   FastLED.delay(500);
}

void green()
{
//  fadeToBlackBy( leds, NUM_LEDS, 20);
  for(int i=0; i<NUM_LEDS; i++)
    leds[i] = CRGB::Green;
  FastLED.delay(500);
}

void blue()
{
//  fadeToBlackBy( leds, NUM_LEDS, 20);
  for(int i=0; i<NUM_LEDS; i++)
    leds[i] = CRGB::Blue;
  FastLED.delay(500);
}

void off()
{
  fadeToBlackBy( leds, NUM_LEDS, 20);
}
