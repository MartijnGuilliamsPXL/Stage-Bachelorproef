import u3
import time
import _thread
import csv

d = u3.U3()
rate = 10 #hz
bat = 25
filename = ""

def readAio():
    global bat
    milliseconds = int(round(time.time() * 1000))
    sen1v = round(d.getAIN(15)/0.063694268,4)
    sen1a = round(d.getAIN(14)/0.0183,4)
    senc1v = round(d.getAIN(8)/0.5,4)
    senc2v = round(d.getAIN(9)/0.5,4)
    senc3v = round(d.getAIN(10)/0.5,4)
    senc4v = round(d.getAIN(11)/0.5,4)
    senc5v = round(d.getAIN(12)/0.5,4)
    senc6v = round(d.getAIN(13)/0.5,4)
    bat = sen1v
    print(str(milliseconds) + " " + str(sen1v) + "V - " + str(sen1a) + "A - " + str(senc1v) + "V - " + str(senc2v) + "V - "+ str(senc3v) + "V - "+ str(senc4v) + "V - "+ str(senc5v) + "V - "+ str(senc6v) + "V - ")
    with open(filename, mode='a', newline='') as csv_file:
        writer = csv.writer(csv_file, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
        writer.writerow([milliseconds, sen1a, sen1v, senc1v, senc2v, senc3v, senc4v, senc5v, senc6v])


def main():
    global filename

    # To learn the if the U3 is an HV
    d.configU3()

    # For applying the proper calibration to readings.
    d.getCalibrationData()

    # Set all EIO to Analog
    d.configIO(EIOAnalog=255)

    # Set FIO4 to digital output
    d.getFeedback(u3.BitDirWrite(16, 1))

    # Set FIO4 to output high
    d.getFeedback(u3.BitStateWrite(16, 0))

    
    filename = time.strftime("%Y%m%d_%H%M%S", time.localtime()) + ".csv"

    while (bat >= 20):
        _thread.start_new_thread(readAio, ())
        time.sleep(1.0/rate)

    # Set FIO4 to output high
    d.getFeedback(u3.BitStateWrite(16, 1))

if __name__ == "__main__":
    main()
