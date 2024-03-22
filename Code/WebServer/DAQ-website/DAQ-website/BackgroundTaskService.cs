using System;
using System.Globalization;
using System.IO.Ports;
using System.Threading;
using System.Threading.Tasks;

public class BackgroundTaskService
{
    private readonly CancellationTokenSource _cancellationTokenSource = new CancellationTokenSource();
    private SerialPort _serialPort; // Declare a private field for the serial port

    public void Start()
    {
        Task.Run(() => ReadSerialData(_cancellationTokenSource.Token));
    }

    private void ReadSerialData(CancellationToken cancellationToken)
    {
        try
        {
            while (!cancellationToken.IsCancellationRequested)
            {
                if (_serialPort == null)
                {
                    _serialPort = new SerialPort("COM4", 115200);
                    _serialPort.Open();
                    Console.WriteLine("Reading data from COM4...");
                }

                if (_serialPort.BytesToRead > 0)
                {
                    var dataline = _serialPort.ReadLine();
                    var values = dataline.Split(',');

                    // Split the string by ":"
                    var parts = values[1].Split(':');



                    Console.WriteLine($"Received: {values[1]}");
                    Console.WriteLine($"Received: {parts[1]}");
                    //Console.WriteLine($"Received: {dataline}");
                }
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Error reading from COM4: {ex.Message}");
        }
        finally
        {
            _serialPort?.Close(); // Close the serial port even if an exception occurs
            _serialPort = null; // Reset the serial port reference
            Console.WriteLine("Serial port closed.");
        }
    }

    public void Stop()
    {
        _cancellationTokenSource.Cancel();
    }
}
