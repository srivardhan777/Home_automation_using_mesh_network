# Home_automation_using_mesh_network

The main objective is to develop a home automation system without using the internet or router. Our main idea is to avoid the internet and use home automation. So, we have developed our project based on a mesh network that enables us to communicate with each node. In a mesh network, each node is capable of connecting to each other node. Actually we can even use any topology, but for greater robustness and time efficiency we are using mesh topology.

We have used 3 nodemcu’s(esp8266) in our project to make it understandable. We can add the nodemcu’s as per our convenience.

1. One of the nodemcu is connected to a DHT11 sensor for measuring temperature and humidity.

2. Another nodemcu is connected with MQ-135, which is an air quality sensor, for detecting gases like NH3, alcohol, benzene, CO2, and smoke.

3. The last nodemcu is connected to the OLED display, here we can see the sensor values of DHT11 and MQ -135. 
