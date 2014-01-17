#include <gst/gst.h>

#if GST_CHECK_VERSION(1,2,0)
    #include "1.2/lzo.c"
#elif GST_CHECK_VERSION(1,0,0)
    #include "1.0/lzo.c"
#else
    #include "0.10/lzo.c"
#endif
