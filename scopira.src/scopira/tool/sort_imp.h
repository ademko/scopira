
/*
 *  Copyright (c) 2002    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED__SCOPIRA_TOOL_SORT_IMP__
#define __INCLUDED__SCOPIRA_TOOL_SORT_IMP__

namespace scopira
{
  namespace tool
  {
    template <class T> void qsort(T &s, int lo0, int hi0);
  }
}

/**
 * This is a generic version of C.A.R Hoare's Quick Sort
 * algorithm.  This will handle arrays that are already
 * sorted, and arrays with duplicate keys.<BR>
 *
 * If you think of a one dimensional array as going from
 * the lowest index on the left to the highest index on the right
 * then the parameters to this function are lowest index or
 * left and highest index or right.  The first time you call
 * this function it will be with the parameters 0, a.length - 1.
 *
 * The sortable object must have the following form:
 *   int compare_element(int lidx, int ridx) [const]
 *   void swap_element(int lidx, int ridx)
 *
 * @param s       the sortable object
 * @param lo0     left boundary of array partition, initially use 0
 * @param hi0     right boundary of array partition, initially use size-1
 * @author Aleksander Demko
 */
template <class T> void scopira::tool::qsort(T &s, int lo0, int hi0)
{
  int lo = lo0;
  int hi = hi0;
  int mid;

  if ( hi0 > lo0) {
    // Arbitrarily establishing partition element as the midpoint of
    // the array.
    mid = ( lo0 + hi0 ) / 2;

    // loop through the array until indices cross
    while( lo <= hi ) {
      // find the first element that is greater than or equal to
      // the partition element starting from the left Index.
      while( ( lo < hi0 ) && ( s.compare_element(lo, mid) < 0 ) )
        lo++;
      // find an element that is smaller than or equal to
      // the partition element starting from the right Index.
      while( ( hi > lo0 ) && ( s.compare_element(hi, mid) > 0 ) )
        hi--;

      // if the indexes have not crossed, swap
      if( lo <= hi ) {
        // because we're dealin with indecies (and not the data directly)
        // we need to see if we swapped out midpoint away
        if (lo == mid)
          mid = hi;
        else if (hi == mid)
          mid = lo;
        s.swap_element(lo, hi);
        lo++;
        hi--;
      }
    }

    qsort(s, lo0, hi );
    qsort(s, lo, hi0 );
  }
}//quick_sort

#endif

