#include "ESPBoard.h"

// WiFi-Setup
#include <WiFi.h>
#include <HTTPClient.h>


ESPBoard::ESPBoard() {
}

void ESPBoard::check_and_reset_watchdog(){
  if(!should_be_reset_soon){
    //Watchdog.reset(); // Reset Watchdog
  }
}

void ESPBoard::do_immediate_restart(){
  should_be_reset_soon = true;
  Serial.println("Restart Arduino immediately");
}

void ESPBoard::setup_wifi_connection(){
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); 

  Serial.print(F("Connecting to wifi"));
  
  unsigned long time_start_connecting = millis();
  
  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");

    // If ESP wasn't able to connect to WiFi within 6 seconds, abort the try and start a new try
    if(millis() - time_start_connecting > 6000){
      //checkIfEverythingIsOkayOrRestartESP(true);
      Serial.println("");
      Serial.print(F("Unable to connect to network. Starting a new try."));
      time_start_connecting = millis();
      WiFi.disconnect();
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD); 
    }
  }
  Serial.println("");
  Serial.printf("Connected with %s.\n", WIFI_SSID);
  ip = WiFi.localIP();
  Serial.printf("IP address: %s.\n", ip.toString().c_str());
}

String ESPBoard::send_post_data_to_server(String server_post_url, String post_data){
  HTTPClient http;   
  http.begin(server_post_url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int http_response_code = http.POST(post_data);

  String return_string = "";
  
  if(http_response_code > 0){
    String response = http.getString();
    //Serial.println("HTTP Response Code: " + String(http_response_code) + ", ");
    //Serial.println(response);
    return_string = response;
  }
  else {
    Serial.println("Error: HTTP Response Code " + String(http_response_code));
    return_string = String(http_response_code);
  }
  http.end();
  return return_string;
}

void ESPBoard::get_request(String server_get_url){
  HTTPClient http;   
  http.begin(server_get_url);
  int http_response_code = http.GET();
  http.end();
}