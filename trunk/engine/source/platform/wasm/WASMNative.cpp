#if defined(DC_PLATFORM_WASM)
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE
void wasm_set_main_loop(void (*callback_func)(void), int fps)
{
    emscripten_set_main_loop(callback_func, fps, 1);//1模拟死循环，后面代码不会再执行，类似while(1){}
}
//请求退出应用
void wasm_quit_application()
{
    emscripten_cancel_main_loop();
}
DC_END_NAMESPACE
#endif