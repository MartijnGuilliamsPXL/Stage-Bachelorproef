using System.Xml.Linq;

namespace WebServer.Models
{
    class DAQ
    {
        public double Temperature
        { get; set; }
        public double RPM
        { get; set; }
        public double Thrust
        { get; set; }
        public double Torque
        { get; set; }
        public double Voltage
        { get; set; }
        public double Current 
        { get; set; }
    }
}
