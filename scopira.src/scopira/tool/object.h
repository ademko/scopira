
/*
 *  Copyright (c) 2001-2003    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED__SCOPIRA_TOOL_OBJECT_H__
#define __INCLUDED__SCOPIRA_TOOL_OBJECT_H__

#include <assert.h>

#include <scopira/tool/mutex.h>
#include <scopira/tool/export.h>

// THIS FILE HAS BEEN FULLY DOCUMENTED

/**
 * All Scopira subsytems fall under the top level scopira namespace.
 *
 * @author Aleksander Demko
 */
namespace scopira
{
  /**
   * The scopira::tool namespace contains scopira-independant utitlies, like autopointers,
   * virtual stream system (with file and network support), threads,
   * property files and a simple vector template.
   *
   * @author Aleksander Demko
   */
  namespace tool
  {
    class object;       // obj for short

    template <class T> class count_ptr;
    template <class T> class count2_ptr;
    //typedef count_ptr<object> objauto; //this is dangerous, dont use it (as it slices t_object's into simple object)

    // forward decl
    class oflow_i;
    class oobjflow_i;
    class iobjflow_i;
  }
}

/**
 * Generic base class for many object types. The class is the common base
 * class for many scopira classes. You may also use it for your own classes.
 *
 * It provides (all optional services):
 *  - a reference counting mechanism (which you can then use manually, or use scopira::tool::count_ptr
 *  - a serialization interface
 *  - a print() function that will be called when the user wants to do human readable printing
 *  - validity testing with is_alive_object()
 *
 * The Programmers Guide details information on how
 * reference counting and serialization works.
 *
 *
 * @see scopira::tool::count_ptr
 * @see @ref scopiratoolobject
 * @author Aleksander Demko
 */
class scopira::tool::object
{
  private:

  mutable volatile int dm_refcount;      // current ref count, mutal and transient
  mutable mutex dm_refcount_mutex;
#ifndef NDEBUG
  volatile int dm_object_magic;          // magic key for object class
  volatile bool dm_use_debug_ref_counter;   // when created, was the ref counter debug thing available?
#endif

  public:

    /**
     * Destructor
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual ~object();

    //
    // reference stuff
    //

    /**
     * Adds a reference.
     *
     * Returns the current reference count (always >=1, obviously)
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT int add_ref(void) const;

    /**
     * Removes a reference and deletes
     * the object if need be.
     *
     * Never call delete (or otherwise explicitly
     * destruct and reference counted object)
     *
     * @return true if the object was actully deleted from memory, false otherwise.
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT bool sub_ref(void) const;

    /**
     * Declares this object as an automatic (stack)
     * variable and thus immune to the sub_ref
     * induced destruction.
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void auto_ref(void) const;

    /**
     * Returns the current reference count.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT int current_ref(void) const;

    //
    // serialization stuff
    //

    /**
     * Pretty ASCII printer. Prints a representation
     * of this object to the given stream.
     *
     * @param o the stream to write out to
     * @return the same stream
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual scopira::tool::oflow_i & print(scopira::tool::oflow_i &o) const;

    /**
     * Serialization loader.
     * Tells the object to load its state information from
     * the given stream.
     * object's load however, should not be called.
     * It will assert failure if it is mistakenly called
     * (which is often the base when one tries to serialize
     * a class which lacks its own load method)
     *
     * @param in the stream to load from
     * @return true on success
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual bool load(scopira::tool::iobjflow_i& in);
    /**
     * Serialization saver.
     * Writes the object's state information to the given
     * stream.
     * object's save however, should not be called.
     * It will assert failure if it is mistakenly called
     * (which is often the base when one tries to serialize
     * a class which lacks its own save method)
     *
     * @param out the stream to write to
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual void save(scopira::tool::oobjflow_i& out) const;

#ifndef NDEBUG
    /**
     * Is this object alive and valid?
     * Calling this on a null this object is valid!
     * @author Aleksander Demko
     */
    bool is_alive_object(void) const { return this && (dm_object_magic == -236944); }
    /**
     * Is this object valid but dead?
     * @author Aleksander Demko
     */
    bool is_dead_object(void) const { return dm_object_magic == 5529022; }
#endif

