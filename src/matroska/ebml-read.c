#include <gst/gst.h>

#if GST_CHECK_VERSION(1,2,0)
    #include "1.2/ebml-read.c"
#elif GST_CHECK_VERSION(1,0,0)
    #include "1.0/ebml-read.c"
#else
    #include "0.10/ebml-read.c"
#endif
