#ifndef __SENSOR_CAL_EXT_FLASH__
#define __SENSOR_CAL_EXT_FLASH__

#include <Adafruit_Sensor_Calibration.h>
#include <Adafruit_FlashTransport.h>
#include <Adafruit_SPIFlash.h>
#include <flash_devices.h>
#include "SdFat.h"

class Sensor_Calibration_External_Flash : public Adafruit_Sensor_Calibration {
    public:
        virtual ~Sensor_Calibration_External_Flash();
        bool start_flash(Adafruit_SPIFlash flash, const char *file);
        bool printSavedCalibration(void);
        bool loadCalibration(void);
        bool saveCalibration(void);
    private:
        FatFileSystem *FS = NULL;
        const char *_cal_filename_ = NULL;
        StaticJsonDocument<512> calibJSON;
};

#endif
