using System.Collections;
using System.Collections.Generic;
using System.Text;
using System;


namespace dc.tools.core
{
    /// <summary>
    /// 字符串
    /// @author hannibal
    /// @time 2014-11-14
    /// </summary>
    public static class StringUtils
    {
        /// <summary>
        /// 是否纯数字
        /// </summary>
        /// <param name="str"></param>
        /// <returns></returns>
        public static bool IsInt(string str)
        {
            int v;
            if (int.TryParse(str, out v))
                return true;
            return false;
        }
        /**获取两个字符串中间的字符串*/
        static public string Search_string(string s, string s1, string s2)
        {
            int n1, n2;
            n1 = s.IndexOf(s1, 0);  //开始位置 
            if (n1 < 0) return "";
            n1 += s1.Length;
            n2 = s.IndexOf(s2, n1);             //结束位置  
            if (n2 < 0) return "";
            return s.Substring(n1, n2 - n1);    //取搜索的条数，用结束的位置-开始的位置,并返回  
        }

        #region Split
        /// <summary>
        /// 分割字符串
        /// </summary>
        /// <param name="str">源字符串</param>
        /// <param name="split">分割符</param>
        public static List<T> Split<T>(string str, char split)
        {
            List<T> tmpList = new List<T>();
            if (string.IsNullOrEmpty(str)) return tmpList;

            string[] strArr = str.Split(split);
            if (typeof(T) == typeof(int))
            {
                for (int i = 0; i < strArr.Length; i++)
                {
                    tmpList.Add((T)(object)int.Parse(strArr[i]));
                }
            }
            else if (typeof(T) == typeof(uint))
            {
                for (int i = 0; i < strArr.Length; i++)
                {
                    tmpList.Add((T)(object)uint.Parse(strArr[i]));
                }
            }
            else if (typeof(T) == typeof(long))
            {
                for (int i = 0; i < strArr.Length; i++)
                {
                    tmpList.Add((T)(object)long.Parse(strArr[i]));
                }
            }
            else if (typeof(T) == typeof(ulong))
            {
                for (int i = 0; i < strArr.Length; i++)
                {
                    tmpList.Add((T)(object)ulong.Parse(strArr[i]));
                }
            }
            else if (typeof(T) == typeof(float))
            {
                for (int i = 0; i < strArr.Length; i++)
                {
                    tmpList.Add((T)(object)float.Parse(strArr[i]));
                }
            }
            else if (typeof(T) == typeof(byte))
            {
                for (int i = 0; i < strArr.Length; i++)
                {
                    tmpList.Add((T)(object)byte.Parse(strArr[i]));
                }
            }
            else if (typeof(T) == typeof(short))
            {
                for (int i = 0; i < strArr.Length; i++)
                {
                    tmpList.Add((T)(object)short.Parse(strArr[i]));
                }
            }
            else if (typeof(T) == typeof(string))
            {
                for (int i = 0; i < strArr.Length; i++)
                {
                    tmpList.Add((T)(object)strArr[i]);
                }
            }
            else
            {
                Log.Error("Split : type error:" + typeof(T));
            }

            return tmpList;
        }

        /// <summary>
        /// 数组里面嵌套键值对
        /// </summary>
        /// <param name="str">需要解析的字符串</param>
        /// <param name="split_arr">数组切分符号</param>
        /// <param name="splite_key">键值对切分符号</param>
        public static List<KeyValuePair<T, K>> SplitKeyValues<T, K>(string str, char split_arr = ',', char split_key = '|')
        {
            List<KeyValuePair<T, K>> list = new List<KeyValuePair<T, K>>();

            if (string.IsNullOrEmpty(str)) return list;

            string[] strArr = str.Split(split_arr);
            for (int i = 0; i < strArr.Length; ++i)
            {
                string strKeyValue = strArr[i];
                if (string.IsNullOrEmpty(strKeyValue)) continue;

                string[] arrKeyValue = strKeyValue.Split(split_key);
                if (arrKeyValue.Length != 2)
                {
                    Log.Error("字符串格式错误:" + str);
                    continue;
                }
                T key = StringUtils.Convert<T>(arrKeyValue[0]);
                K value = StringUtils.Convert<K>(arrKeyValue[1]);
                list.Add(new KeyValuePair<T, K>(key, value));
            }
            return list;
        }
        public static List<List<T>> SplitKeyValues<T>(string str, char split_arr = ',', char split_key = '|')
        {
            List<List<T>> list = new List<List<T>>();
            if (string.IsNullOrEmpty(str)) return list;

            string[] strArr = str.Split(split_arr);
            for (int i = 0; i < strArr.Length; ++i)
            {
                string strKeyValue = strArr[i];
                if (string.IsNullOrEmpty(strKeyValue)) continue;

                string[] arrKeyValue = strKeyValue.Split(split_key);
                List<T> list_item = new List<T>();
                for (int j = 0; j < arrKeyValue.Length; ++j)
                {
                    T v = StringUtils.Convert<T>(arrKeyValue[j]);
                    list_item.Add(v);
                }
                list.Add(list_item);
            }
            return list;
        }
        public static KeyValuePair<T, K> SplitKeyValue<T, K>(string str, char split_key = ',')
        {
            KeyValuePair<T, K> out_value;
            if (string.IsNullOrEmpty(str)) return default(KeyValuePair<T, K>);

            string[] arrKeyValue = str.Split(split_key);
            if (arrKeyValue.Length != 2)
            {
                Log.Error("字符串格式错误:" + str);
                return default(KeyValuePair<T, K>);
            }
            T key = StringUtils.Convert<T>(arrKeyValue[0]);
            K value = StringUtils.Convert<K>(arrKeyValue[1]);
            out_value = new KeyValuePair<T, K>(key, value);
            return out_value;
        }
        #endregion

