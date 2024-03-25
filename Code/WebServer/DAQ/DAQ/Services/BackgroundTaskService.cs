using Microsoft.Extensions.Hosting;
using System;
using System.IO.Ports;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using DAQ.Models;
using System.Globalization;

namespace DAQ.Services
{
    public class BackgroundTaskService : BackgroundService
    {
        private SerialPort _serialPort;
        private StringBuilder _receivedDataBuffer = new StringBuilder();

        public BackgroundTaskService()
        {
            _serialPort = new SerialPort("COM4", 115200);
            _serialPort.DtrEnable = true;
            _serialPort.Open();
            Console.WriteLine("Created SerialService");
        }

        private async Task ReadSerialPortAsync(CancellationToken cancellationToken)
        {
            var buffer = new byte[1024]; // Adjust the buffer size as needed

            while (!cancellationToken.IsCancellationRequested)
            {
                Models.Data myObj = new Models.Data();
                try
                {
                    int bytesRead = await _serialPort.BaseStream.ReadAsync(buffer, 0, buffer.Length, cancellationToken);

                    if (bytesRead > 0)
                    {
                        string data = Encoding.ASCII.GetString(buffer, 0, bytesRead);
                        _receivedDataBuffer.Append(data);

                        // Check if we have received a complete message
                        int newlineIndex;
                        while ((newlineIndex = _receivedDataBuffer.ToString().IndexOf(Environment.NewLine)) >= 0)
                        {
                            string message = _receivedDataBuffer.ToString(0, newlineIndex);
                            _receivedDataBuffer.Remove(0, newlineIndex + Environment.NewLine.Length);

                            Console.WriteLine($"Received: {message}");
                            var values = message.Split(',');
                            // Split the string by ":"
                            var parts = values[4].Split(':');
                            myObj.Voltage = double.Parse(parts[1], CultureInfo.InvariantCulture);


                            //Console.WriteLine($"Received: {values[4]}");
                            //Console.WriteLine($"Received: {myObj.Voltage}");
                            //Console.WriteLine($"Received: {dataline}");

                            // Calculate frequency or perform other operations here
                        }
                    }
                }
                catch (OperationCanceledException)
                {
                    // Task was cancelled, exit the loop
                    break;
                }
                catch (Exception ex)
                {
                    // Handle other exceptions
                    Console.WriteLine($"Error reading from serial port: {ex.Message}");
                }
            }
        }

        public override async Task StopAsync(CancellationToken cancellationToken)
        {
            _serialPort?.Close();
            await base.StopAsync(cancellationToken);
        }

        protected override async Task ExecuteAsync(CancellationToken stoppingToken)
        {
            await ReadSerialPortAsync(stoppingToken);
        }
    }
}
