using System;
using System.Configuration;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Security.Principal;

namespace dc.tools.core
{
    /// <summary>
    /// 通用工具类
    /// @author hannibal
    /// @time 2019-12-30
    /// </summary>
    public static class Utils
    {
        /// <summary>
        /// 获得配置信息
        /// </summary>
        /// <param name="key"></param>
        /// <returns></returns>
        public static string GetConfigValue(string key, string default_value = "")
        {
            try
            {
                return ConfigurationManager.AppSettings[key];
            }
            catch (System.Exception e)
            {
                Log.Error(e.ToString());
                return default_value;
            }
        }
        public static bool SetConfigValue(string key, string value)
        {
            try
            {
                Configuration cfa = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);
                cfa.AppSettings.Settings[key].Value = value;
                cfa.Save();
                System.Configuration.ConfigurationManager.RefreshSection("appSettings");
                return true;
            }
            catch (System.Exception ex)
            {
                Log.Exception("SetConfigValue error", ex);
                return false;
            }
        }
        /// <summary>
        /// 保存配置表
        /// </summary>
        /// <returns></returns>
        public static bool SaveConfig()
        {
            try
            {
                Configuration cfa = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);
                //保存
                cfa.Save();
                System.Configuration.ConfigurationManager.RefreshSection("appSettings");
                return true;
            }
            catch (System.Exception ex)
            {
                Log.Exception("SaveConfig error", ex);
                return false;
            }
        }

        /// <summary>
        /// 获得exe运行路径
        /// </summary>
        /// <returns></returns>
        public static string GetExePath()
        {
            string exe_path = System.Diagnostics.Process.GetCurrentProcess().MainModule.FileName;
            exe_path = Path.GetDirectoryName(exe_path).Replace("\\", "/");
            return exe_path;
        }

        /// <summary>
        /// 设置当前目录
        /// </summary>
        /// <param name="path"></param>
        /// <returns></returns>
        public static bool SetCurrentDirectory(string path)
        {
            try
            {
                Directory.SetCurrentDirectory(path);
                return true;
            }
            catch(System.Exception ex)
            {
                Log.Exception("SetCurrentDirectory", ex);
                return false;
            }
        }
    }
}
