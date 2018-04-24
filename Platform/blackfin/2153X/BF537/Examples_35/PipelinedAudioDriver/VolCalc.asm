
.GLOBAL _AttenuateBuffer16to32;

.section/doubleany data1;



/* declare locals */

.section/doubleany program;

//
// C-callable function to do fixed-point multiply
// of the contents of a 16-bit stereo buffer by
// a left-right pair of "volume" factors.
//
// The results are written to a 32-bit stereo
// buffer, right-justification for 24-bit audio
// is achieved by pre-dividing the volume factors
// by 256.
//
// Args:
// 1 (R0) - the 16-bit input buffer
// 2 (R1) - the 32-bit output buffer
// 3 (R2) - the number of sample pairs in each buffer
// 4 ([SP+12]) - the volume factors
//
_AttenuateBuffer16to32:
	LC0 = R2;
	P0 = R0;
	P1 = R1;
	R1 = [SP+12];
	
	R0 = [P0++];          // load first input pair
	LOOP volcalc LC0;
	LOOP_BEGIN volcalc;
		// vector multiply the left&right samples
		R2 = R0.l * R1.l, R3 = R0.h * R1.h;
		R0 = [P0++];      // load next input pair
		[P1++] = R2;      // write current outputs
		[P1++] = R3;	  //   "      "      "
	LOOP_END   volcalc;
	
	RTS;




._AttenuateBuffer16to32.end:

