using System;

namespace Halite3.Core
{
    public class Input
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
            return new Input(ReadLine());
        }

        public static string ReadLine()
        {
            return Console.ReadLine();
        }
    }
}
