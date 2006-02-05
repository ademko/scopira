
/*
 *  Copyright (c) 2004    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_BASEKIT_COMPLEX_H__
#define __INCLUDED_SCOPIRA_BASEKIT_COMPLEX_H__

#include <scopira/basekit/narray.h>

template <class T, class ELE>
scopira::tool::oflow_i & operator << (scopira::tool::oflow_i &o,
  const std::complex<ELE> &el)
{
  return o << '(' << el.real() << ',' << el.imag() << ')';
}

template <>
  inline void scopira::basekit::print_element<std::complex<float> >(scopira::tool::oflow_i &o, std::complex<float> el)
{
  flow_printf(o, " (%8.2f,%8.2f)", el.real(), el.imag());
}

template <>
  inline void scopira::basekit::print_element<std::complex<double> >(scopira::tool::oflow_i &o, std::complex<double> el)
{
  flow_printf(o, " (%8.2f,%8.2f)", el.real(), el.imag());
}

#endif

