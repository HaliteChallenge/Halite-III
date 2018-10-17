using System.IO;

namespace Halite3.Core
{
    public static class Log
    {
        public static void Info(string message)
        {
            var sw = new StreamWriter(".\\log.log", true);
            sw.WriteLine(message);
            sw.Close();
        }
    }
}