  protected:
    /**
     * Default constructor
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT object(void);

    /**
     * Alternate constructor.
     *
     * If neverusecounter is true the object will never use the debug ref counting system,
     * even if it's available.
     *
     * This is primary for static/globabl objects that perhaps can live/die
     * outside of the main()'s life span.
     *
     * Passing false is the same as the default constructor: use it if it's available.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT explicit object(bool neverusecounter);

  private:
    /**
     * Copy constructor. This constructor does nothing but throw an assert.
     * The purpose of this is to disable the default C++ copy constructor
     * (which doesn't work for object).
     * Decendant classes that want to make copy constructors should simply
     * call object::object(void) (the default constructor).
     *
     * @author Aleksander Demko
     */
    object(const object &o);
};

/**
 * A reference counting auto pointer of scopira::tool::object and its decendants.
 * This class has the following key characteristics:
 *  - safe, null initialization
 *  - always maintain one (and only one) * count to its given pointer
 *
 * You can use this class like so:
 * @code
 *  {
 *    scopira::tool::countr_ptr<X> x = new X;
 *    // you need not (and must not) delete explicitly now
 *  }//at this point, x will automatically delete the pointer
 * @endcode
 *
 * Here is a more complex example:
 * @code
 *  {
 *    scopira::tool::countr_ptr<X> x;  //x will be initialized to null
 *
 *    {
 *      scopira::tool::countr_ptr<X> y = new X; // y points to a new instace of X
 *
 *      x = y;    // y and x now both reference the same, new instance
 *    }//y is destroyed here, however, the instance is now as x still has a reference
 *  }//x is destroyed, as since its the last reference, so is the actual instance of X
 * @endcode
 *
 * @see scopira::tool::object
 * @see @ref scopiratoolobject
 * @author Aleksander Demko
 */
