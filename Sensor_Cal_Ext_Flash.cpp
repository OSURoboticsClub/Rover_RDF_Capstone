#include "Sensor_Cal_Ext_Flash.h"

static FatFileSystem fatfs;

Sensor_Calibration_External_Flash::~Sensor_Calibration_External_Flash() {}


bool Sensor_Calibration_External_Flash::start_flash(Adafruit_SPIFlash flash, const char *file_ptr){
    if(!flash.begin()){
        Serial.println("Unable to initialize flash module- check wiring!");
        return false;
    }
    else {
        Serial.print("JEDEC ID: ");
        Serial.println(flash.getJEDECID(), HEX);
        Serial.print("Flash size: ");
        Serial.println(flash.size());

        if (!fatfs.begin(&flash)) {
            Serial.println("Error, failed to mount newly formatted filesystem!");
            Serial.println("Was it formatted with the fatfs_format example?");
            return false;
        }
        FS = &fatfs;
        Serial.println("Mounted filesystem!");

        //print filesys data
        File root;
        char file[80];
        root = FS->open("/");
        while (1) {
            File entry = root.openNextFile();
            if (!entry) {
            Serial.println("No files found/No more files");
            break; // no more files
            }
            entry.getName(file, 80);
            Serial.print("\t");
            Serial.print(file);
            if (entry.isDirectory()) {
            Serial.println("/");
            } else {
            // files have sizes, directories do not
            Serial.print(" : ");
            Serial.print(entry.size(), DEC);
            Serial.println(" bytes");
            }
        entry.close();
        }


        if (file_ptr) {
            _cal_filename_ = file_ptr;
            //Serial.println(file_ptr);
        } else {
            _cal_filename_ = "sensor_calib.json";
        }

        //Serial.println(_cal_filename_);
        return true;
    }

}

bool Sensor_Calibration_External_Flash::printSavedCalibration() {
    if (!FS)
        return false;
    File file = FS->open(_cal_filename_, O_READ);
    if (!file) {
        Serial.println(F("Failed to read file"));
        return false;
    }

    Serial.println("------------");
    while (file.available()) {
        Serial.write(file.read());
    }
    Serial.println("\n------------");
    file.close();
    yield();
    return true;
}

bool Sensor_Calibration_External_Flash::loadCalibration() {
    if (!FS)
        Serial.println(F("No filesystem found"));
        return false;

    File file = FS->open(_cal_filename_, O_READ);
    if (!file) {
        Serial.println(F("Failed to read file"));
        return false;
    }

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(calibJSON, file);
    if (error) {
        Serial.println(F("Failed to read file (deserialization error)"));
        return false;
    }

    // Close the file (File's destructor doesn't close the file)
    file.close();

    for (int i = 0; i < 3; i++) {
        mag_hardiron[i] = calibJSON["mag_hardiron"][i] | 0.0;
    }
    for (int i = 0; i < 9; i++) {
        float def = 0;
        if (i == 0 || i == 4 || i == 8) {
        def = 1;
        }
        mag_softiron[i] = calibJSON["mag_softiron"][i] | def;
    }
    mag_field = calibJSON["mag_field"] | 0.0;
    for (int i = 0; i < 3; i++) {
        gyro_zerorate[i] = calibJSON["gyro_zerorate"][i] | 0.0;
    }
    for (int i = 0; i < 3; i++) {
        accel_zerog[i] = calibJSON["accel_zerog"][i] | 0.0;
    }

    return true;
}

bool Sensor_Calibration_External_Flash::saveCalibration() {
    if (!FS)
        return false;

    File file = FS->open(_cal_filename_, O_WRITE | O_CREAT | O_TRUNC);
    if (!file) {
        Serial.println(F("Failed to create file"));
        return false;
    }

    JsonObject root = calibJSON.to<JsonObject>();
    JsonArray mag_hard_data = root.createNestedArray("mag_hardiron");
    for (int i = 0; i < 3; i++) {
        mag_hard_data.add(mag_hardiron[i]);
    }
    JsonArray mag_soft_data = root.createNestedArray("mag_softiron");
    for (int i = 0; i < 9; i++) {
        mag_soft_data.add(mag_softiron[i]);
    }
    root["mag_field"] = mag_field;
    JsonArray gyro_zerorate_data = root.createNestedArray("gyro_zerorate");
    for (int i = 0; i < 3; i++) {
        gyro_zerorate_data.add(gyro_zerorate[i]);
    }
    JsonArray accel_zerog_data = root.createNestedArray("accel_zerog");
    for (int i = 0; i < 3; i++) {
        accel_zerog_data.add(accel_zerog[i]);
    }
    // serializeJsonPretty(root, Serial);

    // Serialize JSON to file
    if (serializeJson(calibJSON, file) == 0) {
        Serial.println(F("Failed to write to file"));
        return false;
    }
    // Close the file (File's destructor doesn't close the file)
    file.close();

    return true;
}
