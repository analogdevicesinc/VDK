// BLACKfin module which optionally configures L1 code and
// data memory banks as cache.
//
// Author   : Colin McArthur
// Location : Edinburgh Design Centre
// Date     : 21 March 2002
//
/* Copyright (C) 2000 Analog Devices Inc., All Rights Reserved.
** This contains Analog Devices Background IP and Development IP as
** defined in the ADI/Intel Collaboration Agreement.
** ADI/Intel Confidential.
*/
//
// Assembly macros:-
//      ALL_CACHE     => code and data banks A and B are cache,
//      CODE_CACHE    => code bank as cache,
//      DATA_ABCACHE  => data banks A and B as cache,
//      DATA_ACACHE   => data bank A as cache
//
#include <Def21535.h>

#define ALL_CACHE

.section program;
	.align 2;
.global _init_cache;
_init_cache:
		[--SP] = (R7:3, P5:3);	// save preserved registers

	// Memory map configuration.
	// We assume the presence of the following areas, with the
	// following cache settings:
	// Core and System MMRs, not cached
	// L1 SRAM Scratch pad, not cached
	// L1 SRAM code area, instruction cache of L2
	// L1 SRAM data area A, data cache of L2
	// L1 SRAM data area B, data cache of L2
	// L2 SRAM 1MB, cached
        // Flash
        // SDRAM
        //
        // First we need to set the cache tag entries as invalid.
        // ********************* ISRAM invalidation *********************
        // ISRAM has 1x16kb bank
        //       of 4x4k sub-banks
        //       each of 4 ways
        //       each of 32 lines
        //       each of 32 bytes
	// Configure code bank as cache and enable it
	p0.l = (IMEM_CONTROL & 0xffff);
	p0.h = (IMEM_CONTROL >> 16);
	r0 = (IMC | ENIM);
        ssync;
	[p0] = r0;
        //
        ssync;
        i0.l = (ITEST_COMMAND & 0xffff);
	i0.h = (ITEST_COMMAND >> 16);
        i1.l = (ITEST_DATA0 & 0xffff);
	i1.h = (ITEST_DATA0 >> 16);
        // Write 0 to DATA0, making the line addressed in COMMAND
        // invalid.
        r0 = 0;
        [i1] = r0;
        // Now address each line in each way in each sub-bank.
        p4 = 4;         // Number of sub-banks, outer loop iterations
        r3.l = 0;
        r3.h = 1;       // Sub-bank increment
        r2 = 32;        // Line index increment
        p3 = r2;        // Number of line indices in each way,
                        // inner loop iterations
        // Way 0,1
        r4 = 2;         // Initial value for ITEST_COMMAND,
                        // way 0 - WRITE to TAG
        r5.h = 0x400;   // Initial value for ITEST_COMMAND,
        r5.l = 2;       // way 1 - WRITE to TAG
        lsetup(s_isubbanks, f_isubbanks) lc1=p4;
._init_cache.end:        
s_isubbanks:
        r0 = r4;
        r1 = r5;
        lsetup(s_isets, f_isets) lc0=p3;
s_isets: r0 = r0+|+r2 || [i0]=r0; // Do this line and set up for next
f_isets: r1 = r1+|+r2 || [i0]=r1; // Ditto
        r4=r4+r3;       // Way 0 for next sub-bank
f_isubbanks:
        r5=r5+r3;       // Way 1 for next sub-bank
        //
        // Now do it all again for ways 2 and 3
        r4.h=0x800;     // Initial value for ITEST_COMMAND,
                        // way 2 - WRITE to TAG
        r4.l=2;                
        r5.h=0xc00;     // Initial value for ITEST_COMMAND,
                        // way 3 - WRITE to TAG
        r5.l=2;
        lsetup(s_isubbanksa, f_isubbanksa) lc1=p4;
s_isubbanksa:
        r0=r4;
        r1=r5;
        lsetup(s_isetsa, f_isetsa) lc0=p3;
s_isetsa:r0 = r0+|+r2 || [i0]=r0;
f_isetsa:r1 = r1+|+r2 || [i0]=r1;
        r4=r4+r3;
f_isubbanksa:
        r5=r5+r3;
        //
        // and now both data banks
        // DSRAM has 2x16kb bank
        //       each of 4x4k sub-banks
        //       each of 2 ways
        //       each of 64 lines
        //       each of 32 bytes 
	p0.l = (DMEM_CONTROL & 0xFFFF);
	p0.h = (DMEM_CONTROL >> 16);
	r0 = (ACACHE_BCACHE | ENDM);
        ssync;
	[p0]=r0;
        ssync;
        i0.l = (DTEST_COMMAND & 0xFFFF);
	i0.h = (DTEST_COMMAND >> 16);
        i1.l = (DTEST_DATA0 & 0xFFFF);
	i1.h = (DTEST_DATA0 >> 16);
        // Write 0 to DATA0, making the line addressed in COMMAND
        // invalid.
        r0 = 0;
        [i1] = r0;
        p4 = 4;         // Number of sub-banks, outer loop iterations
        r3.l = 0;
        r3.h = 1;       // Sub-bank increment
        r2 = 64;
        p3 = r2;        // Number of line indices in each way,
                        // inner loop iterations
        r2 = 32;        // Line index increment
        // ********************* DSRAM A invalidation *********************
        r4.h = 0;       // Initial value for ITEST_COMMAND,
        r4.l = 2;       // way 0 - WRITE to TAG
        r5.h = 0x400;   // Initial value for ITEST_COMMAND,
        r5.l = 2;       // way 1 - WRITE to TAG
        lsetup(s_dasubbanks, f_dasubbanks) lc1=p4;
