
/*
 *  Copyright (c) 2002-2007    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INLUCDED__SCOPIRA_TOOL_UTIL_H_
#define __INLUCDED__SCOPIRA_TOOL_UTIL_H_

#include <limits.h>
#include <string>

#include <scopira/tool/array.h>
#include <scopira/tool/platform.h>
#include <scopira/tool/export.h>

// THIS FILE HAS BEEN FULLY DOCUMENTED

namespace scopira
{
  namespace tool
  {
    /**
     * Converts a pointer to an int. Useful for unpacking
     * ints where only a pointer could live.
     * See glib's GPOINTER_TO_INT.
     * @author Aleksander Demko
     */
    inline intptr_t ptr_to_int(const void *ptr) { return reinterpret_cast<intptr_t>(ptr); }
    /**
     * Converts a int to a pointer.
     * See glib's GINT_TO_POINTER.
     * @author Aleksander Demko
     */
    template <class C>
      inline C * int_to_ptr(intptr_t x) { return reinterpret_cast<C*>(x); }
    /**
      * See glib's GINT_TO_POINTER - fixed to void*.
      * @author Aleksander Demko
      */
    inline void * int_to_void(intptr_t x) { return reinterpret_cast<void*>(x); }

      /**
     * Converts a bool to a string
     * @param val the bool value
     * @return the string representation of the bool
     * @author Rodrigo Vivanco
     */
    inline std::string bool_to_string(bool val){ if( val ) return "true"; else return "false"; };
    /**
     * Converts a bool to a string. Write out to an existing string
     * @param val the bool value
     * @param out the string to save the convertion to
     * @author Rodrigo Vivanco
     */
    inline void int_to_string(bool val, std::string &out) { if( val ) out="true"; else out="false"; }

    /**
     * Converts an int to a string
     * @param i the integer
     * @return the string representation of the integer
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT std::string int_to_string(int i);
    /**
     * Converts an int to a string. Write out to an existing string
     * @param i the integer
     * @param out the string to save the convertion to
     * @author Aleksander Demko
     */
    inline void int_to_string(int i, std::string &out) { out = int_to_string(i); }
    /**
     * Converts a int to string.
     * @param i the int in question
     * @param out the output buffer, must be non-null
     * @param buflen the length of the buffer, including null
     * @return the actualy length written, including null
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT int int_to_string(int i, char *out, int buflen);
    /**
     * Converts a string to an int (without reporting failure)
     * @param s the string to convert
     * @return the integer representation of the string
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT int string_to_int(const std::string &s);
    /**
     * Converts a string to an int, and reports failure, if any
     * @param s the string to convert
     * @param outint the int read from the string
     * @return true on success
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT bool string_to_int(const std::string &s, int &outint);

    /**
     * Converts an long to a string
     * @param i the long
     * @return the string representation of the long
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT std::string long_to_string(long i);
    /**
     * Converts an long to a string. Write out to an existing string
     * @param i the long
     * @param out the string to save the convertion to
     * @author Aleksander Demko
     */
    inline void long_to_string(long i, std::string &out) { out = long_to_string(i); }
    /**
     * Converts a string to an long (without reporting failure)
     * @param s the string to convert
     * @return the long representation of the string
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT long string_to_long(const std::string &s);
    /**
     * Converts a string to an long, and reports failure, if any
     * @param s the string to convert
     * @param outlong the long read from the string
     * @return true on success
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT bool string_to_long(const std::string &s, long &outlong);

    /**
     * Converts an size_t to a string
     * @param i the size_t
     * @return the string representation of the size_t
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT std::string size_t_to_string(size_t i);
    /**
     * Converts an size_t to a string. Write out to an existing string
     * @param i the size_t
     * @param out the string to save the convertion to
     * @author Aleksander Demko
     */
    inline void size_t_to_string(size_t i, std::string &out) { out = size_t_to_string(i); }
    /**
     * Converts a string to an size_t (without reporting failure)
     * @param s the string to convert
     * @return the size_t representation of the string
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT size_t string_to_size_t(const std::string &s);
    /**
     * Converts a string to an size_t, and reports failure, if any
     * @param s the string to convert
     * @param outsize_t the size_t read from the string
     * @return true on success
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT bool string_to_size_t(const std::string &s, size_t &outsize_t);

    /**
     * Converts an int64_t to a string
     * @param i the int64_t
     * @return the string representation of the int64_t
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT std::string int64_t_to_string(int64_t i);
    /**
     * Converts an int64_t to a string. Write out to an existing string
     * @param i the int64_t
     * @param out the string to save the convertion to
     * @author Aleksander Demko
     */
    inline void int64_t_to_string(int64_t i, std::string &out) { out = int64_t_to_string(i); }
    /**
     * Converts a string to an int64_t (without reporting failure)
     * @param s the string to convert
     * @return the int64_t representation of the string
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT int64_t string_to_int64_t(const std::string &s);
    /**
     * Converts a string to an int64_t, and reports failure, if any
     * @param s the string to convert
     * @param outint64_t the int64_t read from the string
     * @return true on success
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT bool string_to_int64_t(const std::string &s, int64_t &outint64_t);

    /**
     * Converts a double to string
     * @param i the double
     * @return a string version of the double
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT std::string double_to_string(double i);
    /**
     * Converts a double to string
     * @param i the double
     * @param out the string to write out to
     * @author Aleksander Demko
     */
    inline void double_to_string(double i, std::string &out) { out = double_to_string(i); }
    /**
     * Converts a double to string, with precision
     * @param i the double
     * @param pres the number of post-decimal point digits
     * @return a string version of the double
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT std::string double_to_string(double i, int pres);
    /**
     * Converts a double to string.
     * @param i the double in question
     * @param out the output buffer, must be non-null
     * @param buflen the length of the buffer, including null
     * @return the actualy length written, including null
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT int double_to_string(double i, char *out, int buflen);
    /**
     * Converts a double to string - exponential edition
     * @param i the double
     * @param pres the number digits, 25 by default
     * @return a string version of the double
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT std::string double_to_exp_string(double i, int pres = 25);
    /**
     * Convert a string to a double (doesn't report failure)
     * @param s the string
     * @return the decoded double
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT double string_to_double(const std::string &s);
    /**
     * Convert a string to a double (reports failure)
     * @param s the string
     * @param outdouble the decoded double
     * @return true on success, false on failure
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT bool string_to_double(const std::string &s, double &outdouble);

    /**
     * A generic "compare" method.
     * returns -1 if lhs is < than rhs, -1 of rhs<lhs
     * else 0. (so you only need to implement <)
     *
     * @author Aleksander Demko
     */
    template <class T>
      char compare(const T &lhs, const T &rhs) {
        if (lhs<rhs)
          return -1;
        else if (rhs<lhs)
          return 1;
        else
          return 0;
      }

    /**
     * Converts a string to upper case. You may use the same
     * string for both parameters
     * @param s the input string
     * @param upper the uppercase'd string
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void uppercase(const std::string& s, std::string& upper);
    /**
     * Converts a string to uppercase
     * @param s the input string
     * @return the uppercase'd string
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT std::string uppercase(const std::string& s);
    /**
     * Converts a string to lower case. You may use the same
     * string for both parameters
     * @param s the input string
     * @param lower the lowercase'd string
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void lowercase(const std::string& s, std::string& lower);
    /**
     * Converts a string to lowercase
     * @param s the input string
     * @return the lowercase'd string
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT std::string lowercase(const std::string& s);


    /**
     * Justifies a string. If the given string is not width characters
     * long, it will (left) pad the string with padchar and return that.
     * @param s the string to inspect
     * @param width with minimum width of the final string
     * @param padchar the character to use for left padding
     * @return the possibly padded string
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT std::string pad_left(const std::string &s, int width, char padchar = ' ');

    // internal func
    SCOPIRA_EXPORT void impl_cstring_to_fixed_array_impl(const char *in, size_t srclen, char *out, size_t N) throw();

    /**
     * Copies std::string to a fixed_array<char, N>. The destination
     * will always be null terminated and well formed.
     * @author Aleksander Demko
     */
    template <size_t N>
      inline void string_to_fixed_array(const std::string &src, fixed_array<char, N> &out) throw()
        { impl_cstring_to_fixed_array_impl(src.c_str(), src.size(), out.c_array(), N); }
    /**
     * Copies null terminated C string to a fixed_array<char, N>. The destination
     * will always be null terminated and well formed.
     * @author Aleksander Demko
     */
    template <size_t N>
      inline void c_string_to_fixed_array(const char *in, fixed_array<char, N> &out) throw()
        { impl_cstring_to_fixed_array_impl(in, in?strlen(in):0, out.c_array(), N); }

    /**
     * Splits a string into two along a split character.
     *
     * @param src the string to inspect
     * @param split the character to use as the split marker
     * @param left_out on success, this will be the left side of the split
     * @param right_out on success, this will be the right side of the split
     * @return true if the split character and the string was split
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT bool split_char(const std::string &src, char split, std::string &left_out, std::string &right_out);

    /**
     * Meshes a number into a filename mast. For example, "blah###" and 20
     * produces "blah020".
     *
     * @param fname the input string, possibly containing the ## mast
     * @param num the number to mesh in
     * @return the result of the mesh
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT std::string filename_number(const std::string &fname, int num);

    /**
     * trims whitespace from the left side of the string
     *
     * @param str string to trim whitespace from
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void trim_left(std::string& str);

    /**
     * trims whitespace from the right side of the string
     *
     * @param str string to trim whitespace from
     * @author Aleksander Demko
     */
    void trim_right(std::string& str);

    /**
     * splits a string using the provided sub-string
     * the string is only split using the sub-string provided
     *
     * @author Aleksander Demko
     */
    template<class container>
      void string_tokenize_word(const std::string& src, container& result, const std::string& substr)
    {
      std::string::size_type start = 0;
      std::string::size_type pos;

      // clear vector
      result.clear();

      // loop through source string to split
      while ((pos = src.find(substr, start)) != std::string::npos) {
        // push back found sub-string
        result.push_back(src.substr(start, pos-start));
        // advance starting position
        start = pos + substr.length();
      }

      if (start != src.length())
        result.push_back(src.substr(start, src.length()-start)); // push back last sub-string
    }

    /**
     * Splits and tokenizes a string into zero or more parts.
     *
     * @param containing the container type (often implicit)
     * @param src the string to tokenize
     * @param result the container of strings that will be filled
     * with the output tokens. It will initially be cleared.
     * @param delimiters one or more delimiters to use. Default is space, tab and newline.
     * @author Aleksander Demko
     */
    template<class container>
      void string_tokenize(const std::string& src, container& result, const std::string& delimiters = " \t\n")
    {
      const std::string::size_type len = src.length();
      std::string::size_type i = 0;

      result.clear();

      while (i < len) {
        // remove leading whitespace
        i = src.find_first_not_of(delimiters, i);
        if (i == std::string::npos)
            return;   // nothing left but white space

        // find the end of the token
        std::string::size_type j = src.find_first_of(delimiters, i);

        // push token
        if (j == std::string::npos) {
            result.push_back(src.substr(i));
            return;
        } else
            result.push_back(src.substr(i, j-i));

        // set up for next loop
        i = j + 1;
      }
    }//string_tokenize

    /**
     * Generic byte swapper. Does nothing.
     * @author Aleksander Demko
     */
    template <class T>
      inline T byte_swap(T x) { return x; }

    /**
     * Swaps the byte (endianess) of a short. Currently only used
     * in binflow.
     * @author Aleksander Demko
     */
    template <>
      inline short byte_swap<short>(short x)
    {
      return
        ( (x & 0xFF) << 8 ) |
        ( (x & 0xFF00) >> 8 );
    }

    /**
     * Swaps the byte (endianess) of an int. Currently only used
     * in binflow.
     * @author Aleksander Demko
     */
    template <>
      inline int byte_swap<int>(int x)
    {
      return
        ( (x & 0xFF) << 24 ) |
        ( (x & 0xFF00) << 8 ) |
        ( (x & 0xFF0000) >> 8 ) |
        ( (x & 0xFF000000) >> 24 );
    }
    /**
     * Swaps the byte (endianess) of an int64_t.
     * @author Aleksander Demko
     */
    template <>
      inline int64_t byte_swap<int64_t>(int64_t x)
    {
      return
        ( (x & 0xFFll) << (8*7) ) |
        ( (x & 0xFF00ll) << (8*5) ) |
        ( (x & 0xFF0000ll) << (8*3) ) |
        ( (x & 0xFF000000ll) << 8 ) |
        ( (x & 0xFF00000000ll) >> 8 ) |
        ( (x & 0xFF0000000000ll) >> (8*3) ) |
        ( (x & 0xFF000000000000ll) >> (8*5) ) |
        ( (x & 0xFF00000000000000ll) >> (8*7) );
    }

    /**
     * Swaps the byte (endianess) of an uint64_t.
     * @author Aleksander Demko
     */
    template <>
      inline uint64_t byte_swap<uint64_t>(uint64_t x)
    {
      return
        ( (x & 0xFFull) << (8*7) ) |
        ( (x & 0xFF00ull) << (8*5) ) |
        ( (x & 0xFF0000ull) << (8*3) ) |
        ( (x & 0xFF000000ull) << 8 ) |
        ( (x & 0xFF00000000ull) >> 8 ) |
        ( (x & 0xFF0000000000ull) >> (8*3) ) |
        ( (x & 0xFF000000000000ull) >> (8*5) ) |
        ( (x & 0xFF00000000000000ull) >> (8*7) );
    }

    /**
     * Swaps the byte (endianess) of an float.
     *
     * @author Aleksander Demko
     */
    template <>
      inline float byte_swap<float>(float x)
    {
      float ret;
      char *I = reinterpret_cast<char*>(&x);
      char *O = reinterpret_cast<char*>(&ret);
      O[0] = I[3];
      O[1] = I[2];
      O[2] = I[1];
      O[3] = I[0];
      return ret;
    }

    /**
     * Swaps the byte (endianess) of an double.
     *
     * @author Aleksander Demko
     */
    template <>
      inline double byte_swap<double>(double x)
    {
      double ret;
      char *I = reinterpret_cast<char*>(&x);
      char *O = reinterpret_cast<char*>(&ret);
      O[0] = I[7];
      O[1] = I[6];
      O[2] = I[5];
      O[3] = I[4];
      O[4] = I[3];
      O[5] = I[2];
      O[6] = I[1];
      O[7] = I[0];
      return ret;
    }

    /**
     * Applies byte_swap to every element in the iteration
     * @author Aleksander Demko
     */
    template <class ITER>
      void byte_swap_all(ITER head, ITER tail);
  }//namespace scopira::tool
}

template <class ITER>
  void scopira::tool::byte_swap_all(ITER head, ITER tail)
{
  for (; head != tail; ++head)
    *head = byte_swap(*head);
}

#endif

