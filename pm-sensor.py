#-- Standalone particulate matter measuring station.
#-- csokertesi 2023.11.22.

from sen44 import SEN44
from datetime import datetime
from time import sleep
import os

# Global constants
PREP_TIME = 10  # Spin the fan for x ms before taking the measurement values
MEASUREMENT_PERIOD = 50  # Measurement period in ms
sensor = SEN44()

sensor.reset()

print("Sensor init successfull.\n")

version = sensor.get_version()
print(f"Sensor info:\nFirmware: { version.firmware_major }.{ version.firmware_minor }\nHardware: { version.hardware_major }.{ version.hardware_minor }\n")

# Check if ./measurements/*.csv exists
if not os.path.exists("./measurements"):
    os.mkdir("./measurements")

def now() -> str: return datetime.now().strftime("%Y.%m.%d %H:%M:%S")
def now_file() -> str: return datetime.now().strftime("%Y.%m.%d_%H-%M-%S")

# Measurement
def take_measurement() -> str:
	sensor.start()
	sleep(PREP_TIME)
	measurement = sensor.measure()

	# TODO: send measurement values to server

	raw = f"{ now() };{ measurement.pm2p5 };{ measurement.pm10p0 };{ measurement.ambient_humidity };{ measurement.ambient_temp }"

	return raw

filename = f"./measurements/{ now_file() }.csv"

with open(filename, "w") as f:
    print(f"Starting measurement. Saving to { filename }")
    f.write("timestamp;pm2.5;pm10;humidity;temperature\n")
    while True:
        sleep(MEASUREMENT_PERIOD)
        mes = take_measurement()
        sensor.stop()
        f.write(f"{ mes }\n")
        print(f"{ mes }")