template <class T> class scopira::tool::count_ptr
{
  public:
    typedef T data_type;

  protected:
    T *dm_ptr;

  public:
    /**
     * Constructor, null initializing
     *
     * @author Aleksander Demko
     */
    count_ptr(void)
      : dm_ptr(0) { }
    /**
     * Initializing constructor
     *
     * @author Aleksander Demko
     */
    count_ptr(T *o)
      : dm_ptr(o) { if (o) o->add_ref(); }
    /**
     * Copy constructor
     *
     * @author Aleksander Demko
     */
    count_ptr(const count_ptr<T> &o)
      : dm_ptr(o.dm_ptr) { if (dm_ptr) dm_ptr->add_ref(); }
    /**
     * Copy constructor
     *
     * @author Aleksander Demko
     */
    template <class Y> count_ptr(const count_ptr<Y> &o)
      : dm_ptr(o.get()) { if (dm_ptr) dm_ptr->add_ref(); }
    /**
     * Destructor
     * @author Aleksander Demko
     */
    ~count_ptr()
      { if (dm_ptr) dm_ptr->sub_ref(); }

    /**
     * Sets the current pointer. Any old pointer will ofcourse
     * be unreferenced.
     *
     * @author Aleksander Demko
     */
    void set(T* o)
      { if (o) o->add_ref(); if (dm_ptr) dm_ptr->sub_ref(); dm_ptr = o; }
    /**
     * Basic assignment.
     *
     * @author Aleksander Demko
     */
    count_ptr & operator = (T *o)
      { set(o); return *this; }

    /**
     * Basic assignment.
     *
     * @author Aleksander Demko
     */
    count_ptr & operator = (const count_ptr<T> &o)
      { set(o.dm_ptr); return *this; }
    /**
     * Comparison (equals) operator - compares the internal pointer values.
     *
     * @author Aleksander Demko
     */
    bool operator == (const count_ptr<T> &rhs) const { return dm_ptr == rhs.dm_ptr; }
    /**
     * Comparison (equals) operator - compares the internal pointer value to the given pointer.
     *
     * @author Aleksander Demko
     */
    bool operator == (const T* rhs) const { return dm_ptr == rhs; }
    /**
     * Comparison (not equals) operator - compares the internal pointer values.
     *
     * @author Aleksander Demko
     */
    bool operator != (const count_ptr<T> &rhs) const { return dm_ptr != rhs.dm_ptr; }
    /**
     * Comparison (not equals) operator - compares the internal pointer value to the given pointer.
     *
     * @author Aleksander Demko
     */
    bool operator != (const T* rhs) const { return dm_ptr != rhs; }
    /**
     * Comparison (less than) operator - compares the internal pointer values.
     *
     * @author Aleksander Demko
     */
    bool operator < (const count_ptr<T> &rhs) const { return dm_ptr < rhs.dm_ptr; }

    /**
     * Pretty ascii printer, redirects to internal class.
     *
     * @author Aleksander Demko
     */
    scopira::tool::oflow_i & print(scopira::tool::oflow_i &o) const
       { if (dm_ptr) dm_ptr->print(o); /*else o<<"(null)";*/ return o; }

    // do save and load like the above?

    /**
     * Gets the current object as a regular pointer.
     *
     * @author Aleksander Demko
     */
    T* get(void) const { return dm_ptr; }

    /**
     * Gets the current object, as a reference
     *
     * @author Aleksander Demko
     */
    T& ref(void) const
      { assert("[NULL pointer dereferenced in count_ptr]" && dm_ptr); return *dm_ptr; }
    //T& ref(void)
      //{ assert(dm_ptr); return *dm_ptr; }

    /**
     * Convinient -> accesor to the object
     *
     * @author Aleksander Demko
     */
    T* operator ->(void) const
      { assert("[NULL pointer dereferenced in count_ptr]" && dm_ptr); return dm_ptr; }
    //T* operator ->(void)
      //{ assert(dm_ptr); return dm_ptr; }

    /**
      * Convinient * deferencer.
      *
      * @author Aleksander Demko
      */
    T & operator *(void) const
      { assert("[NULL pointer dereferenced in count_ptr]" && dm_ptr); return *dm_ptr; }

    /**
     * Is the pointer null?
     *
     * @return true if the internal pointer is null.
     * @author Aleksander Demko
     */
    bool is_null(void) const { return dm_ptr == 0; }
};

/**
 * This auto pointer is a more flexibile (but much less
 * used and usefull) version of scopira::tool::count_ptr.
 *
 * This version is used in only special cases, you typically should prefer the popular
 * scopira::tool::count_ptr.
 *
 * This pointer stores an internal bool (which is set only via the
 * constructor) that controls weither or not this auto pointer
 * does any reference counting at all. If set to true,
 * then it acts like a normal count_ptr. If set to false,
 * it will not call any reference counting methods, but just
 * act like a plain, vanilla pointer.
 *
 * @see scopira::tool::count_ptr
 * @author Aleksander Demko
 */
