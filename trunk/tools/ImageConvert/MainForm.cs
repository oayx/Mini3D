using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using dc.tools.core;

namespace ImageConvert
{
    public partial class MainForm : Form
    {
        private string[] SrcExts = new string[]{ "tga", "bmp", "jpg", "png", "ppm", "hdr"};
        private string[] DDSSupportExts = new string[] { "tga", "bmp", "jpg", "png", "ppm", "hdr" };
        private string[] PKMSupportExts = new string[] { "tga", "bmp", "jpg", "png", "ppm", "hdr" };
        private string[] ASTCSupportExts = new string[] { "tga", "bmp", "jpg", "png", "hdr", "exr" };
        private string[] DstExts = new string[] { "dds", "pkm", "astc" };

        public MainForm()
        {
            InitializeComponent();
        }

        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);

            Control.CheckForIllegalCrossThreadCalls = false;

            _txt_path.Text = Utils.GetConfigValue("srcPath");
            if(string.IsNullOrEmpty(_txt_path.Text))
            {
                _txt_path.Text = Utils.GetExePath();
            }
            _txt_dst_path.Text = Utils.GetConfigValue("dstPath");
            if (string.IsNullOrEmpty(_txt_dst_path.Text))
            {
                _txt_dst_path.Text = Utils.GetExePath() + "/Output";
            }

            _src_ext.Items.Clear();
            for(int i = 0; i < SrcExts.Length; ++i)
            {
                _src_ext.Items.Add(SrcExts[i]);
                _src_ext.SetItemChecked(i, true);
            }

            {
                _cb_dds_format.Items.Add("");
                _cb_dds_format.Items.Add("DXT1");
                _cb_dds_format.Items.Add("DXT3");
                _cb_dds_format.Items.Add("DXT5");
                _cb_dds_format.SelectedIndex = 0;
            }

            {
                _cb_pk_format.Items.Add("");
                _cb_pk_format.Items.Add("RGB");
                _cb_pk_format.Items.Add("RGBA");
                _cb_pk_format.SelectedIndex = 0;

                _cb_pk_type.Items.Add("etc1");
                _cb_pk_type.Items.Add("etc2");
                _cb_pk_type.SelectedIndex = 1;
            }

