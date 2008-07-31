
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

#ifndef __INCLUDED_SCOPIRA_CORE_REGISTER_H__
#define __INCLUDED_SCOPIRA_CORE_REGISTER_H__

#include <scopira/tool/objflowloader.h>

namespace scopira
{
  namespace core
  {
    /**
     * View Groups.
     *
     * Views can belong to groups. These are repsented as a bitmask.
     *
     * 0x0F0F is reseved for scopira definitions.
     * 0xF0F0 are for end user programmers to use.
     * 0x00FF is the default SHOW mask. So if you want to make a new group, and have
     * it visible by default, put it in 0x00F0 somewhere. If you want it hidden,
     * use 0xF000.
     *
     * @author Aleksander Demko
     */ 
    enum
    {
      vg_primary =      0x0001,       // not used, but i might sometime
      vg_noui_c =       0x0002,       // doesnt actually have a UI component (and doesnt decent from coreui::widget, etc)
      vg_windowui_c =   0x0004,       // has a gui, and always wants to be in its own window (having this and vg_noui_c)

      vg_needs_project_c = 0x0100,    // a view that requires the model to have a project and be clicked from the project tree manager
      vg_needs_copyfunc_c = 0x0200,   // a view that requires the model have a registered copy ctor/func

      vg_show_c =       0x00FF,       // lower 16 are shown by default
      vg_hide_c =       0xFF00,       // default that are hidden (just to be complete)
    };

    // another mapping of the above
    enum
    {
      no_uimode_c = vg_noui_c,
      default_uimode_c = 0,
      windowonly_uimode_c = vg_windowui_c,
    };

    /// a class type for models that are just interfaces
    class interface_model_type { };
    /// a class type for models that are copyable
    class copyable_model_type { };

    typedef scopira::tool::object* (*copyfunc_t)(const scopira::tool::object*);
    template <class T> scopira::tool::object* objcopyfunc_g(const scopira::tool::object *srco) { return new T(*dynamic_cast<const T*>(srco)); }

    class mi_set;

    template <class T> class register_flow;
    template <class T> class register_object;
    template <class T> class register_model;
    template <class T> class register_view;
    //class register_task;

    /// internal functions
    SCOPIRA_EXPORT void flow_reg_imp(const std::type_info& id, const char * name, scopira::tool::objflowloader::loadfunc_t loader);
    /// internal functions
    SCOPIRA_EXPORT void link_object_parent_imp(const std::type_info &id, const char *parent_name);
    /// internal functions
    SCOPIRA_EXPORT void link_object_parent_imp(const std::type_info &id, const char **parents);
    /// internal functions
    SCOPIRA_EXPORT void link_object_copyfunc(const std::type_info &id, copyfunc_t cf);
    /// internal functions
    SCOPIRA_EXPORT void link_model_imp(const std::type_info &id, bool linktomodeltype);
    /// internal functions
    SCOPIRA_EXPORT void link_view_imp(const std::type_info &id, const char *model_type, const char *menu_name, int vgmask);
    class objecttype;
    /// internal functions
    SCOPIRA_EXPORT void build_objecttype_tree(scopira::tool::count_ptr<scopira::core::objecttype> &out);

    /// common type string: base of everything
    extern SCOPIRA_EXPORT const char *object_type_c;
    /// common type string: base of all models
    extern SCOPIRA_EXPORT const char *model_type_c;
    /// common type string: base of all project-enabled models
    extern SCOPIRA_EXPORT const char *project_type_c;
    /// common type string: base of all views
    extern SCOPIRA_EXPORT const char *view_type_c;
  }
}

// Lots of C-array stuff for efficincy when making static objects
// all resolves to std::strings for storage, though

/**
 * A set of const char *s that are used to specifiy multiple
 * inheritance sets when registering
 *
 * @author Aleksander Demko
 */
class scopira::core::mi_set
{
  public:
    const char * pm_parents[4];

  public:
    // ctors

    mi_set(const char *parent0)
      { pm_parents[0] = parent0; pm_parents[1] = 0; }
    mi_set(const char *parent0, const char *parent1)
      { pm_parents[0] = parent0; pm_parents[1] = parent1; pm_parents[2] = 0; }
    mi_set(const char *parent0, const char *parent1, const char *parent2)
      { pm_parents[0] = parent0; pm_parents[1] = parent1; pm_parents[2] = parent2; pm_parents[3] = 0; }
};

/**
 * A registration object for registering objects just for
 * serialization (or at the least, virtual ctor loading).
 * This is the most basic registration object.
 *
 * @author Aleksander Demko
 */
template <class T>
class scopira::core::register_flow
{
  public:
    /// ctor, yes, just plain char *
    register_flow(const char *fullname) {
      scopira::core::flow_reg_imp(typeid(T), (fullname), scopira::tool::objloadfunc_g<T>);
    }
    /// version that has a NULL loader
    register_flow(const char *fullname, int dummy) {
      scopira::core::flow_reg_imp(typeid(T), (fullname), 0);
    }
};

