using Microsoft.Extensions.Hosting;
using System;
using System.IO.Ports;
using System.Text;
using DAQ.Models;
using System.Globalization;
using System.Drawing;
using System.Timers;
using PSC.Blazor.Components.Chartjs.Models.Bar;

namespace DAQ.Services
{
    public class BackgroundTaskService : BackgroundService
    {
        public event Action TimerElapsed;
        private SerialPort _serialPort;
        private StringBuilder _receivedDataBuffer = new StringBuilder();
        public List<DataPoint> buffer = new List<DataPoint>(); // Buffer list to store data temporarily
        public System.Timers.Timer timer;
        public bool addValues = false;

        public BackgroundTaskService()
        {
            _serialPort = new SerialPort("COM4", 115200);
            _serialPort.DtrEnable = true;
            _serialPort.Open();
            Console.WriteLine("Created SerialService");

            timer = new System.Timers.Timer(100); // Set the interval to 1000 milliseconds (1 second)
            timer.Elapsed += OnTimerElapsed;
            timer.AutoReset = true;
            timer.Enabled = true;
        }

        private async Task ReadSerialPortAsync(CancellationToken cancellationToken)
        {
            var buffer = new byte[1024]; // Adjust the buffer size as needed

            while (!cancellationToken.IsCancellationRequested)
            {
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
                            string message = DateTime.Now + ",";
                            message = message + _receivedDataBuffer.ToString(0, newlineIndex);
                            _receivedDataBuffer.Remove(0, newlineIndex + Environment.NewLine.Length);

                            var values  = message.Split(',');

                            Console.WriteLine($"Received: {message}");
                            ParseAndAddData(message);

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


        private void ParseAndAddData(string rawData)
        {
            string[] parts = rawData.Split(',');
            if (parts.Length == 7)
            {
                DataPoint newDataPoint = new DataPoint();
                newDataPoint.Time = DateTime.ParseExact(parts[0], "dd/MM/yyyy HH:mm:ss", CultureInfo.InvariantCulture);
                newDataPoint.RPM = int.Parse(parts[1].Split(':')[1]);
                newDataPoint.Temperature = decimal.Parse(parts[2].Split(':')[1], CultureInfo.InvariantCulture);
                newDataPoint.ThrustN = decimal.Parse(parts[3].Split(':')[1], CultureInfo.InvariantCulture);
                newDataPoint.TorqueNm = decimal.Parse(parts[4].Split(':')[1], CultureInfo.InvariantCulture);
                newDataPoint.VoltageV = decimal.Parse(parts[5].Split(':')[1], CultureInfo.InvariantCulture);
                newDataPoint.CurrentA = decimal.Parse(parts[6].Split(':')[1], CultureInfo.InvariantCulture);

                buffer.Add(newDataPoint);
            }
        }

        private async Task SaveBufferToCSV()
        {
            string filePath = "data.csv";
            if (buffer.Count > 0)
            {
                using (var writer = new StreamWriter(filePath, true)) // Append to existing file if it exists
                {
                    foreach (var point in buffer)
                    {
                        await writer.WriteLineAsync($"{point.Time};{point.RPM};{point.Temperature};{point.ThrustN};{point.TorqueNm};{point.VoltageV};{point.CurrentA}");
                    }
                }
                Console.WriteLine("buffer.Clear");
                buffer.Clear(); // Clear the buffer after data is saved
            }
            //string dataToSend = "1200";
            //_serialPort.WriteLine(dataToSend);
        }

        private void OnTimerElapsed(object sender, ElapsedEventArgs e)
        {
            TimerElapsed?.Invoke();
            SaveBufferToCSV();
            //UpdateChart();
            //buffer.Clear();
        }

    }
}
