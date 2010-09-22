
/*
 *  Copyright (c) 2002-2010    National Research Council
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

#include <vector>

#include <scopira/tool/flow.h>
#include <scopira/tool/util.h>
#include <scopira/tool/platform.h>

namespace scopira
{
  namespace tool
  {
    // base for traits
    template <class T> class flowtraits_base_g;
    // serialization via traits (default)
    template <class T> class flowtraits_g;
    
    // specializtions
    template <> class flowtraits_g<bool>;
    template <> class flowtraits_g<unsigned char>;
    template <> class flowtraits_g<char>;
    template <> class flowtraits_g<short>;
    template <> class flowtraits_g<int>;
#ifdef PLATFORM_32
    template <> class flowtraits_g<long>; // no longer needed?
#endif
    template <> class flowtraits_g<int64_t>;
    template <> class flowtraits_g<float>;
    template <> class flowtraits_g<double>;

    template <> class flowtraits_g<std::string>;

    template <class EE> class flowtraits_g<std::vector<EE> >;
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

  public:
    template <class COL>
      static bool load_collection(itflow_i &in, COL &v) {
        typename COL::iterator ii, endii;
        int64_t sz;

        if (!in.read_int64_t(sz))
          return false;
        // or should this be changed to a clear/push_back like system to support lists, etc?
        v.resize(static_cast<size_t>(sz));
        endii = v.end();
        for (ii=v.begin(); ii != endii; ++ii)
          if (!in.read_generic(*ii))
            return false;

        return true;
      }

    template <class COL>
      static void save_collection(otflow_i &out, const COL &v) {
        typename COL::const_iterator ii, endii;
        int64_t sz;

        sz = v.size();

        out.write_int64_t(sz);

        endii = v.end();
        for (ii=v.begin(); ii !=endii; ++ii)
          out.write_generic(*ii);
      }
};

/**
 * Generic version, assumes object has its own save/load methods.
 *
 * @author Aleksander Demko
 */ 
template <class T> class scopira::tool::flowtraits_g : public scopira::tool::flowtraits_base_g<T>
{
  public:
    static bool load(itflow_i &in, T & outv) { return outv.load(in); }
    static void save(otflow_i &out, const T &v) { v.save(out); }
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
/// done need it anymore becuase int and int64 should be enough?
#ifdef PLATFORM_32
template <>
class scopira::tool::flowtraits_g<long> : public scopira::tool::flowtraits_base_g<long>
{
  public:
    static bool load(itflow_i &in, long & outv) { return in.read_long(outv); }
    static void save(otflow_i &out, const long &v) { out.write_long(v); }
    static void to_string(const long &v, std::string &out) { tool::long_to_string(v,out); }
    static bool from_string(std::string &s, long &out) { return tool::string_to_long(s,out); }
};
#endif

/// specialization: long
template <>
class scopira::tool::flowtraits_g<int64_t> : public scopira::tool::flowtraits_base_g<int64_t>
{
  public:
    static bool load(itflow_i &in, int64_t & outv) { return in.read_int64_t(outv); }
    static void save(otflow_i &out, const int64_t &v) { out.write_int64_t(v); }
    static void to_string(const int64_t &v, std::string &out) { tool::int64_t_to_string(v,out); }
    static bool from_string(std::string &s, int64_t &out) { return tool::string_to_int64_t(s,out); }
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


// basic object specializations
template <>
class scopira::tool::flowtraits_g<std::string> : public scopira::tool::flowtraits_base_g<std::string>
{
  public:
    static bool load(itflow_i &in, std::string & outv) { return in.read_string(outv); }
    static void save(otflow_i &out, const std::string &v) { out.write_string(v); }
    static void to_string(const std::string &v, std::string &out) { out = v; }
    static bool from_string(std::string &s, std::string &out) { out = s; return true; }
};


// some specializations of STL containers
template <class EE>
class scopira::tool::flowtraits_g<std::vector<EE> > : public scopira::tool::flowtraits_base_g<std::vector<EE> >
{
  public:
    static bool load(itflow_i &in, std::vector<EE> & outv) { return load_collection(in, outv); }
    static void save(otflow_i &out, const std::vector<EE> &v) { save_collection(out, v); }
    static void to_string(const std::vector<EE> &v, std::string &out) { out = "std::vector<EE>"; }
    static bool from_string(std::string &s, std::vector<EE> &out) { return false; }
};

//
// implementation of some stuff in flow.h to avoid circular refs
//

template <class TT>
inline bool scopira::tool::itflow_i::read_generic(TT &v)
{
  return scopira::tool::flowtraits_g<TT>::load(*this, v);
}

template <class TT>
inline void scopira::tool::otflow_i::write_generic(const TT &v)
{
  scopira::tool::flowtraits_g<TT>::save(*this, v);
}

#endif

