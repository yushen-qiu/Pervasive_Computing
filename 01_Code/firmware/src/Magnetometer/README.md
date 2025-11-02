# Magnetometer
Instructions for MMC5603 magnetometer module

**Magnetometer.h**: Header file for module

**Magnetometer.cpp**: Magnetometer logic

**MMC5603.cpp**: Standalone magnetometer code (Original file)

## Initial Calibration
1. After upload and connection, slowly rotate the magnetometer around the x and z axis
2. Enter 'n' into the Serial Monitor (SM) input while pointing the magnetometer's x-axis in the north direction (use phone compass to identify North) to complete calibration and set the North direction offset.

The output will then show the calculated bearings and the cardinal direction (e.g. North, South, East & West).

## Re-calibration

Use the SM input and enter the following characters:

'r' : Reset north offset calibration to 0 (raw Magnetometer readings)

'n' : Sets North offset calibration according to the current orientation of the magnetometer

