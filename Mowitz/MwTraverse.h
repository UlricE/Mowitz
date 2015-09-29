/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwTraverse_h_
#define _MwTraverse_h_

#define MW_TRAVERSE_FIRST          1
#define MW_TRAVERSE_LAST           2
#define MW_TRAVERSE_PREV           3
#define MW_TRAVERSE_NEXT           4
#define MW_TRAVERSE_ACTUAL         5
#define MW_TRAVERSE_NEXT_FIRST     6
#define MW_TRAVERSE_PREV_LAST      7

#if NeedFunctionPrototypes
extern void MwSetTraverseDirection(Widget , int);
#else
extern void MwSetTraverseDirection();
#endif

#endif