template <class T> class scopira::tool::count2_ptr
{
  protected:
    T* dm_ptr;
    bool dm_doref;

  public:
    typedef T ptr_type;

    /**
     * constructor
     *
     * @author Aleksander Demko
     */
    /*count2_ptr(bool doref)
      : dm_ptr(0), dm_doref(doref) { }*/
    /**
     * Initializing constructor.
     *
     * @param doref Wiether or not reference counting will be done at all
     * @param o initial value
     * @author Aleksander Demko
     */
    count2_ptr(bool doref, T* o)
      : dm_ptr(o), dm_doref(doref) { if (doref && o) o->add_ref(); }
    /**
     * Copy constructor
     *
     * @author Aleksander Demko
     */
    count2_ptr(const count_ptr<T>& o)
      : dm_ptr(o.dm_ptr), dm_doref(o.dm_doref) { if (dm_doref && dm_ptr) dm_ptr->add_ref(); }
    /**
     * Destructor
     * @author Aleksander Demko
     */
    ~count2_ptr()
      { if (dm_doref && dm_ptr) dm_ptr->sub_ref(); }

    /**
     * Sets the current pointer.
     *
     * @author Aleksander Demko
     */
    void set(T* o)
      { if (dm_doref && o) o->add_ref(); if (dm_doref && dm_ptr) dm_ptr->sub_ref(); dm_ptr = o; }
    /**
     * Assignment.
     *
     * @author Aleksander Demko
     */
    count2_ptr& operator=(T* o)
      { set(o); return *this; }

    /**
     * Assignment.
     *
     * @author Aleksander Demko
     */
    count2_ptr& operator=(const count2_ptr<T>& o)
      { set(o.dm_ptr); return *this; }
    /**
     * Comparison (equals) - compares internal pointer values.
     *
     * @author Aleksander Demko
     */
    bool operator == (const count2_ptr<T>& rhs) const { return dm_ptr == rhs.dm_ptr; }
    /**
     * Comparison (equals) operator - compares the internal pointer value to the given pointer.
     *
     * @author Aleksander Demko
     */
    bool operator == (const T* rhs) const { return dm_ptr == rhs; }
    /**
     * Comparison (not equals) - compares internal pointer values.
     *
     * @author Aleksander Demko
     */
    bool operator != (const count2_ptr<T>& rhs) const { return dm_ptr != rhs.dm_ptr; }
    /**
     * Comparison (not equals) operator - compares the internal pointer value to the given pointer.
     *
     * @author Aleksander Demko
     */
    bool operator != (const T* rhs) const { return dm_ptr != rhs; }
    /**
     * Comparison (less than) - compares internal pointer values.
     *
     * @author Aleksander Demko
     */
    bool operator < (const count2_ptr<T>& rhs) const { return dm_ptr < rhs.dm_ptr; }

    /**
     * Pretty ascii printer, redirects to internal class.
     *
     * @author Aleksander Demko
     */
     oflow_i& print(oflow_i& o) const
       { if (dm_ptr) dm_ptr->print(o); return o; }

    // do save and load like the above?

    /**
     * Gets the current object as a regular pointer.
     *
     * @author Aleksander Demko
     */
    T* get(void) const { return dm_ptr; }

    /**
     * Returns the current object as a regular pointer.
     * This is an implicit conversion.
     *
     * @author Aleksander Demko
     */ 
    operator T*(void) const { return dm_ptr; }
    //T* get(void)
      //{ return dm_ptr; }

    /**
     * Gets the current object, as a reference
     *
     * @author Aleksander Demko
     */
    T& ref(void) const
      { assert("[NULL pointer dereferenced in count_ptr]" && dm_ptr); return *dm_ptr; }
    //T& ref(void)
      //{ assert(dm_ptr); return *dm_ptr; }

    /**
     * Convinient -> accesor to the object
     *
     * @author Aleksander Demko
     */
    T* operator ->(void) const
      { assert("[NULL pointer dereferenced in count_ptr]" && dm_ptr); return dm_ptr; }
    //T* operator ->(void)
      //{ assert(dm_ptr); return dm_ptr; }

    /**
      * Convinient * deferencer.
      *
      * @author Aleksander Demko
      */
    T & operator *(void) const
      { assert("[NULL pointer dereferenced in count_ptr]" && dm_ptr); return *dm_ptr; }

    /**
     * Is the pointer null?
     *
     * @return true if the internal pointer is null.
     * @author Aleksander Demko
     */
    bool is_null(void) const { return dm_ptr == 0; }
};

/**
 * Output stream operator. This will call the objects print()
 * routine on the given stream.
 *
 * @param o the output stream
 * @param vl the object
 * @return the same stream
 * @author Aleksander Demko
 */
SCOPIRA_EXPORT scopira::tool::oflow_i & operator << (scopira::tool::oflow_i &o, const scopira::tool::object &vl);

/**
 * Output operator varient for pointers to object.
 *
 * @param o the output stream
 * @param vl the object
 * @return the same stream
 * @author Aleksander Demko
 */
SCOPIRA_EXPORT scopira::tool::oflow_i & operator << (scopira::tool::oflow_i &o, const scopira::tool::object *vl);

/**
 * Output operator varient for count_ptr's.
 *
 * @param o the output stream
 * @param ptr the object
 * @return the same stream
 * @author Aleksander Demko
 */
