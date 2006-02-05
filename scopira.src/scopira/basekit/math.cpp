
/*
 *  Copyright (c) 2001    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/basekit/math.h>

using namespace scopira;

int scopira::basekit::factorial(int n, int m)
{
#ifdef DEBUG 
  if (n < m)
    throw std::domain_error("more::math::factorial(int n, int m):\nUndefined for n < m.");
#endif

  // simplest case
  if(n == m) return 1;

  // bad index returns
  if (n<0)  return 0;
  if (m<0)  return 0;
  if (m>n)  return 0;
  
  int k = n;
  while(--n > m) k *= n;
  return k;
}


int scopira::basekit::binomial(int n, int i)
{
#ifdef DEBUG 
  if (n < 0)
    throw std::domain_error("more::math::binominal(int n, int):\nUndefined for n < 0.");
#endif

  if (i >= n)
    return i == n? 1 : 0;
  if (i <= 0)
    return i == 0? 1 : 0;
  int j = n - i;
  if (i > j) {
    j = i;
    i = n - j;
  }
  return factorial(n, j)/factorial(i);
} 

//BBlibs
//BBtargets libscopira.so

