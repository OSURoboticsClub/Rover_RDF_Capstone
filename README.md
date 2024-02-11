# Rover_RDF_Capstone
Repository for the 2022-2023 Mars Rover ECE Capstone Project "Lost Without Direction"

# About the Project
"Lost Without Direction" aims to build a robust system to localize our rover in the field to improve communications. The project aims to fix the issues in the latency of communication with the rover when it travels out of the line of sight of the main communications antenna. To remedy this our team took inspiration from common radio direction-finding/radio triangulation techniques and built an antenna rotator that will follow the movement of the rover at a threshold of +/-45 degrees. We utilize GPS coordinates of both the rover and the antenna itself to calculate a bearing to point the antenna towards. The real-time position from the rover is sent over LoRA to a receiver at the base station. Then the angle is calculated and sent to a control module on the antenna stand over UART serial. This repository contains the finalized code for the UI, Tracking Algorithm, and Microcontroller libraries from June 2023. It also contains schematics of the final PCB presented at the 2023 Engineering Expo. This project is still under development to improve localization accuracy, with planned upgrades to both the hardware and software.

## Planned Upgrades:
- New GPS module that boasts 1.5m accuracy and increased noise tolerance as opposed to the 2.5m accuracy of the current chip
- Development of a sensor fusion algorithm with GPS/IMU to smooth out position reports from the rover, and allow for dead-reckoning via the IMU if the GPS should become unavailable
- Rework of the serial communication network from the ground station to the antenna module to reduce the amount of wires needed
