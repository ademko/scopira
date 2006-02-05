
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

#ifndef __INCLUDED__SCOPIRA_TOOL_TRAITS_H__
#define __INCLUDED__SCOPIRA_TOOL_TRAITS_H__

#include <scopira/tool/flow.h>
#include <scopira/tool/util.h>

namespace scopira
{
  namespace tool
  {
    // serialization via traits (default)
    template <class T> class flowtraits_g;
    // base for traits
    template <class T> class flowtraits_base_g;
    
    // specializtions
    template <> class flowtraits_g<bool>;
    template <> class flowtraits_g<unsigned char>;
    template <> class flowtraits_g<char>;
    template <> class flowtraits_g<short>;
    template <> class flowtraits_g<int>;
    template <> class flowtraits_g<long>;
    template <> class flowtraits_g<float>;
    template <> class flowtraits_g<double>;
  }
}

/**
 * serialization triats, for polymorphic object
 * saving (serialization)
 *
 * @author Aleksander Demko
 */ 
template <class T> class scopira::tool::flowtraits_base_g
{
  public:
    typedef T data_type;
};

/**
 * generic version
 *
 * @author Aleksander Demko
 */ 
template <class T> class scopira::tool::flowtraits_g : public scopira::tool::flowtraits_base_g<T>
{
  public:
    static bool load(itflow_i &in, T & outv) { return outv.read(in); }
    static void save(otflow_i &out, const T &v) { v.write(out); }
    static void to_string(const T &v, std::string &out) { out = "binary"; }
    static bool from_string(std::string &s, T &out) { return false; }
};

/// specialization: bool
template <>
class scopira::tool::flowtraits_g<bool> : public scopira::tool::flowtraits_base_g<bool>
{
  public:
    static bool load(itflow_i &in, bool & outv) { return in.read_bool(outv); }
    static void save(otflow_i &out, const bool &v) { out.write_bool(v); }
    static void to_string(const bool &v, std::string &out) { tool::int_to_string(v,out); }
    static bool from_string(std::string &s, bool &out) { int i; bool b = tool::string_to_int(s,i); out= i!=0; return b; }
};

/// specialization: unsigned char
template <>
class scopira::tool::flowtraits_g<unsigned char> : public scopira::tool::flowtraits_base_g<unsigned char>
{
  public:
    static bool load(itflow_i &in, unsigned char & outv) { return in.read_byte(outv)>0; }
    static void save(otflow_i &out, const unsigned char &v) { out.write_byte(v); }
    static void to_string(const unsigned char &v, std::string &out) { tool::int_to_string(v,out); }
    static bool from_string(std::string &s, unsigned char &out) { int i; bool b = tool::string_to_int(s,i); out=static_cast<unsigned char>(i); return b; }
};

/// specialization: char
template <>
class scopira::tool::flowtraits_g<char> : public scopira::tool::flowtraits_base_g<char>
{
  public:
    static bool load(itflow_i &in, char & outv) { return in.read_char(outv); }
    static void save(otflow_i &out, const char &v) { out.write_char(v); }
    static void to_string(const char &v, std::string &out) { tool::int_to_string(v,out); }
    static bool from_string(std::string &s, char &out) { int i; bool b = tool::string_to_int(s,i); out=static_cast<char>(i); return b; }
};

/// specialization: short
template <>
class scopira::tool::flowtraits_g<short> : public scopira::tool::flowtraits_base_g<short>
{
  public:
    static bool load(itflow_i &in, short & outv) { bool b;int ii; b = in.read_int(ii); outv = ii; return b; }
    static void save(otflow_i &out, const short &v) { out.write_int(v); }
    static void to_string(const short &v, std::string &out) { tool::int_to_string(v,out); }
    static bool from_string(std::string &s, short &out) { int i; bool b = tool::string_to_int(s,i); out=static_cast<short>(i); return b; }
};

/// specialization: int
template <>
class scopira::tool::flowtraits_g<int> : public scopira::tool::flowtraits_base_g<int>
{
  public:
    static bool load(itflow_i &in, int & outv) { return in.read_int(outv); }
    static void save(otflow_i &out, const int &v) { out.write_int(v); }
    static void to_string(const int &v, std::string &out) { tool::int_to_string(v,out); }
    static bool from_string(std::string &s, int &out) { return tool::string_to_int(s,out); }
};

/// specialization: long
template <>
class scopira::tool::flowtraits_g<long> : public scopira::tool::flowtraits_base_g<long>
{
  public:
    static bool load(itflow_i &in, long & outv) { return in.read_long(outv); }
    static void save(otflow_i &out, const long &v) { out.write_long(v); }
    static void to_string(const long &v, std::string &out) { tool::long_to_string(v,out); }
    static bool from_string(std::string &s, long &out) { return tool::string_to_long(s,out); }
};

/// specialization: float
template <>
class scopira::tool::flowtraits_g<float> : public scopira::tool::flowtraits_base_g<float>
{
  public:
    static bool load(itflow_i &in, float & outv) { return in.read_float(outv); }
    static void save(otflow_i &out, const float &v) { out.write_float(v); }
    static void to_string(const float &v, std::string &out) { tool::double_to_string(v,out); }
    static bool from_string(std::string &s, float &out) { double i; bool b = tool::string_to_double(s,i); out=static_cast<float>(i); return b; }
};

/// specialization: double
template <>
class scopira::tool::flowtraits_g<double> : public scopira::tool::flowtraits_base_g<double>
{
  public:
    static bool load(itflow_i &in, double & outv) { return in.read_double(outv); }
    static void save(otflow_i &out, const double &v) { out.write_double(v); }
    static void to_string(const double &v, std::string &out) { tool::double_to_string(v,out); }
    static bool from_string(std::string &s, double &out) { return tool::string_to_double(s,out); }
};


#endif

