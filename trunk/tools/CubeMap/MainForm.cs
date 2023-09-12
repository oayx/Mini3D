using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using dc.tools.core;

namespace dc.tools.cubemap
{
    public partial class MainForm : System.Windows.Forms.Form
    {
        private const string TMP_FILE = "tmp/tmp_save_{0}.jpg";
        private int _tmp_file_count = 0;
        private string _open_path = string.Empty;

        public MainForm()
        {
            InitializeComponent();
        }

        private void MainFor_load(object sender, EventArgs e)
        {
            this.pictureBox_right.AllowDrop = true;
            this.pictureBox_left.AllowDrop = true;
            this.pictureBox_top.AllowDrop = true;
            this.pictureBox_bottom.AllowDrop = true;
            this.pictureBox_front.AllowDrop = true;
            this.pictureBox_back.AllowDrop = true;

            try
            {
                string path = Path.GetDirectoryName(TMP_FILE);
                if (!Directory.Exists(path))
                    Directory.CreateDirectory(path);
            }
            catch
            {
                MessageBox.Show("create tmp path error", "Error", MessageBoxButtons.OK);
            }
        }

        private void MainFor_formClosed(object sender, FormClosedEventArgs e)
        {
            //删除临时产生的文件
            try
            {
                //删除文件
                string path = Path.GetDirectoryName(TMP_FILE);
                DirectoryInfo dir = new DirectoryInfo(path);
                FileInfo[] allFile = dir.GetFiles();
                foreach (FileInfo fi in allFile)
                {
                    File.Delete(fi.FullName);
                }
                //删除目录
                Directory.Delete(path);
            }
            catch
            {
                MessageBox.Show("delete tmp path error", "Error", MessageBoxButtons.OK);
            }
        }

        private void pictureBox_right_DragDrop(object sender, DragEventArgs e)
        {
            string fileName = ((System.Array)e.Data.GetData(DataFormats.FileDrop)).GetValue(0).ToString();
            this.pictureBox_right.Image = Image.FromFile(fileName);
            this.pictureBox_right.ImageLocation = fileName;
        }

        private void pictureBox_right_DragOver(object sender, DragEventArgs e)
        {
            e.Effect = DragDropEffects.All;
        }

        private void pictureBox_left_DragDrop(object sender, DragEventArgs e)
        {
            string fileName = ((System.Array)e.Data.GetData(DataFormats.FileDrop)).GetValue(0).ToString();
            this.pictureBox_left.Image = Image.FromFile(fileName);
            this.pictureBox_left.ImageLocation = fileName;
        }

        private void pictureBox_left_DragOver(object sender, DragEventArgs e)
        {
            e.Effect = DragDropEffects.All;
        }

        private void pictureBox_top_DragDrop(object sender, DragEventArgs e)
        {
            string fileName = ((System.Array)e.Data.GetData(DataFormats.FileDrop)).GetValue(0).ToString();
            this.pictureBox_top.Image = Image.FromFile(fileName);
            this.pictureBox_top.ImageLocation = fileName;
        }

        private void pictureBox_top_DragOver(object sender, DragEventArgs e)
        {
            e.Effect = DragDropEffects.All;
        }

        private void pictureBox_botto_dragDrop(object sender, DragEventArgs e)
        {
            string fileName = ((System.Array)e.Data.GetData(DataFormats.FileDrop)).GetValue(0).ToString();
            this.pictureBox_bottom.Image = Image.FromFile(fileName);
            this.pictureBox_bottom.ImageLocation = fileName;
        }

        private void pictureBox_botto_dragOver(object sender, DragEventArgs e)
        {
            e.Effect = DragDropEffects.All;
        }

        private void pictureBox_front_DragDrop(object sender, DragEventArgs e)
        {
            string fileName = ((System.Array)e.Data.GetData(DataFormats.FileDrop)).GetValue(0).ToString();
            this.pictureBox_front.Image = Image.FromFile(fileName);
            this.pictureBox_front.ImageLocation = fileName;
        }

        private void pictureBox_front_DragOver(object sender, DragEventArgs e)
        {
            e.Effect = DragDropEffects.All;
        }

