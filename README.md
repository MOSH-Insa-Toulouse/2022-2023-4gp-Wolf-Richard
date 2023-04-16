# 2022-2023-4gp-Wolf-Richard

## General Description

One of our projects as part of our 4th year at INSA Toulouse in physical engineering is to create a measuring system for a paper and graphite based strain sensor. We must be able to measure the resistance of this sensor and send it to an android app.

To that we added some elements. One can choose the best gain setting for their sensor thanks to the digital potentiometer controlled by the nunchuk and helped by the OLED screen.

## Deliverables
- PCB shield for Arduino
- Arduino code
- Android APK
- Test bench and protocole used to test the sensor
- Datasheet

## Test Bench

## User Manual of the circuit and the app
When the arduino is plugged in the program starts automatically.

First, the user needs to calibrate the circuit based on its sensor. Flatten the sensor as much as you can and try to find the best level of the potentiometer to optimise the sensor’s response. To do so, push up on the nunchuk to increase the potentiometer value or push down to decrease. When satisfied, push the C button.

Second, the user is invited to put the sensor in the most deflected position of the test that they want to do and to press C again. This allows for the deflection percentage to be shown on the OLED screen.

From that moment on, the Bluetooth module sends the value of the resistance that was calculated in the program. Thanks to the app one can retrieve the value and see its evolution.

On the app, one must first connect to the right Bluetooth module with the button "Let’s go Bluetooth". Once confirmed that the connection was done properly, the toggle "Tic tac boom" must be on to receive the values. The values are sent by the Bluetooth module every 500ms. The button "Send to Grisolia" saves a text file with the values of the resistance in it.

## Problems and improvements

We had
