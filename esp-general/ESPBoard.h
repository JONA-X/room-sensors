#ifndef ESP_SENSOR_BOARD
#define ESP_SENSOR_BOARD
#include "Arduino.h" // Needed for datatype String
#include "credentials.h"



class ESPBoard {
public:  
  ESPBoard();

  void do_immediate_restart();
  void check_and_reset_watchdog();
  void setup_wifi_connection();
  String send_post_data_to_server(String server_post_url, String post_data);
  void get_request(String server_get_url);

  
private:
  bool reset_watchdog_override_remotely;
  bool should_be_reset_soon = false;
  IPAddress ip;
};

#endif