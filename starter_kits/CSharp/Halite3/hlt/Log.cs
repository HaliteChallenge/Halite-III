using System.IO;

namespace Halite3.hlt
{
    /// <summary>
    /// A class that can be used to log messages to a file.
    /// </summary>
    public class Log
    {
        private TextWriter file;
        private static Log instance;

        private Log(TextWriter f)
        {
            file = f;
        }

        public static void Initialize(TextWriter f)
        {
            instance = new Log(f);
        }

        public static void LogMessage(string message)
        {
            try
            {
                instance.file.WriteLine(message);
                instance.file.Flush();
            }
            catch (IOException)
            {
            }
        }
    }
}
