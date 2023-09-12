namespace dc.tools.project
{
    partial class MainForm
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this._tab_publish = new System.Windows.Forms.TabControl();
            this._windows_plat = new System.Windows.Forms.TabPage();
            this._android_plat = new System.Windows.Forms.TabPage();
            this._ios_plat = new System.Windows.Forms.TabPage();
            this._tab_root = new System.Windows.Forms.TabControl();
            this._tabCreate = new System.Windows.Forms.TabPage();
            this._label_create_progress = new System.Windows.Forms.Label();
            this._bar_create_progress = new System.Windows.Forms.ProgressBar();
            this._btn_create = new System.Windows.Forms.Button();
            this._txt_create_name = new System.Windows.Forms.TextBox();
            this.label10 = new System.Windows.Forms.Label();
            this._txt_create_path = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this._tabPublish = new System.Windows.Forms.TabPage();
            this.label11 = new System.Windows.Forms.Label();
            this._cb_publish_architecture = new System.Windows.Forms.ComboBox();
            this.label12 = new System.Windows.Forms.Label();
            this._cb_publish_plat = new System.Windows.Forms.ComboBox();
            this._txtLog = new System.Windows.Forms.RichTextBox();
            this._btnClearLog = new System.Windows.Forms.Button();
            this._tab_publish.SuspendLayout();
            this._tab_root.SuspendLayout();
            this._tabCreate.SuspendLayout();
            this._tabPublish.SuspendLayout();
            this.SuspendLayout();
            // 
            // _tab_publish
            // 
            this._tab_publish.Controls.Add(this._windows_plat);
            this._tab_publish.Controls.Add(this._android_plat);
            this._tab_publish.Controls.Add(this._ios_plat);
            this._tab_publish.Location = new System.Drawing.Point(4, 7);
            this._tab_publish.Name = "_tab_publish";
            this._tab_publish.SelectedIndex = 0;
            this._tab_publish.Size = new System.Drawing.Size(463, 214);
            this._tab_publish.TabIndex = 0;
            // 
            // _windows_plat
            // 
            this._windows_plat.Location = new System.Drawing.Point(4, 22);
            this._windows_plat.Name = "_windows_plat";
            this._windows_plat.Padding = new System.Windows.Forms.Padding(3);
            this._windows_plat.Size = new System.Drawing.Size(455, 188);
            this._windows_plat.TabIndex = 0;
            this._windows_plat.Text = "windows";
            this._windows_plat.UseVisualStyleBackColor = true;
            // 
            // _android_plat
            // 
            this._android_plat.Location = new System.Drawing.Point(4, 22);
            this._android_plat.Name = "_android_plat";
            this._android_plat.Padding = new System.Windows.Forms.Padding(3);
            this._android_plat.Size = new System.Drawing.Size(455, 188);
            this._android_plat.TabIndex = 1;
            this._android_plat.Text = "android";
            this._android_plat.UseVisualStyleBackColor = true;
            // 
            // _ios_plat
            // 
            this._ios_plat.Location = new System.Drawing.Point(4, 22);
            this._ios_plat.Name = "_ios_plat";
            this._ios_plat.Size = new System.Drawing.Size(455, 188);
            this._ios_plat.TabIndex = 2;
            this._ios_plat.Text = "ios";
            this._ios_plat.UseVisualStyleBackColor = true;
            // 
            // _tab_root
            // 
            this._tab_root.Controls.Add(this._tabCreate);
            this._tab_root.Controls.Add(this._tabPublish);
            this._tab_root.Location = new System.Drawing.Point(1, 2);
            this._tab_root.Name = "_tab_root";
            this._tab_root.SelectedIndex = 0;
            this._tab_root.Size = new System.Drawing.Size(480, 557);
            this._tab_root.TabIndex = 6;
            // 
            // _tabCreate
            // 
            this._tabCreate.BackColor = System.Drawing.Color.WhiteSmoke;
            this._tabCreate.Controls.Add(this._label_create_progress);
            this._tabCreate.Controls.Add(this._bar_create_progress);
            this._tabCreate.Controls.Add(this._btn_create);
            this._tabCreate.Controls.Add(this._txt_create_name);
            this._tabCreate.Controls.Add(this.label10);
            this._tabCreate.Controls.Add(this._txt_create_path);
            this._tabCreate.Controls.Add(this.label9);
            this._tabCreate.Location = new System.Drawing.Point(4, 22);
            this._tabCreate.Name = "_tabCreate";
            this._tabCreate.Size = new System.Drawing.Size(472, 531);
            this._tabCreate.TabIndex = 3;
            this._tabCreate.Text = "创建";
            // 
            // _label_create_progress
            // 
            this._label_create_progress.AutoSize = true;
            this._label_create_progress.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this._label_create_progress.Location = new System.Drawing.Point(438, 512);
            this._label_create_progress.Name = "_label_create_progress";
            this._label_create_progress.Size = new System.Drawing.Size(21, 13);
            this._label_create_progress.TabIndex = 21;
            this._label_create_progress.Text = "0%";
            this._label_create_progress.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // _bar_create_progress
            // 
            this._bar_create_progress.Location = new System.Drawing.Point(1, 507);
            this._bar_create_progress.Name = "_bar_create_progress";
            this._bar_create_progress.Size = new System.Drawing.Size(436, 23);
            this._bar_create_progress.TabIndex = 20;
            // 
            // _btn_create
            // 
            this._btn_create.Location = new System.Drawing.Point(394, 14);
            this._btn_create.Name = "_btn_create";
            this._btn_create.Size = new System.Drawing.Size(65, 42);
            this._btn_create.TabIndex = 14;
            this._btn_create.Text = "创 建";
            this._btn_create.UseVisualStyleBackColor = true;
            this._btn_create.Click += new System.EventHandler(this._btn_create_Click);
            // 
            // _txt_create_name
            // 
            this._txt_create_name.Location = new System.Drawing.Point(51, 10);
            this._txt_create_name.Name = "_txt_create_name";
            this._txt_create_name.Size = new System.Drawing.Size(120, 21);
            this._txt_create_name.TabIndex = 13;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(7, 14);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(41, 12);
            this.label10.TabIndex = 12;
            this.label10.Text = "名  称";
            // 
            // _txt_create_path
            // 
            this._txt_create_path.Location = new System.Drawing.Point(51, 40);
            this._txt_create_path.Name = "_txt_create_path";
            this._txt_create_path.Size = new System.Drawing.Size(326, 21);
            this._txt_create_path.TabIndex = 11;
            this._txt_create_path.Click += new System.EventHandler(this._txt_create_path_Click);
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(7, 44);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(41, 12);
            this.label9.TabIndex = 10;
            this.label9.Text = "目  录";
            // 
            // _tabPublish
            // 
            this._tabPublish.BackColor = System.Drawing.Color.WhiteSmoke;
            this._tabPublish.Controls.Add(this.label11);
            this._tabPublish.Controls.Add(this._cb_publish_architecture);
            this._tabPublish.Controls.Add(this.label12);
            this._tabPublish.Controls.Add(this._cb_publish_plat);
            this._tabPublish.Controls.Add(this._tab_publish);
            this._tabPublish.Location = new System.Drawing.Point(4, 22);
            this._tabPublish.Name = "_tabPublish";
            this._tabPublish.Padding = new System.Windows.Forms.Padding(3);
            this._tabPublish.Size = new System.Drawing.Size(472, 531);
            this._tabPublish.TabIndex = 0;
            this._tabPublish.Text = "发布";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(216, 232);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(41, 12);
            this.label11.TabIndex = 12;
            this.label11.Text = "架  构";
            // 
            // _cb_publish_architecture
            // 
            this._cb_publish_architecture.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._cb_publish_architecture.FormattingEnabled = true;
            this._cb_publish_architecture.Location = new System.Drawing.Point(260, 228);
            this._cb_publish_architecture.Name = "_cb_publish_architecture";
            this._cb_publish_architecture.Size = new System.Drawing.Size(120, 20);
            this._cb_publish_architecture.TabIndex = 13;
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(7, 231);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(41, 12);
            this.label12.TabIndex = 10;
            this.label12.Text = "平  台";
            // 
            // _cb_publish_plat
            // 
            this._cb_publish_plat.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._cb_publish_plat.FormattingEnabled = true;
            this._cb_publish_plat.Location = new System.Drawing.Point(51, 227);
            this._cb_publish_plat.Name = "_cb_publish_plat";
            this._cb_publish_plat.Size = new System.Drawing.Size(120, 20);
            this._cb_publish_plat.TabIndex = 11;
            this._cb_publish_plat.SelectedIndexChanged += new System.EventHandler(this._cb_publish_plat_SelectedIndexChanged);
            // 
            // _txtLog
            // 
            this._txtLog.Location = new System.Drawing.Point(481, 23);
            this._txtLog.Name = "_txtLog";
            this._txtLog.ReadOnly = true;
            this._txtLog.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.Vertical;
            this._txtLog.Size = new System.Drawing.Size(396, 535);
            this._txtLog.TabIndex = 5;
            this._txtLog.Text = "运行日志";
            // 
            // _btnClearLog
            // 
            this._btnClearLog.Location = new System.Drawing.Point(813, 0);
            this._btnClearLog.Name = "_btnClearLog";
            this._btnClearLog.Size = new System.Drawing.Size(64, 23);
            this._btnClearLog.TabIndex = 7;
            this._btnClearLog.Text = "清除日志";
            this._btnClearLog.UseVisualStyleBackColor = true;
            this._btnClearLog.Click += new System.EventHandler(this._btnClearLog_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(881, 561);
            this.Controls.Add(this._btnClearLog);
            this.Controls.Add(this._txtLog);
            this.Controls.Add(this._tab_root);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "工具";
            this._tab_publish.ResumeLayout(false);
            this._tab_root.ResumeLayout(false);
            this._tabCreate.ResumeLayout(false);
            this._tabCreate.PerformLayout();
            this._tabPublish.ResumeLayout(false);
            this._tabPublish.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl _tab_publish;
        private System.Windows.Forms.TabPage _windows_plat;
        private System.Windows.Forms.TabPage _android_plat;
        private System.Windows.Forms.TabPage _ios_plat;
        private System.Windows.Forms.TabControl _tab_root;
        private System.Windows.Forms.TabPage _tabPublish;
        private System.Windows.Forms.RichTextBox _txtLog;
        private System.Windows.Forms.Button _btnClearLog;
        private System.Windows.Forms.TabPage _tabCreate;
        private System.Windows.Forms.TextBox _txt_create_name;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TextBox _txt_create_path;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.ComboBox _cb_publish_architecture;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.ComboBox _cb_publish_plat;
        private System.Windows.Forms.Button _btn_create;
        private System.Windows.Forms.Label _label_create_progress;
        private System.Windows.Forms.ProgressBar _bar_create_progress;
    }
}

