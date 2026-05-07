# SCHEMATIC DESIGN AND PCB 

![](/images/receiver_3d.jpg)

Above is the 3D PCB view of the Smart pump Controller `SPC` device. The PCB was made with DipTrace software as well as the schematic of the device both of which can be found in this folder. This device is AC powered with upper threshold voltage of 240VAC. It is properly equipped with AC protection circuits such as Variac and Resettable fuse (PTC), and it also has filtering circuits to ensure electrical noise are properly handled.

There are several power supply sources such as `5V supply`, `3.3V supply`, and `12V supply` on the board. The 5V supply is provided by a AC-DC HLK-2M05 module, this module supplies 2 watt and steady 5VDC to the ATmega328P chip which is the microcontroller on the board. While the 3.3V supply comes from a LDO AMS1117 voltage regulator, whose input comes from the 5V supply then dropped to 3.3V. This serves as power supply to the F8L10A LoRa module. Therefore, the 12V supply comes from the 220VAC/17VAC transformer via bridge rectification circuit and LM7812 voltage regulator. This supplies constant 12V to the relay for motor control. This relay is rated 30A which is suitable to withstand the in-rush starting current experienced by induction motors.

ATmeaga328P serves as the brain of the system. It process and enable the various control required on the system.This standalone ATmega328P IC is conneted to peripheral circuit which contains filtering capacitors, FTDI port for programming, 16MHz Crystal, and other components such as LED, pushbuttons, ceramic capacitor, etc.



![](/images/transmitter_3d.jpg)


The smart Sensing Device (SSD) is both battery and solar powered. Two 3.7V 2000mAh batteries connected in parallel are used to power this device. These batteries are charged through TP4056 charge controller. MT3608 module supplies steady 5V supply to power ATmega328P. Meanwhile, AMS1117 voltage regulator supplies 3.3V which supplies power to F8L10A LoRa module. This module is connected to small float switches which serve the purpose of water level sensor. This device senses water level, then send it on periodic duration to the controller via Radio communication. It also receives feedback from the Controller on what control state it is per time.