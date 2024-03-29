# 2022-2023-4gp-Wolf-Richard

## General Description

One of our projects as part of our 4th year at INSA Toulouse in physical engineering is to create a measuring system for a paper and graphite based strain sensor. We must be able to measure the resistance of this sensor and send it to an android app.

To that we added some elements. One can choose the best gain setting for their sensor thanks to the digital potentiometer controlled by the nunchuk and helped by the OLED screen. The Nunchuk is here to choose the calibrated value for the digital potentiometer. Then, the resistance of the strain gauge is sent to an Android app thanks to a Bluetooth module.

## Deliverables

- PCB shield for Arduino Uno
- Arduino code
- Android APK
- Test bench and protocole used to test the sensor
- Datasheet

## Software versions

- KiCad 6.0.11-0
- Arduino 2.0.4
- MIT app inventor nb192

## PCB shield for Arduino Uno

The whole circuit can be seen in the picture below. Only the pins that are connected to the circuit are welded to the PCB.

![whole circuit](https://github.com/MOSH-Insa-Toulouse/2022-2023-4gp-Wolf-Richard/blob/main/Photos/whole%20circuit.jpeg)

## Test Bench and protocole used to test the sensor

The test bench used is shown in the following picture.

![test bench](https://github.com/MOSH-Insa-Toulouse/2022-2023-4gp-Wolf-Richard/blob/main/Photos/test%20bench.jpeg)

To test the sensor, we put it on the bench and try to make it take the shape of the bench as much as possible. We payed attention not to touch the graphite or the aligator clips for the test to be reliable. We took the measurments once we had a stable contact with the bench and a stable value. To determine the displacement of the sensor we used the formula:

displacement = thickness / 2*R_curvature

## User Manual of the circuit and the app

When the arduino is plugged in the program starts automatically.

First, the user needs to calibrate the circuit based on its sensor. Flatten the sensor as much as you can and try to find the best level of the potentiometer to optimise the sensor’s response. To do so, push up on the nunchuk to increase the potentiometer value or push down to decrease. When satisfied, push the C button.

Second, the user is invited to put the sensor in the most deflected position of the test that they want to do and to press C again. This allows for the deflection percentage to be shown on the OLED screen.

From that moment on, the Bluetooth module sends the value of the resistance that was calculated in the program. Thanks to the app one can retrieve the value and see its evolution.

On the app of which you can see a screenshot below, one must first connect to the right Bluetooth module with the button "Let’s go Bluetooth". Once confirmed that the connection was done properly, the toggle "Tic tac boom", which controls the clock, must be on to receive the values. The values are sent by the Bluetooth module every 500ms. The button "Send to Grisolia" saves a text file with the values of the resistance in it. It is only clickable when the clock is off.

![android apk](https://github.com/MOSH-Insa-Toulouse/2022-2023-4gp-Wolf-Richard/blob/main/Photos/android%20apk.jpeg)

One can also select a point on the graph and see its value in the orange box. Above is displayed the value received at the moment by the app. Once a measurment done, the graph can be cleared thanks to the "Clear" button.

## Problems and improvements

We discovered too late (as the put the sheild on the Arduino) that our pin which was supposed to be connected to the +5V output pin was in fact connected to the Vin pin which delivers 4.3V. To solve that problem we cut the Vin pin and connected a cable from it to the +5V output.

We had problems using the rotary encoder, as a turn could be registered by the program even if no one had touched it. In the end, we just replaced it by the nunchuk which is much more precise and has way more functionnalities.
