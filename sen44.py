import ctypes
from ctypes import byref as ptr
from time import sleep

sen44 = ctypes.CDLL("./sensirion_sen44.so")

class Measurement:
    def __init__(self, pm1p0: int, pm2p5: int, pm4p0: int, pm10p0: int, voc_index: float, ambient_humidity: float, ambient_temp: float) -> None:
        self.pm1p0, self.pm2p5, self.pm4p0, self.pm10p0, self.voc_index, self.ambient_humidity, self.ambient_temp = pm1p0, pm2p5, pm4p0, pm10p0, voc_index, ambient_humidity, ambient_temp

class SEN44:
    def __init__(self) -> None:
        if sen44.init() != 0:
            raise RuntimeError("SEN44 failed to initialize.")

        sen44.reset()
        self.started = False

    def start(self) -> None:
        if sen44.start() != 0:
            raise RuntimeError("SEN44 failed to execute: start()")
        sleep(1.5)  # Wait for actual start-up
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
        version = ctypes.create_string_buffer(32)
        sen44.get_version(version)
        return version.value.decode("utf-8")

    def stop(self) -> None:
        if sen44.stop() != 0:
            raise RuntimeError("SEN44 failed to execute: stop()")
        self.started = False

