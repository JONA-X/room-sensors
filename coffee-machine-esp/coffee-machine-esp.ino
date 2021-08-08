#include "credentials.h"
// WiFi-Setup
#include <WiFi.h>
#include <HTTPClient.h>
#include "ESPBoard.h"

ESPBoard ESP_board;

#define COFFEE_1_PIN 15
#define COFFEE_on 1
#define COFFEE_off 0

unsigned long time_last_time_coffee = 0;
unsigned int time_devide_to_sleep = 60000; // Devices automatically goes to sleep after 60s
unsigned int max_time_for_coffee = 30000; // Maximum time that the machine needs to make a coffee. This is the time that is waited for between making two coffees in order not to interrupt the first coffee process.
unsigned long check_interval = 5000;
unsigned long last_time_coffee_made = 0;

int number_of_coffees_to_be_made = 0; // Stores how many coffees should still be made. Zero if there should no further coffee be made.

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");

  ESP_board.setup_wifi_connection();
  Serial.println("setup done");

  pinMode(COFFEE_1_PIN, OUTPUT);
}


void loop()
{
  digitalWrite(COFFEE_1_PIN, COFFEE_off);
  if(number_of_coffees_to_be_made <= 0){
    number_of_coffees_to_be_made = check_if_coffee_should_be_made();
  }
  else { // Then number_of_coffees_to_be_made > 0, i.e. there are still more coffees to be made. Therefore don't check again but after 30s do the next coffee
    if(time_last_time_coffee == 0 || (millis() - time_last_time_coffee) > max_time_for_coffee){
      make_coffee(); // time_last_time_coffee is automatically updated trough this function
      --number_of_coffees_to_be_made;
    }
  }
  delay(check_interval);
}


void make_coffee(){
  make_coffee_once(); // "Press the button"
  // If the machine was sleeping, it is now awake but didnt make a coffee yet. Check that and make sure a coffee is made:
  if(time_last_time_coffee == 0 || (millis() - time_last_time_coffee) > time_devide_to_sleep){
    delay(500); // Wait shortly. Devide is now in the mode to receive real commands
    make_coffee_once();// Make coffee
  }
  time_last_time_coffee = millis();
}


void make_coffee_once(){
    Serial.println("ON");
    digitalWrite(COFFEE_1_PIN, COFFEE_on);
    delay(100);
    digitalWrite(COFFEE_1_PIN, COFFEE_off);
    Serial.println("OFF");
}

int check_if_coffee_should_be_made(){
  String auth_code = COFFEE_MACHINE_SERVER_AUTHENTICATION_CODE;
  String httpResponse = ESP_board.send_post_data_to_server(COFFEE_MACHINE_CHECK_STATE_URL, "code=" + auth_code);
  Serial.println(httpResponse);
  if(httpResponse == "1"){
    return 1;
  }
  else if(httpResponse == "2"){
    return 2;
  }
  else { // No (zero) coffees should be made
    return 0;
  }
}