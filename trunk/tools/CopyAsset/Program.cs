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
            string dst_path = args[0];
            List<string> src_paths = new List<string>();
            for (int i = 1; i < args.Length; ++i)
                src_paths.Add(args[i]);

            bool has_error = false;
            try
            {
                //提取文件列表
                StringBuilder file_builder = new StringBuilder();
                foreach (string src_path in src_paths)
                {
                    if(string.IsNullOrEmpty(Path.GetExtension(src_path)))
                    {//目录
                        List<string> files = new List<string>();
                        FileUtils.GetDirectoryFiles(src_path, ref files);
                        foreach (string file in files)
                        {
                            int dir_length = Path.GetFileNameWithoutExtension(src_path).Length;
                            string relative_path = src_path.Replace("\\", "/");
                            relative_path = relative_path.Replace("../", "");
                            relative_path = relative_path.Replace("./", "");
                            string file_path = file.Replace("\\", "/");
                            file_path = file_path.Substring(file_path.IndexOf(relative_path) + relative_path.Length - dir_length);
                            file_builder.AppendLine(file_path);
                        }
                    }
                    else
                    {//文件
                        file_builder.AppendLine(Path.GetFileName(src_path));
                    }
                }
                //写入文件列表
                string file_content = file_builder.ToString();
                FileUtils.WriteFileText(dst_path + "/file_list.txt", file_content);

                //拷贝文件
                foreach (string src_path in src_paths)
                {
                    if (string.IsNullOrEmpty(Path.GetExtension(src_path)))
                    {//目录
                        if (!FileUtils.DirectoryCopy(src_path, dst_path + "/" + Path.GetFileNameWithoutExtension(src_path), false))
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
                            string file_name = Path.GetFileName(src_path);
                            string dst_file = Path.Combine(dst_path, file_name);
                            File.Copy(src_path, dst_file, true);
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