template <class C>
  inline scopira::tool::oflow_i & operator << (scopira::tool::oflow_i &o, const scopira::tool::count_ptr<C> &ptr)
{
  return o << ptr.get();
}



//
// *** DEBUG ROUTINES
//

#ifndef NDEBUG
namespace scopira
{
  namespace tool
  {
    class objrefcounter;
  }
}

/**
 * Internal, singleton like class usefull for debuggin.
 * This class summarized and prints all the add_refs
 * and sub_refs done throughout the application.
 * Any inbalances will be reported.
 *
 * Only available (and thus used) in debug mode.
 *
 * @author Aleksander Demko
 */
class scopira::tool::objrefcounter
{
  protected:
    static int dm_ref;
    static int dm_peak;
    static int dm_tot;
    static int dm_baddel;
    static int dm_real;
    
  public:
    /**
     * Construct the class. This should be done in your main method
     * (or similar) and defore any reference counting methods (withing
     * object) have been called)
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT objrefcounter(void);
    /**
     * Destruct the class and report the counts.
     * This should be done after all reference counting methods have been
     * called (neat the end of your main (or similar).
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT ~objrefcounter();

  public:
    /**
     * Just to avoid "class unused" compiler warnings when you
     * instantiate this class.
     * @author Aleksander Demko
     */
    void foo(void) const { }
    
  private:
    friend class object;

    static bool has_counter(void);
    static void add_ref(void);
    static void sub_ref(void);
    static void bad_del(void);
    static void add_real(void);
    static void sub_real(void);

    static void output(void);
};

// NDEBUG
#endif

/**
  \page scopiratoolobject Reference counting

  \section introsec Introduction

  The base object class is scopira::tool::object.
  This is the base class for all classes that want to be:
  
  - Reference counted via count_ptr
  - Be serializable (streamable to I/O streams)
  - Lifecycle debugging methods

  You should almost always inherit from this class virtually. That is:

  \code
  class myclass : public virtual scopira::tool::object { } ;
  \endcode

  \section debugsec Debugging

  Each object descendant, during debug builds exposes an scopira::tool::object::is_alive_object
  method that returns true if the given object is non-null and valid (not destroyed)
  and matches an internal magic number.
  This is useful in catching many bad-memory manipulation errors.

  \code
    assert(is_alive_object());              // assert that "this" is valid
    assert(someobj->is_alive_object());     // assert that someobj is valid
  \endcode

  Also while in debug builds, Scopira will notify the user on exit if there
  are any non-deleted scopira::tool::object decendants in memory.
  This greatly helps in detecting memory leaks.

  \section countptrsec Reference counting

  Reference counting is the core memory managment style of most Scopira objects
  in a Scopira application.

  All scopira::tool::object decendant instances can be reference counted, but need not be
  (ie. they can still be created on the stack, etc). However, once an object
  is reference counted by atleast one counter, that instance now must be reference
  counted until its destruction. Reference counted objects may be shared by multiple
  reference counters. When the last counter releases their count, the object is destroyed.

  scopira::tool::count_ptr is the basic "smart pointer" template class for handling
  reference counts. This smart pointer provides all the usual pointer like methods,
  but makes sure to keep a reference count on the object they contain.

  Some sample code:

  \code
  {
    scopira::tool::count_ptr<someclass> p, p2;    // initialized to null by default

    p = new p;              // create an instance, it is now reference counted
    p->somemethods();       // access p
    p.get();                // returns a pointer to someclass
    *p;                     // returns a reference to someclass

    p2 = p;                 // two ferences to the same object

    p = 0;                  // won't delete the instance, p2 still has a reference
    p2 = 0;                 // will delete the instance

    // note that count_ptr's naturally de-reference count their instance upon their
    // destruction. You need not assign null to them explicitly
  }
  \endcode

  All scopira::tool::object descendants can be reference counted by scopira::tool::count_ptr
  by simply proving your own add_ref() and sub_ref() methods. This is typically only done
  in the most sepecialized of cases, however.

  \section setobjsec Serializable Objects

  To make your own objects serialization, see \ref scopiracoreserializationpage
*/

#endif

