namespace DAQ.Models
{
    public class DataPoint
    {
        public DateTime Time { get; set; }
        public int RPM { get; set; }
        public double Temperature { get; set; }
        public double ThrustN { get; set; }
        public double TorqueNm { get; set; }
        public double VoltageV { get; set; }
        public double CurrentA { get; set; }
    }

}
