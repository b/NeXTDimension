/*****************************************************************************

    mp12hlrect.c

    CONFIDENTIAL
    Copyright (c) 1989 NeXT, Inc. as an unpublished work.
    All Rights Reserved.
    
    Created: 11Aug89 from highlightrectnext.s Terry

    Modification History:
    
    25Aug89 Ted  ANSI prototyping.

    Code to highlight a particular rectangle.
    On the two bit/pixel NeXT screen this is equivalent to
    swapping white pixels with light gray ones.  This is done
    by taking the not of the high-order bits of each pixel
    and exclusive-oring it with the low-order bit of that pixel.

******************************************************************************/

#import "package_specs.h"
#import "device.h"
#import "devpattern.h"
#import "bintree.h"
#import "mp12.h"

#define LOWMASK (0x55555555) /* Low order bit mask */

void HighlightRect(register LineOperation *lop, register uint srcRowBytes,
	register uint destRowBytes, register uint height)
{
    register uint *dstPtr;
    register uint lowmask, lmask, rmask;
    register int n, numInts;

    dstPtr = lop->loc.dstPtr;
    lowmask = LOWMASK;
    lmask   = lop->loc.leftMask & lowmask;
    rmask   = lop->loc.rightMask & lowmask;
    numInts = lop->loc.numInts;

    if (numInts < 1)
    {
	do {
	    *dstPtr ^= ((~*dstPtr)>>1) & lmask;
	    dstPtr = (uint *)(((uchar *)dstPtr) + destRowBytes);
	} while(--height);
    }
    else if (numInts == 1)
    {
	destRowBytes -= 4;
	do {
	    *dstPtr ^= ((~*dstPtr)>>1) & lmask; dstPtr++;
	    *dstPtr ^= ((~*dstPtr)>>1) & rmask;
	    dstPtr = (uint *)(((uchar *)dstPtr) + destRowBytes);
	} while(--height);
    }
    else
    {
	destRowBytes -= numInts << 2;
	numInts--;
	do {
	    *dstPtr ^= ((~*dstPtr)>>1) & lmask; dstPtr++;
	    n = numInts;
	    do {
		*dstPtr ^= ((~*dstPtr)>>1) & lowmask; dstPtr++;
	    } while(--n);
	    *dstPtr ^= ((~*dstPtr)>>1) & rmask;
	    dstPtr = (uint *)(((uchar *)dstPtr) + destRowBytes);
	} while(--height);
    }
}



