#include <iostream>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <thread>

class BlackBoxEngine {
    int i2c_file;
    const int WITTY_ADDR = 0x08;
    const char* TX_SOCKET = "/tmp/bb_tx.sock"; // C++ to Python
    const char* RX_SOCKET = "/tmp/bb_rx.sock"; // Python to C++
    bool stay_on_override = false;

public:
    BlackBoxEngine() {
        i2c_file = open("/dev/i2c-1", O_RDWR);
        ioctl(i2c_file, I2C_SLAVE, WITTY_ADDR);
        std::thread(&BlackBoxEngine::listenForUI, this).detach();
    }

    void listenForUI() {
        if (os.path.exists(RX_SOCKET)) unlink(RX_SOCKET);
        int server_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
        struct sockaddr_un addr;
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, RX_SOCKET, sizeof(addr.sun_path)-1);
        bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));

        char buffer[1024];
        while (true) {
            int n = read(server_fd, buffer, sizeof(buffer));
            if (n > 0) {
                std::string msg(buffer, n);
                if (msg == "STAY_ON") stay_on_override = true;
                if (msg == "CANCEL_STAY") stay_on_override = false;
            }
        }
    }

    float getVin() {
        unsigned char v_int, v_dec, r1 = 1, r2 = 2;
        write(i2c_file, &r1, 1); read(i2c_file, &v_int, 1);
        write(i2c_file, &r2, 1); read(i2c_file, &v_dec, 1);
        return (float)v_int + (v_dec / 100.0f);
    }

    void broadcast(std::string msg) {
        int sock = socket(AF_UNIX, SOCK_STREAM, 0);
        struct sockaddr_un addr;
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, TX_SOCKET, sizeof(addr.sun_path)-1);
        if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
            send(sock, msg.c_str(), msg.length(), 0);
        }
        close(sock);
    }

    void run() {
        int counter = 60;
        while (true) {
            float v = getVin();
            broadcast("VOLT:" + std::to_string(v));
            if (v < 13.0 && !stay_on_override) {
                if (counter > 0) {
                    broadcast("WARN:" + std::to_string(counter--));
                } else {
                    system("sudo shutdown -h now");
                }
            } else {
                counter = 60; // Reset
            }
            sleep(1);
        }
    }
};

int main() { BlackBoxEngine e; e.run(); return 0; }
