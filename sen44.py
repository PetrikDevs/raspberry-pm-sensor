import ctypes
from ctypes import byref as ptr
from time import sleep

sen44 = ctypes.CDLL("./sensirion_sen44.so")

class Measurement:
    def __init__(self, pm1p0: int, pm2p5: int, pm4p0: int, pm10p0: int, voc_index: float, ambient_humidity: float, ambient_temp: float) -> None:
        self.pm1p0, self.pm2p5, self.pm4p0, self.pm10p0, self.voc_index, self.ambient_humidity, self.ambient_temp = pm1p0, pm2p5, pm4p0, pm10p0, voc_index, ambient_humidity, ambient_temp

class SEN44Version:
    def __init__(self, firmware_major: int, firmware_minor: int, firmware_debug: bool, hardware_major: int, hardware_minor: int, protocol_major: int, protocol_minor: int) -> None:
        self.firmware_major, self.firmware_minor, self.firmware_debug, self.hardware_major, self.hardware_minor, self.protocol_major, self.protocol_minor = firmware_major, firmware_minor, firmware_debug, hardware_major, hardware_minor, protocol_major, protocol_minor


class SEN44:
    def __init__(self) -> None:
        if sen44.init() != 0:
            raise RuntimeError("SEN44 failed to initialize.")

        sen44.reset()
        self.started = False

    def start(self) -> None:
        if sen44.start() != 0:
            raise RuntimeError("SEN44 failed to execute: start()")
        self.started = True

    def sleep(self, ms: int) -> None:
        sen44.sleep(ms)

    def reset(self) -> None:
        if sen44.reset() != 0:
            raise RuntimeError("SEN44 failed to execute: reset()")

    def measure(self) -> Measurement:
        if not self.started:
            raise RuntimeError("SEN44 attempted measurement before start")
        pm1p0 = ctypes.c_int()
        pm2p5 = ctypes.c_int()
        pm4p0 = ctypes.c_int()
        pm10p0 = ctypes.c_int()
        voc_index = ctypes.c_float()
        ambient_humidity = ctypes.c_float()
        ambient_temp = ctypes.c_float()
        sen44.measure( ptr(pm1p0), ptr(pm2p5), ptr(pm4p0), ptr(pm10p0), ptr(voc_index), ptr(ambient_humidity), ptr(ambient_temp) )

        return Measurement(pm1p0.value, pm2p5.value, pm4p0.value, pm10p0.value, voc_index.value, ambient_humidity.value, ambient_temp.value)
    
    def get_serial(self) -> str:
        serial = ctypes.create_string_buffer(32)
        sen44.get_serial(serial)
        return serial.value.decode("utf-8")
    
    def get_version(self) -> str:
        firmware_major = ctypes.c_int()
        firmware_minor = ctypes.c_int()
        firmware_debug = ctypes.c_bool()
        hardware_major = ctypes.c_int()
        hardware_minor = ctypes.c_int()
        protocol_major = ctypes.c_int()
        protocol_minor = ctypes.c_int()
        sen44.get_version(ptr(firmware_major), ptr(firmware_minor), ptr(firmware_debug), ptr(hardware_major), ptr(hardware_minor), ptr(protocol_major), ptr(protocol_minor))

        return SEN44Version(firmware_major.value, firmware_minor.value, firmware_debug.value, hardware_major.value, hardware_minor.value, protocol_major.value, protocol_minor.value)
        
    def stop(self) -> None:
        if sen44.stop() != 0:
            raise RuntimeError("SEN44 failed to execute: stop()")
        self.started = False

