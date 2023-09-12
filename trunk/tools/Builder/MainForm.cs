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

namespace dc.tools.builder
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }
        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);
            Control.CheckForIllegalCrossThreadCalls = false;

            string save_path = Utils.GetConfigValue("save_path");
            _txt_path.Text = save_path;
        }
        private void _txt_path_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog dialog = new FolderBrowserDialog();
            dialog.Description = "请选择文件路径";
            dialog.RootFolder = Environment.SpecialFolder.Desktop;
            if (dialog.ShowDialog() == DialogResult.OK)
            {
                string foldPath = dialog.SelectedPath;
                _txt_path.Text = foldPath;
            }
        }

        private void _btn_builder_Click(object sender, EventArgs e)
        {
            if(string.IsNullOrEmpty(_txt_path.Text))
            {
                MessageBox.Show("请先选择目录", "Error", MessageBoxButtons.OK);
                return;
            }
            Utils.SetConfigValue("save_path", _txt_path.Text);
            SetProgress(0);
            _txt_path.Enabled = false;
            _btn_builder.Enabled = false;

            Task.Run(async delegate
            {
                do
                {
                    string exe_path = Utils.GetExePath();
                    //拷贝头文件
                    string src_path = exe_path + "/../../engine/source";
                    string dst_path = _txt_path.Text + "/engine/source";
                    bool result = FileUtils.CopyFiles(src_path, dst_path, new List<string> { ".h", ".hpp", ".inl" });
                    if (!result)
                    {
                        MessageBox.Show("拷贝include失败", "Error", MessageBoxButtons.OK);
                        break;
                    }
                    SetProgress(20);

                    //拷贝运行时
                    {
                        src_path = exe_path + "/../../bin/win";
                        dst_path = _txt_path.Text + "/engine/bin/win";
                        result = FileUtils.DirectoryCopy(src_path, dst_path);
                        if (!result)
                        {
                            MessageBox.Show("拷贝bin-win失败", "Error", MessageBoxButtons.OK);
                            break;
                        }
                        SetProgress(40);
                    }

                    //拷贝默认资源
                    {
                        src_path = exe_path + "/../../bin/assets/internal";
                        dst_path = _txt_path.Text + "/engine/assets/internal";
                        result = FileUtils.DirectoryCopy(src_path, dst_path);
                        if (!result)
                        {
                            MessageBox.Show("拷贝resource失败", "Epprror", MessageBoxButtons.OK);
                            break;
                        }
                        SetProgress(50);
                    }

                    //拷贝到template
                    {
                        //拷贝CopyAsset.exe到template
                        src_path = exe_path + "/../../bin/tools/CopyAsset.exe";
                        dst_path = exe_path + "/../../template/project.android/app/CopyAsset.exe";
                        try
                        {
                            File.Copy(src_path, dst_path, true);
                        }
                        catch (System.Exception)
                        {
                            MessageBox.Show("拷贝CopyAsset.exe失败", "Error", MessageBoxButtons.OK);
                            break;
                        }
                        SetProgress(55);

                        //拷贝bin目录下的android目录到template的android项目
                        src_path = exe_path + "/../../bin/android";
                        dst_path = exe_path + "/../../template/project.android/app/libs";
                        result = FileUtils.DirectoryCopy(src_path, dst_path);
                        if (!result)
                        {
                            Log.Error("拷贝bin-android失败");
                            break;
                        }
                        SetProgress(60);
                    }

                    //拷贝template到publish目录
                    {
                        src_path = exe_path + "/../../template";
                        dst_path = _txt_path.Text + "/project";
                        result = FileUtils.DirectoryCopy(src_path, dst_path);
                        if (!result)
                        {
                            MessageBox.Show("拷贝template失败", "Epprror", MessageBoxButtons.OK);
                            break;
                        }
                        SetProgress(100);
                    }

                    MessageBox.Show("执行完毕", "Info", MessageBoxButtons.OK);
                    _txt_path.Enabled = true;
                    _btn_builder.Enabled = true;
                } while (false);
            });
        }
        private void SetProgress(int value)
        {
            _bar_publish.Value = value;
            _label_progress.Text = value + "%";
        }
    }
}