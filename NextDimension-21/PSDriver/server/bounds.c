/*****************************************************************************

    bounds.c
    Collection of useful functions with rectangles

    CONFIDENTIAL
    Copyright (c) 1990 NeXT Computer, Inc. as an unpublished work.
    All Rights Reserved.

    Modified:

******************************************************************************/

#import "package_specs.h"
#import "bintree.h"


/*****************************************************************************
    boundsBounds
    Sets the third argument to be the bounding rectangle of the first two.
    It also returns whether the result is equal to the first argument.
    The result can point to one of the parameters.
******************************************************************************/
int boundBounds(Bounds *one, Bounds *two, Bounds *result)
{
    int sameAsFirst = 1;
    
    result->minx = (two->minx < one->minx ? (sameAsFirst = 0, two->minx) :
	one->minx);
    result->miny = (two->miny < one->miny ? (sameAsFirst = 0, two->miny) :
	one->miny);
    result->maxx = (two->maxx > one->maxx ? (sameAsFirst = 0, two->maxx) :
	one->maxx);
    result->maxy = (two->maxy > one->maxy ? (sameAsFirst = 0, two->maxy) :
	one->maxy);
    return sameAsFirst;
}


/*****************************************************************************
    BoundsFromIPrim
******************************************************************************/
void BoundsFromIPrim(DevPrim *ip, Bounds *b)
{
    b->minx = ip->bounds.x.l;
    b->miny = ip->bounds.y.l;
    b->maxx = ip->bounds.x.g;
    b->maxy = ip->bounds.y.g;
    for (ip = ip->next; ip; ip = ip->next) {
	if (ip->bounds.x.l < b->minx)
	    b->minx = ip->bounds.x.l;
	if (ip->bounds.y.l < b->miny)
	    b->miny = ip->bounds.y.l;
	if (ip->bounds.x.g > b->maxx)
	    b->maxx = ip->bounds.x.g;
	if (ip->bounds.y.g > b->maxy)
	    b->maxy = ip->bounds.y.g;
    }
}


/*****************************************************************************
    clipBounds
    Clips the first rectangle to be wholly inside the second.  It also
    adjusts the third by the same amount the second is clipped by,
    if three is non-null.
******************************************************************************/
void clipBounds(Bounds *one, Bounds *two, Bounds *three)
{
    short diff;
    
    diff = two->minx - one->minx;
    if (diff > 0) {
	one->minx += diff;
	if (three)
	    three->minx += diff;
    }
    diff = two->miny - one->miny;
    if (diff > 0) {
	one->miny += diff;
	if (three)
	    three->miny += diff;
    }
    diff = one->maxx - two->maxx;
    if (diff > 0) {
	one->maxx -= diff;
	if (three)
	    three->maxx -= diff;
    }
    diff = one->maxy - two->maxy;
    if (diff > 0) {
	one->maxy -= diff;
	if (three)
	    three->maxy -= diff;
    }
}


/*****************************************************************************
    collapseBounds
    Sets bounds "one" to be the inside-out of bounds "two".
******************************************************************************/
void collapseBounds(Bounds *one, Bounds *two)
{
    one->minx = two->maxx;
    one->maxx = two->minx;
    one->miny = two->maxy;
    one->maxy = two->miny;
}


/*****************************************************************************
    DevBoundsCompare
    Evaluates and returns classification of rectangle arrangement.
    (See IntersectAndCompareBounds for description of return values.)
******************************************************************************/
BBoxCompareResult DevBoundsCompare(DevBounds *figBds, DevBounds *clipBds)
{
    if (figBds->y.l >= clipBds->y.l && figBds->y.g <= clipBds->y.g &&
	figBds->x.l >= clipBds->x.l && figBds->x.g <= clipBds->x.g)
	return inside;
    if (figBds->y.g <= clipBds->y.l || figBds->y.l >= clipBds->y.g ||
       figBds->x.g <= clipBds->x.l || figBds->x.l >= clipBds->x.g)
       return outside;
    return overlap;
}


