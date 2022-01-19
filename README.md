# 34338-group-7
IOT project with Arduino

In this project we have created a home security system that uses several sensors to detect burglary. We have implemented a RFID and a webserver to connect and disconnect the security system. In addition, we will use Google Firebase to log the activity of our security system, as well as use Firebase as a backend database for our program.

The overall idea of our security system is that when you leave the house, you will activate the system using a key card or from an webserver. This will update our database, which furthermore will activate all the sensors in the house. If a sensor detects an intruder, the sensors will again update the database, which will trigger the alert system in the house. The alert system will run until it is turned off again using the key card or the webserver. Each time the security system is turned on and off or the alert system is activated, information regarding last access time, time for most recent activation of the alert system as well as access ID, will also be transmitted to the database and logs this information. 
