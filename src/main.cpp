
#include <Arduino.h>
#include <heltec.h>
#include <WiFi.h>
#include <MQTT.h>
#include <OneButton.h>
#include "credentials.h"

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

OneButton button(0);

void connect() {
  // Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    // Serial.print(".");
    delay(100);
  }

  // Serial.print("\nconnecting...");
  while (!client.connect(CLIENT_ID, "public", "public")) {
    // Serial.print(".");
    delay(1000);
  }

  // Serial.println("\nconnected!");

  client.subscribe("/BWIWS21KS/Claudius/button");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void doClick() {
    Serial.println("publish");
    client.publish("/BWIWS21KS/Claudius/button", "click");
}

void setup() {
  Heltec.begin(true, false, true);
  Heltec.display->clear();

  // Hier die WLAN-Zugangsdaten eingeben...
  WiFi.begin(WIFI_NAME, WIFI_PASS);
  Heltec.display->drawString(0,0, "WiFi connecting...");
  Heltec.display->display();
  Serial.println("WiFi connecting...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(100); // 100ms warten...
  }  
  Heltec.display->clear();

  Heltec.display->drawString(0,0, "WiFi connected");
  Heltec.display->drawString(0,12, "IP: " + WiFi.localIP().toString());
  Heltec.display->display();

  button.attachClick(doClick);

  // Es gibt einen Browser-basierten Client unter
  // http://www.hivemq.com/demos/websocket-client/
  // Um die Meldungen zu sehen muss das gleiche "Topic" subscribed werden (Hier "/hello")
  client.begin("broker.hivemq.com", 1883, net);
  client.onMessage(messageReceived);

  connect();
}

void loop() {
  client.loop();
  button.tick();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }
}
