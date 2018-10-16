using System;
using NLog;
using NLog.Config;
using NLog.Targets;

namespace Halite3.Core
{
    public static class Log
    {
        private static Logger loggerInstance;

        public static Logger logger()
        {
            if (Log.loggerInstance == null)
            {
                var config = new LoggingConfiguration();

                var fileTarget = new FileTarget("target2")
                {
                    FileName = "${basedir}/log.log",
                    Layout = "${longdate} ${level} ${message}  ${exception}"
                };
                config.AddTarget(fileTarget);

                config.AddRuleForAllLevels(fileTarget);

                LogManager.Configuration = config;

                Log.loggerInstance = LogManager.GetLogger("MyBot");
            }
            return Log.loggerInstance;
        }
    }
}