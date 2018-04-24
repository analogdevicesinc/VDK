/****************************************************************************
 *
 * seg_init is needed by the C run time system to assist in the
 * runtime initialization of
 *	stack start address
 *	stack length
 *	malloc heaps
 *
 *	global variables (if mem21k was run)
 *
 * This data must reside in seg_init if mem21k is to be run on executables
 * generated with seg_init code.  Data for segments other than seg_init and
 * seg_rth is compressed into seg_init by mem21k; the __inits value in this
 * section points to this data and must be available to extract the data,
 * so obviously it cannot be *in* the data, which means it has to go in
 * seg_init or seg_rth.  In fact, __inits is in seg_init, below.  Mem21k
 * places a pointer to the initialization data in __inits as part of this
 * processing.
 *
 * There can be multiple heaps specified in this file.
 * Each heap specification consists of the following
 *	8 bytes - ASCII heap name
 *	2 bytes - unused
 *	2 bytes - heap location
 *		  0x0001  PM location
 *		  0xFFFF  DM location
 *	6 bytes   zero
 *	6 bytes   heap start address (in high order 32 bits)
 *	6 bytes   heap length (in high order 32 bits)
 *
 ****************************************************************************/

#if defined(__SHORT_WORD_CODE__)
.section/nw seg_init;
#else
.section/pm seg_init;
#endif

/*
 * The following initializations rely on several values being established
 * externally, typically by the linker description file.
 */

.extern ldf_stack_space;	/* The base of the stack */
.extern ldf_stack_length;	/* The length of the stack */
.extern ldf_heap_space;		/* The base of a primary DM heap "seg_heap" */
.extern ldf_heap_length;	/* The length of heap "seg_heap" */

.extern ldf_userheap_space;	/* The base of heap "seg_userheap" */
.extern ldf_userheap_length;	/* The length of heap "seg_userheap" */

/*
 * The linker description file will typically look something like:
 *
 *	MEMORY
 *	{
 *		heap_memory  { START(0x2c000) LENGTH(0x2000) TYPE(DM RAM) }
 *		stack_memory { START(0x2e000) LENGTH(0x2000) TYPE(DM RAM) }
 *	}
 *
 *	...
 *
 *	SECTIONS
 *	{
 *	    stack
 *	    {
 *		ldf_stack_space = .;
 *		ldf_stack_length = MEMORY_SIZEOF(stack_memory);
 *	    } > stack_memory
 *
 *	    heap
 *	    {
 *		ldf_heap_space = .;
 *		ldf_heap_length = MEMORY_SIZEOF(heap_memory);
 *	    } > heap_memory
 *	}
 */

.global ___lib_stack_space;
.var    ___lib_stack_space = ldf_stack_space;
.___lib_stack_space.end:

.global ___lib_stack_length;
.var    ___lib_stack_length = ldf_stack_length;
.___lib_stack_length.end:

.global ___inits;

.var 	___inits = 0;
.___inits.end:

/* The first two PM words represent the heap name and the heap location 
   The heap name must be exactly 8 characters long, and the heap location  
   should be either FFFFFFFF for DM or 00000001 for PM locations.  
   The next 3 words set the heap's initialization value, size and length. 
   The size and length are set with macros whose values are calculated 
   according the information in the project's .ldf file. */


.___lib_heap_descriptions:
.global ___lib_heap_space;
.var	___lib_heap_space[5] =
	0x7365675F6865, /* 'seg_he' */
	0x6170FFFFFFFF, /* 'ap'     */
	0,
	ldf_heap_space,
	ldf_heap_length;
.___lib_heap_space.end:

.global ___lib_userheap_space;
.var    ___lib_userheap_space[5] =
    0x757365726865, /* 'userhe' */
    0x6170FFFFFFFF, /* 'ap' */
    1,
    ldf_userheap_space,
    ldf_userheap_length;
.___lib_userheap_space.end:


/* Add more heap descriptions here */

.global ___lib_end_of_heap_descriptions;
.var ___lib_end_of_heap_descriptions = 0; /* Zero for end of list */
.___lib_end_of_heap_descriptions.end:

.___lib_heap_descriptions.end:


