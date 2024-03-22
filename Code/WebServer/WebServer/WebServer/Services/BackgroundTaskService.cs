using System.Globalization;
using System.IO.Ports;
using WebServer.Models;

namespace WebServer.Services
{
    public class BackgroundTaskService : IHostedService, IAsyncDisposable
    {
        //private readonly CancellationTokenSource _cancellationTokenSource = new CancellationTokenSource();
        private SerialPort _serialPort; // Declare a private field for the serial port

        public ValueTask DisposeAsync()
        {
            throw new NotImplementedException();
        }

        public Task StartAsync(CancellationToken cancellationToken)
        {
            while (true)
            {
                DAQ myObj = new DAQ();

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
                    myObj.Temperature = double.Parse(parts[1], CultureInfo.InvariantCulture);


                    Console.WriteLine($"Received: {values[1]}");
                    Console.WriteLine($"Received: {myObj.Temperature}");
                    //Console.WriteLine($"Received: {dataline}");
                }
            }
            throw new NotImplementedException();
        }

        public Task StopAsync(CancellationToken cancellationToken)
        {
            throw new NotImplementedException();
        }
    }
}
