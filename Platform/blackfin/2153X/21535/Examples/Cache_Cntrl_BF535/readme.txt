Description:
------------

This example provides code for initialising 21535 L1 memory as cache.

The bulk of the work is done in cache_init.asm, the device driver class 
CacheController is extremely simple and is responsible only for calling into 
cache_init.asm during initialisation. The thread class MainThread is unchanged 
from the skeleton created by the IDDE and is included only as a placeholder 
to allow the project to build, it should be replaced with the user's own thread
implementation(s).

One of the following symbols must be #defined in cache_init.asm:

    ALL_CACHE	     - all 3 L1 blocks configured as cache
    DATA_ACACHE	     - only L1 data block 1 configured as cache
    DATA_ABCACHE     - both L1 data blocks configured as cache
    CODE_CACHE	     - only L1 code block configured as cache

As supplied, ALL_CACHE is defined (on line 15).

Note that any L1 block that is configured as cache cannot be used as memory.
Also, there is no option DATA_BCACHE. If only one data bank is to be configured
as cache it must be bank A.  


Files:
------

CacheController.cpp
CacheController.h
     A VDK device-driver class which calls the function init_cache() 
     (in cache_init.asm) during initialisation.
MainThread.cpp
MainThread.h
     A placeholder boot thread type.
cache_init.asm
     Contains the C-callable function init_cache() and its associated data 
     tables.

