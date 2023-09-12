namespace ImageConvert
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
            this.components = new System.ComponentModel.Container();
            this._txt_path = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this._cb_path = new System.Windows.Forms.CheckBox();
            this._btn_convert = new System.Windows.Forms.Button();
            this._src_ext = new System.Windows.Forms.CheckedListBox();
            this.label1 = new System.Windows.Forms.Label();
            this._txt_dst_path = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this._cb_override = new System.Windows.Forms.CheckBox();
            this.label5 = new System.Windows.Forms.Label();
            this._txt_mipmap = new System.Windows.Forms.TextBox();
            this._cb_file_lower = new System.Windows.Forms.CheckBox();
            this.label6 = new System.Windows.Forms.Label();
            this.toolTip = new System.Windows.Forms.ToolTip(this.components);
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this._cb_dds_format = new System.Windows.Forms.ComboBox();
            this._cb_dds = new System.Windows.Forms.CheckBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this._cb_pk_format = new System.Windows.Forms.ComboBox();
            this.label7 = new System.Windows.Forms.Label();
            this._cb_pk_type = new System.Windows.Forms.ComboBox();
            this._cb_pkm = new System.Windows.Forms.CheckBox();
            this.label2 = new System.Windows.Forms.Label();
            this._progress = new System.Windows.Forms.ProgressBar();
            this._labal_progress = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this._cb_astc_quality = new System.Windows.Forms.ComboBox();
            this.label10 = new System.Windows.Forms.Label();
            this._cb_astc_model = new System.Windows.Forms.ComboBox();
            this.label11 = new System.Windows.Forms.Label();
            this._cb_astc_format = new System.Windows.Forms.ComboBox();
            this.label8 = new System.Windows.Forms.Label();
            this._cb_astc_type = new System.Windows.Forms.ComboBox();
            this._cb_astc = new System.Windows.Forms.CheckBox();
            this.label9 = new System.Windows.Forms.Label();
            this.toolTip_astc_model = new System.Windows.Forms.ToolTip(this.components);
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // _txt_path
            // 
            this._txt_path.Location = new System.Drawing.Point(69, 12);
            this._txt_path.Name = "_txt_path";
            this._txt_path.Size = new System.Drawing.Size(361, 21);
            this._txt_path.TabIndex = 26;
            this._txt_path.Click += new System.EventHandler(this._txt_path_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(10, 16);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(53, 12);
            this.label3.TabIndex = 25;
            this.label3.Text = "源 位 置";
            // 
            // _cb_path
            // 
            this._cb_path.AutoSize = true;
            this._cb_path.Checked = true;
            this._cb_path.CheckState = System.Windows.Forms.CheckState.Checked;
            this._cb_path.Location = new System.Drawing.Point(440, 15);
            this._cb_path.Name = "_cb_path";
            this._cb_path.Size = new System.Drawing.Size(72, 16);
            this._cb_path.TabIndex = 27;
            this._cb_path.Text = "是否目录";
            this._cb_path.UseVisualStyleBackColor = true;
            // 
            // _btn_convert
            // 
            this._btn_convert.Location = new System.Drawing.Point(440, 43);
            this._btn_convert.Name = "_btn_convert";
            this._btn_convert.Size = new System.Drawing.Size(69, 40);
            this._btn_convert.TabIndex = 28;
            this._btn_convert.Text = "转 换";
            this._btn_convert.UseVisualStyleBackColor = true;
            this._btn_convert.Click += new System.EventHandler(this._btn_convert_Click);
            // 
            // _src_ext
            // 
            this._src_ext.CheckOnClick = true;
            this._src_ext.ColumnWidth = 50;
            this._src_ext.FormattingEnabled = true;
            this._src_ext.Items.AddRange(new object[] {
            "dds",
            "tga",
            "hdr",
            "ppm",
            "bmp",
            "jpg",
            "png"});
            this._src_ext.Location = new System.Drawing.Point(69, 71);
            this._src_ext.MultiColumn = true;
            this._src_ext.Name = "_src_ext";
            this._src_ext.Size = new System.Drawing.Size(361, 20);
            this._src_ext.TabIndex = 37;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(10, 75);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(53, 12);
            this.label1.TabIndex = 38;
            this.label1.Text = "源 格 式";
            // 
            // _txt_dst_path
            // 
            this._txt_dst_path.Location = new System.Drawing.Point(69, 39);
            this._txt_dst_path.Name = "_txt_dst_path";
            this._txt_dst_path.Size = new System.Drawing.Size(361, 21);
            this._txt_dst_path.TabIndex = 42;
            this._txt_dst_path.Click += new System.EventHandler(this._txt_dst_path_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(10, 43);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(53, 12);
            this.label4.TabIndex = 41;
            this.label4.Text = "目标位置";
            // 
            // _cb_override
            // 
            this._cb_override.AutoSize = true;
            this._cb_override.Checked = true;
            this._cb_override.CheckState = System.Windows.Forms.CheckState.Checked;
            this._cb_override.Location = new System.Drawing.Point(148, 263);
            this._cb_override.Name = "_cb_override";
            this._cb_override.Size = new System.Drawing.Size(96, 16);
            this._cb_override.TabIndex = 44;
            this._cb_override.Text = "覆盖同名文件";
            this._cb_override.UseVisualStyleBackColor = true;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(278, 264);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(41, 12);
            this.label5.TabIndex = 45;
            this.label5.Text = "mipmap";
            // 
            // _txt_mipmap
            // 
            this._txt_mipmap.Location = new System.Drawing.Point(325, 260);
            this._txt_mipmap.Name = "_txt_mipmap";
            this._txt_mipmap.Size = new System.Drawing.Size(72, 21);
            this._txt_mipmap.TabIndex = 46;
            this._txt_mipmap.Text = "1";
            this.toolTip.SetToolTip(this._txt_mipmap, "0:默认生成所有，1:不生成，大于1生成指定的");
            // 
            // _cb_file_lower
            // 
            this._cb_file_lower.AutoSize = true;
            this._cb_file_lower.Checked = true;
            this._cb_file_lower.CheckState = System.Windows.Forms.CheckState.Checked;
            this._cb_file_lower.Location = new System.Drawing.Point(12, 263);
            this._cb_file_lower.Name = "_cb_file_lower";
            this._cb_file_lower.Size = new System.Drawing.Size(96, 16);
            this._cb_file_lower.TabIndex = 47;
            this._cb_file_lower.Text = "文件名转小写";
            this._cb_file_lower.UseVisualStyleBackColor = true;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(60, 18);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(53, 12);
            this.label6.TabIndex = 48;
            this.label6.Text = "内部格式";
            // 
            // toolTip
            // 
            this.toolTip.AutoPopDelay = 5000;
            this.toolTip.InitialDelay = 100;
            this.toolTip.ReshowDelay = 100;
            this.toolTip.ShowAlways = true;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this._cb_dds_format);
            this.groupBox1.Controls.Add(this._cb_dds);
            this.groupBox1.Controls.Add(this.label6);
            this.groupBox1.Location = new System.Drawing.Point(4, 97);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(508, 43);
            this.groupBox1.TabIndex = 50;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "DDS";
            // 
            // _cb_dds_format
            // 
            this._cb_dds_format.FormattingEnabled = true;
            this._cb_dds_format.Location = new System.Drawing.Point(120, 14);
            this._cb_dds_format.Name = "_cb_dds_format";
            this._cb_dds_format.Size = new System.Drawing.Size(121, 20);
            this._cb_dds_format.TabIndex = 51;
            // 
            // _cb_dds
            // 
            this._cb_dds.AutoSize = true;
            this._cb_dds.Checked = true;
            this._cb_dds.CheckState = System.Windows.Forms.CheckState.Checked;
            this._cb_dds.Location = new System.Drawing.Point(10, 18);
            this._cb_dds.Name = "_cb_dds";
            this._cb_dds.Size = new System.Drawing.Size(42, 16);
            this._cb_dds.TabIndex = 50;
            this._cb_dds.Text = "DDS";
            this._cb_dds.UseVisualStyleBackColor = true;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this._cb_pk_format);
            this.groupBox2.Controls.Add(this.label7);
            this.groupBox2.Controls.Add(this._cb_pk_type);
            this.groupBox2.Controls.Add(this._cb_pkm);
            this.groupBox2.Controls.Add(this.label2);
            this.groupBox2.Location = new System.Drawing.Point(4, 141);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(508, 43);
            this.groupBox2.TabIndex = 51;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "PKM";
            // 
            // _cb_pk_format
            // 
            this._cb_pk_format.FormattingEnabled = true;
            this._cb_pk_format.Location = new System.Drawing.Point(120, 14);
            this._cb_pk_format.Name = "_cb_pk_format";
            this._cb_pk_format.Size = new System.Drawing.Size(121, 20);
            this._cb_pk_format.TabIndex = 53;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(60, 18);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(53, 12);
            this.label7.TabIndex = 52;
            this.label7.Text = "内部格式";
            // 
            // _cb_pk_type
            // 
            this._cb_pk_type.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._cb_pk_type.FormattingEnabled = true;
            this._cb_pk_type.Location = new System.Drawing.Point(305, 14);
            this._cb_pk_type.Name = "_cb_pk_type";
            this._cb_pk_type.Size = new System.Drawing.Size(121, 20);
            this._cb_pk_type.TabIndex = 51;
            // 
            // _cb_pkm
            // 
            this._cb_pkm.AutoSize = true;
            this._cb_pkm.Checked = true;
            this._cb_pkm.CheckState = System.Windows.Forms.CheckState.Checked;
            this._cb_pkm.Location = new System.Drawing.Point(10, 17);
            this._cb_pkm.Name = "_cb_pkm";
            this._cb_pkm.Size = new System.Drawing.Size(42, 16);
            this._cb_pkm.TabIndex = 50;
            this._cb_pkm.Text = "PKM";
            this._cb_pkm.UseVisualStyleBackColor = true;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(270, 19);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(29, 12);
            this.label2.TabIndex = 48;
            this.label2.Text = "类型";
            // 
            // _progress
            // 
            this._progress.Location = new System.Drawing.Point(4, 287);
            this._progress.Name = "_progress";
            this._progress.Size = new System.Drawing.Size(451, 23);
            this._progress.TabIndex = 52;
            // 
            // _labal_progress
            // 
            this._labal_progress.BackColor = System.Drawing.Color.Transparent;
            this._labal_progress.Location = new System.Drawing.Point(461, 291);
            this._labal_progress.Name = "_labal_progress";
            this._labal_progress.Size = new System.Drawing.Size(51, 15);
            this._labal_progress.TabIndex = 53;
            this._labal_progress.Text = "0/0";
            this._labal_progress.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this._cb_astc_quality);
            this.groupBox3.Controls.Add(this.label10);
            this.groupBox3.Controls.Add(this._cb_astc_model);
            this.groupBox3.Controls.Add(this.label11);
            this.groupBox3.Controls.Add(this._cb_astc_format);
            this.groupBox3.Controls.Add(this.label8);
            this.groupBox3.Controls.Add(this._cb_astc_type);
            this.groupBox3.Controls.Add(this._cb_astc);
            this.groupBox3.Controls.Add(this.label9);
            this.groupBox3.Location = new System.Drawing.Point(4, 184);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(508, 71);
            this.groupBox3.TabIndex = 54;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "ASTC";
            // 
            // _cb_astc_quality
            // 
            this._cb_astc_quality.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._cb_astc_quality.FormattingEnabled = true;
            this._cb_astc_quality.Location = new System.Drawing.Point(120, 42);
            this._cb_astc_quality.Name = "_cb_astc_quality";
            this._cb_astc_quality.Size = new System.Drawing.Size(121, 20);
            this._cb_astc_quality.TabIndex = 57;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(60, 46);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(53, 12);
            this.label10.TabIndex = 56;
            this.label10.Text = "质    量";
            // 
            // _cb_astc_model
            // 
            this._cb_astc_model.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._cb_astc_model.FormattingEnabled = true;
            this._cb_astc_model.Location = new System.Drawing.Point(305, 42);
            this._cb_astc_model.Name = "_cb_astc_model";
            this._cb_astc_model.Size = new System.Drawing.Size(121, 20);
            this._cb_astc_model.TabIndex = 55;
            this.toolTip_astc_model.SetToolTip(this._cb_astc_model, "-cl : use the linear LDR color profile.\r\n-cs : use the sRGB LDR color profile.\r\n-" +
        "ch : use the HDR color profile, tuned for HDR RGB and LDR A.\r\n-cH : use the HDR " +
        "color profile, tuned for HDR RGBA.");
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(270, 47);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(29, 12);
            this.label11.TabIndex = 54;
            this.label11.Text = "模式";
            // 
            // _cb_astc_format
            // 
            this._cb_astc_format.FormattingEnabled = true;
            this._cb_astc_format.Location = new System.Drawing.Point(120, 14);
            this._cb_astc_format.Name = "_cb_astc_format";
            this._cb_astc_format.Size = new System.Drawing.Size(121, 20);
            this._cb_astc_format.TabIndex = 53;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(60, 18);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(53, 12);
            this.label8.TabIndex = 52;
            this.label8.Text = "内部格式";
            // 
            // _cb_astc_type
            // 
            this._cb_astc_type.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._cb_astc_type.FormattingEnabled = true;
            this._cb_astc_type.Location = new System.Drawing.Point(305, 14);
            this._cb_astc_type.Name = "_cb_astc_type";
            this._cb_astc_type.Size = new System.Drawing.Size(121, 20);
            this._cb_astc_type.TabIndex = 51;
            // 
            // _cb_astc
            // 
            this._cb_astc.AutoSize = true;
            this._cb_astc.Checked = true;
            this._cb_astc.CheckState = System.Windows.Forms.CheckState.Checked;
            this._cb_astc.Location = new System.Drawing.Point(10, 17);
            this._cb_astc.Name = "_cb_astc";
            this._cb_astc.Size = new System.Drawing.Size(48, 16);
            this._cb_astc.TabIndex = 50;
            this._cb_astc.Text = "ASTC";
            this._cb_astc.UseVisualStyleBackColor = true;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(270, 19);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(29, 12);
            this.label9.TabIndex = 48;
            this.label9.Text = "类型";
            // 
            // toolTip_astc_model
            // 
            this.toolTip_astc_model.AutoPopDelay = 5000;
            this.toolTip_astc_model.InitialDelay = 100;
            this.toolTip_astc_model.ReshowDelay = 100;
            this.toolTip_astc_model.ShowAlways = true;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(516, 312);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this._labal_progress);
            this.Controls.Add(this._progress);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this._cb_override);
            this.Controls.Add(this._txt_dst_path);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.label5);
            this.Controls.Add(this._src_ext);
            this.Controls.Add(this._cb_file_lower);
            this.Controls.Add(this._btn_convert);
            this.Controls.Add(this._txt_mipmap);
            this.Controls.Add(this._txt_path);
            this.Controls.Add(this.label3);
            this.Controls.Add(this._cb_path);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Image Convert";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox _txt_path;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.CheckBox _cb_path;
        private System.Windows.Forms.Button _btn_convert;
        private System.Windows.Forms.CheckedListBox _src_ext;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox _txt_dst_path;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.CheckBox _cb_override;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox _txt_mipmap;
        private System.Windows.Forms.CheckBox _cb_file_lower;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.ToolTip toolTip;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.CheckBox _cb_dds;
        private System.Windows.Forms.ComboBox _cb_dds_format;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.ComboBox _cb_pk_format;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.ComboBox _cb_pk_type;
        private System.Windows.Forms.CheckBox _cb_pkm;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ProgressBar _progress;
        private System.Windows.Forms.Label _labal_progress;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.ComboBox _cb_astc_format;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.ComboBox _cb_astc_type;
        private System.Windows.Forms.CheckBox _cb_astc;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.ComboBox _cb_astc_quality;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.ComboBox _cb_astc_model;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.ToolTip toolTip_astc_model;
    }
}

