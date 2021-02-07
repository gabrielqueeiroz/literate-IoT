/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface. The device's I2C address is either 0x76 or 0x77.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
  See the LICENSE file for details.
 ***************************************************************************/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "EspMQTTClient.h"
#define SEALEVELPRESSURE_HPA (1013.25)
#define SDA_PIN 4
#define SCL_PIN 15

Adafruit_BME280 bme; // I2C

unsigned long delayTime = 3000;
float temperature, humidity, pressure, altitude;

EspMQTTClient client(
  "albendazol",
  "vitamina",
  "broker.mqttdashboard.com",    // MQTT Broker server ip
  "imesppublish32",     // Client name that uniquely identify your device
  1883                  // The MQTT port, default to 1883. this line can be omitted
);

// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished(){
  char *temperature, *humidity, *pressure, *altitude;
  // Subscribe to "mytopic/test" and display received message to Serial
  client.subscribe("topico/liga_botao1_desliga", [](const String & payload) {
    Serial.println(payload);
  });
}

void setup() {
    Serial.begin(115200);
    while(!Serial);    // time to get serial running
    Serial.println(F("BME280 test"));

    Wire.begin(SDA_PIN, SCL_PIN);
    pinMode(27, OUTPUT);
    pinMode(12, OUTPUT);
    pinMode(14, OUTPUT);

    client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
    client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
    client.enableLastWillMessage("testtest/deathrattle", "I am going offline");  // You can activate the retain flag by setting the third parameter to true

    unsigned status;
    
    // default settings
    status = bme.begin(0x76, &Wire);  
    
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
    }
    Serial.println();
}

void loop() { 
    client.loop();
    sensorRead();
    delay(delayTime);
}

void sensorRead(){
  temperature = bme.readTemperature();
  pressure = bme.readPressure() / 100.0F;
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  humidity = bme.readHumidity();

  if (isnan(temperature) || isnan(pressure) || isnan(altitude) || isnan(humidity)) {
    Serial.println("Failed to read from BME280 sensor!");
    return;
  }

  char buff[10];
  int offGreen = 0;

  if(offGreen == 0){
    digitalWrite(12, HIGH); //green
  }
  
  dtostrf(temperature,0, 0, buff);
  client.publish("gabrielpaic/temperature",buff);
  if (temperature > 30){
    digitalWrite(14, HIGH); //red
    digitalWrite(12, LOW); //green
    offGreen++;
  }else{
    digitalWrite(14, LOW); //red
    offGreen--;
  }

  dtostrf(pressure,0, 0, buff);
  client.publish("gabrielpaic/pressure",buff);

  dtostrf(altitude,0, 0, buff);
  client.publish("gabrielpaic/altitude",buff);

  dtostrf(humidity,0, 0, buff);
  client.publish("gabrielpaic/humidity",buff);
  if(humidity > 77){
    digitalWrite(27, HIGH); //blue
    digitalWrite(12, LOW); //green
    offGreen++;
  }else{
    digitalWrite(27, LOW); //blue
    offGreen--;
  }
  
  Serial.print("Temperature = ");
//    client.publish("gabrielpaic/temperature", temperature);
  Serial.print(temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");
//    client.publish("gabrielpaic/pressure", pressure);
  Serial.print(pressure);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
//    client.publish("gabrielpaic/altitude", altitude);
  Serial.print(altitude);
  Serial.println(" m");

  Serial.print("Humidity = ");
//    client.publish("gabrielpaic/humidity", humidity);
  Serial.print(humidity);
  Serial.println(" %");

  Serial.println();
}
