import u3
import time


# LabJack U3 device
d = u3.U3()

# Configure SDA and SCL pins
d.getFeedback(u3.BitDirWrite(4, 1))  # Set FIO4 as output for SDA
d.getFeedback(u3.BitDirWrite(5, 1))  # Set FIO5 as output for SCL

# Create a BNO055 instance using default I2C address
import adafruit_bno055
import board

i2c = board.I2C()

sensor = adafruit_bno055.BNO055_I2C(i2c)
# Main loop
while True:
    # Read orientation data
    print(sensor.temperature)
    print(sensor.euler)
    print(sensor.gravity)
