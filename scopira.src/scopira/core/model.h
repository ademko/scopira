
/*
 *  Copyright (c) 2005    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_CORE_MODEL_H__
#define __INCLUDED_SCOPIRA_CORE_MODEL_H__

#include <assert.h>

#include <vector>
#include <map>

#include <scopira/tool/object.h>
#include <scopira/tool/reactor.h>
#include <scopira/tool/iterator.h>
#include <scopira/tool/export.h>

namespace scopira
{
  namespace core
  {
    class model_ptr_base;
    template <class T> class model_ptr;

    class model_i;

    class project_i;  //fwd

    class view_i;   // fwd

    /// the model iterator type
    typedef scopira::tool::iterator_g<model_i*> model_iterator;
  }
}

/**
 * Concrete class for some viewwidget/button magic.
 *
 * @author Aleksander Demko
 */ 
class scopira::core::model_ptr_base
{
  protected:
    /// internal: the actual pointer
    model_i *dm_model;
  public:
    /// ctor
    model_ptr_base(void)
      : dm_model(0) { }

    /// gets the raw base pointer (really specialized use here)
    model_i * const & get_model_ptr(void) const { return dm_model; }
};

/**
 * Like count_ptr, but does additional view-subscription
 * to model_i stuff
 * @author Aleksander Demko
 */ 
template <class T> class scopira::core::model_ptr : public scopira::core::model_ptr_base
{
  public:
    /// the type of the data this model_ptr points too
    typedef T data_type;

  protected:
    /// the view
    view_i *dm_ins;
    /// the actual pointer
    T *dm_ptr;

  public:
    /**
     * Constructor, null initializing
     *
     * @author Aleksander Demko
     */
    model_ptr(view_i *ins)
      : dm_ins(ins), dm_ptr(0) { assert(ins); }
    /**
     * Destructor
     * @author Aleksander Demko
     */
    ~model_ptr()
      { set(0); }

    /**
     * Sets the current pointer. Any old pointer will ofcourse
     * be unreferenced.
     *
     * @author Aleksander Demko
     */
    void set(T* o) {
      if (o) {
        o->add_ref();
        o->add_view(dm_ins);
      }
      if (dm_ptr) {
        dm_ptr->remove_view(dm_ins);
        dm_ptr->sub_ref();
      }
      dm_ptr = o;
      dm_model = o;   // double concrete magic [NOTE]
    }
    /**
     * Assignment.
     *
     * @author Aleksander Demko
     */
    model_ptr & operator = (T *o)
      { set(o); return *this; }

    /**
     * Assignment.
     *
     * @author Aleksander Demko
     */
    model_ptr & operator = (const model_ptr<T> &o)
      { set(o.dm_ptr); return *this; }
    /**
     * Comparison (equals) - compares internal pointer values.
     *
     * @author Aleksander Demko
     */
    bool operator == (const model_ptr<T> &rhs) const
      { return dm_ptr == rhs.dm_ptr; }
    /**
     * Comparison (not equals) - compares internal pointer values.
     *
     * @author Aleksander Demko
     */
    bool operator != (const model_ptr<T> &rhs) const
      { return dm_ptr != rhs.dm_ptr; }
    /**
     * Comparison (less than) - compares internal pointer values.
     *
     * @author Aleksander Demko
     */
    bool operator < (const model_ptr<T> &rhs) const
      { return dm_ptr < rhs.dm_ptr; }

    /**
     * Pretty ascii printer, redirects to internal class.
     *
     * @author Aleksander Demko
     */
     //oflow_i & print(oflow_i &o) const
       //{ if (dm_ptr) dm_ptr->print(o); /*else o<<"(null)";*/ return o; }

    // do save and load like the above?

    /**
     * Gets the current object
     *
     * @author Aleksander Demko
     */
    T* get(void) const
      { return dm_ptr; }
    //T* get(void)
      //{ return dm_ptr; }

    /**
     * Gets the current object, as a reference
     *
     * @author Aleksander Demko
     */ 
    T& ref(void) const
      { assert(dm_ptr); return *dm_ptr; }
    //T& ref(void)
      //{ assert(dm_ptr); return *dm_ptr; }

    /**
     * Convinient -> accesor to the object
     *
     * @author Aleksander Demko
     */
    T* operator ->(void) const
      { assert(dm_ptr); return dm_ptr; }
    //T* operator ->(void)
      //{ assert(dm_ptr); return dm_ptr; }

    /**
      * Convinient * deferencer.
      *
      * @author Aleksander Demko
      */
    T & operator *(void) const
      { assert(dm_ptr); return *dm_ptr; }

    /**
     * Is the pointer null?
     *
     * @author Aleksander Demko
     */
    bool is_null(void) const { return dm_ptr == 0; }
};