        private void pictureBox_back_DragDrop(object sender, DragEventArgs e)
        {
            string fileName = ((System.Array)e.Data.GetData(DataFormats.FileDrop)).GetValue(0).ToString();
            this.pictureBox_back.Image = Image.FromFile(fileName);
            this.pictureBox_back.ImageLocation = fileName;
        }

        private void pictureBox_back_DragOver(object sender, DragEventArgs e)
        {
            e.Effect = DragDropEffects.All;
        }

        private void button_open_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = "CubeMap|*.cube";// 设置保存文件的类型，即文件的扩展名
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                string fileName = dlg.FileName;

                FileStream stream = null;
                try
                {
                    stream = File.Open(fileName, FileMode.Open, FileAccess.Read);
                    byte[] by = new byte[4];

                    //header
                    stream.Read(by, 0, by.Length);
                    string header = ByteUtils.ByteToString(by);
                    if(header != "cube")
                    {
                        MessageBox.Show("file flag error", "Error", MessageBoxButtons.OK);
                        return;
                    }

                    //size
                    stream.Read(by, 0, by.Length);
                    int w = ByteUtils.Byte4ToInt32(by[0], by[1], by[2], by[3]);
                    stream.Read(by, 0, by.Length);
                    int h = ByteUtils.Byte4ToInt32(by[0], by[1], by[2], by[3]);

                    for(int face = 0; face < 6; ++face)
                    {
                        //flag
                        int flag = stream.ReadByte();
                        //size
                        stream.Read(by, 0, by.Length);
                        int size = ByteUtils.Byte4ToInt32(by[0], by[1], by[2], by[3]);
                        //content
                        byte[] content = new byte[size];
                        stream.Read(content, 0, content.Length);

                        //生成贴图
                        PictureBox pic = null;
                        switch (face)
                        {
                            case 0: pic = this.pictureBox_right; break;
                            case 1: pic = this.pictureBox_left; break;
                            case 2: pic = this.pictureBox_top; break;
                            case 3: pic = this.pictureBox_bottom; break;
                            case 4: pic = this.pictureBox_front; break;
                            case 5: pic = this.pictureBox_back; break;
                        }
                        Bitmap bitmap = new Bitmap(w, h, PixelFormat.Format24bppRgb);
                        BitmapData bmpData = bitmap.LockBits(new System.Drawing.Rectangle(0, 0, bitmap.Width, bitmap.Height), ImageLockMode.ReadOnly, PixelFormat.Format24bppRgb);
                        System.Runtime.InteropServices.Marshal.Copy(content, 0, bmpData.Scan0, bitmap.Width * bitmap.Height * 3);
                        bitmap.UnlockBits(bmpData);

                        pic.Image = Image.FromHbitmap(bitmap.GetHbitmap());

                        //保存
                        try
                        {
                            string path = string.Format(TMP_FILE, ++_tmp_file_count);
                            pic.Image.Save(path);
                            pic.ImageLocation = path;
                        }
                        catch
                        {
                            MessageBox.Show("save tmp error", "Error", MessageBoxButtons.OK);
                        }
                    }
                }
                catch
                {
                    MessageBox.Show("open error", "Error", MessageBoxButtons.OK);
                }
                finally
                {
                    if (stream != null)
                    {
                        stream.Close();
                        stream.Dispose();
                    }
                }

