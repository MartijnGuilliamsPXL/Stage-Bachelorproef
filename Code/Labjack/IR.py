import u3
import time

# Open the first found LabJack U3
device = u3.U3()

# Ensure the device is opened and communication is established
device.getCalibrationData()

# Configure the FIO4 as a counter for the ST0050 IR sensor
# Assuming the sensor's output is connected to FIO4
device.configIO(FIOAnalog=0b11101111, EnableCounter1=True)

# Initialize variables
last_pulses = 0
last_time = time.time() * 1e6  # Current time in microseconds

# Main loop
print("Starting RPM measurement...")
try:
    while True:
        # Read the counter value (number of pulses)
        # Reset the counter by setting Reset=True
        pulses = device.getFeedback(u3.Counter1(Reset=True))[0]

        # If pulses have changed, calculate RPM
        if pulses != last_pulses:
            current_time = time.time() * 1e6  # Current time in microseconds
            time_interval_microseconds = current_time - last_time
            if time_interval_microseconds > 0:
                rpm = (1 / (time_interval_microseconds * 1e-6)) * 60
                print(f"RPM: {rpm:.2f}")
            last_time = current_time
            last_pulses = pulses
except KeyboardInterrupt:
    # Close the device on a keyboard interrupt (Ctrl+C)
    print("Stopping RPM measurement...")
    device.close()
