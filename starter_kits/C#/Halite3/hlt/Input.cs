using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Halite_Project.hlt {
    class Input {
        private string[] input;
        private int current;

        public Input(string line) {
            input = line.Split(' ');
        }

        public int GetInt() {
            return int.Parse(input[current++]);
        }

        public static Input ReadInput() {
            return new Input(ReadLine());
        }

        public static string ReadLine() {
            try {
                StringBuilder builder = new StringBuilder();

                int buffer;
                for(; (buffer = Console.Read()) >= 0;) {
                    if(buffer == '\n') {
                        break;
                    }
                    if(buffer == '\r') {
                        // Ignore carriage return if on windows for manual testing.
                        continue;
                    }
                    builder.Append((char)buffer);
                }

                return builder.ToString();
            }catch(Exception e) {
                Log.CreateLog("Input connection from server closed. Exiting...");
                Environment.Exit(1);
                throw new Exception(e.ToString());
            }
        }
    }
}
