#include <Arduino.h>
#include <Adafruit_FXAS21002C.h>
#include <Adafruit_FXOS8700.h>
#include <Adafruit_FlashTransport.h>
#include "Sensor_Cal_Ext_Flash.h"

Adafruit_FXOS8700 fxos = Adafruit_FXOS8700(0x8700A, 0x8700B);
Adafruit_FXAS21002C fxas = Adafruit_FXAS21002C(0x0021002C);

#define CUSTOM_CS 6
#define CUSTOM_SPI SPI

Adafruit_Sensor *accelerometer, *gyroscope, *magnetometer;
const char* _file_ = NULL;

sensors_event_t mag_event, gyro_event, accel_event;

int loopcount = 0;


Adafruit_FlashTransport_SPI flashTransport(CUSTOM_CS, CUSTOM_SPI);
Adafruit_SPIFlash flash(&flashTransport); //create flash object
Sensor_Calibration_External_Flash cal;

bool check_sensor(void);
void write_calibration(void);

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(100);

  //Initialize sensors
  if(check_sensor()){
    accelerometer = fxos.getAccelerometerSensor();
    gyroscope = &fxas;
    magnetometer = fxos.getMagnetometerSensor();

    accelerometer->printSensorDetails();
    gyroscope->printSensorDetails();
    magnetometer->printSensorDetails();
  }

  //start flash filesys
  cal.start_flash(flash, _file_);

  //load calibration file, if it exists
  if (!cal.loadCalibration()) {
    Serial.println("No calibration loaded/found... will start with writing last manually entered calibration");
    write_calibration();
  } else {
    Serial.println("Loaded existing calibration");
  }

  cal.printSavedCalibration(); //if cal is saved- print it

  Serial.println("Calibrations found: ");
  Serial.print("\tMagnetic Hard Offset: ");
  for (int i=0; i<3; i++) {
    Serial.print(cal.mag_hardiron[i]); 
    if (i != 2) Serial.print(", ");
  }
  Serial.println();
  
  Serial.print("\tMagnetic Soft Offset: ");
  for (int i=0; i<9; i++) {
    Serial.print(cal.mag_softiron[i]); 
    if (i != 8) Serial.print(", ");
  }
  Serial.println();

  Serial.print("\tMagnetic Field Magnitude: ");
  Serial.println(cal.mag_field);

  Serial.print("\tGyro Zero Rate Offset: ");
  for (int i=0; i<3; i++) {
    Serial.print(cal.gyro_zerorate[i]); 
    if (i != 2) Serial.print(", ");
  }
  Serial.println();

  Serial.print("\tAccel Zero G Offset: ");
  for (int i=0; i<3; i++) {
    Serial.print(cal.accel_zerog[i]); 
    if (i != 2) Serial.print(", ");
  }
  Serial.println();

}

void loop() {
  magnetometer->getEvent(&mag_event);
  gyroscope->getEvent(&gyro_event);
  accelerometer->getEvent(&accel_event);
  
  // 'Raw' values to match expectation of MotionCal
  Serial.print("Raw:");
  Serial.print(int(accel_event.acceleration.x*8192/9.8)); Serial.print(",");
  Serial.print(int(accel_event.acceleration.y*8192/9.8)); Serial.print(",");
  Serial.print(int(accel_event.acceleration.z*8192/9.8)); Serial.print(",");
  Serial.print(int(gyro_event.gyro.x*SENSORS_RADS_TO_DPS*16)); Serial.print(",");
  Serial.print(int(gyro_event.gyro.y*SENSORS_RADS_TO_DPS*16)); Serial.print(",");
  Serial.print(int(gyro_event.gyro.z*SENSORS_RADS_TO_DPS*16)); Serial.print(",");
  Serial.print(int(mag_event.magnetic.x*10)); Serial.print(",");
  Serial.print(int(mag_event.magnetic.y*10)); Serial.print(",");
  Serial.print(int(mag_event.magnetic.z*10)); Serial.println("");

  // unified data
  Serial.print("Uni:");
  Serial.print(accel_event.acceleration.x); Serial.print(",");
  Serial.print(accel_event.acceleration.y); Serial.print(",");
  Serial.print(accel_event.acceleration.z); Serial.print(",");
  Serial.print(gyro_event.gyro.x, 4); Serial.print(",");
  Serial.print(gyro_event.gyro.y, 4); Serial.print(",");
  Serial.print(gyro_event.gyro.z, 4); Serial.print(",");
  Serial.print(mag_event.magnetic.x); Serial.print(",");
  Serial.print(mag_event.magnetic.y); Serial.print(",");
  Serial.print(mag_event.magnetic.z); Serial.println("");
  loopcount++;

  // occasionally print calibration
  if (loopcount == 50 || loopcount > 100) {
    Serial.print("Cal1:");
    for (int i=0; i<3; i++) {
      Serial.print(cal.accel_zerog[i], 3); 
      Serial.print(",");
    }
    for (int i=0; i<3; i++) {
      Serial.print(cal.gyro_zerorate[i], 3);
      Serial.print(",");
    }  
    for (int i=0; i<3; i++) {
      Serial.print(cal.mag_hardiron[i], 3); 
      Serial.print(",");
    }  
    Serial.println(cal.mag_field, 3);
    loopcount++;
  }
  if (loopcount >= 100) {
    Serial.print("Cal2:");
    for (int i=0; i<9; i++) {
      Serial.print(cal.mag_softiron[i], 4); 
      if (i < 8) Serial.print(',');
    }
    Serial.println();
    loopcount = 0;
  }
 
  delay(100); 
}

// put function definitions here:

bool check_sensor(){
    if(!fxas.begin() || !fxos.begin()){
      Serial.println("Failed to start IMU. Check your wiring!");
      return false;
    }
    else {
      Serial.println("IMU connected successfully");
      return true;
    }
}

void write_calibration(){
  //manually enter calibrations- then send to flash/eeprom memory
  //values grabbed from MotionCal, writing manually due to issues with crc checks failing
  cal.mag_hardiron[0] = 44.51;
  cal.mag_hardiron[1] = 84.44;
  cal.mag_hardiron[2] = 43.75;

  cal.mag_softiron[0] = 0.965;
  cal.mag_softiron[1] = -0.027;
  cal.mag_softiron[2] = -0.010;  
  cal.mag_softiron[3] = -0.027;
  cal.mag_softiron[4] = 0.946;
  cal.mag_softiron[5] = 0.007;  
  cal.mag_softiron[6] = -0.010;
  cal.mag_softiron[7] = 0.007;
  cal.mag_softiron[8] = 1.096;

  // Earth total magnetic field strength in uTesla (dependent on location and time of the year),
  // visit: https://www.ngdc.noaa.gov/geomag/calculators/magcalc.shtml#igrfwmm)
  cal.mag_field = 46.31;

  // from sensorlab library cal
  cal.gyro_zerorate[0] = 0.0349;
  cal.gyro_zerorate[1] = -0.0211;
  cal.gyro_zerorate[2] = -0.0068;

  if (! cal.saveCalibration()) {
    Serial.println("**WARNING** Couldn't save calibration");
  } else {
    Serial.println("Wrote calibration");    
  }
}
