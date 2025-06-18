# STATUS-MESH
## ⚙️ COMPONENTS REQUIRED
* ESP 32 (2)
* IR Sensors (2)

## 🔗Wiring
* VCC and GND to respective pins and out pin of both Sensors to GPIO4 of two separate ESP's

## HOW IT WORKS
*When a card is inserted on the slot provided on different room/labs/stores, the IR sensor detects it and the signal is sent by the ESPs through the wifi, which is
then displayed on the website acting as a server. (Opened/closed). The activities like last seen etc can also be displayed. (Check the "SM images" folder for server dashboard).
