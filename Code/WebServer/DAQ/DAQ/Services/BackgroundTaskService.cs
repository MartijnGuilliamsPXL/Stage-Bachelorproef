using Microsoft.Extensions.Hosting;
using System;
using System.IO.Ports;
using System.Text;
using DAQ.Models;
using System.Globalization;
using System.Timers;
using System.Security.Cryptography.X509Certificates;

namespace DAQ.Services
{
    public class BackgroundTaskService : BackgroundService
    {
        //public Action TimerElapsed;
        private SerialPort _serialPort;
        private StringBuilder _receivedDataBuffer = new StringBuilder();
        public List<DataPoint> buffer = new List<DataPoint>(); // Buffer list to store data temporarily
        public DataPoint Data = new DataPoint();
        //public System.Timers.Timer? timer;
        //public bool addValues = false;
        private int count = 0;
        //public event EventHandler Handler;

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
                            string message = DateTime.Now + ",count:" + count + ",";
                            message = message + _receivedDataBuffer.ToString(0, newlineIndex);
                            _receivedDataBuffer.Remove(0, newlineIndex + Environment.NewLine.Length);

                            var values  = message.Split(',');

                            Console.WriteLine($"Received: {message}");
                            ParseAndAddData(message);
                            //OnValueUpdated(EventArgs.Empty);
                            //TimerElapsed?.Invoke();
                            SaveBufferToCSV();
                            count++;

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
            if (parts.Length == 8)
            {
                DataPoint newDataPoint = new DataPoint();
                newDataPoint.Time = DateTime.ParseExact(parts[0], "dd/MM/yyyy HH:mm:ss", CultureInfo.InvariantCulture);
                newDataPoint.Count = int.Parse(parts[1].Split(':')[1]);
                newDataPoint.RPM = int.Parse(parts[2].Split(':')[1]);
                newDataPoint.Temperature = decimal.Parse(parts[3].Split(':')[1], CultureInfo.InvariantCulture);
                newDataPoint.ThrustN = decimal.Parse(parts[4].Split(':')[1], CultureInfo.InvariantCulture);
                newDataPoint.TorqueNm = decimal.Parse(parts[5].Split(':')[1], CultureInfo.InvariantCulture);
                newDataPoint.VoltageV = decimal.Parse(parts[6].Split(':')[1], CultureInfo.InvariantCulture);
                newDataPoint.CurrentA = decimal.Parse(parts[7].Split(':')[1], CultureInfo.InvariantCulture);
                Data = newDataPoint;
            }
        }

        private async Task SaveBufferToCSV()
        {
            string filePath = "data.csv";
            //if (buffer.Count > 0)
            //{
            try
            {
                using (var writer = new StreamWriter(filePath, true)) // Append to existing file if it exists
                {
                    if(Data != null)
                    {
                        await writer.WriteLineAsync($"{Data.Time};{Data.Count};{Data.RPM};{Data.Temperature};{Data.ThrustN};{Data.TorqueNm};{Data.VoltageV};{Data.CurrentA}");
                    }
                    else
                    {
                       Console.WriteLine("Niet ok");
                    }
                }
                Console.WriteLine("buffer.Clear");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"FAILED SaveBufferToCSV: {ex.Message}");
            }

            //string dataToSend = "1200";
            //_serialPort.WriteLine(dataToSend);
        }
    }
}
