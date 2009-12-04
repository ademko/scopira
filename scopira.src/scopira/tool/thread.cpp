
#include <scopira/tool/platform.h>

// include the real .cpp file

#ifdef PLATFORM_QTTHREADS
#include <scopira/tool/thread_qtthreads.cpp>
#else
#include <scopira/tool/thread_pthreads.cpp>
#endif