                _open_path = fileName;
            }
        }

        private void button_save_Click(object sender, EventArgs e)
        {
            if (!string.IsNullOrEmpty(_open_path))
                this.ProcessSave(_open_path);
            else
                button_saveas_Click(null, null);
        }

        private void button_saveas_Click(object sender, EventArgs e)
        {
            SaveFileDialog dlg = new SaveFileDialog();
            dlg.Filter = "CubeMap|*.cube";// 设置保存文件的类型，即文件的扩展名
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                string fileName = dlg.FileName;
                this.ProcessSave(fileName);
                _open_path = fileName;
            }
        }

        private void button_clear_Click(object sender, EventArgs e)
        {
            try
            {
                for (int face = 0; face < 6; ++face)
                {
                    PictureBox pic = null;
                    switch (face)
                    {
                        case 0: pic = this.pictureBox_right; break;
                        case 1: pic = this.pictureBox_left; break;
                        case 2: pic = this.pictureBox_top; break;
                        case 3: pic = this.pictureBox_bottom; break;
                        case 4: pic = this.pictureBox_front; break;
                        case 5: pic = this.pictureBox_back; break;
                    }
                    pic.Image.Dispose();
                    pic.ImageLocation = string.Empty;
                }
            }
            catch
            {
                MessageBox.Show("clear error", "Error", MessageBoxButtons.OK);
            }
        }

        private bool CheckValid()
        {
            int w = -1, h = -1;
            for (int face = 0; face < 6; ++face)
            {
                PictureBox pic = null;
                switch (face)
                {
                    case 0: pic = this.pictureBox_right; break;
                    case 1: pic = this.pictureBox_left; break;
                    case 2: pic = this.pictureBox_top; break;
                    case 3: pic = this.pictureBox_bottom; break;
                    case 4: pic = this.pictureBox_front; break;
                    case 5: pic = this.pictureBox_back; break;
                }
                if (string.IsNullOrEmpty(pic.ImageLocation))
                {
                    MessageBox.Show("需要设置好6面图片", "Error", MessageBoxButtons.OK);
                    return false;
                }
                if (w == -1 || h == -1)
                {
                    w = pic.Image.Width; h = pic.Image.Height;
                }
                if (pic.Image.Width != w || pic.Image.Height != h)
                {
                    MessageBox.Show("图片像素需要一致", "Error", MessageBoxButtons.OK);
                    return false;
                }
                if (pic.Image.Width != pic.Image.Height)
                {
                    MessageBox.Show("图片像素宽高需要一致", "Error", MessageBoxButtons.OK);
                    return false;
                }
                if (pic.Image.Width % 2 != 0)
                {
                    MessageBox.Show("图片宽高需要需要是2的倍数", "Error", MessageBoxButtons.OK);
                    return false;
                }
            }

            return true;
        }

        private void ProcessSave(string save_path)
        {
            if (!CheckValid())
            {
                return;
            }

            FileStream stream = null;
            try
            {
                if (Path.GetExtension(save_path).ToLower() != ".cube")
                    save_path += ".cube";
                string folder = Path.GetDirectoryName(save_path);
                if (!Directory.Exists(folder))
                    Directory.CreateDirectory(folder);
                stream = File.Open(save_path, FileMode.Create, FileAccess.Write);

                //写入头
                byte[] by = ByteUtils.StringToByte("cube");
                stream.Write(by, 0, by.Length);
                //size
                by = ByteUtils.Int32ToByte4(this.pictureBox_right.Image.Size.Width);
                stream.Write(by, 0, by.Length);
                by = ByteUtils.Int32ToByte4(this.pictureBox_right.Image.Size.Height);
                stream.Write(by, 0, by.Length);

                //写入内容
                for (int i = 0; i < 6; ++i)
                {
                    PictureBox pic = null;
                    switch(i)
                    {
                        case 0: pic = this.pictureBox_right;break;
                        case 1: pic = this.pictureBox_left; break;
                        case 2: pic = this.pictureBox_top; break;
                        case 3: pic = this.pictureBox_bottom; break;
                        case 4: pic = this.pictureBox_front; break;
                        case 5: pic = this.pictureBox_back; break;
                    }
                    Bitmap bitmap = new Bitmap(pic.ImageLocation);
                    BitmapData bmpData = bitmap.LockBits(new System.Drawing.Rectangle(0, 0, bitmap.Width, bitmap.Height), ImageLockMode.ReadOnly, PixelFormat.Format24bppRgb);

                    //flag
                    stream.WriteByte((byte)i);

                    //bytes
                    by = ByteUtils.Int32ToByte4(bmpData.Stride * bmpData.Height);
                    stream.Write(by, 0, by.Length);

                    //contents
                    byte[] pixels = new byte[bmpData.Stride * bmpData.Height];
                    System.Runtime.InteropServices.Marshal.Copy(bmpData.Scan0, pixels, 0, bmpData.Stride * bmpData.Height);
                    stream.Write(pixels, 0, pixels.Length);
                    bitmap.UnlockBits(bmpData);
                }

                stream.Flush();
                MessageBox.Show("save succeed", "Info", MessageBoxButtons.OK);
            }
            catch
            {
                MessageBox.Show("save error", "Error", MessageBoxButtons.OK);
            }
            finally
            {
                if (stream != null)
                {
                    stream.Close();
                    stream.Dispose();
                }
            }
        }
    }
}