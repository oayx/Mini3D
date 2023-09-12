
using System.Runtime.InteropServices;

namespace ImageConvert
{
    public class EngineWrapper
    {
        /// <summary>
        /// 反射hlsl信息，写入到material文件
        /// </summary>
        /// <param name="argc">字符串数量</param>
        /// <param name="argv">字符串数组</param>
        /// <return>0:成功</return>
        /// CallingConvention介绍：
        /// Cdecl	    调用方清理堆栈。这使您能够调用具有 varargs 的函数（如 Printf），使之可用于接受可变数目的参数的方法。 
        /// FastCall    不支持此调用约定。
        /// StdCall     被调用方清理堆栈。这是使用平台 invoke 调用非托管函数的默认约定。 
        /// ThisCall    第一个参数是 this 指针，它存储在寄存器 ECX 中。其他参数被推送到堆栈上。此调用约定用于对从非托管 DLL 导出的类调用方法。 
        /// Winapi      此成员实际上不是调用约定，而是使用了默认平台调用约定。例如，在 Windows 上默认为 StdCall，在 Windows CE.NET 上默认为 Cdecl。 
        [DllImport("engine.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int dll_astc_main(int argc, string[] argv);
        [DllImport("engine.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int dll_dxt_main(int argc, string[] argv);
        [DllImport("engine.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int dll_etc_main(int argc, string[] argv);
    }
}
