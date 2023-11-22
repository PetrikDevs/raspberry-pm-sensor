# raspberry-pm-sensor
Standalone particulate matter measuring station.
Made for Raspberry Pi 4 with Sensirion SEN44 sensor.

**Connection: UART**

## sen44.py module example usage
```py
from sen44 import SEN44

sensor = SEN44()  # Automatic initialization of sensor

sensor.start()  # Start measurement (also starts ventillation)

for _ in range(10):
  measurement = sensor.measure()  # -> Measure object from sen44
  print(f"Ambient temperature: { measurement.ambient_temp }")
  print(f"Ambient humidity: { measurement.ambient_humidity }%")
  print(f"pm10p0: { measurement.pm10p0 }  pm2p5: { measurement.pm2p5 }\n")

  sensor.sleep(5000)  # 5s

sensor.stop()
```

SEN44 Driver: [Sensirion](https://github.com/Sensirion/raspberry-pi-uart-sen44)

*This is a work in progress.*
