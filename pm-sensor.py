#-- Standalone particulate matter measuring station.
#-- csokertesi 2023.11.22.

from sen44 import SEN44
from datetime import datetime
from time import sleep
import os
import requests

# Global constants
PREP_TIME = 3  # Spin the fan for x seconds before taking the measurement values
MEASUREMENT_PERIOD = 7  # Measurement period in seconds
sensor = SEN44()

sensor.reset()

print("Sensor init successfull.\n")

version = sensor.get_version()
print(f"Sensor info:\nFirmware: { version.firmware_major }.{ version.firmware_minor }\nHardware: { version.hardware_major }.{ version.hardware_minor }\n")

# Check if ./measurements/*.csv exists
if not os.path.exists("./measurements"):
    os.mkdir("./measurements")

def now() -> str: return datetime.now().strftime("%Y/%m/%d;%H:%M:%S")
def now_file() -> str: return datetime.now().strftime("%Y.%m.%d_%H-%M-%S")

secret_key = ""
with open("./secret.key", "r") as f:
    secret_key = f.read().strip()

# Measurement
def take_measurement() -> str:
    sensor.reset()
    sensor.sleep(5)
    sensor.start()
    sleep(PREP_TIME)
    measurement = sensor.measure()
    date = datetime.now().strftime("%Y-%m-%d")
    time = datetime.now().strftime("%H:%M:%S")
    response = requests.get(f"https://kertesi.hu/measurements/upload?date={ date }&time={ time }&pm1p0={ measurement.pm1p0 }&pm2p5={ measurement.pm2p5 }&pm4p0={ measurement.pm4p0 }&pm10p0={ measurement.pm10p0 }&humidity={ measurement.ambient_humidity }&temp={ measurement.ambient_temp }&key={ secret_key }")
    raw = f"{ now() },{ measurement.pm1p0 },{measurement.pm2p5 },{ measurement.pm4p0 },{ measurement.pm10p0 },{ measurement.ambient_humidity }%,{ measurement.ambient_temp }"
    if response.text != "OK":
        print(f"Upload failed: { response.text }")
        return ""
    return raw

filename = f"./measurements/{ now_file() }.csv"

print(f"Starting measurement. Saving to { filename }")
try:
    while True:
        sleep(MEASUREMENT_PERIOD)
        with open(filename, "a") as f:
            f.write(f"{ take_measurement() }\n")
        sensor.stop()
except KeyboardInterrupt:
    print("Measurement stopped.")



