import u3
import time
import _thread
import csv

d = u3.U3()
rate = 10 #hz
bat = 0
filename = ""

def readAio():
    global bat
    milliseconds = int(round(time.time() * 1000))

    # Read the analog value from FIO0
    current = d.getAIN(0) * 35.5460075138381 # Reads from channel 0 (FIO0)
    voltage = d.getAIN(1) * 15.7237782689010 # Reads from channel 1 (FIO1)

    # Print the voltage and current reading
    print(f"Current on FIO0: {current:.8f} A")
    print(f"Voltage on FIO0: {voltage:.8f} V")

    print(str(milliseconds) + " " + str(current) + "A - "+ str(voltage) + "V - ")
    with open(filename, mode='a', newline='') as csv_file:
        writer = csv.writer(csv_file, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
        writer.writerow([milliseconds, current, voltage])
    bat = bat +1


def main():
    global filename

    # To learn if the U3 is an HV
    d.configU3()

    # For applying the proper calibration to readings.
    d.getCalibrationData()

    # Set all EIO to Analog
    #d.configIO(EIOAnalog=255)

    # Set FIO4 to digital output
    #d.getFeedback(u3.BitDirWrite(16, 1))

    # Set FIO4 to output high
    #d.getFeedback(u3.BitStateWrite(16, 0))

    #d.configIO(FIOAnalog=1)
    d.configIO(FIOAnalog = 0x03)

    
    filename = time.strftime("%Y%m%d_%H%M%S", time.localtime()) + ".csv"

    while (bat <= 100):
        _thread.start_new_thread(readAio, ())
        time.sleep(1.0/rate)

    # Set FIO4 to output high
    #d.getFeedback(u3.BitStateWrite(16, 1))

if __name__ == "__main__":
    main()
