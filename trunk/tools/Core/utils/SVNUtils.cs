using System;
using System.Diagnostics;

namespace dc.tools.core
{
    /// <summary>
    /// svn工具
    /// @author hannibal
    /// @time 2020-12-30
    /// </summary>
    public static class SVNUtils
    {
        /// <summary>
        /// svn地址
        /// </summary>
        private static string SVN_URL = "https://122.51.45.14/svn/dc_engine/trunk";

        public static void SVNUpdate(string path)
        {
            try
            {
                Process.Start("TortoiseProc.exe", "/command:update /path:" + GetUrlPath(path) + " /closeonend:0");
            }
            catch(System.Exception ex)
            {
                Log.Exception("SVNUpdate", ex);
            }
        }
        public static void SVNCommit(string path)
        {
            try
            {
                Process.Start("TortoiseProc.exe", "/command:commit /path:" + GetUrlPath(path) + " /closeonend:0");
            }
            catch (System.Exception ex)
            {
                Log.Exception("SVNCommit", ex);
            }
        }
        public static void SVNUpdateCommit(string path)
        {
            try
            {
                Process process = Process.Start("TortoiseProc.exe", "/command:update /path:" + GetUrlPath(path) + " /closeonend:0");
                process.WaitForExit();
                Process.Start("TortoiseProc.exe", "/command:commit /path:" + GetUrlPath(path) + " /closeonend:0");
            }
            catch (System.Exception ex)
            {
                Log.Exception("SVNUpdateCommit", ex);
            }
        }
        public static void SVNRevert(string path)
        {
            try
            {
                Process.Start("TortoiseProc.exe", "/command:revert /path:" + GetUrlPath(path) + " /closeonend:0");
            }
            catch (System.Exception ex)
            {
                Log.Exception("SVNRevert", ex);
            }
        }
        public static void SVNClearup(string path)
        {
            try
            {
                Process.Start("TortoiseProc.exe", "/command:cleanup /path:" + GetUrlPath(path) + " /closeonend:0");
            }
            catch (System.Exception ex)
            {
                Log.Exception("SVNClearup", ex);
            }
        }
        
        private static string GetUrlPath(string path)
        {
            if (!string.IsNullOrEmpty(path))
                return SVN_URL + "/" + path;
            else
                return SVN_URL;
        }
    }
}