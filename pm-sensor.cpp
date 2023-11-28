#include <iostream>
#include <dlfcn.h>
#include <cstring>

class Measurement {
public:
    int pm1p0, pm2p5, pm4p0, pm10p0;
    float voc_index, ambient_humidity, ambient_temp;

    Measurement(int pm1p0, int pm2p5, int pm4p0, int pm10p0, float voc_index, float ambient_humidity, float ambient_temp)
        : pm1p0(pm1p0), pm2p5(pm2p5), pm4p0(pm4p0), pm10p0(pm10p0), voc_index(voc_index), ambient_humidity(ambient_humidity), ambient_temp(ambient_temp) {}
};

class SEN44Version {
public:
    int firmware_major, firmware_minor, hardware_major, hardware_minor, protocol_major, protocol_minor;
    bool firmware_debug;

    SEN44Version(int firmware_major, int firmware_minor, bool firmware_debug, int hardware_major, int hardware_minor, int protocol_major, int protocol_minor)
        : firmware_major(firmware_major), firmware_minor(firmware_minor), firmware_debug(firmware_debug), hardware_major(hardware_major), hardware_minor(hardware_minor), protocol_major(protocol_major), protocol_minor(protocol_minor) {}
};

class SEN44 {
private:
    void* sen44;

    int (*init)();
    void (*reset)();
    int (*start)();
    void (*stop)();
    void (*sleep)(int);
    void (*measure)(int*, int*, int*, int*, float*, float*, float*);
    void (*get_serial)(char*);
    void (*get_version)(int*, int*, bool*, int*, int*, int*, int*);

public:
    SEN44() : sen44(nullptr) {
        sen44 = dlopen("./sensirion_sen44.so", RTLD_LAZY);
        if (!sen44) {
            std::cerr << "Error loading sensirion_sen44.so: " << dlerror() << std::endl;
            std::exit(1);
        }

        init = reinterpret_cast<int (*)()>(dlsym(sen44, "init"));
        reset = reinterpret_cast<void (*)()>(dlsym(sen44, "reset"));
        start = reinterpret_cast<int (*)()>(dlsym(sen44, "start"));
        stop = reinterpret_cast<void (*)()>(dlsym(sen44, "stop"));
        sleep = reinterpret_cast<void (*)(int)>(dlsym(sen44, "sleep"));
        measure = reinterpret_cast<void (*)(int*, int*, int*, int*, float*, float*, float*)>(dlsym(sen44, "measure"));
        get_serial = reinterpret_cast<void (*)(char*)>(dlsym(sen44, "get_serial"));
        get_version = reinterpret_cast<void (*)(int*, int*, bool*, int*, int*, int*, int*)>(dlsym(sen44, "get_version"));

        if (!init || !reset || !start || !stop || !sleep || !measure || !get_serial || !get_version) {
            std::cerr << "Error loading function pointers: " << dlerror() << std::endl;
            std::exit(1);
        }

        if (init() != 0) {
            std::cerr << "SEN44 failed to initialize." << std::endl;
            std::exit(1);
        }

        reset();
        started = false;
    }

    ~SEN44() {
        if (sen44) {
            dlclose(sen44);
        }
    }

    void start() {
        if (start() != 0) {
            std::cerr << "SEN44 failed to execute: start()" << std::endl;
            std::exit(1);
        }
        started = true;
    }

    void sleep(int ms) {
        sleep(ms);
    }

    void reset() {
        if (reset() != 0) {
            std::cerr << "SEN44 failed to execute: reset()" << std::endl;
            std::exit(1);
        }
    }

    Measurement measure() {
        if (!started) {
            std::cerr << "SEN44 attempted measurement before start" << std::endl;
            std::exit(1);
        }

        int pm1p0, pm2p5, pm4p0, pm10p0;
        float voc_index, ambient_humidity, ambient_temp;

        measure(&pm1p0, &pm2p5, &pm4p0, &pm10p0, &voc_index, &ambient_humidity, &ambient_temp);

        return Measurement(pm1p0, pm2p5, pm4p0, pm10p0, voc_index, ambient_humidity, ambient_temp);
    }

    std::string get_serial() {
        char serial[32];
        get_serial(serial);
        return std::string(serial);
    }

    SEN44Version get_version() {
        int firmware_major, firmware_minor;
        bool firmware_debug;
        int hardware_major, hardware_minor, protocol_major, protocol_minor;

        get_version(&firmware_major, &firmware_minor, &firmware_debug, &hardware_major, &hardware_minor, &protocol_major, &protocol_minor);

        return SEN44Version(firmware_major, firmware_minor, firmware_debug, hardware_major, hardware_minor, protocol_major, protocol_minor);
    }

    void stop() {
        if (stop() != 0) {
            std::cerr << "SEN44 failed to execute: stop()" << std::endl;
            std::exit(1);
        }
        started = false;
    }

private:
    bool started;
};

int main() {
    SEN44 sen44;

    sen44.start();
    sen44.sleep(1000);
    Measurement measurement = sen44.measure();
    SEN44Version version = sen44.get_version();
    sen44.stop();

    std::cout << "Measurement: PM1.0 = " << measurement.pm1p0 << ", PM2.5 = " << measurement.pm2p5
              << ", PM4.0 = " << measurement.pm4p0 << ", PM10.0 = " << measurement.pm10p0 << std::endl;
    std::cout << "VOC Index = " << measurement.voc_index << ", Ambient Humidity = " << measurement.ambient_humidity
              << ", Ambient Temperature = " << measurement.ambient_temp << std::endl;
    std::cout << "Version: Firmware " << version.firmware_major << "." << version.firmware_minor
              << (version.firmware_debug ? " (Debug)" : "") << ", Hardware " << version.hardware_major << "."
              << version.hardware_minor << ", Protocol " << version.protocol_major << "." << version.protocol_minor << std::endl;

    return 0;
}
