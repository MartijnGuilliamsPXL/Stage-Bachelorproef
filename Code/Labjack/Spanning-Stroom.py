import u3
import time
import _thread
import csv

d = u3.U3()
rate = 10 #hz
bat = 100
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
    bat = bat -1


def main():
    global filename

    d.configU3()
    d.getCalibrationData()
    d.configIO(FIOAnalog = 0x03)

    filename = time.strftime("%Y%m%d_%H%M%S", time.localtime()) + ".csv"

    while (bat >= 0):
        _thread.start_new_thread(readAio, ())
        time.sleep(1.0/rate)

if __name__ == "__main__":
    main()
