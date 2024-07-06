#Kalman Filter Dev Branch
This branch of the capstone project is used to track the development of libraries and files required to run kalman filters on an ARM Cortex-MO+ based microcontroller. The actual project will end up using a RP2040, but for testing and dev purposes, a feather M0 is being used.

Linear algebra for this project is handled using the Eigen library- specifically Bolder Flight's port of the library for micrcontrollers. The final project will end up implementing an Unscented Kalman Filter (to handle non-linearity of the process model and measurements).
