using System;
using System.Collections.Generic;
using System.Text;

namespace Halite3.hlt
{
    /// <summary>
    /// A class that wraps Console.Readline() and int.Parse() with exception handling.
    /// </summary>
    public class Input
    {
        private readonly string[] input;
        private int current;

        public Input(string line)
        {
            input = line.Split(" ");
        }

        public int GetInt()
        {
            return int.Parse(input[current++]);
        }

        public static Input ReadInput()
        {
            return new Input(ReadLine());
        }

        public static String ReadLine()
        {
            try
            {
                StringBuilder builder = new StringBuilder();

                int buffer;
                for (; (buffer = Console.Read()) >= 0;)
                {
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
                Log.LogMessage("Input connection from server closed. Exiting...");
                throw new InvalidOperationException(e.Message);
            }
        }
    }
}
