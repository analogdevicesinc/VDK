#include "defBF537.h"

#define delay 0xf00

.global _InitFlags;

.section/doubleany L1_code;

_InitFlags:
   
	// configure programmable flags
	// set port f function enable register (need workaround)
	p0.l = lo(PORTF_FER);
    p0.h = hi(PORTF_FER);
    r0.l = 0x0000;
    r1 = w[p0] (z);
    ssync;
    w[p0] = r0;
    ssync;
    w[p0] = r0;
    ssync;

	// set port f direction register
    p0.l = lo(PORTFIO_DIR);
    p0.h = hi(PORTFIO_DIR);
    r0.l = 0x1FC0;
    w[p0] = r0;
    ssync; 
        
   	// set port f input enable register
    p0.l = lo(PORTFIO_INEN);
    p0.h = hi(PORTFIO_INEN);
    r0.l = 0x3C;
    w[p0] = r0;
    ssync; 
         
	// set port f clear register
    p0.l = lo(PORTFIO_CLEAR);
    p0.h = hi(PORTFIO_CLEAR);
    r0.l = 0xFC0;
    w[p0] = r0;
    ssync;	
 
    // give some time for reset to take affect
    p2.l = lo(delay);
	p2.h = hi(delay);
	lsetup(delay_loop, delay_loop) lc0=p2;
	delay_loop: nop;	
	
    // set port f set register
    p0.l = lo(PORTFIO_SET);
    p0.h = hi(PORTFIO_SET);
    r0.l = PF12;
    w[p0] = r0;
    ssync;
    
	RTS;
	
._InitFlags.end:
