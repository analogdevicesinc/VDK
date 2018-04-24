### VDK Real-time Operating System

*Module:* VDK Real Time Operating System

*URL:*

*Description:* Public port of the VDK Real-time operating system from Analog
    Devices. The RTOS was originally provided as part of the VisualDSP++
    development studio, for Analog Devices SHARC, Blackfin and TigerSHARC
    processors.

*Licensing:* Please refer to LICENSE.md

*Support:*  This product is provided with no support whatsoever.
    This product is not supported by Analog Devices.


*Building Requirements:*  To build this product you will require the following:
    * A host System running Windows 7 or Windows 10.
    * Cygwin installation on host PC
    * Analog Devices DSP Development tools. Either
    *   VisualDSP++ 5.1.2 and a valid license
    * OR
    *   CrossCore Embedded Studio 2.8.0 and a valid license
    * Active Python TCL version 8.6

*Building the Product:* The product is built as a microkernel and platform
    specific support libraries.
    The following architectures, processors and silicon revisions are supported.
    These values should be passed into the top level makefile.
    BE CAREFUL of case in the architecture, processor and core fields, enterring 
    incorrect uppper/lower case will result in the build doing nothing rather than
    failing.
    ARCH:
	blackfin: CORE=10x (lowercase x)
		BF512: 0.0
		BF514: 0.0
		BF516: 0.0
		BF522: 0.0
		BF525: 0.0
		BF527: 0.0
		BF531: 0.1 0.2 0.3 0.5
		BF532: 0.1 0.2 0.3 0.5
		BF533: 0.1 0.2 0.3 0.5
		BF534: 0.0 0.3
		BF535: 0.2 1.0 1.1 1.2 1.3
		BF536: 0.0 0.3
		BF537: 0.0 0.3
		BF539: 0.0 0.3
		BF561: 0.2 0.3 0.4 0.5 any
		BF542: 0.0
		BF544: 0.0
		BF547: 0.0
		BF548: 0.0
		BF549: 0.0
	SHARC: CORE=(2106x, 2116x, 2126x, 2136x, 2137x, 2146x, 2147x, 2148x) (lowercase x)
		21060: 2.1 3.0 3.1
		21061: 3.1
		21062: 2.1 3.0 3.1
		21065L: 2.1 3.0 3.1
		21160: 0.0 0.1 1.1 1.2
		21161: 0.3 1.0 1.1 1.2
		21261: 0.0 0.1
		21262: 0.1
		21266: 0.1 0.2
		21267: 0.0 0.1 0.2
		21362: 0.0
		21363: 0.0 0.1 0.2
		21364: 0.0 0.1 0.2
		21365: 0.0 0.2
		21366: 0.0 0.1 0.2
		21367: 0.0
		21368: 0.0
		21369: 0.0
		21371: 0.0
		21375: 0.0
		21462: 0.0
		21465: 0.0
		21467: 0.0
		21469: 0.0
		21472: 0.0
		21475: 0.0
		21477: 0.0
		21478: 0.0
		21479: 0.0
    Kernel and libs needs to be built individually.
    To Build the kernel:
	1. Edit the top level Makefile to configure the location of your toolchain,
	   prefered architecture, core, processor, silicon revision, output directory
	2. make kernel
    To Build the libraries:
	1. Edit the top level Makefile to configure the location of your toolchain,
	   prefered architecture, core, processor, silicon revision, output directory
	2. make libs
    Note that by default the release libraries will be built and installed.
    Debug libraries can be built by replacing the release command in the top level
    Makefile for a debug command.

    The output will be placed in the output directory (OUT_DIR in Makefile).
    The components in the output directory are structured so that they can be dropped
    directly on top of the VisualDSP toolchain.
    Additional movement of the files may be required if you wish to install on top of
    CrossCore Embeded Studio.
	
