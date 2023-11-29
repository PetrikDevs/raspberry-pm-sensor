#include <iostream>
#include <dlfcn.h>
#include <stdlib.h>
#include <cstring>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <fstream>

#define PREP_TIME 3000
#define MEASUREMENT_PERIOD 7000

std::string get_date() {
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d");
	return ss.str();
}

std::string get_time() {
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%H:%M:%S");
	return ss.str();
}

std::string get_time_file() {
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%H-%M-%S");
	return ss.str();

void checkerr(uint8_t err, const char* msg) {
	if (err != 0) {
		std::cerr << "Error: " << msg << ": " << err << '\n';
		exit(1);
	}
}

int main(int argc, char* argv[]) {
	void* handle = dlopen("./sensirion_sen44.so", RTLD_LAZY);
	
	int (*init)(void) = reinterpret_cast<int (*)(void)>(dlsym(handle, "init"));
	int (*reset)(void) = reinterpret_cast<int (*)(void)>(dlsym(handle, "reset"));
	int (*start)(void) = reinterpret_cast<int (*)(void)>(dlsym(handle, "start"));
	int (*stop)(void) = reinterpret_cast<int (*)(void)>(dlsym(handle, "stop"));
	int (*measure)(int16_t*, int16_t*, int16_t*, int16_t*, float*, float*, float*) = reinterpret_cast<int (*)(int16_t*, int16_t*, int16_t*, int16_t*, float*, float*, float*)>(dlsym(handle, "measure"));
	void (*sleep)(int) = reinterpret_cast<void (*)(int)>(dlsym(handle, "sleep"));
	void (*get_serial)(char*) = reinterpret_cast<void (*)(char*)>(dlsym(handle, "get_serial"));
	void (*get_version)(int*, int*, bool*, int*, int*, int*, int*) = reinterpret_cast<void (*)(int*, int*, bool*, int*, int*, int*, int*)>(dlsym(handle, "get_version"));

	uint8_t err;
	err = init();
	checkerr(err, "init");
	err = reset();
	checkerr(err, "reset");
	
	// If ./measurements/ doesn't exist, create it
	std::string dir = "./measurements/";
	if (opendir(dir.c_str()) == NULL) {
		mkdir(dir.c_str(), 0777);
	}

	// Create ./measurements/DATE_TIMEFILE.csv
	std::string filename = dir + get_date() + "_" + get_time_file() + ".csv";
	std::ofstream file(filename);

	while (true) {
		sleep(50);
		err = start();
		checkerr(err, "start");
		sleep(PREP_TIME);
		int16_t pm1p0, pm2p5, pm4p0, pm10p0;
		float voc, humidity, temp;
		measure(&pm1p0, &pm2p5, &pm4p0, &pm10p0, &voc, &humidity, &temp);
		
		// Write to file
		file << get_time() << "," << pm1p0 << "," << pm2p5 << "," << pm4p0 << "," << pm10p0 << "," << voc << "," << humidity << "," << temp << '\n';


		err = stop();
		sleep(MEASUREMENT_PERIOD);
	}

	return 0;
}


