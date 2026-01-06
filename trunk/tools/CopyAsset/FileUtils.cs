using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Xml;
using System.Linq;
using System;


namespace dc
{
    /// <summary>
    /// 文件+目录操作
    /// @author hannibal
    /// @time 2014-11-20
    /// </summary>
    public static class FileUtils
    {
        #region 目录
        /// <summary>
        /// 遍历目录，获取所有文件
        /// </summary>
        /// <param name="dir">查找的目录</param>
        /// <param name="listFiles">文件列表</param>
        static public void GetDirectoryFiles(string dirPath, ref List<string> list_files)
        {
            if (!Directory.Exists(dirPath)) return;

            DirectoryInfo dir = new DirectoryInfo(dirPath);
            RecursiveDirectoryFiles(dir, dirPath + '/', ref list_files);
        }
        static private void RecursiveDirectoryFiles(DirectoryInfo dir, string parent_path, ref List<string> list_files)
        {
            FileInfo[] allFile = dir.GetFiles();
            foreach (FileInfo fi in allFile)
            {
                string ext = fi.Extension.ToLower();
                if (ext == ".manifest" || ext == ".svn") continue;
                if ((fi.Attributes & FileAttributes.Hidden) == FileAttributes.Hidden) continue;
                list_files.Add(parent_path + fi.Name);
            }
            DirectoryInfo[] allDir = dir.GetDirectories();
            foreach (DirectoryInfo d in allDir)
            {
                if (d.Name == "." || d.Name == "..") continue;
                if ((d.Attributes & FileAttributes.Hidden) == FileAttributes.Hidden) continue;
                RecursiveDirectoryFiles(d, parent_path + d.Name + '/', ref list_files);
            }
        }
        static public string[] GetDirectoryFiles(string dirPath, List<string> withoutExtensions)
        {
            if (!Directory.Exists(dirPath))
                return new string[0];
            //List<string> withoutExtensions = new List<string>() { ".prefab", ".unity", ".mat", ".asset", ".controller", ".anim", ".fontsettings", ".playable", ".guiskin", ".spriteatlas", ".flare", ".mixer", ".cubemap", ".json", ".txt" };
            string[] files = Directory.GetFiles(dirPath, "*.*", SearchOption.AllDirectories)
                .Where(s => withoutExtensions.Contains(Path.GetExtension(s).ToLower())).ToArray();
            return files;
        }
        /// <summary>
        /// 获得所有目录
        /// </summary>
        /// <param name="dirPath"></param>
        /// <param name="list_dirs"></param>
        static public void GetDirectorys(string dirPath, ref List<string> list_dirs)
        {
            if (!Directory.Exists(dirPath)) return;

            DirectoryInfo dir = new DirectoryInfo(dirPath);
            RecursiveDirectory(dir, dirPath + '/', ref list_dirs);
        }
        static private void RecursiveDirectory(DirectoryInfo dir, string parent_path, ref List<string> list_dirs)
        {
            DirectoryInfo[] allDir = dir.GetDirectories();
            foreach (DirectoryInfo d in allDir)
            {
                if (d.Name == "." || d.Name == "..") continue;
                if ((d.Attributes & FileAttributes.Hidden) == FileAttributes.Hidden) continue;
                list_dirs.Add(d.Name);
                RecursiveDirectory(d, parent_path + d.Name + '/', ref list_dirs);
            }
        }
        /// <summary>
        /// 拷贝目录
        /// </summary>
        /// <param name="sourceDirName"></param>
        /// <param name="destDirName"></param>
        /// <param name="console_copy_file">是否打印日志</param>
        public static bool DirectoryCopy(string sourceDirName, string destDirName, bool clear = true)
        {
            try
            {
                if (!Directory.Exists(sourceDirName))
                {
                    return false;
                }
                DirectoryInfo dir = new DirectoryInfo(sourceDirName);
                DirectoryInfo[] dirs = dir.GetDirectories();

                if (Directory.Exists(destDirName))
                {
                    if(clear) ClearDirectory(destDirName);
                }
                else
                {
                    Directory.CreateDirectory(destDirName);
                }

                FileInfo[] files = dir.GetFiles();
                foreach (FileInfo file in files)
                {
                    string temppath = Path.Combine(destDirName, file.Name);
                    file.CopyTo(temppath, true);
                }

                foreach (DirectoryInfo subdir in dirs)
                {
                    string temppath = Path.Combine(destDirName, subdir.Name);
                    bool b = DirectoryCopy(subdir.FullName, temppath, clear);
                    if (!b) return false;
                }
                return true;
            }
            catch (System.Exception)
            {
                Console.WriteLine("拷贝失败");
                return false;
            }
        }
        /// <summary>
        /// 创建目录
        /// </summary>
        /// <param name="folder"></param>
        /// <returns></returns>
        public static bool CreateDirectory(string folder)
        {
            if (string.IsNullOrEmpty(folder)) return false;
            if (Directory.Exists(folder)) return true;

            int num = 0;
            bool result = false;
            while (num < 3)
            {
                try
                {
                    Directory.CreateDirectory(folder);
                    result = true;
                    break;
                }
                catch (Exception e)
                {
                    Console.WriteLine(e.ToString());
                }
                finally
                {
                    num++;
                }
            }
            return result;
        }
        /// <summary>
        /// 删除目录
        /// </summary>
        /// <param name="folder"></param>
        /// <returns></returns>
        public static bool DeleteDirectory(string folder)
        {
            if (string.IsNullOrEmpty(folder))
            {
                Console.WriteLine("待删除的目录不能为空");
                return false;
            }
            if (!Directory.Exists(folder))
            {
                Console.WriteLine("不存在此目录:" + folder);
                return false;
            }

            int num = 0;
            bool result = false;
            while (num < 3)
            {
                try
                {
                    Directory.Delete(folder, true);
                    result = true;
                    break;
                }
                catch (Exception e)
                {
                    Console.WriteLine(e.ToString());
                }
                finally
                {
                    num++;
                }
            }
            return result;
        }
        /// <summary>
        /// 清空目录下内容
        /// </summary>
        /// <param name="folder"></param>
        /// <returns></returns>
        public static bool ClearDirectory(string folder)
        {
            DirectoryInfo dir = new DirectoryInfo(folder);
            DirectoryInfo[] dirs = dir.GetDirectories();
            if (!dir.Exists)
            {
                return true;
            }

            try
            {
                FileInfo[] files = dir.GetFiles();
                foreach (FileInfo file in files)
                {
                    File.Delete(file.FullName);
                }

                foreach (DirectoryInfo subdir in dirs)
                {
                    DeleteDirectory(subdir.FullName);
                }
                return true;
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
                return false;
            }
        }
        /// <summary>
        /// 拷贝文件
        /// </summary>
        public static bool CopyFiles(string srcPath, string dstPath, List<string> ext)
        {
            try
            {
                if (!Directory.Exists(srcPath))
                    return false;

                if (Directory.Exists(dstPath))
                    FileUtils.DeleteDirectory(dstPath);
                Directory.CreateDirectory(dstPath);

                srcPath = srcPath.Replace("\\", "/");
                dstPath = dstPath.Replace("\\", "/");
                string[] files = Directory.GetFiles(srcPath, "*.*", SearchOption.AllDirectories).Where(s => ext.Contains(Path.GetExtension(s).ToLower())).ToArray();
                for (int i = 0; i < files.Length; ++i)
                {
                    string src_full_name = files[i].Replace("\\", "/");
                    string src_file_name = Path.GetFileName(src_full_name);
                    string src_file_path = Path.GetDirectoryName(src_full_name);
                    string src_relative_path = src_file_path.Substring(srcPath.Length);
                    string dst_file_path = dstPath;
                    if (!string.IsNullOrEmpty(src_relative_path)) dst_file_path += "/" + src_relative_path;
                    string dst_full_name = dst_file_path + "/" + src_file_name;
                    if (!string.IsNullOrEmpty(src_relative_path) && !Directory.Exists(dst_file_path))
                    {
                        Directory.CreateDirectory(dst_file_path);
                    }
                    File.Copy(src_full_name, dst_full_name);
                }
                return true;
            }
            catch (System.Exception)
            {
                Console.WriteLine("拷贝失败");
                return false;
            }
        }
        #endregion