/**
 * A model_i.
 * Multiple views can monitor and watch an instance of this.
 *
 * If your descendant is serializable, YOU must handle the saveing/loading
 * of dm_title, if need be (or just call model_i::save/load())
 *
 * @author Aleksander Demko
 */
class scopira::core::model_i : public virtual scopira::tool::object
{
  private:
    typedef std::vector<view_i*> view_list;
    typedef std::vector<scopira::tool::rename_reactor_i*> rename_list;
    typedef std::map<std::string, scopira::tool::count_ptr<model_i> > archive_cache_t;

    mutable view_list dm_views;
    mutable rename_list dm_rename_reactors;
    std::string dm_title;
    project_i *dm_owner;

    // mutable? mutex locked?
    archive_cache_t dm_archive_cache;

  public:
    /// for title, mostly
    SCOPIRA_EXPORT virtual bool load(scopira::tool::iobjflow_i& in);
    /// for title, mostly
    SCOPIRA_EXPORT virtual void save(scopira::tool::oobjflow_i& out) const;

    /**
     * Sets the title.
     * Descendants may override this to do addtional actions
     * on a title change request, but they should make sure
     * to call this one evetually.
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual void set_title(const std::string &newtitle);

    /**
     * Is this a model project?
     *
     * @author Aleksander Demko
     */
    virtual bool is_project(void) const { return false; }

    /**
     * Gets the current title
     * @author Aleksander Demko
     */
    const std::string & get_title(void) const { return dm_title; }

    /**
     * Adds an view to this model_i's watch list.
     * The view's bind_model will be called.
     * This ptr will not be count_ptr'ed here.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void add_view(view_i *ins) const;
    /**
     * Removes an view from this model_i's watch list.
     * The view's bind_model will be called.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void remove_view(view_i *ins) const;
    /**
     * Calls viewodel() on all the current views
     * that are watching this model_i, with the given src.
     * The src view will NOT have his viewodel
     * method called, however.
     * src may be null (in which case all the views are called)
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void notify_views(view_i *src);

    /**
     * Adds a new rename reactor that will listen to rename events.
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void add_rename_reactor(tool::rename_reactor_i *r) const;
    /**
     * Removes a rename reactor from the listing list
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void remove_rename_reactor(tool::rename_reactor_i *r) const;

    /**
     * Sets the owning project. project do this themselves
     * in their add_record calls.
     * May be 0 to clear the previous owner.
     * Descendants should call this version, which (for now), simply
     * sets dm_owner.
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual void set_project(project_i *newowner);

    /**
     * Gets the current owner -- may be 0 if this record currently
     * does not have an owner.
     * @author Aleksander Demko
     */
    project_i * get_project(void) const { return dm_owner; }

    /**
     * Sets a tagged (named) model attached to this model.
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void set_tagged_model(const std::string &name, model_i *what);

    /**
     * Gets the tagged model attached to this type.
     *
     * @param name the model name
     * @param out where to place the model, this should be a count_ptr or model_ptr of your type
     * @return true on success (and fillsin out).
     * @author Aleksander Demko
     */ 
    template <class W>
      bool get_tagged_model(const std::string &name, W &out) {
        typedef typename W::data_type data_type;   // just to make sure they dont pass us a simple *T
        scopira::tool::count_ptr<scopira::core::model_i> mm;
        if (!get_tagged_model_impl(name, mm))
          return false;
        out = dynamic_cast<data_type*>(mm.get());
        return mm.is_null() || out.get();   // success is if the data was null OR the cast worked
      }

  protected:
    /// Default initing ctor
    SCOPIRA_EXPORT model_i(void);
    /// title initing ctor
    SCOPIRA_EXPORT model_i(const std::string &title);
    /// copy ctor
    SCOPIRA_EXPORT model_i(const model_i &src);
    /// dtor (mainly, to flush the cache
    SCOPIRA_EXPORT virtual ~model_i();

  private:
    /// internal function
    SCOPIRA_EXPORT bool get_tagged_model_impl(const std::string &name,
        scopira::tool::count_ptr<scopira::core::model_i> &out);
};

#endif