s_dasubbanks:
        r0 = r4;
        r1 = r5;
        lsetup(s_dasets, f_dasets) lc0=p3;
s_dasets: r0 = r0+|+r2 || [i0]=r0; // Do this line and set up for next
f_dasets: r1 = r1+|+r2 || [i0]=r1; // Ditto
        r4=r4+r3;       // Way 0 for next sub-bank
f_dasubbanks:
        r5=r5+r3;       // Way 1 for next sub-bank
        // ********************* DSRAM B invalidation *********************
        r4.h = 0x080;   // Initial value for ITEST_COMMAND,
        r4.l = 2;       // way 0 - WRITE to TAG
        r5.h = 0x480;   // Initial value for ITEST_COMMAND,
        r5.l = 2;       // way 1 - WRITE to TAG
        lsetup(s_dbsubbanks, f_dbsubbanks) lc1=p4;
s_dbsubbanks:
        r0 = r4;
        r1 = r5;
        lsetup(s_dbsets, f_dbsets) lc0=p3;
s_dbsets: r0 = r0+|+r2 || [i0]=r0; // Do this line and set up for next
f_dbsets: r1 = r1+|+r2 || [i0]=r1; // Ditto
        r4=r4+r3;       // Way 0 for next sub-bank
f_dbsubbanks:
        r5=r5+r3;       // Way 1 for next sub-bank
        //
        // ********************* Set up ICPLBS *********************
#define NUM_CPLBS 9
	// Create DCPLB_ADDR and ICPLB_ADDR entries
	i0.l = (DCPLB_ADDR0 & 0xFFFF);
	i0.h = (DCPLB_ADDR0 >> 16);
	i1.l = (ICPLB_ADDR0 & 0xFFFF);
	i1.h = (ICPLB_ADDR0 >> 16);
	i2.l = cplb_addrs;
	i2.h = cplb_addrs;
	p3=NUM_CPLBS;
	lsetup(s_cplb_addrs, e_cplb_addrs) lc0=p3;
s_cplb_addrs:
        r0 = [i2++];
	[i0++] = r0;
e_cplb_addrs:
        [i1++] = r0;

	// Configure DCPLB_DATA entries
	i0.l = (DCPLB_DATA0 & 0xFFFF);
	i0.h = (DCPLB_DATA0 >> 16);
	i3.l = (ICPLB_DATA0 & 0xFFFF);
	i3.h = (ICPLB_DATA0 >> 16);
	i2.l = cplb_set;
	i2.h = cplb_set;
	lsetup(s_cplb_bits, e_cplb_bits) lc0 = p3;
s_cplb_bits:
	r0 = [i2++];
	[i0++] = r0;
e_cplb_bits:
        [i3++] = r0;

	// Configure L1 data banks as cache or SRAM
	p0.l = (DMEM_CONTROL & 0xFFFF);
	p0.h = (DMEM_CONTROL >> 16);
#if defined(ALL_CACHE) || defined(DATA_ABCACHE)
        r0 = (ACACHE_BCACHE | ENDCPLB |ENDM);
#elif defined(DATA_ACACHE)
        r0 = (ACACHE_BSRAM | ENDCPLB |ENDM);
#else
        r0 = 1; // SRAM
#endif
        ssync;
	[p0] = r0;
        ssync;
	// Configure L1 code bank as cache or SRAM
	p0.l = (IMEM_CONTROL & 0xFFFF);
	p0.h = (IMEM_CONTROL >> 16);
#if defined(ALL_CACHE) || defined(CODE_CACHE)
        r0 = (IMC | ENICPLB | ENIM);
#else
        r0 = 1; // SRAM
#endif
        ssync;
	[p0] = r0;


	(R7:3, P5:3) = [SP++];	// restore preserved registers
	RTS;

	.section data1;
	.align 4;
	// Addresses of the memory areas we configure.
.align 4;
	cplb_addrs:
	.byte4 =
	0xFFC00000,     // MMRs FOR EMULATOR
        0xEF000000,     // reset vector
	0xFFB00000,	// Scratchpad
	0xFFA00000,	// Instruction memory
	0xFF900000,	// Data B
	0xFF800000,	// Data A
	0xF0000000,	// L2 SRAM - 256KB
        0x00000000,     // SDRAM
	0x20000000;     // FLASH
		
cplb_set:
#define L2_FLAGS    CPLB_WT|CPLB_L1_CHBL|CPLB_L1SRAM|CPLB_SUPV_WR|CPLB_USER_WR|CPLB_USER_RD|CPLB_VALID
#define SDRAM_FLAGS CPLB_WT|CPLB_L1_CHBL|CPLB_L1SRAM|CPLB_SUPV_WR|CPLB_USER_WR|CPLB_USER_RD|CPLB_VALID
.byte4 =
	(PAGE_SIZE_4MB|CPLB_DIRTY|CPLB_SUPV_WR|CPLB_VALID),  // MMRs FOR EMULATOR
	(PAGE_SIZE_4MB|CPLB_DIRTY|CPLB_SUPV_WR|CPLB_VALID),  // reset vector
	(PAGE_SIZE_1MB|CPLB_DIRTY|CPLB_SUPV_WR|CPLB_USER_WR|CPLB_USER_RD|CPLB_VALID), // Scr
        (PAGE_SIZE_1MB|CPLB_WT|CPLB_VALID), // Code
	(PAGE_SIZE_1MB|CPLB_WT|CPLB_VALID), // D B
	(PAGE_SIZE_1MB|CPLB_WT|CPLB_VALID), // D A
	(PAGE_SIZE_1MB|L2_FLAGS),           // L2
	(PAGE_SIZE_4MB|SDRAM_FLAGS),        // SDRAM
	(PAGE_SIZE_4MB|SDRAM_FLAGS);        // FLASH
