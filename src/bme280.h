#ifndef __BME280_H
#define __BME280_H
#include <iostream>
#include <fcntl.h>
extern "C" {
    #include <i2c/smbus.h>
    #include <linux/i2c-dev.h>
}
#include <math.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "compensation.h"

#include "httplib.h"

#define I2C_PATH "/dev/i2c-1"

#define IDENT 0xD0
#define SOFT_RESET 0xE0
#define CTRL_HUM 0xF2
#define CTRL_MEAS 0xF4
#define CONFIG 0xF5

#define DATA_START_ADDR 0xF7
#define DATA_LENGTH 8

class BME280{
    private:
    int fd;
    uint8_t dataBlock[8];
    public:
    BME280(int gpioIdx);
    bool ReadData();
    double GetTemp();
    double GetPressure();
    double GetHumidity();
};
#endif