/*****************************************************************************
    FindDiffBounds
    Will call the procedure that is its third argument with any or all of
    the rectangles that are outside its first argument but inside its
    second.  When called, a pointer to the rectangle will be the proc's
    first argument, and the given arg will be its second.
******************************************************************************/
void FindDiffBounds(Bounds *minus, Bounds *sum, void (*proc)(), void *arg)
{
    Bounds diffBounds;
    
    if (NULLBOUNDS((*minus)) || !sectBounds(sum,minus,&diffBounds))
	(*proc) (sum, arg);
    else {
	diffBounds.miny = minus->miny > sum->miny ? minus->miny : sum->miny;
	diffBounds.maxy = minus->maxy < sum->maxy ? minus->maxy : sum->maxy;
	if (sum->minx < minus->minx) {
	    diffBounds.minx = sum->minx;
	    diffBounds.maxx = minus->minx;
	    (*proc) (&diffBounds, arg);
	}
	if (sum->maxx > minus->maxx) {
	    diffBounds.minx = minus->maxx;
	    diffBounds.maxx = sum->maxx;
	    (*proc) (&diffBounds, arg);
	}
	diffBounds.minx = sum->minx;
	diffBounds.maxx = sum->maxx;
	if (sum->miny < minus->miny) {
	    diffBounds.miny = sum->miny;
	    diffBounds.maxy = minus->miny;
	    (*proc) (&diffBounds, arg);
	}
	if (sum->maxy > minus->maxy) {
	    diffBounds.miny = minus->maxy;
	    diffBounds.maxy = sum->maxy;
	    (*proc) (&diffBounds, arg);
	}
    }
}


/*****************************************************************************
    IntersectAndCompareBounds
    Return the intersection of "one" and "two" in "result" and return
    classification "overlap", "inside" or "outside" depending on the
    arrangement of both rectangles.
    
    overlap:	neither rectangle is completely within the other.
    inside:	rectangles are congruent.
    outside:	rectangles do not intersect (or they are zerobounds).
******************************************************************************/
BBoxCompareResult IntersectAndCompareBounds(Bounds *one,
    Bounds *two, Bounds *result)
{
    BBoxCompareResult compare = inside;

    if (two->minx > one->minx)
	{result->minx = two->minx; compare = overlap;}
    else
	result->minx = one->minx;
    if (two->miny > one->miny)
	{result->miny = two->miny; compare = overlap;}
    else
	result->miny = one->miny;
    if (two->maxx < one->maxx)
	{result->maxx = two->maxx; compare = overlap;}
    else
	result->maxx = one->maxx;
    if (two->maxy < one->maxy)
	{result->maxy = two->maxy; compare = overlap;}
    else
	result->maxy = one->maxy;
    if ((result->minx >= result->maxx) || (result->miny >= result->maxy))
	return outside;
    return compare;
}


/*****************************************************************************
    offsetBounds
    Move the given bounds relatively by dx and dy.
******************************************************************************/
void offsetBounds(Bounds *b, short dx, short dy)
{
    b->minx += dx;
    b->maxx += dx;
    b->miny += dy;
    b->maxy += dy;
}


/*****************************************************************************
    sectBounds
    Return intersection of bounds "one" and "two" in "result".  Also
    return true if there is an intersection.
******************************************************************************/
int sectBounds(Bounds *one, Bounds *two, Bounds *result)
{
    result->minx = (two->minx > one->minx ? two->minx : one->minx);
    result->miny = (two->miny > one->miny ? two->miny : one->miny);
    result->maxx = (two->maxx < one->maxx ? two->maxx : one->maxx);
    result->maxy = (two->maxy < one->maxy ? two->maxy : one->maxy);
    return (result->minx < result->maxx) && (result->miny < result->maxy);
}


/*****************************************************************************
    withinBounds
    Returns true if the bounds "one" is entirely within bounds "two".
******************************************************************************/
int withinBounds(Bounds *one, Bounds *two)
{
    return ((one->minx >= two->minx) && (one->miny >= two->miny) &&
	(one->maxx <= two->maxx) && (one->maxy <= two->maxy));
}





