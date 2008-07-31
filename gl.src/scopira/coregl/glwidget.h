
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

#ifndef __INCLUDED_SCOPIRA_COREGL_GLWIDGET_H__
#define __INCLUDED_SCOPIRA_COREGL_GLWIDGET_H__

#include <string.h>

#include <string>

#include <scopira/tool/array.h>
#include <scopira/coreui/widget.h>
#include <scopira/coreui/canvas.h>
#include <scopira/coregl/export.h>

#include <gdk/gdkgl.h>
#include <gtk/gtkgl.h>

#include <GL/gl.h>
#include <GL/glu.h>

namespace scopira
{
  /**
   * This subsystem contains some fundamental OpenGL bases classes
   * (that in turn, utilize GtkGLExt).
   *
   * @author Aleksander Demko
   */ 
  namespace coregl
  {
    class push_pop;
    class begin_end;

    class glfont;
    class glwidget;
  }
}

/**
 * Does an inline glPushMatrix and glPopMatrix
 * @author Aleksander Demko
 */
class scopira::coregl::push_pop
{
  public:
    /// ctor
    push_pop(void) { glPushMatrix(); }
    /// dtor
    ~push_pop(void) { glPopMatrix(); }
};

/**
 * Does inline glBegin and glEnd
 * @author Aleksander Demko
 */
class scopira::coregl::begin_end
{
  public:
    /// ctor
    begin_end(GLenum mode) { glBegin(mode); }
    /// dtor
    ~begin_end() { glEnd(); }
};

/**
 * A OpenGL display-list based font
 * @author Aleksander Demko
 */
class scopira::coregl::glfont
{
  private:
    enum { num_lists_c = 128 };

    bool dm_inited;

    GLuint dm_list_base;
    int dm_font_height;

  public:
    /// ctor
    SCOPIRAGL_EXPORT glfont(void);
    /// dtor
    SCOPIRAGL_EXPORT ~glfont();

    /**
     * You must call this (well, one of the inits()) in your handle_init.
     * This version loads a default font.
     * @author Aleksander Demko
     */
    SCOPIRAGL_EXPORT void init(void);
    /**
     * You must call this (well, one of the inits()) in your handle_init
     * This version loads the specific font.
     * @param fontname A pango like font name. For example, "Courier 12"
     * @author Aleksander Demko
     */
    SCOPIRAGL_EXPORT void init(const std::string &fontname);
    /**
     * You must call this in your handle_close.
     * @author Aleksander Demko
     */
    SCOPIRAGL_EXPORT void close(void);

    /**
     * Render a string, with this font, to the given
     * Raster position. You normallay call
     * glRasterPos3d or something before calling this.
     * Don't do this in a glBegin/glEnd pair, ofcourse!
     * @author Aleksander Demko
     */
    SCOPIRAGL_EXPORT void render_string(const std::string &v);

    /**
     * Render a string, with this font, to the given
     * Raster position. You normallay call
     * glRasterPos3d or something before calling this.
     * Don't do this in a glBegin/glEnd pair, ofcourse!
     * @author Aleksander Demko
     */
    SCOPIRAGL_EXPORT void render_string(const char *c);
};

/**
 * Base GL widget for OpenGL-enabled Scopira proponents.
 *
 * Decendants must:
 *  - call init_gui in their constructor (see its docs though)
 *  - implement handle_render(): try to render something a unit
 *    cubed about the origin
 *  - remember! gl*() methods may only be called from with
 *    handle_*() methods. You may also call handle_* methods
 *    from within other handle_* methods.
 *
 * @author Aleksander Demko
 */
class scopira::coregl::glwidget : public scopira::coreui::widget
{
  private:
    typedef scopira::coreui::widget parent_type;
    
  public:
    /// dtor
    SCOPIRAGL_EXPORT virtual ~glwidget();

  protected:
    /**
     * Constructor.
     *
     * This does it's own init_gui stuff. You may not do any of that.
     *
     * @author Aleksander Demko
     */
    SCOPIRAGL_EXPORT glwidget(void);

    /**
     * A helper method for alternative init_gui() sequences.
     * If adddefaultbuts is false, then the default Home/Perspective buttons
     * will not be added, and you'll just get a pure display.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAGL_EXPORT widget * make_drawing_area(bool adddefaultbuts = true);

    /**
     * Sets up the UI. Your descendants must call this. OR you can do the following:
     *
     * Call make_drawing_area(), and build your gui. Make sure you count_ptr your
     * top level widget. Then, pass your top level widget to widget::init_gui(GtkWidget*)
     *
     * This version simple goes a make_drawing_area then a init_gui operation, ofcourse.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAGL_EXPORT void init_gui(bool adddefaultbuts = true);

    /**
     * Called once on window creation - do you gl related
     * initialization here. This implementation does some
     * trivial stuff.
     * @author Aleksander Demko
     */
    SCOPIRAGL_EXPORT virtual void handle_init(void);
    /**
     * Called on window close -- this implementation does nothing.
     * FIXME this does not get called right now -- technicalities.
     * @author Aleksander Demko
     */
    SCOPIRAGL_EXPORT virtual void handle_close(void);
    /**
     * Called on every window resize. Always called atleast
     * once. Default implementation does viewport stuff
     * @author Aleksander Demko
     */
    SCOPIRAGL_EXPORT virtual void handle_resize(int newwidth, int newheight);
    /**
     * Decendants should always implementation (otherwise, what
     * would be the point). This implementation, which you may or
     * may not call, sets up some informative stuff.
     * You should render something 1-unit cubed around the origin
     * @author Aleksander Demko
     */
    SCOPIRAGL_EXPORT virtual void handle_render(void);
    /**
     * Called when the user clicks something (but doesn't drag).
     * Drags are always handled by the base class and never passed
     * to decendants.
     *
     * This implementation goes through the SELECTION system.
     *
     * @param button button number. 0=left, 1=middle 2=right
     * @param x the X [0..1)
     * @param y the Y [0..1)
     * @author Aleksander Demko
     */
    SCOPIRAGL_EXPORT virtual void handle_press(char button, int x, int y);
    /**
     * Called when a selection has been made.
     * This implementaion prints out the hit stack.
     * @param numhits the number of hits
     * @param hitstack the stack of the first hit (yeah, you can't get
     * at the others)
     * @author Aleksander Demko
     */
    SCOPIRAGL_EXPORT virtual void handle_select(int numhits, const scopira::tool::basic_array<int> &hitstack);

