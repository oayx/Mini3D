using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using dc.tools.core;

namespace dc.tools.project
{
    public partial class MainForm : Form
    {
        [DllImport("User32.DLL")]
        public static extern int SendMessage(int hWnd, int Msg, int wParam, int lParam);
        private const int WM_USER = 0x0400;
        private const int SHADER_COMPILE_ONE = WM_USER + 100;

        public MainForm()
        {
            InitializeComponent();
        }
        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);

            Control.CheckForIllegalCrossThreadCalls = false;
            Log.SetLogEvt(OnLogEvt);
            InitCreateProject();
            InitPublish();
        }
        protected override void WndProc(ref Message m)
        {
            switch (m.Msg)
            {
                case SHADER_COMPILE_ONE:
                    {
                    }
                    break;
            }
            base.WndProc(ref m);
        }

        #region log窗口
        System.Text.StringBuilder g_log = new System.Text.StringBuilder();
        private void OnLogEvt(eLogType type, string msg)
        {
            g_log.AppendLine(msg);
            if (g_log.Length > 10000) g_log.Remove(0, g_log.Length - 10000);
            _txtLog.Text = g_log.ToString();
        }
        /// <summary>
        /// 清除日志
        /// </summary>
        private void _btnClearLog_Click(object sender, EventArgs e)
        {
            g_log.Clear();
            _txtLog.Text = string.Empty;
        }
        #endregion

        #region 创建项目
        private void InitCreateProject()
        {
            _txt_create_name.Text = Utils.GetConfigValue("create_name");
            _txt_create_path.Text = Utils.GetConfigValue("create_path");
        }
        private void _txt_create_path_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog dialog = new FolderBrowserDialog();
            dialog.Description = "请选择文件路径";
            dialog.RootFolder = Environment.SpecialFolder.Desktop;
            if (dialog.ShowDialog() == DialogResult.OK)
            {
                string foldPath = dialog.SelectedPath;
                _txt_create_path.Text = foldPath;
            }
        }
        private void _btn_create_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(_txt_create_name.Text))
            {
                Log.Error("名称不能为空");
                return;
            }
            if (string.IsNullOrEmpty(_txt_create_path.Text))
            {
                Log.Error("请先选择目录");
                return;
            }
            Utils.SetConfigValue("create_name", _txt_create_name.Text);
            Utils.SetConfigValue("create_path", _txt_create_path.Text);

            SetProgress(0);
            _btn_create.Enabled = false;
            _txt_create_name.Enabled = false;
            _txt_create_path.Enabled = false;

            Task.Run(async delegate
            {
                do
                {
                    string target_path = _txt_create_path.Text + "/" + _txt_create_name.Text;

                    //拷贝引擎
                    {
                        string srcPath = "../../";
                        string dstPath = target_path + "/engine/";
                        bool result = FileUtils.DirectoryCopy(srcPath, dstPath);
                        if (!result)
                        {
                            Log.Error("拷贝engine失败");
                            break;
                        }
                        SetProgress(30);
                    }

                    //拷贝template project
                    {
                        //拷贝项目
                        string srcPath = "../../../project";
                        string dstPath = target_path + "/project";
                        bool result = FileUtils.DirectoryCopy(srcPath, dstPath);
                        if (!result && Directory.Exists(srcPath))
                        {
                            Log.Error("拷贝template失败");
                            break;
                        }
                        SetProgress(60);
                    }

                    {
                        //拷贝engine目录下的bin到外层
                        string srcPath = target_path + "/engine/bin/win/x86";
                        string dstPath = target_path + "/bin/win/x86";
                        bool result = FileUtils.DirectoryCopy(srcPath, dstPath);
                        if (!result && Directory.Exists(srcPath))
                        {
                            Log.Error("拷贝engine/engine/bin/win/x86失败");
                            break;
                        }
                        SetProgress(70);
                        srcPath = target_path + "/engine/bin/win/x64";
                        dstPath = target_path + "/bin/win/x64";
                        result = FileUtils.DirectoryCopy(srcPath, dstPath);
                        if (!result && Directory.Exists(srcPath))
                        {
                            Log.Error("拷贝engine/bin/win/x64失败");
                            break;
                        }
                        SetProgress(80);

                        //拷贝engine目录下的assets到bin
                        srcPath = target_path + "/engine/assets";
                        dstPath = target_path + "/bin/assets";
                        result = FileUtils.DirectoryCopy(srcPath, dstPath);
                        if (!result && Directory.Exists(srcPath))
                        {
                            Log.Error("拷贝engine/assets失败");
                            break;
                        }
                        SetProgress(100);
                    }

                    Log.Info("创建完成");
                } while (false);
                _btn_create.Enabled = true;
                _txt_create_name.Enabled = true;
                _txt_create_path.Enabled = true;
            });
        }
        private void SetProgress(int value)
        {
            _bar_create_progress.Value = value;
            _label_create_progress.Text = value + "%";
        }
        #endregion

        #region 发布
        private void InitPublish()
        {
            _cb_publish_plat.Items.Add("Windows");
            _cb_publish_plat.Items.Add("Android");
            _cb_publish_plat.Items.Add("Mac");
            _cb_publish_plat.Items.Add("iOS");
            _cb_publish_plat.Items.Add("Web");
            _cb_publish_plat.SelectedIndex = 0;
            FillPublishArchitecture(_cb_publish_plat.SelectedItem.ToString());
        }
        private void FillPublishArchitecture(string plat)
        {
            _cb_publish_architecture.Items.Clear();
            switch (plat)
            {
                case "Windows":
                    _cb_publish_architecture.Items.Add("x86-64");
                    _cb_publish_architecture.Items.Add("x86");
                    break;
                case "Android":
                    _cb_publish_architecture.Items.Add("arm32-v7a");
                    _cb_publish_architecture.Items.Add("arm64-v8a");
                    _cb_publish_architecture.Items.Add("x86");
                    break;
                case "Mac":
                    _cb_publish_architecture.Items.Add("Default");
                    break;
                case "iOS":
                    _cb_publish_architecture.Items.Add("Default");
                    break;
                case "Web":
                    _cb_publish_architecture.Items.Add("Default");
                    break;
            }
            if (_cb_publish_architecture.Items.Count > 0)
                _cb_publish_architecture.SelectedIndex = 0;
        }
        private void _cb_publish_plat_SelectedIndexChanged(object sender, EventArgs e)
        {
            FillPublishArchitecture(_cb_publish_plat.SelectedItem.ToString());
        }
        #endregion
    }
}