/**
 * Registers object in a run time type tree.
 *
 * @author Aleksander Demko
 */ 
template <class T>
class scopira::core::register_object : public scopira::core::register_flow<T>
{
  private:
    typedef scopira::core::register_flow<T> parent_type;
  public:
    /// ctor, null parent_name means "top level object" (ie from tool::object)
    register_object(const char *fullname, const char *parent_name = 0)
      : parent_type(fullname)
      { link_object_parent_imp(typeid(T), parent_name); }
    /// ctor
    register_object(const char *fullname, mi_set s)
      : parent_type(fullname)
      { link_object_parent_imp(typeid(T), s.pm_parents); }
    /// version that registers interfaces (ie. no loader)
    /// parent_name may be null
    register_object(const char *fullname, const char *parent_name, int dummy)
      : parent_type(fullname, dummy)
      { link_object_parent_imp(typeid(T), parent_name); }

    // a pure conat char ** one can be made too, if need be
};

/**
 * Registers object in a run time type tree, that is
 * a "model". In addition to what register_object does,
 * this makes the class decent from the model base class
 * and stores any additional model specific info.
 *
 * There is no additional model specific info, yet.
 *
 * @author Aleksander Demko
 */ 
template <class T>
class scopira::core::register_model : public scopira::core::register_flow<T>
{
  private:
    typedef scopira::core::register_flow<T> parent_type;
  public:
    /// ctor, null parent_name means "model_i"
    register_model(const char *fullname, const char *parent_name = 0)
      : parent_type(fullname)
      { link_model_imp(typeid(T), !parent_name); link_object_parent_imp(typeid(T), parent_name); }
    /// ctor, null parent_name means "model_i", this version is for stuff with cpy ctors
    register_model(const char *fullname, const char *parent_name, copyable_model_type dummyinstance)
      : parent_type(fullname)
      { link_model_imp(typeid(T), !parent_name); link_object_parent_imp(typeid(T), parent_name);
        link_object_copyfunc(typeid(T), objcopyfunc_g<T>); }
    /// ctor
    register_model(const char *fullname, mi_set s)
      : parent_type(fullname)
      { link_model_imp(typeid(T), false); link_object_parent_imp(typeid(T), s.pm_parents); }
    /// ctor, null parent_name means "model_i", interface version (ie. no loader)
    register_model(const char *fullname, const char *parent_name, interface_model_type dummyinstance)
      : parent_type(fullname, 1)
      { link_model_imp(typeid(T), !parent_name); link_object_parent_imp(typeid(T), parent_name); }

    // a pure const char ** one can be made too, if need be
};

/**
 * Registers object in a run time type tree, that is
 * a "model". In addition to what register_object does,
 * this makes the class decent from the model base class
 * and stores any additional model specific info.
 *
 * There is no additional model specific info, yet.
 *
 * @author Aleksander Demko
 */ 
template <class T>
class scopira::core::register_view : public scopira::core::register_flow<T>
{
  private:
    typedef scopira::core::register_flow<T> parent_type;
  public:
    /// ctor, null parent_name means "top level object" (ie from tool::object)
    /**
     * Constructor.
     *
     * fullname is the full name of the class that is being registered, this is usually
     * the full c++ name.
     * model_type is the full name of the model to be associated with this view.
     * menu_name is the name in the popup menu. Slashes denote sub menus ("blah/View" for example)
     * vgmask is a bitmask of flags and options
     *
     * @author Aleksander Demko
     */ 
    register_view(const char *fullname, const char *model_type, const char *menu_name, int vgmask = default_uimode_c)
      : parent_type(fullname)
      { link_view_imp(typeid(T), model_type, menu_name, vgmask); }
};

/**
  \page scopiracoreserializationpage Object I/O

  Serializable objects are able to convert themselves to/from a series of primitive types. Programmers may then utilize the flexibility
  of the flow architecture to then store objects in various places, such as ASCII text files, binary files, in memory, or even over a 
  network connection. By stacking and connecting filtering flows to an end flow, the possibilities are limitless. For an object to be 
  serializable it must:

  - Descend from scopira::tool::object.
  - Implement scopira::tool::object::save and scopira::tool::object::load. When implementing these methods, descendants must first take care to call 
    their parents save or load method, as the build process is recursive. 
  - Register themselves for serialization. This is done by instatiating a scopira::core::register_flow singleton in your .cpp file.

  An example of scopira::core::register_flow:

  \code
  #include <scopira/core/register.h>

  class yournamespace::yourclass : public virtual scopira::tool::object
  {
    public:
      virtual bool load(scopira::tool::iobjflow_i& in) {
        // load self from in
      }
      virtual void save(scopira::tool::oobjflow_i& out) const {
        // write self to out
      }
  };

  // place one of these in the .cpp file of your class
  static scopira::core::register_flow<yournamespace::yourclass> r1("yournamespace::yourclass");
  \endcode

*/
#endif

