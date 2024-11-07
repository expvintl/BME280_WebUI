#include "bme280.h"

BME280::BME280(int gpioIdx){ //初始化设备
    if ((fd = open(I2C_PATH, O_RDWR)) < 0) {
        perror("无法打开I2C设备!");
        return;
    }
    if (ioctl(fd, I2C_SLAVE, gpioIdx) < 0) {
        perror("无法打开GPIO!");
        close(fd);
        return;
    }
    if (i2c_smbus_read_byte_data(fd, IDENT) != 0x60) {
        perror("无法读取数据!");
        close(fd);
        return;
    }
    i2c_smbus_write_byte_data(fd, SOFT_RESET, 0xB6);
    usleep(50000);
    setCompensationParams(fd);
    i2c_smbus_write_byte_data(fd, CTRL_HUM, 0x1);
    i2c_smbus_write_byte_data(fd, CONFIG, 0);
    i2c_smbus_write_byte_data(fd, CTRL_MEAS, 0x25);
}
bool BME280::ReadData(){
    if ((i2c_smbus_read_byte_data(fd, 0xF3) & 0x9) != 0) {
            printf("%s\n", "无法读取温度数据!");
            return false;
    }
    i2c_smbus_read_i2c_block_data(fd, DATA_START_ADDR, DATA_LENGTH, dataBlock);
    i2c_smbus_write_byte_data(fd, CTRL_MEAS, 0x25);
    return true;
}
double BME280::GetTemp(){
    return BME280_compensate_T_double((dataBlock[3] << 16 | dataBlock[4] << 8 | dataBlock[5]) >> 4);
}
double BME280::GetPressure(){
    return BME280_compensate_P_double((dataBlock[0] << 16 | dataBlock[1] << 8 | dataBlock[2]) >> 4)/100.f;
}
double BME280::GetHumidity(){
    return BME280_compensate_H_double(dataBlock[6] << 8 | dataBlock[7]);
}

int main() {
    //初始化设备
    BME280 b(0x76);
    httplib::Server sv;
    sv.Get("/", [&b](const httplib::Request &, httplib::Response &res) {
        b.ReadData();
        char text[1024]="Unk";
        sprintf(text,"<html>"
        "<head>"
        "<meta charset=\"UTF-8\">"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0,maximum-scale=1.0\">"
        "<style>"
        ".card {"
            "width: 300px;"
            "padding: 16px;"
            "border: 1px solid #ddd;"
            "border-radius: 8px;"
            "box-shadow: 2px 2px 6px rgba(0, 0, 0, 0.1);"
            "background-color: #fff;"
            "font-family: Arial, sans-serif;"
            "margin: 20px;"
        "}"
        ".card p {"
            "margin: 0;"
            "margin-bottom: 10px;"
        "}"
    "</style>"
        "</head>"
        "<body>"
        "<div class=\"card\">"
        "<p>🌡温度: %.2f C<br/>💨气压: %.2f Pha<br/>💧湿度: %.2f %rH</p>"
        "</div>"
        "</body>"
        "</html>"
        ,b.GetTemp(),b.GetPressure(),b.GetHumidity());

        res.set_content(text, "text/html");
    });
    sv.listen("0.0.0.0", 1234);
    return 0;
}

