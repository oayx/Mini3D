using System.Collections;
using System.Runtime.InteropServices;
using System;

namespace dc.tools.core
{
    /// <summary>
    /// 字节处理
    /// </summary>
    public class ByteUtils
    {
        public static int Byte4ToInt32(byte[] bytes, int idx = 0)
        {
            int vl = 0;
            if (bytes.Length > idx + 0) vl |= (bytes[idx] << 0);
            if (bytes.Length > idx + 1) vl |= (bytes[idx + 1] << 8);
            if (bytes.Length > idx + 2) vl |= (bytes[idx + 2] << 16);
            if (bytes.Length > idx + 3) vl |= (bytes[idx + 3] << 24);
            return vl;
        }
        public static int Byte4ToInt32(byte byte0, byte byte1, byte byte2, byte byte3)
        {
            int vl = 0;
            vl |= (byte0 << 0);
            vl |= (byte1 << 8);
            vl |= (byte2 << 16);
            vl |= (byte3 << 24);
            return vl;
        }
        public static byte[] Int16ToByte2(int vl)
        {
            byte[] by = new byte[2];
            by[0] = (byte)(vl >> 0);
            by[1] = (byte)(vl >> 8);
            return by;
        }
        public static byte[] Int32ToByte4(int vl)
        {
            byte[] by = new byte[4];
            by[0] = (byte)(vl >> 0);
            by[1] = (byte)(vl >> 8);
            by[2] = (byte)(vl >> 16);
            by[3] = (byte)(vl >> 24);
            return by;
        }
        
        public static string ByteToString(byte[] total)
        {
            string utf8string = System.Text.Encoding.UTF8.GetString(total);
            return utf8string;
        }
        public static byte[] StringToByte(string utf8string)
        {
            byte[] by = System.Text.Encoding.UTF8.GetBytes(utf8string);
            return by;
        }
    }
}