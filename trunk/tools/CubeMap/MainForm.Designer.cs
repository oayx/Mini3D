namespace dc.tools.cubemap
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
            this.pictureBox_right = new System.Windows.Forms.PictureBox();
            this.pictureBox_left = new System.Windows.Forms.PictureBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.pictureBox_bottom = new System.Windows.Forms.PictureBox();
            this.pictureBox_top = new System.Windows.Forms.PictureBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.pictureBox_back = new System.Windows.Forms.PictureBox();
            this.pictureBox_front = new System.Windows.Forms.PictureBox();
            this.button_open = new System.Windows.Forms.Button();
            this.button_save = new System.Windows.Forms.Button();
            this.button_saveas = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.button_clear = new System.Windows.Forms.Button();
            this.label8 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox_right)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox_left)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox_bottom)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox_top)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox_back)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox_front)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // pictureBox_right
            // 
            this.pictureBox_right.BackColor = System.Drawing.SystemColors.ControlDark;
            this.pictureBox_right.Location = new System.Drawing.Point(12, 24);
            this.pictureBox_right.Name = "pictureBox_right";
            this.pictureBox_right.Size = new System.Drawing.Size(200, 200);
            this.pictureBox_right.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox_right.TabIndex = 0;
            this.pictureBox_right.TabStop = false;
            this.pictureBox_right.DragDrop += new System.Windows.Forms.DragEventHandler(this.pictureBox_right_DragDrop);
            this.pictureBox_right.DragOver += new System.Windows.Forms.DragEventHandler(this.pictureBox_right_DragOver);
            // 
            // pictureBox_left
            // 
            this.pictureBox_left.BackColor = System.Drawing.SystemColors.ControlDark;
            this.pictureBox_left.Location = new System.Drawing.Point(12, 249);
            this.pictureBox_left.Name = "pictureBox_left";
            this.pictureBox_left.Size = new System.Drawing.Size(200, 200);
            this.pictureBox_left.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox_left.TabIndex = 1;
            this.pictureBox_left.TabStop = false;
            this.pictureBox_left.DragDrop += new System.Windows.Forms.DragEventHandler(this.pictureBox_left_DragDrop);
            this.pictureBox_left.DragOver += new System.Windows.Forms.DragEventHandler(this.pictureBox_left_DragOver);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(82, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(59, 12);
            this.label1.TabIndex = 2;
            this.label1.Text = "Right(+X)";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(82, 234);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(53, 12);
            this.label2.TabIndex = 3;
            this.label2.Text = "Left(-X)";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(308, 234);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(65, 12);
            this.label3.TabIndex = 7;
            this.label3.Text = "Bottom(-Y)";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(308, 9);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(47, 12);
            this.label4.TabIndex = 6;
            this.label4.Text = "Top(+Y)";
            // 
            // pictureBox_bottom
            // 
            this.pictureBox_bottom.BackColor = System.Drawing.SystemColors.ControlDark;
            this.pictureBox_bottom.Location = new System.Drawing.Point(238, 249);
            this.pictureBox_bottom.Name = "pictureBox_bottom";
            this.pictureBox_bottom.Size = new System.Drawing.Size(200, 200);
            this.pictureBox_bottom.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox_bottom.TabIndex = 5;
            this.pictureBox_bottom.TabStop = false;
            this.pictureBox_bottom.DragDrop += new System.Windows.Forms.DragEventHandler(this.pictureBox_botto_dragDrop);
            this.pictureBox_bottom.DragOver += new System.Windows.Forms.DragEventHandler(this.pictureBox_botto_dragOver);
            // 
            // pictureBox_top
            // 
            this.pictureBox_top.BackColor = System.Drawing.SystemColors.ControlDark;
            this.pictureBox_top.Location = new System.Drawing.Point(238, 24);
            this.pictureBox_top.Name = "pictureBox_top";
            this.pictureBox_top.Size = new System.Drawing.Size(200, 200);
            this.pictureBox_top.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox_top.TabIndex = 4;
            this.pictureBox_top.TabStop = false;
            this.pictureBox_top.DragDrop += new System.Windows.Forms.DragEventHandler(this.pictureBox_top_DragDrop);
            this.pictureBox_top.DragOver += new System.Windows.Forms.DragEventHandler(this.pictureBox_top_DragOver);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(533, 234);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(53, 12);
            this.label5.TabIndex = 11;
            this.label5.Text = "Back(-Z)";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(533, 9);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(59, 12);
            this.label6.TabIndex = 10;
            this.label6.Text = "Front(+Z)";
            // 
            // pictureBox_back
            // 
            this.pictureBox_back.BackColor = System.Drawing.SystemColors.ControlDark;
            this.pictureBox_back.Location = new System.Drawing.Point(463, 249);
            this.pictureBox_back.Name = "pictureBox_back";
            this.pictureBox_back.Size = new System.Drawing.Size(200, 200);
            this.pictureBox_back.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox_back.TabIndex = 9;
            this.pictureBox_back.TabStop = false;
            this.pictureBox_back.DragDrop += new System.Windows.Forms.DragEventHandler(this.pictureBox_back_DragDrop);
            this.pictureBox_back.DragOver += new System.Windows.Forms.DragEventHandler(this.pictureBox_back_DragOver);
            // 
            // pictureBox_front
            // 
            this.pictureBox_front.BackColor = System.Drawing.SystemColors.ControlDark;
            this.pictureBox_front.Location = new System.Drawing.Point(463, 24);
            this.pictureBox_front.Name = "pictureBox_front";
            this.pictureBox_front.Size = new System.Drawing.Size(200, 200);
            this.pictureBox_front.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox_front.TabIndex = 8;
            this.pictureBox_front.TabStop = false;
            this.pictureBox_front.DragDrop += new System.Windows.Forms.DragEventHandler(this.pictureBox_front_DragDrop);
            this.pictureBox_front.DragOver += new System.Windows.Forms.DragEventHandler(this.pictureBox_front_DragOver);
            // 
            // button_open
            // 
            this.button_open.Location = new System.Drawing.Point(20, 15);
            this.button_open.Name = "button_open";
            this.button_open.Size = new System.Drawing.Size(75, 30);
            this.button_open.TabIndex = 12;
            this.button_open.Text = "Open";
            this.button_open.UseVisualStyleBackColor = true;
            this.button_open.Click += new System.EventHandler(this.button_open_Click);
            // 
            // button_save
            // 
            this.button_save.Location = new System.Drawing.Point(20, 51);
            this.button_save.Name = "button_save";
            this.button_save.Size = new System.Drawing.Size(75, 30);
            this.button_save.TabIndex = 13;
            this.button_save.Text = "Save";
            this.button_save.UseVisualStyleBackColor = true;
            this.button_save.Click += new System.EventHandler(this.button_save_Click);
            // 
            // button_saveas
            // 
            this.button_saveas.Location = new System.Drawing.Point(20, 87);
            this.button_saveas.Name = "button_saveas";
            this.button_saveas.Size = new System.Drawing.Size(75, 30);
            this.button_saveas.TabIndex = 14;
            this.button_saveas.Text = "SaveAs";
            this.button_saveas.UseVisualStyleBackColor = true;
            this.button_saveas.Click += new System.EventHandler(this.button_saveas_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.button_clear);
            this.groupBox1.Controls.Add(this.button_save);
            this.groupBox1.Controls.Add(this.button_saveas);
            this.groupBox1.Controls.Add(this.button_open);
            this.groupBox1.Location = new System.Drawing.Point(670, 16);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(112, 164);
            this.groupBox1.TabIndex = 15;
            this.groupBox1.TabStop = false;
            // 
            // button_clear
            // 
            this.button_clear.Location = new System.Drawing.Point(20, 123);
            this.button_clear.Name = "button_clear";
            this.button_clear.Size = new System.Drawing.Size(75, 30);
            this.button_clear.TabIndex = 15;
            this.button_clear.Text = "Clear";
            this.button_clear.UseVisualStyleBackColor = true;
            this.button_clear.Click += new System.EventHandler(this.button_clear_Click);
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.ForeColor = System.Drawing.Color.Red;
            this.label8.Location = new System.Drawing.Point(13, 459);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(299, 12);
            this.label8.TabIndex = 16;
            this.label8.Text = "*贴图大小必须一样，像素是2的倍数，保存格式为RGB24";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(788, 482);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.pictureBox_back);
            this.Controls.Add(this.pictureBox_front);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.pictureBox_bottom);
            this.Controls.Add(this.pictureBox_top);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.pictureBox_left);
            this.Controls.Add(this.pictureBox_right);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "CubeMap Generate";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.MainFor_formClosed);
            this.Load += new System.EventHandler(this.MainFor_load);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox_right)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox_left)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox_bottom)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox_top)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox_back)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox_front)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox pictureBox_right;
        private System.Windows.Forms.PictureBox pictureBox_left;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.PictureBox pictureBox_bottom;
        private System.Windows.Forms.PictureBox pictureBox_top;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.PictureBox pictureBox_back;
        private System.Windows.Forms.PictureBox pictureBox_front;
        private System.Windows.Forms.Button button_open;
        private System.Windows.Forms.Button button_save;
        private System.Windows.Forms.Button button_saveas;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button button_clear;
        private System.Windows.Forms.Label label8;
    }
}

