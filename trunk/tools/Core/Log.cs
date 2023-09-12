using System;
using System.Collections.Generic;


namespace dc.tools.core
{
    public enum eLogType
    {
        DEBUG = 0,
        INFO,
        WARNING,
        ERROR,
        EXCEPTION,
        MAX,
    }
    public static class Log
    {
        public delegate void LogEvt(eLogType type, string msg);
        private static LogEvt _log_evt = null;
        public static void SetLogEvt(LogEvt evt) { _log_evt = evt; }

        public static void Debug(string msg)
        {
            Console.ForegroundColor = ConsoleColor.White; //设置前景色，即字体颜色
            Console.WriteLine(msg);
        }
        public static void Info(string msg)
        {
            Console.ForegroundColor = ConsoleColor.White; //设置前景色，即字体颜色
            Console.WriteLine(msg);
            if (_log_evt != null) _log_evt(eLogType.INFO, msg);
        }
        public static void Warn(string msg)
        {
            Console.ForegroundColor = ConsoleColor.Yellow; //设置前景色，即字体颜色
            Console.WriteLine(msg);
            if (_log_evt != null) _log_evt(eLogType.WARNING, msg);
        }
        public static void Error(string msg)
        {
            Console.ForegroundColor = ConsoleColor.Red; //设置前景色，即字体颜色
            Console.WriteLine(msg);
            if (_log_evt != null) _log_evt(eLogType.ERROR, msg);
        }
        public static void Exception(string msg, System.Exception e)
        {
            Console.ForegroundColor = ConsoleColor.Red; //设置前景色，即字体颜色
#if DEBUG
            msg += "\r\n" + e.ToString();
#endif
            Console.WriteLine(msg);
            if (_log_evt != null) _log_evt(eLogType.ERROR, msg);
        }
    }
}