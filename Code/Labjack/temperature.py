import u3
import time

# Function to read temperature from DS18B20 sensor
def read_temperature(d):
    # Read temperature using Modbus protocol
    # You may need to adjust the Modbus address and register based on your sensor configuration
    temperature = d.readRegister(30001, 0, 4)  # Assuming DS18B20 is connected to FIO0
    # Convert raw temperature to Celsius
    temperature_celsius = temperature / 10.0
    return temperature_celsius

# Main function
def main():
    # Initialize LabJack U3
    d = u3.U3()

    # Set FIO0 to digital output
    d.configIO(FIOAnalog = 0)

    # Configure FIO0 as an output to control DS18B20 sensor
    d.getFeedback(u3.PortDirWrite(Direction = [1, 0, 0]))  # Set FIO0 as output

    # Main loop
    while True:
        # Start temperature conversion
        d.getFeedback(u3.BitDirWrite(IONumber = 0, Direction = 1))  # Set FIO0 high

        # Wait for temperature conversion to complete
        time.sleep(1)  # Adjust this delay as needed based on DS18B20 conversion time

        # Read temperature
        temperature = read_temperature(d)

        # Print temperature
        print("Temperature: {:.2f} °C".format(temperature))

        # Delay before next reading
        time.sleep(1)  # Adjust this delay as needed

if __name__ == "__main__":
    main()
