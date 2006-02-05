
/*
 *  Copyright (c) 2002-2003    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INLUCDED__SCOPIRA_TOOL_UUID_H_
#define __INLUCDED__SCOPIRA_TOOL_UUID_H_

#include <scopira/tool/export.h>
#include <scopira/tool/platform.h>
#include <scopira/tool/export.h>
#include <scopira/tool/mutex.h>
#include <scopira/tool/flow.h>

#if defined(PLATFORM_linux) || defined(PLATFORM_osx)
#define PLATFORM_E2UUID
#endif

#ifdef PLATFORM_E2UUID
#include <uuid/uuid.h>
#endif

// THIS FILE HAS BEEN FULLY DOCUMENTED

namespace scopira
{
  namespace tool
  {
    // rename this to "key" or "ukey" or "token"?
    class uuid;
    class uuid_generator;
  }
}

/// Output operator for uuids
SCOPIRA_EXPORT scopira::tool::oflow_i& operator << (scopira::tool::oflow_i& o, const scopira::tool::uuid &id);


#ifdef PLATFORM_E2UUID
/**
 * A universally unique identifier.
 * You can print this out (for debugging purposes), do
 * comparison tests and serialize it to stream. That's it.
 * This class is heavily inline and includes two seperate
 * implementations.
 *
 * Linux specific implementation.
 *
 * @author Aleksander Demko
 */ 
class scopira::tool::uuid                       // super-Linux implementation
{
  private:
    ::uuid_t dm_id;
  public:
    /// Constructor. Builds a "zero" UUID (one with all zeros)
    uuid(void) { ::uuid_clear(dm_id); }

    /**
     * Sets this uuid to be a zero UUID, the "null" of UUIDs.
     * @author Aleksander Demko
     */ 
    void set_zero(void) { ::uuid_clear(dm_id); }
    /**
     * Is this UUID a "zero" uuid?
     * @author Aleksander Demko
     */ 
    bool is_zero(void) const { return ::uuid_is_null(dm_id) != 0; }

    /// comparator
    bool operator == (const uuid &rhs) const { return ::uuid_compare(dm_id, rhs.dm_id) == 0; }
    /// comparator
    bool operator != (const uuid &rhs) const { return ::uuid_compare(dm_id, rhs.dm_id) != 0; }
    /// comparator
    bool operator < (const uuid &rhs) const { return ::uuid_compare(dm_id, rhs.dm_id) < 0; }

    /// Serialization - yes, non-virtual
    bool load(scopira::tool::itflow_i& in) { return in.read(dm_id, sizeof(dm_id)) == sizeof(dm_id); }
    /// Serialization - yes, non-virtual
    void save(scopira::tool::otflow_i& out) const { out.write(dm_id, sizeof(dm_id)); }

    /// return a nice string representation
    std::string as_string(void) const;
    /// parse the string into this object, return false if failed
    bool parse_string(const std::string &s);

  public:
    friend class scopira::tool::uuid_generator;
    friend scopira::tool::oflow_i& ::operator << (scopira::tool::oflow_i& o, const scopira::tool::uuid &id);
};
#endif

#ifdef PLATFORM_win32
/**
 * A universally unique identifier.
 * You can print this out (for debugging purposes), do
 * comparison tests and serialize it to stream. That's it.
 * This class is heavily inline and includes two seperate
 * implementations.
 *
 * Win32 specific implementation.
 *
 * @author Aleksander Demko
 */ 
class scopira::tool::uuid                       // super-Win32 implementation
{
  private:
    GUID dm_id;
  public:
    /// Constructor. Builds a "zero" UUID (one with all zeros)
    uuid(void) { dm_id = GUID_NULL; }

    /**
     * Sets this uuid to be a zero UUID, the "null" of UUIDs.
     * @author Aleksander Demko
     */ 
    void set_zero(void) { dm_id = GUID_NULL; }
    /**
     * Is this UUID a "zero" uuid?
     * @author Aleksander Demko
     */ 
    bool is_zero(void) const { return *this == uuid(); }

