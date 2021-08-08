import requests
from settings import *
from datetime import datetime, timedelta


try:
    response = requests.get(SERVER_ALARM_CHECK_AND_GET_DATA_URL)
    alarms_json = response.json()

    alarms_active = False
    turn_lights_on = '0'
    make_coffee = '0'
    coffee_delay = '0'
    try:
        alarm = alarms_json[0] # There should not be more than one alarm at a time. If it is, this is not supported and just the first alarm is executed.
        turn_lights_on = alarm["turn_lights_on"]
        make_coffee = alarm["make_coffee"]
        coffee_delay = alarm["coffee_delay"]
        alarms_active = True
        datetime_alarm = datetime.strptime(alarm["time"], "%H:%M")
    except:
        print() 
    
    if(alarms_active == False):
        print("Exception: There is no alarm.") 
    else: # There is a valid alarm
        if(turn_lights_on == '1'):
            try:
                response_plug1 = requests.get(LOCAL_TURN_PLUG_1_ON_URL)
            except:
                print("Exception: Light 1 could not be turned on.") 

            try:
                response_plug2 = requests.get(LOCAL_TURN_PLUG_2_ON_URL)
            except:
                print("Exception: Light 2 could not be turned on.") 
        if(make_coffee == '1'):
            print("Schedule making a coffee")
            new_time = datetime_alarm + timedelta(minutes = int(coffee_delay))
            new_time_str = new_time.strftime("%H:%M")
            print(new_time_str)
            try:
                response_coffee_machine = requests.get(SERVER_SCHEDULE_COFFEE_URL + "?t=" + new_time_str + "&c=2") # c=2 stands for: "Make a double coffee", argument t is the scheduled time
                print(response_coffee_machine)
            except:
                print("Exception: Light 2 could not be turned on.") 
except:
    print("Exception: Alarms could not be loaded.") 