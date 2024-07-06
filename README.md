# MAUI Software

Measurements from Arterial Ultrasound Imaging (or MAUI for short) is a research tool we created in order to measure the intima-to-intima, the intima-media thickness (IMT), and velocity from ultrasound images and videos.  Currently our users are using MAUI to track common carotid arteries (CCA), internal carotid arteries, brachial arteries, and popliteal arteries.  MAUI also outputs the blood flow, which is calculated from the diameter of the vessel and the velocity of the blood.

The tracking data for each frame is output to a CSV file which you can open in Excel for viewing.  This data can be plotted to observe the behavior of the top IMT, bottom IMT as well as the lumen diameter. A video with the tracking overlay can also be exported. For example: https://youtu.be/k_fAxHmO8Ts?si=VUWwLRIIIs6g-JFj

## Project status

This project was started in 2016 and sold on a subscription basis via hedgehogmedical.com until summer 2024.
It is now open source software under the MIT license. We hope it continues to provide value to the handful of research labs who still use it.

## How to build

The GUI is a Qt Creator (4.10.2) application (see qt-src/maui-gui.pro), and includes a test suite (see qt-src/test/maui-test.pro). The computer vision code was written in Matlab and made accessible to the C++ code by exporting a libary (see matlab-src).
