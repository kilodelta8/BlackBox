#include <iostream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <unistd.h>

class WittyPiEngine {
    int file;
    const int addr = 0x08;

public:
    WittyPiEngine() {
        file = open("/dev/i2c-1", O_RDWR);
        ioctl(file, I2C_SLAVE, addr);
    }

    float getInVoltage() {
        // Registers 1 (int) and 2 (dec) for Vin
        unsigned char reg1 = 1, reg2 = 2;
        unsigned char v_int, v_dec;
        
        write(file, &reg1, 1);
        read(file, &v_int, 1);
        write(file, &reg2, 1);
        read(file, &v_dec, 1);

        return (float)v_int + (float)v_dec / 100.0f;
    }

    ~WittyPiEngine() { close(file); }
};

int main() {
    WittyPiEngine wp;
    while(true) {
        float voltage = wp.getInVoltage();
        std::cout << "Equinox Battery: " << voltage << "V" << std::endl;
        
        // Logic: If voltage < 13.0, car is likely off.
        if (voltage < 13.0) {
            std::cout << "CRITICAL: Ignition Off. Signaling UI..." << std::endl;
        }
        sleep(2);
    }
    return 0;
}
