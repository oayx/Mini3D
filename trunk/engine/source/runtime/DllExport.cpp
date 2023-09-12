#include "DllExport.h"

extern int astc_main(int argc, char ** argv);
extern int dxt_main(int argc, char** argv);
extern int etc_main(int argc, char** argv);
extern void tinystl_main();

int dll_astc_main(int argc, char ** argv)
{
	return astc_main(argc, argv);
}
int dll_dxt_main(int argc, char** argv)
{
#if defined(DC_PLATFORM_WIN32)
	return dxt_main(argc, argv);
#else
	return -1;
#endif
}
int dll_etc_main(int argc, char** argv)
{
#if defined(DC_PLATFORM_WIN32)
	return etc_main(argc, argv);
#else
	return -1;
#endif
}
void dll_tinystl_test()
{
	tinystl_main();
}