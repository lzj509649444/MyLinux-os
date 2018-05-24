/* const.h */
#ifndef _CONST_H
#define _CONST_H

#define     PUBLIC                      
#define     PRIVATE static  /* PRIVATE x limits the scope of x */
#define     EXTERN	extern	/* EXTERN is defined as extern except in global.c */

/* constant for GDT descriptor */
#define BASE_HIGH_SHIFT     24  /* shift for base --> base_high */
#define BASE_MIDDLE_SHIFT   16  /* shift for middle -->base_middle */
#define OFFSET_HIGH_SHIFT   16  /* shift for (gate) offset --> offset_high */
#define GRANULARITY_SHIFT   16  /* shift for limit --> granularity */


#endif

