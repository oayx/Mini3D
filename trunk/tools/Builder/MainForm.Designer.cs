namespace dc.tools.builder
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
            this._btn_builder = new System.Windows.Forms.Button();
            this._txt_path = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this._bar_publish = new System.Windows.Forms.ProgressBar();
            this._label_progress = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // _btn_builder
            // 
            this._btn_builder.Location = new System.Drawing.Point(383, 4);
            this._btn_builder.Name = "_btn_builder";
            this._btn_builder.Size = new System.Drawing.Size(65, 31);
            this._btn_builder.TabIndex = 17;
            this._btn_builder.Text = "发 布";
            this._btn_builder.UseVisualStyleBackColor = true;
            this._btn_builder.Click += new System.EventHandler(this._btn_builder_Click);
            // 
            // _txt_path
            // 
            this._txt_path.Location = new System.Drawing.Point(51, 10);
            this._txt_path.Name = "_txt_path";
            this._txt_path.Size = new System.Drawing.Size(326, 21);
            this._txt_path.TabIndex = 16;
            this._txt_path.Click += new System.EventHandler(this._txt_path_Click);
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(7, 14);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(41, 12);
            this.label9.TabIndex = 15;
            this.label9.Text = "目  录";
            // 
            // _bar_publish
            // 
            this._bar_publish.Location = new System.Drawing.Point(0, 288);
            this._bar_publish.Name = "_bar_publish";
            this._bar_publish.Size = new System.Drawing.Size(421, 23);
            this._bar_publish.TabIndex = 18;
            // 
            // _label_progress
            // 
            this._label_progress.AutoSize = true;
            this._label_progress.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this._label_progress.Location = new System.Drawing.Point(424, 293);
            this._label_progress.Name = "_label_progress";
            this._label_progress.Size = new System.Drawing.Size(21, 13);
            this._label_progress.TabIndex = 19;
            this._label_progress.Text = "0%";
            this._label_progress.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(460, 311);
            this.Controls.Add(this._label_progress);
            this.Controls.Add(this._bar_publish);
            this.Controls.Add(this._btn_builder);
            this.Controls.Add(this._txt_path);
            this.Controls.Add(this.label9);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "发布";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button _btn_builder;
        private System.Windows.Forms.TextBox _txt_path;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.ProgressBar _bar_publish;
        private System.Windows.Forms.Label _label_progress;
    }
}