    /// comparator
    bool operator == (const uuid &rhs) const { return IsEqualGUID(dm_id, rhs.dm_id) == TRUE; }
    /// comparator
    bool operator != (const uuid &rhs) const { return IsEqualGUID(dm_id, rhs.dm_id) == FALSE; }
    /// comparator
    SCOPIRA_EXPORT bool operator < (const uuid &rhs) const;

    /// Serialization - yes, non-virtual
    bool load(scopira::tool::itflow_i& in) { return in.read_void(&dm_id, sizeof(dm_id)) == sizeof(dm_id); }
    /// Serialization - yes, non-virtual
    void save(scopira::tool::otflow_i& out) const { out.write_void(&dm_id, sizeof(dm_id)); }

    /// return a nice string representation
    SCOPIRA_EXPORT std::string as_string(void) const;
    /// parse the string into this object, return false if failed
    SCOPIRA_EXPORT bool parse_string(const std::string &s);

  public:
    friend class scopira::tool::uuid_generator;
    friend scopira::tool::oflow_i& ::operator << (scopira::tool::oflow_i& o, const scopira::tool::uuid &id);
};
#endif

#if !defined(PLATFORM_E2UUID) && !defined(PLATFORM_win32)
/**
 * A universally unique identifier.
 * You can print this out (for debugging purposes), do
 * comparison tests and serialize it to stream. That's it.
 * This class is heavily inline and includes two seperate
 * implementations.
 *
 * Generic implementation: broken.
 *
 * @author Aleksander Demko
 */ 
class scopira::tool::uuid                       // lame non-Linux implementation[1]
{
  private:
    int dm_id;    // [1] This implementation will NEED to be synchronized with the Linux
                  // one if there is to be any network communication/collabaration
                  // Under Win32, we'll use MS's GUID stuff, which is basicaly the same thing
                  // What to do for the (non-Linux) UNIXes though?

  public:
    /// Constructor. Builds a "zero" UUID (one with all zeros)
    uuid(void) : dm_id(0) { }

    /**
     * Sets this uuid to be a zero UUID, the "null" of UUIDs.
     * @author Aleksander Demko
     */ 
    void set_zero(void) { dm_id = 0; }
    /**
     * Is this UUID a "zero" uuid?
     * @author Aleksander Demko
     */ 
    bool is_zero(void) const { return dm_id == 0; }

    /// comparator
    bool operator == (const uuid &rhs) const { return dm_id == rhs.dm_id; }
    /// comparator
    bool operator != (const uuid &rhs) const { return dm_id != rhs.dm_id; }
    /// comparator
    bool operator < (const uuid &rhs) const { return dm_id < rhs.dm_id; }

    /// Serialization - yes, non-virtual
    bool load(scopira::tool::itflow_i& in) { return in.read_int(dm_id); }
    /// Serialization - yes, non-virtual
    void save(scopira::tool::otflow_i& out) const { out.write_int(dm_id); }

    /// return a nice string representation
    //SCOPIRA_EXPORT std::string as_string(void) const;   // for "int"s? i dunno...
    /// parse the string into this object, return false if failed
    //SCOPIRA_EXPORT bool parse_string(const std::string &s);

  public:
    friend class scopira::tool::uuid_generator;
    friend scopira::tool::oflow_i& ::operator << (scopira::tool::oflow_i& o, const scopira::tool::uuid &id);
};
#endif

/**
 * A generator of UUIDs.
 * UUIDs are universally unique... well, as unique as one can hope for.
 * @author Aleksander Demko
 */
class scopira::tool::uuid_generator
{
  private:
#if !defined(PLATFORM_E2UUID) && !defined(PLATFORM_win32)
    volatile int dm_next;
    mutex dm_mut;
#endif
  public:
    /// Constructor
#if !defined(PLATFORM_E2UUID) && !defined(PLATFORM_win32)
    SCOPIRA_EXPORT uuid_generator(void);
#endif
    /**
     * Generate a new UUID. This method is reentrant (thread safe)
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void next(uuid &out);

    // array/multi gen version of next()??
    // or just do some kind of +1 system?
};

#endif