        #region 类型转化
        public static byte ToByte(this string str)
        {
            byte v = 0;
            if (byte.TryParse(str, out v))
                return v;
            Log.Warn("ToByte类型转化失败:" + str);
            return 0;
        }
        public static short ToShort(this string str)
        {
            short v = 0;
            if (short.TryParse(str, out v))
                return v;
            Log.Warn("ToShort类型转化失败:" + str);
            return 0;
        }
        public static ushort ToUShort(this string str)
        {
            ushort v = 0;
            if (ushort.TryParse(str, out v))
                return v;
            Log.Warn("ToUShort类型转化失败:" + str);
            return 0;
        }
        public static int ToInt(this string str)
        {
            int v = 0;
            if (int.TryParse(str, out v))
                return v;
            Log.Warn("ToInt类型转化失败:" + str);
            return 0;
        }
        public static uint ToUInt(this string str)
        {
            uint v = 0;
            if (uint.TryParse(str, out v))
                return v;
            Log.Warn("ToUInt类型转化失败:" + str);
            return 0;
        }
        public static long ToLong(this string str)
        {
            long v = 0;
            if (long.TryParse(str, out v))
                return v;
            Log.Warn("ToLong:" + str);
            return 0;
        }
        public static ulong ToULong(this string str)
        {
            ulong v = 0;
            if (ulong.TryParse(str, out v))
                return v;
            Log.Warn("ToULong类型转化失败:" + str);
            return 0;
        }
        public static float ToFloat(this string str)
        {
            float v = 0;
            if (float.TryParse(str, out v))
                return v;
            Log.Warn("ToFloat类型转化失败:" + str);
            return 0;
        }
        public static double ToDouble(this string str)
        {
            double v = 0;
            if (double.TryParse(str, out v))
                return v;
            Log.Warn("ToDouble类型转化失败:" + str);
            return 0;
        }
        /// <summary>
        /// 字符串转基础类型
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="s"></param>
        /// <returns></returns>
        public static T Convert<T>(string s)
        {
            if (string.IsNullOrEmpty(s)) return default(T);
            System.Type fieldType = typeof(T);
            object value = null;
            try
            {
                if (fieldType == typeof(int))
                {
                    value = s.ToInt();
                }
                else if (fieldType == typeof(uint))
                {
                    value = s.ToUInt();
                }
                else if (fieldType == typeof(string))
                {
                    value = s;
                }
                else if (fieldType == typeof(long))
                {
                    value = s.ToLong();
                }
                else if (fieldType == typeof(ulong))
                {
                    value = s.ToULong();
                }
                else if (fieldType == typeof(float))
                {
                    value = s.ToFloat();
                }
                else if (fieldType == typeof(double))
                {
                    value = s.ToDouble();
                }
                else if (fieldType == typeof(short))
                {
                    value = s.ToShort();
                }
                else if (fieldType == typeof(ushort))
                {
                    value = s.ToUShort();
                }
                else if (fieldType == typeof(byte))
                {
                    value = s.ToByte();
                }
                else
                {
                    Log.Error("StringConvert : type error:" + fieldType);
                    value = default(T);
                }
            }
            catch (Exception e)
            {
                Log.Error("StringConvert : type error:" + e.ToString());
                value = default(T);
            }
            return (T)value;
        }
        public static object Convert(string s, Type fieldType)
        {
            if (string.IsNullOrEmpty(s)) return default(Type);

            if (fieldType == typeof(int))
            {
                return s.ToInt();
            }
            else if (fieldType == typeof(uint))
            {
                return s.ToUInt();
            }
            else if (fieldType == typeof(string))
            {
                return s;
            }
            else if (fieldType == typeof(long))
            {
                return s.ToLong();
            }
            else if (fieldType == typeof(ulong))
            {
                return s.ToULong();
            }
            else if (fieldType == typeof(float))
            {
                return s.ToFloat();
            }
            else if (fieldType == typeof(double))
            {
                return double.Parse(s);
            }
            else if (fieldType == typeof(short))
            {
                return s.ToShort();
            }
            else if (fieldType == typeof(ushort))
            {
                return s.ToUShort();
            }
            else if (fieldType == typeof(byte))
            {
                return s.ToByte();
            }
            else
            {
                Log.Error("StringConvert : Convert : type error:" + fieldType);
                return null;
            }
        }
        #endregion

        #region 时间
        /**
         * 分钟与秒格式(如-> 40:15)
         * @param seconds
         * @return 
         */
        public static string MinuteFormat(uint seconds)
        {
            uint min = seconds / 60;
            uint sec = seconds % 60;

            string min_str = min < 10 ? ("0" + min.ToString()) : (min.ToString());
            string sec_str = sec < 10 ? ("0" + sec.ToString()) : (sec.ToString());

            return min_str + ":" + sec_str;
        }

        /**
         * 时分秒格式(如-> 05:32:20)
         * @param seconds(秒)
         * @return 
         */
        public static string HourFormat(uint seconds)
        {
            uint hour = seconds / 3600;

            string hour_str = hour < 10 ? ("0" + hour.ToString()) : (hour.ToString());

            return hour_str + ":" + MinuteFormat(seconds % 3600);
        }
        #endregion
        
        #region 判断是否为特殊字符
        public static bool IsSpecialCharacter(char ch)
        {
            int change_num = (int)ch;
            if ((change_num >= 65 && change_num <= 90) || (change_num >= 97 && change_num <= 122) || change_num == 95)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        #endregion
    }
}