using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Threading;

namespace MyBot.hlt
{
    static class Log
    {
        const string pathToLog = "BotLog";

        public static void log(string message)
        {
            start:
            try
            {
                File.AppendAllLines(pathToLog + Constants.BOT_ID + ".txt", new string[] { message });
            }
            catch (Exception)
            {
                Thread.Sleep(10);
                goto start;
            }
        }
    }
}