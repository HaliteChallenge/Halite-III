using System;
using System.Collections.Generic;
using System.Collections;
using System.Linq;
using System.Text;

namespace MyBot.hlt
{
    class Input
    {
        private string[] input;
        private int current;

        public Input(string line)
        {
            input = line.Split(" ");
        }

        public int getInt()
        {
            return int.Parse(input[current++]);
        }

        public static Input readInput()
        {
            return new Input(readLine());
        }

        public static string readLine()
        {
            try
            {
                StringBuilder builder = new StringBuilder();

                int buffer;
                for (; (buffer = Console.In.Read()) >= 0;) {
                    if (buffer == '\n')
                    {
                        break;
                    }
                    if (buffer == '\r')
                    {
                        // Ignore carriage return if on windows for manual testing.
                        continue;
                    }
                    builder.Append((char)buffer);
                }

                return builder.ToString();
            }
            catch (Exception e)
            {
                Log.log("Input connection from server closed. Exiting...");
                throw new Exception(e.Message, e);
            }
        }
    }
}
