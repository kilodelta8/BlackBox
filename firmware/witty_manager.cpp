#include <iostream>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>

class BlackBoxEngine {
    int i2c_file;
    const int WITTY_ADDR = 0x08;
    const char* SOCKET_PATH = "/tmp/blackbox.sock";

public:
    BlackBoxEngine() {
        i2c_file = open("/dev/i2c-1", O_RDWR);
        ioctl(i2c_file, I2C_SLAVE, WITTY_ADDR);
    }

    float getVin() {
        unsigned char v_int, v_dec;
        unsigned char r1 = 1, r2 = 2;
        write(i2c_file, &r1, 1); read(i2c_file, &v_int, 1);
        write(i2c_file, &r2, 1); read(i2c_file, &v_dec, 1);
        return (float)v_int + (v_dec / 100.0f);
    }

    void sendToUI(std::string msg) {
        int sock = socket(AF_UNIX, SOCK_STREAM, 0);
        struct sockaddr_un addr;
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path)-1);

        if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
            send(sock, msg.c_str(), msg.length(), 0);
        }
        close(sock);
    }

    ~BlackBoxEngine() { close(i2c_file); }
};

int main() {
    BlackBoxEngine engine;
    while(true) {
        float voltage = engine.getVin();
        // Format: "VOLT:13.8"
        engine.sendToUI("VOLT:" + std::to_string(voltage));
        usleep(500000); // Update twice per second
    }
    return 0;
}
