#include "credentials.h"
// WiFi-Setup
#include <WiFi.h>
#include <HTTPClient.h>
#include "ESPBoard.h"

ESPBoard ESP_board;


//#include <Wire.h>

// BME680 and BME280
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <Adafruit_BME280.h>
Adafruit_BME680 sensor_bme680;
Adafruit_BME280 sensor_bme280;

// BHT1750 brightness sensor
#include <BH1750.h>
BH1750 sensor_bh1750(0x77);

// TSL 2561
/*
#include <Adafruit_TSL2561_U.h>
Adafruit_TSL2561_Unified sensor_tsl2561 = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
*/

// DHT22
#include "DHT.h"
#define DHTPIN 26
#define DHTTYPE DHT22
DHT sensor_dht22(DHTPIN, DHTTYPE);

// TSL2591
#include "Adafruit_TSL2591.h"
Adafruit_TSL2591 sensor_tsl2591 = Adafruit_TSL2591(2591);

// GYML8511
/*
#define GYML8511_PIN 27
const float GYML8511_in_min = 0.99;
const float GYML8511_in_max = 2.9;
const float GYML8511_out_min = 0.0;
const float GYML8511_out_max = 15.0;
const byte GYML8511_number_of_readings_for_avg = 30;
*/


// -------------------------------------------------------------------------------------------

// Variables storing the last reads
float bme680_temperature = 0;
float bme680_humidity;
float bme680_pressure;
float bme680_gasResistance;

float bme280_temperature;
float bme280_humidity;
float bme280_pressure;

float dht22_temperature;
float dht22_humidity;

float bh1750_brightness = 0; // Brightness from BH1750

float tsl2561_brightness = 0; // Brightness from TSL2561

uint32_t tsl2591_luminosity = 0;
uint16_t tsl2591_full = 0;
uint16_t tsl2591_ir = 0;
uint16_t tsl2591_visible = 0;
uint16_t tsl2591_lux = 0;

//float gyml8511_uv = 0;


unsigned long lastTime = 0;  
unsigned long timerDelay = 30000;  // Send readings timer


float temperature_avg = 0;
unsigned int temperature_count = 0;

String post_data = "";



void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");

  ESP_board.setup_wifi_connection();
  Serial.println("WiFi setup done");


  // ---------------------- BME680 ----------------------
  if (!sensor_bme680.begin()) {
    Serial.println(F("BME680 error: No sensor found, check wiring!"));
  }
  else {
    Serial.println(F("BME680: Setup done"));
  }
  
  
  // ---------------------- BME280 ----------------------
  if (!sensor_bme280.begin(0x76)) {
    Serial.println(F("BME280 error: No sensor found, check wiring!"));
  }
  else {
    Serial.println(F("BME280: Setup done"));
  }

  
  // ---------------------- BH1750 ----------------------
  if (!sensor_bh1750.begin()) {
    Serial.println(F("BH1750 error: No sensor found, check wiring!"));
  }
  else {
    Serial.println(F("BH1750: Setup done"));
  }

  
  // ---------------------- TSL2561 ----------------------
  /*
  if(!sensor_tsl2561.begin()){
    Serial.println(F("TSL2561 error: No sensor found, check wiring!"));
  }
  else {
    Serial.println(F("TSL2561: Setup done"));
  }
  */


  // ---------------------- TSL2591 ----------------------
  if (!sensor_tsl2591.begin()){
    Serial.println(F("TSL2591 error: No sensor found, check wiring!"));
  }
  else {
    sensor_tsl2591.setGain(TSL2591_GAIN_MED);                 // 428x gain
    sensor_tsl2591.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time of 600ms
    Serial.println(F("TSL2591: Setup done"));
  }

  
  // ---------------------- DHT22 ----------------------
  sensor_dht22.begin();
  pinMode(DHTPIN, INPUT_PULLUP);
  Serial.println(F("DHT22: Setup done"));

  
  // ---------------------- GYML8511 ----------------------
  /*
  pinMode(GYML8511_PIN, INPUT);
  Serial.println(F("GYML8511: Setup done"));
  */

  
  // ---------------------- Rest of setup() ----------------------
  Serial.println(F("Setup completed."));
}

