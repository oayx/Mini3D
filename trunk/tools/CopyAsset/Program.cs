using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace dc
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args == null || args.Length < 2)
            {
                Console.WriteLine("args error");
                return;
            }
            string dstPath = args[0];
            List<string> src_paths = new List<string>();
            for (int i = 1; i < args.Length; ++i)
                src_paths.Add(args[i]);

            bool has_error = false;
            try
            {
                //提取文件列表
                StringBuilder file_builder = new StringBuilder();
                foreach (string srcPath in src_paths)
                {
                    if(string.IsNullOrEmpty(Path.GetExtension(srcPath)))
                    {//目录
                        List<string> files = new List<string>();
                        FileUtils.GetDirectoryFiles(srcPath, ref files);
                        foreach (string file in files)
                        {
                            int dir_length = Path.GetFileNameWithoutExtension(srcPath).Length;
                            string relative_path = srcPath.Replace("\\", "/");
                            relative_path = relative_path.Replace("../", "");
                            relative_path = relative_path.Replace("./", "");
                            string filePath = file.Replace("\\", "/");
                            filePath = filePath.Substring(filePath.IndexOf(relative_path) + relative_path.Length - dir_length);
                            file_builder.AppendLine(filePath);
                        }
                    }
                    else
                    {//文件
                        file_builder.AppendLine(Path.GetFileName(srcPath));
                    }
                }
                //写入文件列表
                string file_content = file_builder.ToString();
                FileUtils.WriteFileText(dstPath + "/file_list.txt", file_content);

                //拷贝文件
                foreach (string srcPath in src_paths)
                {
                    if (string.IsNullOrEmpty(Path.GetExtension(srcPath)))
                    {//目录
                        if (!FileUtils.DirectoryCopy(srcPath, dstPath + "/" + Path.GetFileNameWithoutExtension(srcPath), false))
                        {
                            Console.WriteLine("DirectoryCopy error");
                            has_error = true;
                            return;
                        }
                    }
                    else
                    {//文件
                        try
                        {
                            string fileName = Path.GetFileName(srcPath);
                            string dst_file = Path.Combine(dstPath, fileName);
                            File.Copy(srcPath, dst_file, true);
                        }
                        catch(System.Exception ex)
                        {
                            Console.WriteLine("copy file error:" + ex.ToString());
                            has_error = true;
                        }
                    }
                }
            }
            catch(System.Exception ex)
            {
                Console.WriteLine("copy error:" + ex.ToString());
                has_error = true;
            }

            if(has_error) System.Console.ReadKey();
        }
    }
}