        #region 读写文件
        /// <summary>
        /// 读文本文件
        /// </summary>
        /// <param name="path">目录</param>
        /// <returns>文件内容</returns>
        public static string ReadFileText(string path)
        {
            if (string.IsNullOrEmpty(path))
            {
                Console.WriteLine("路径错误");
                return string.Empty;
            }
            if (!File.Exists(path))
            {
                Console.WriteLine("未找到文件:" + path);
                return string.Empty;
            }
            try
            {
                return File.ReadAllText(path);
            }
            catch (System.Exception e)
            {
                Console.WriteLine(e.ToString());
                return string.Empty;
            }
        }
        /// <summary>
        /// 写入文本文件
        /// </summary>
        /// <param name="path">目录</param>
        /// <param name="content">内容</param>
        public static void WriteFileText(string path, string content)
        {
            if (string.IsNullOrEmpty(path))
            {
                Console.WriteLine("路径错误");
                return;
            }
            string folder = Path.GetDirectoryName(path);
            if (!Directory.Exists(folder))
                FileUtils.CreateDirectory(folder);

            StreamWriter sw = null;
            try
            {
                if (!File.Exists(path))
                {
                    using (sw = File.CreateText(path))
                    {
                        sw.Write(content);
                        sw.Flush();
                    }
                }
                else
                {
                    using (sw = new StreamWriter(path, false))
                    {
                        sw.Write(content);
                        sw.Flush();
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
            finally
            {
                if (sw != null)
                {
                    sw.Close();
                }
            }
        }
        /// <summary>
        /// 读二进制文件
        /// </summary>
        /// <param name="path"></param>
        /// <returns></returns>
        public static byte[] ReadFileByte(string path)
        {
            if (string.IsNullOrEmpty(path))
            {
                Console.WriteLine("路径错误");
                return null;
            }
            if (!File.Exists(path))
            {
                Console.WriteLine("未找到文件:" + path);
                return null;
            }
            try
            {
                return File.ReadAllBytes(path);
            }
            catch (System.Exception e)
            {
                Console.WriteLine(e.ToString());
                return null;
            }
        }
        /// <summary>
        /// 写二进制文件
        /// </summary>
        /// <param name="filePath"></param>
        /// <param name="data"></param>
        public static void WriteFileByte(string path, byte[] data)
        {
            if (string.IsNullOrEmpty(path))
            {
                Console.WriteLine("路径错误");
                return;
            }

            FileStream fs = null;
            try
            {
                string folder = Path.GetDirectoryName(path);
                if (!Directory.Exists(folder))
                    FileUtils.CreateDirectory(folder);
                using (fs = File.Open(path, FileMode.Create, FileAccess.Write))
                {
                    fs.Write(data, 0, data.Length);
                    fs.Flush();
                }
            }
            catch (System.Exception e)
            {
                Console.WriteLine(e.ToString());
            }
            finally
            {
                if (fs != null)
                {
                    fs.Close();
                    fs.Dispose();
                }
            }
        }
        /// <summary>
        /// 删除文件
        /// </summary>
        /// <param name="filePath"></param>
        /// <returns></returns>
        public static bool DeleteFile(string path)
        {
            if (string.IsNullOrEmpty(path)) return false;

            int num = 0;
            bool result = false;
            while (num < 3)
            {
                try
                {
                    File.Delete(path);
                    result = true;
                    break;
                }
                catch (Exception e)
                {
                    num++;
                    if (num >= 3)
                    {
                        Console.WriteLine(e.ToString());
                        result = false;
                        break;
                    }
                }
            }
            return result;
        }
        #endregion

        #region md5
        /// <summary>
        /// 计算文件md5值
        /// </summary>
        /// <param name="pathName"></param>
        /// <returns></returns>
        private static System.Security.Cryptography.MD5 md5Provider = new System.Security.Cryptography.MD5CryptoServiceProvider();
        static public string GetFileMD5(string pathName)
        {
            try
            {
                byte[] retVal = FileUtils.ReadFileByte(pathName);
                return BitConverter.ToString(md5Provider.ComputeHash(retVal)).Replace("-", string.Empty);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
            return string.Empty;
        }
        static public string GetMD5(string content)
        {
            try
            {
                return BitConverter.ToString(md5Provider.ComputeHash(Encoding.UTF8.GetBytes(content))).Replace("-", string.Empty);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
            return string.Empty;
        }
        static public string GetMD5(byte[] content)
        {
            try
            {
                return BitConverter.ToString(md5Provider.ComputeHash(content)).Replace("-", string.Empty);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
            return string.Empty;
        }
        #endregion

        #region 其他
        /// <summary>
        /// 获得文件占用磁盘大小
        /// </summary>
        /// <param name="assetPath">绝对路径</param>
        /// <returns>单位M</returns>
        public static float GetFileSize(string fullPath)
        {
            try
            {
                FileInfo fileInfo = new FileInfo(fullPath);
                long size = fileInfo.Length;
                float mem = size / 1024f / 1024f;
                return mem;
            }
            catch (System.Exception e)
            {
                Console.WriteLine(e.ToString());
                return 0;
            }
        }
        #endregion
    }
}