void loop()
{
  if ((millis() - lastTime) > timerDelay) {
    if(bme680_temperature != 0 || bme680_humidity != 0 || bme680_pressure != 0){
      Serial.println(temperature_count);
      Serial.printf("Temperature = %.2f ºC \n", bme680_temperature);
      Serial.printf("Temperature = %.2f ºC \n", temperature_avg/temperature_count);
      Serial.printf("Humidity = %.2f %% \n", bme680_humidity);
      Serial.printf("Pressure = %.2f hPa \n", bme680_pressure);
      Serial.printf("Gas Resistance = %.2f KOhm \n", bme680_gasResistance);
      Serial.printf("Temperature (BME280) = %.2f ºC \n", bme280_temperature);
      Serial.printf("Humidity (BME280) = %.2f %% \n", bme280_humidity);
      Serial.printf("Pressure (BME280) = %.2f hPa \n", bme280_pressure);
      Serial.printf("Temperature (DHT22) = %.2f ºC \n", dht22_temperature);
      Serial.printf("Humidity (DHT22) = %.2f %% \n", dht22_humidity);
      Serial.printf("Brightness (BH1750) = %.2f Lux \n", bh1750_brightness);
      //Serial.printf("Brightness (TSL2561) = %.2f Lux \n", tsl2561_brightness);
      //Serial.printf("UV Intensity (GYML8511) = %.2f mW/cm^2 \n", gyml8511_uv);
      
      Serial.print(F("Luminosity (TSL2591) = "));
      Serial.print(tsl2591_luminosity);
      Serial.println("");
      
      Serial.print(F("Brightness (TSL2591) = "));
      Serial.print(tsl2591_lux);
      Serial.println("Lux");
      
      Serial.print(F("IR (TSL2591) = "));
      Serial.print(tsl2591_ir);
      Serial.println("");
      
      Serial.print(F("Visible (TSL2591) = "));
      Serial.print(tsl2591_visible);

      Serial.println();

  
      post_data = "";
      post_data += "room=";
      post_data += ROOM_SENSOR_ROOM_NAME;
      post_data += "&bme680_temperature=";
      post_data += bme680_temperature;
      post_data += "&bme680_humidity=";
      post_data += bme680_humidity;
      post_data += "&bme680_pressure=";
      post_data += bme680_pressure;
      post_data += "&bme680_gas=";
      post_data += bme680_gasResistance;
      post_data += "&bme280_temperature=";
      post_data += bme280_temperature;
      post_data += "&bme280_humidity=";
      post_data += bme280_humidity;
      post_data += "&bme280_pressure=";
      post_data += bme280_pressure;
      post_data += "&bh1750_brightness=";
      post_data += bh1750_brightness;
      //post_data += "&tsl2561_brightness=";
      //post_data += tsl2561_brightness;
      post_data += "&dht22_temperature=";
      post_data += dht22_temperature;
      post_data += "&dht22_humidity=";
      post_data += dht22_humidity;
      post_data += "&tsl2591_brightness=";
      post_data += tsl2591_lux;
      //post_data += "&gyml8511_uv=";
      //post_data += gyml8511_uv;
      ESP_board.send_post_data_to_server(ROOM_SENSOR_SERVER_POST_URL, post_data);


      
    }
    lastTime = millis();

    temperature_avg = 0;
    temperature_count = 0;
  }
  else {
    get_data_from_BME680();
    get_data_from_BME280();
    get_data_from_BH1750();
    //get_data_from_TSL2561();
    get_data_from_DHT22();
    get_data_from_TSL2591();
    //get_data_from_GYML8511();
    
    temperature_avg += bme680_temperature;
    temperature_count += 1;
    
    delay(1000);
  }
}

// -------------------------------------------------------------------------------------------


// ---------------------- BME680 ----------------------
void get_data_from_BME680(){
  unsigned long end_time = sensor_bme680.beginReading();
  if (end_time == 0) {
    Serial.println(F("BME680 error: Failed to begin reading"));
    return;
  }
  if (!sensor_bme680.endReading()) {
    Serial.println(F("BME680 error: Failed to complete reading"));
    return;
  }
  bme680_temperature = sensor_bme680.temperature;
  bme680_pressure = sensor_bme680.pressure / 100.0;
  bme680_humidity = sensor_bme680.humidity;
  bme680_gasResistance = sensor_bme680.gas_resistance / 1000.0;
}


// ---------------------- BME280 ----------------------
void get_data_from_BME280(){
  bme280_temperature = sensor_bme280.readTemperature();
  bme280_pressure = sensor_bme280.readPressure() / 100.0;
  bme280_humidity = sensor_bme280.readHumidity();
}


// ---------------------- BH1750 ----------------------
void get_data_from_BH1750(){
  bh1750_brightness = sensor_bh1750.readLightLevel();
}


// ---------------------- TSL2561 ----------------------
/*
void get_data_from_TSL2561(){
  sensors_event_t event;
  sensor_tsl2561.getEvent(&event);
 
  tsl2561_brightness = event.light;
}
*/


// ---------------------- DHT22 ----------------------
void get_data_from_DHT22(){
  dht22_temperature = sensor_dht22.readTemperature();
  dht22_humidity = sensor_dht22.readHumidity();
}


// ---------------------- TSL2591 ----------------------
void get_data_from_TSL2591(){
  tsl2591_luminosity = sensor_tsl2591.getFullLuminosity();
  tsl2591_ir = tsl2591_luminosity >> 16;
  tsl2591_full = tsl2591_luminosity & 0xFFFF;
  tsl2591_visible = tsl2591_full - tsl2591_ir;
  tsl2591_lux = sensor_tsl2591.calculateLux(tsl2591_full, tsl2591_ir);
}


// ---------------------- GYML8511 ----------------------
/*void get_data_from_GYML8511(){
  unsigned int avg_sum = 0; 
  for(int i = 0; i < GYML8511_number_of_readings_for_avg; i++){
    avg_sum += analogRead(GYML8511_PIN);
  }
  float uv_signal = avg_sum / GYML8511_number_of_readings_for_avg; 

  float uv_signal_voltage = 3.3 * uv_signal/4095;
  gyml8511_uv = mapfloat(uv_signal_voltage, GYML8511_in_min, GYML8511_in_max, GYML8511_out_min, GYML8511_out_max);
}



float mapfloat(float x, float in_min, float in_max, float out_min, float out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}*/