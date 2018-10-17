using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Halite_Project.hlt {
    class Log {
        private FileStream file;
        private StreamWriter fileWriter;

        private static Log INSTANCE;
        private static List<string> LOG_BUFFER = new List<string>();

        private static void AtExit() {
            if(INSTANCE != null) {
                return;
            }

            long now_in_nanos = Stopwatch.GetTimestamp() * 10000L;
            now_in_nanos /= TimeSpan.TicksPerMillisecond;
            now_in_nanos *= 100L;
            string filename = "bot-unknown-" + now_in_nanos + ".log";
            try{
                FileStream fileStream = File.Open(filename, FileMode.Create);
                StringBuilder stringBuilder = new StringBuilder();

                foreach (string message in LOG_BUFFER) {
                    stringBuilder.AppendLine(message);
                }

                byte[] bytes = new UTF8Encoding(true).GetBytes(stringBuilder.ToString());
                fileStream.Write(bytes, 0, bytes.Length);
                fileStream.Close();
            } catch (IOException) {
                // Nothing much we can do here.
            }
        }

        private Log(FileStream f) {
            file = f;
            fileWriter = new StreamWriter(file);
        }

        public static void Open(int botId) {
            if(INSTANCE != null) {
                Log.CreateLog("Error: log: tried to open(" + botId + ") but we have already opened before.");
                throw new Exception();
            }

            string filename = "bot-" + botId + ".log";
            FileStream writer;
            try {
                writer = new FileStream(filename, FileMode.Create);
            }catch(IOException e) {
                throw new Exception(e.ToString());
            }
            INSTANCE = new Log(writer);

            StreamWriter writerStream = new StreamWriter(writer);

            try {
                foreach(string message in LOG_BUFFER){
                    writerStream.WriteLine(message);
                }
            }catch(IOException e) {
                throw new Exception(e.ToString());
            }

            LOG_BUFFER.Clear();
        }

        public static void CreateLog(string message) {
            if(INSTANCE == null) {
                LOG_BUFFER.Add(message);
                return;
            }

            try {
                INSTANCE.fileWriter.WriteLine(message);
                INSTANCE.fileWriter.Flush();
            }catch(IOException e) {
                Console.Error.WriteLine(e.ToString());
            }
        }
    }
}
