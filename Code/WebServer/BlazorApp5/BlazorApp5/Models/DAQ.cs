namespace BlazorApp5.Models
{
    public class DataPoint
    {
        public DateTime Time { get; set; }
        public int Count { get; set; }
        public int RPM { get; set; }
        public decimal Temperature { get; set; }
        public decimal ThrustN { get; set; }
        public decimal TorqueNm { get; set; }
        public decimal VoltageV { get; set; }
        public decimal CurrentA { get; set; }
    }

}