            {
                _cb_astc_format.Items.Add("");
                _cb_astc_format.Items.Add("RGB");
                _cb_astc_format.Items.Add("RGBA");
                _cb_astc_format.SelectedIndex = 0;

                _cb_astc_type.Items.Add("4x4");
                _cb_astc_type.Items.Add("5x5");
                _cb_astc_type.Items.Add("6x6");
                _cb_astc_type.Items.Add("8x8");
                _cb_astc_type.Items.Add("10x10");
                _cb_astc_type.Items.Add("12x12");
                _cb_astc_type.SelectedIndex = 0;

                _cb_astc_quality.Items.Add("fastest");
                _cb_astc_quality.Items.Add("fast");
                _cb_astc_quality.Items.Add("medium");
                _cb_astc_quality.Items.Add("thorough");
                _cb_astc_quality.Items.Add("exhaustive");
                _cb_astc_quality.SelectedIndex = 2;

                _cb_astc_model.Items.Add("cl");//use the linear LDR color profile.
                _cb_astc_model.Items.Add("cs");//use the sRGB LDR color profile.
                _cb_astc_model.Items.Add("ch");//use the HDR color profile, tuned for HDR RGB and LDR A.
                _cb_astc_model.Items.Add("cH");//use the HDR color profile, tuned for HDR RGBA.
                _cb_astc_model.SelectedIndex = 0;
            }
        }
        private void _btn_convert_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(_txt_path.Text))
            {
                MessageBox.Show("需要设置目录", "Error", MessageBoxButtons.OK);
                return;
            }

            Utils.SetConfigValue("srcPath", _txt_path.Text);
            Utils.SetConfigValue("dst_define", _txt_dst_path.Text);

            if (!string.IsNullOrEmpty(_txt_dst_path.Text))
            {
                FileUtils.CreateDirectory(_txt_dst_path.Text);
            }

            List<string> src_exts = new List<string>();
            foreach (var item in _src_ext.CheckedItems)
                src_exts.Add(item.ToString());

            Task.Run(async delegate
            {
                try
                {
                    if (_cb_dds.Checked)
                    {
                        foreach (string src_ext in src_exts)
                        {
                            if (!DDSSupportExts.Contains(src_ext)) continue;
                            DDSConvertTexture(src_ext);
                        }
                    }
                    if (_cb_pkm.Checked)
                    {
                        foreach (string src_ext in src_exts)
                        {
                            if (!PKMSupportExts.Contains(src_ext)) continue;
                            PKMConvertTexture(src_ext);
                        }
                    }
                    if (_cb_astc.Checked)
                    {
                        foreach (string src_ext in src_exts)
                        {
                            if (!ASTCSupportExts.Contains(src_ext)) continue;
                            ASTConvertTexture(src_ext);
                        }
                    }
                    MessageBox.Show("转换完成", "Info", MessageBoxButtons.OK);
                }
                catch (System.Exception ex)
                {
                    MessageBox.Show("转换失败:" + ex.ToString(), "Error", MessageBoxButtons.OK);
                }
            });
        }

        private void DDSConvertTexture(string src_ext)
        {
            List<string> list_args = new List<string>();
            string srcPath = _txt_path.Text.Replace("/", "\\");
            if (_cb_path.Checked)
            {//目录
                list_args.Add("-r:keep");
                list_args.Add(srcPath + "\\*." + src_ext);
            }
            else
            {//文件
                list_args.Add(srcPath);
            }
            if (!string.IsNullOrEmpty(_txt_dst_path.Text))
            {//输出目录
                string dstPath = _txt_dst_path.Text.Replace("/", "\\");
                list_args.Add("-o");
                list_args.Add(_txt_dst_path.Text);
            }
            {//目标格式
                list_args.Add("-ft");
                list_args.Add("dds");
            }
            {//mipmap
                list_args.Add("-m");
                list_args.Add(_txt_mipmap.Text.ToString());
            }
            if (_cb_file_lower.Checked)
            {//文件名转小写
                list_args.Add("-l");
            }
            if (_cb_override.Checked)
            {//覆盖已有文件
                list_args.Add("-y");
            }
            if(_cb_dds_format.SelectedIndex == 0)
            {//内部格式
                if (src_ext == "jpg")
                {
                    list_args.Add("-f");
                    list_args.Add("DXT1");
                }
                else
                {
                    list_args.Add("-f");
                    list_args.Add("DXT5");
                }
            }
            else
            {
                string format = _cb_dds_format.Items[_cb_dds_format.SelectedIndex].ToString();
                list_args.Add("-f");
                list_args.Add(format);
            }
            
            string[] args = list_args.ToArray();
            try
            {
                int result = EngineWrapper.dll_dxt_main(args.Length, args);
                if (result != 0)
                {
                    MessageBox.Show("转换失败:" + result, "Error", MessageBoxButtons.OK);
                }
            }
            catch(System.Exception ex)
            {
                MessageBox.Show("转换失败:" + ex.ToString(), "Error", MessageBoxButtons.OK);
            }
        }

        private void PKMConvertTexture(string src_ext)
        {
            string srcPath = _txt_path.Text.Replace("/","\\");
            string dstPath = _txt_dst_path.Text.Replace("/", "\\");
            if (string.IsNullOrEmpty(dstPath))dstPath = srcPath;

            string[] list_args = new string[10];
            list_args[0] = "-s";
            list_args[1] = "fast";
            list_args[2] = "-e";
            list_args[3] = "perceptual";
            list_args[4] = "-c";
            list_args[5] = _cb_pk_type.Items[_cb_pk_type.SelectedIndex].ToString();
            list_args[6] = "-f";
            if(_cb_pk_type.SelectedIndex == 0)
            {
                list_args[7] = "RGB";
            }
            else
            {
                if (_cb_pk_format.SelectedIndex == 0)
                    list_args[7] = src_ext == "jpg" ? "RGB" : "RGBA8";
                else
                    list_args[7] = _cb_pk_format.Items[_cb_pk_format.SelectedIndex].ToString();
            }

            string pattern = "*." + src_ext;
            string[] list_files = Directory.GetFiles(srcPath, pattern, SearchOption.AllDirectories);

            _labal_progress.Text = "0/" + list_files.Length;
            _progress.Maximum = list_files.Length;
            _progress.Value = 0;
            foreach (var file in list_files)
            {
                string full_name = file;
                if (_cb_file_lower.Checked) full_name = full_name.ToLower();
                string fileName = Path.GetFileNameWithoutExtension(full_name);
                string filePath = Path.GetDirectoryName(full_name);
                string relative_path = filePath.Substring(srcPath.Length);
                string output_path = dstPath + relative_path;
                FileUtils.CreateDirectory(output_path);
                list_args[8] = full_name;
                list_args[9] = output_path + "\\" + fileName + ".pkm";
                int result = EngineWrapper.dll_etc_main(list_args.Length, list_args);
                if (result != 0)
                {
                    MessageBox.Show("转换失败:" + result, "Error", MessageBoxButtons.OK);
                }
                _progress.Value = _progress.Value + 1;
                _labal_progress.Text = _progress.Value + "/" + list_files.Length;
            }
        }
        private void ASTConvertTexture(string src_ext)
        {
            string srcPath = _txt_path.Text.Replace("/", "\\");
            string dstPath = _txt_dst_path.Text.Replace("/", "\\");
            if (string.IsNullOrEmpty(dstPath)) dstPath = srcPath;

            string[] list_args = new string[6];
            list_args[0] = "";//无效，占位用
            list_args[1] = "-" + _cb_astc_model.Items[_cb_astc_model.SelectedIndex].ToString();
            list_args[4] = _cb_astc_type.Items[_cb_astc_type.SelectedIndex].ToString();
            list_args[5] = "-" + _cb_astc_quality.Items[_cb_astc_quality.SelectedIndex].ToString();

            string pattern = "*." + src_ext;
            string[] list_files = Directory.GetFiles(srcPath, pattern, SearchOption.AllDirectories);

            _labal_progress.Text = "0/" + list_files.Length;
            _progress.Maximum = list_files.Length;
            _progress.Value = 0;
            foreach (var file in list_files)
            {
                string full_name = file;
                if (_cb_file_lower.Checked) full_name = full_name.ToLower();
                string fileName = Path.GetFileNameWithoutExtension(full_name);
                string filePath = Path.GetDirectoryName(full_name);
                string relative_path = filePath.Substring(srcPath.Length);
                string output_path = dstPath + relative_path;
                FileUtils.CreateDirectory(output_path);
                list_args[2] = full_name;
                list_args[3] = output_path + "\\" + fileName + ".astc";
                int result = EngineWrapper.dll_astc_main(list_args.Length, list_args);
                if (result != 0)
                {
                    MessageBox.Show("转换失败:" + result, "Error", MessageBoxButtons.OK);
                }
                _progress.Value = _progress.Value + 1;
                _labal_progress.Text = _progress.Value + "/" + list_files.Length;
            }
        }
        private void _txt_path_Click(object sender, EventArgs e)
        {
            string exe_path = Utils.GetExePath();

            if (_cb_path.Checked)
            {//针对目录
                FolderBrowserDialog dialog = new FolderBrowserDialog();
                dialog.Description = "请选择文件路径";
                dialog.RootFolder = Environment.SpecialFolder.Desktop;
                if (dialog.ShowDialog() == DialogResult.OK)
                {
                    string foldPath = dialog.SelectedPath;
                    _txt_path.Text = foldPath;
                }
            }
            else
            {
                string filter = "image";
                var select_items = _src_ext.CheckedItems;
                for (int i = 0; i < select_items.Count; ++i)
                {
                    var item = select_items[i];
                    if(i == 0)
                        filter += "|*." + item;
                    else
                        filter += ";*." + item;
                }

                OpenFileDialog dlg = new OpenFileDialog();
                dlg.InitialDirectory = Path.GetDirectoryName(exe_path);
                dlg.Filter = filter;
                if (dlg.ShowDialog() == DialogResult.OK)
                {
                    string fileName = dlg.FileName;
                    _txt_path.Text = fileName;
                }
            }
        }

        private void _txt_dst_path_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog dialog = new FolderBrowserDialog();
            dialog.Description = "请选择保存路径";
            dialog.RootFolder = Environment.SpecialFolder.Desktop;
            if (dialog.ShowDialog() == DialogResult.OK)
            {
                string foldPath = dialog.SelectedPath;
                _txt_dst_path.Text = foldPath;
            }
        }
    }
}