    /**
     * Requests an immediate handle_render()
     * @author Aleksander Demko
     */
    SCOPIRAGL_EXPORT void request_render(void);

    /**
     * Sets the dirty flag, used in redraw buffering.
     * This flag, by default, is on. If you set it to off,
     * the base class will not call your handle_render
     * and instead use its cached image.
     *
     * This speads up redrawing of widgets that have non-changing,
     * complex scenes.
     *
     * @author Aleksander Demko
     */
    void set_dirty(bool dirty) { dm_dirty = dirty; }

    /**
     * Flush all the errors, if any, to standard output.
     * Returns true if there where any.
     *
     * @author Aleksander Demko
     */
    SCOPIRAGL_EXPORT bool error_flush(void);

    /**
     * A object that may be used to
     * obtain gl context (block of code where you can make gl*() calls).
     * outside of handle_* methods.
     * @author Aleksander Demko
     */
    class gl_lock
    {
      private:
        GdkGLDrawable *dm_dr;
      public:
        gl_lock(glwidget *bas) {
          GdkGLContext *con = gtk_widget_get_gl_context(bas->dm_glw);
          dm_dr = gtk_widget_get_gl_drawable(bas->dm_glw);
          if (!gdk_gl_drawable_gl_begin(dm_dr, con))
            assert(false && "[Can't seem to access the OpenGL layer]");
        }
        ~gl_lock() { gdk_gl_drawable_gl_end(dm_dr); }
        void flush(void) {
          /* Swap buffers. */
          if (gdk_gl_drawable_is_double_buffered(dm_dr))
            gdk_gl_drawable_swap_buffers(dm_dr);
          else
            assert(false);// glFlush();
        }
    };

    /**
     * Is the mouse button currently pressed?
     *
     * @author Aleksander Demko
     */ 
    bool is_mouse_down(void) const { return dm_mousedown; }
    /**
     * Are we in a selection-mode render?
     *
     * @author Aleksander Demko
     */ 
    bool is_select_down(void) const { return dm_selectdown; }

  protected:
    /**
     * Is the mouse button currently pressed?
     * You should use is_mouse_down() instead.
     * This really should be a parm in handle_render().
     *
     * @author Aleksander Demko
     */ 
    bool dm_mousedown;
    /**
     * Are we in a selection-mode render?
     * You should use is_select_down() instead.
     * This really should be a parm in handle_render().
     *
     * @author Aleksander Demko
     */ 
    bool dm_selectdown;    // this really should be a parm in handle_render()

  protected:
    scopira::tool::fixed_array<double, 16> dm_xform;    /// current transform matrix
    GdkGLConfig * dm_glconfig;    
    GtkWidget *dm_glw;
    
    enum {camera_offset_c = -7} ;
    double dm_camera_scale;  /// scaling factor because of the "camera"
    double dm_basex, dm_basey;    /// used in mouse handlers    
    int dm_width, dm_height;
    bool dm_perspective, dm_inited;                  /// or orthogonal view?
    char dm_basecmd;       /// what button/function is currently down, 0 for none, +100 for not-yet-motioned
    bool dm_dirty;            /// use backbuffer if not dm_isdirty
    scopira::coreui::pixmap_canvas dm_dirty_buffer;
    
  private:
    /**
     * Generates the 3D drawing area.
     *
     * @author Aleksander Demko
     */ 
    GtkWidget * make_drawing_area_impl(bool adddefaultbuts);

    /**
     * Sets up the camera
     * @author Aleksander Demko
     */
    void setup_camera(int selx = -1, int sely = -1);
    
    /// Called once on startup
    static void h_realize(GtkWidget *widget, gpointer data);
    /// Called on widget resize
    static gboolean h_configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer data);
    /// Redraw handler
    static gboolean h_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data);
    /// mouse press and release handler
    static gboolean h_mouse(GtkWidget *widget, GdkEventButton *event, gpointer data);
    /// mouse motion handler
    static gboolean h_mouse_motion(GtkWidget *widget, GdkEventMotion *event, gpointer data);
    /// static handler, distroy a gtk widget
    //static void h_destroy(GtkWidget *widget, gpointer data);
    /// button hanlder
    static void h_but_per(GtkWidget *widget, gpointer data);
    /// button hanlder
    static void h_but_home(GtkWidget *widget, gpointer data);

  protected:
    /// saves the dirty buffer
    SCOPIRAGL_EXPORT void save_dirty_buffer(GtkWidget *widget);
    /// exponensial camera scaling
    SCOPIRAGL_EXPORT static double adjust_camera(double oldcamera, double pixdelta);        
};

/**
  \page scopiraglsyspage Scopira OpenGL Reference

  This subsytem provides the scopira::coregl::glwidget, a
  base class for building widgets that can draw on the display using
  OpenGL commands.

  Some helper classes are also in the namespace scopira::coregl.

  See scopira::coregl::glwidget for usage.

*/

#endif

