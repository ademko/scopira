
/*
 *  Copyright (c) 2002-2004    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */


#ifndef __INCLUDED__SCOPIRA_COREUI_CANVASCOMMON_H__
#define __INCLUDED__SCOPIRA_COREUI_CANVASCOMMON_H__

#include <vector>

#include <scopira/tool/array.h>
#include <scopira/coreui/widget.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace coreui
  {
    GdkColor packed_to_gdkcolor(int packedcolour);
    int gdkcolor_to_packed(GdkColor c);

    class point;

    class font;
    class layout;
    class context;
    class canvas;

    class widget_context;
    class widget_canvas;
    class pixmap_context;
    class pixmap_canvas;

    class color_vec;

    class rgb_image;

    class zoom_widget;

    class mouse_event {
      public:
        enum {
          shift_mod_c = 1,
          control_mod_c = 2
        };
      public:
        int x, y;
        bool press;     // true on press, false for release
        bool double_press;  // true on a double click
        short button;   // button number. 0=left, 1=middle 2=right
        short modifier; // 0=none, 1=shift, 2=control, see constants in this class
    };
  }
}

/**
 * a simple point or size spec
 *
 * @author Aleksander Demko
 */
class scopira::coreui::point
{
  public:
    int x, y;

    /// 0,0 initing ctor
    point(void)
      : x(0), y(0) { }
    /// initing ctor
    point(int _x, int _y)
      : x(_x), y(_y) { }
};

/**
 * a font
 *
 * @author Aleksander Demko
 * @author Marina Mandelzweig
 */
class scopira::coreui::font
{
  private:
    PangoFontDescription *dm_font;
  public:
    /**
     * Constructs a default font using the string "sans 8" 
     * This is sans family, no style options, size 8
     * 
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT font(void);
    
    /**
     * Constructs a font from a given string in the form "[family] [style] [size]"
     * family - a comma separated list of families (ie. sans, serif, monospace)
     * style - a whitespace separated list of styles (ie. normal, oblique, italic), variants (ie. normal, small-caps), 
     *             weights (ie. light, bold, heavy, etc) or stretch (ie. condensed, normal, expanded)
     * size - a decimal number representing size in points
     * See PangoFontDescription documentation for more options
     * 
     * eg. "sans bold 8"
     *        "monospace,serif light condensed 16"
     *        "normal 10"
     * 
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT font(const char *str);
    
    //diconstructor
    SCOPIRAUI_EXPORT ~font(void);
    
    /**
     * Sets the size given a size in points
     */
    SCOPIRAUI_EXPORT void set_size(int size);
    
    /**
     * Gets the size in points
     */
    SCOPIRAUI_EXPORT int get_size(void);

    friend class canvas;

  private:
    // disabled, not implemented:
    font(const font&);
    font & operator=(const font&);
};

/**
 * a graphics context
 *
 * @author Aleksander Demko
 */
class scopira::coreui::context
{
  protected:
    GdkGC *dm_gc;
  public:
    /// ctor (will link to the given one)
    SCOPIRAUI_EXPORT context(GdkGC *gc = 0) : dm_gc(gc) { }
    
    /// sets the foreground colour, from this previously mapped colour
    SCOPIRAUI_EXPORT void set_foreground(GdkColor &col);
    /**
     * Sets the foreground colour, via packedcolour 0xRRGGBB.
     *
     * This function does not need or use palettes.
     * If the current display is NOT a true direct colour
     * display (ie. its paletted/8-bit or something), then
     * a psuedo-true colour palette will be allocated.
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT void set_foreground(int packedcolour);
    /**
     * Sets the foreground colour, via r/g/b 0..255 each
     *
     * This function does not need or use palettes.
     * If the current display is NOT a true direct colour
     * display (ie. its paletted/8-bit or something), then
     * a psuedo-true colour palette will be allocated.
     * @author Aleksander Demko
     */ 
    void set_foreground(int r, int g, int b) {
      set_foreground( (r<<16) | (g<<8) | b );
    }
    /// sets the line attributes
    SCOPIRAUI_EXPORT void set_line_attributes(int line_width, GdkLineStyle line_style, 
      GdkCapStyle cap_style, GdkJoinStyle join_style);
    /// gets the context
    GdkGC* getContext() { return dm_gc; }
    
    friend class canvas;
};

/** 
 * A graphics layout shadow for Pango.
 *
 * @author Aleksander Demko
 */
class scopira::coreui::layout : public virtual scopira::tool::object
{
  protected:
    PangoContext *dm_context;
    PangoLayout *dm_layout;
  public:
     //constructor
    SCOPIRAUI_EXPORT layout(void);
    //diconstructor
    SCOPIRAUI_EXPORT ~layout();
    //gets layout
    PangoLayout* get_layout(void) const { return dm_layout; }
    
   private:
    PangoContext* create_pango_context(void);
};

/**
 * a canvas/graphics context multiplatform implementation
 *
 * @author Aleksander Demko
 * @author Shantha Ramachandran
 */ 
class scopira::coreui::canvas
{
  protected:
    GdkDrawable *dm_drawable;        /// drawable
    GdkColormap *dm_cmap;            /// the colour map
    int dm_width;
    int dm_height;
    
  public:
    /// ctor
    SCOPIRAUI_EXPORT explicit canvas(GdkDrawable *drawable, GdkColormap *cmap, int w, int h);
    
    /// width
    int width(void) const { return dm_width; }
    /// height
    int height(void) const { return dm_height; }

    // inline these?

    void clear(context c)
      { draw_rectangle(c, true, 0, 0, dm_width, dm_height); }

    /// line
    SCOPIRAUI_EXPORT void draw_line(context c, int x, int y, int x2, int y2);
    /// line, via pt
    void draw_line(context c, point fromp, point top)
      { draw_line(c, fromp.x, fromp.y, top.x, top.y); }
    /// rectangle
    SCOPIRAUI_EXPORT void draw_rectangle(context c, bool filled, int x, int y, int w, int h);
    /// rectangle, via pt
    void draw_rectangle(context c, bool filled, point base, point sz)
      { draw_rectangle(c, filled, base.x, base.y, sz.x, sz.y); }

    /// polygon, via pts
    SCOPIRAUI_EXPORT void draw_polygon(context c, bool filled, std::vector<point> points);
    /// segments (unconnected lines.) via points
    SCOPIRAUI_EXPORT void draw_segments(context c, std::vector<point> points);

      /// draw point
    SCOPIRAUI_EXPORT void draw_point(context c, int x, int y);
    /// draw point< via pt
    void draw_point(context c, point p)
      { draw_point(c, p.x, p.y); }

    /// draw circle
    SCOPIRAUI_EXPORT void draw_circle(context c, bool filled, int x, int y, int w, int h);
    /// draw circle< via pt
    void draw_circle(context c, bool filled, point p, point sz)
      { draw_circle(c, filled, p.x, p.y, sz.x, sz.y); }
      
    /// writes text (x,y specify top left corner)
    SCOPIRAUI_EXPORT void draw_text(context c, font &f, int x, int y, const std::string &s);
    /// draw draw_text via pt
    void draw_text(context c, font &f, point p, const std::string &s)
      { draw_text(c, f, p.x, p.y, s); }
    /// writes text, RIGHT justified (ie. x,y specify top right corner and a width)
    SCOPIRAUI_EXPORT void draw_text_right(context c, font &f, int x, int y, int width, const std::string &s);
    /// draw draw_text_right< via pt
    void draw_text_right(context c, font &f, point p, int width, const std::string &s)
      { draw_text_right(c, f, p.x, p.y, width, s); }
      
    /// blits one canvas to another
    SCOPIRAUI_EXPORT void draw_canvas(context c, int xsrc, int ysrc, canvas &src, int xdest, int ydest, int w, int h);
    /// full copy
    SCOPIRAUI_EXPORT void draw_canvas(context c, canvas &src);
    SCOPIRAUI_EXPORT void draw_image(context c, int xsrc, int ysrc, rgb_image &src, int xdest, int ydest, int w, int h);
    SCOPIRAUI_EXPORT void draw_scaled_image(context c, rgb_image &src, int xdest, int ydest, int w, int h);
    SCOPIRAUI_EXPORT void draw_scaled_image(context c, int xsrc, int ysrc, int wsrc, int hsrc, rgb_image &src,
        int xdest, int ydest, int wdest, int hdest);

    /// allocates and prepares a color_vec for this canvas
    SCOPIRAUI_EXPORT void map_colors(color_vec &cvec);
    /// unallocates these colors
    SCOPIRAUI_EXPORT void unmap_colors(color_vec &cvec);

  private:
    // disabled, not implemented:
    canvas(const canvas&);
    canvas & operator=(const canvas&);
};

/**
 * a context that will make a gc based on the given widget.
 * it will do additional managment on the gc
 *
 * @author Aleksander Demko
 */
class scopira::coreui::widget_context : public scopira::coreui::context
{
  public:
    /// default ctor, call set after!
    SCOPIRAUI_EXPORT widget_context(void);
    /// dtor
    SCOPIRAUI_EXPORT ~widget_context();
    /// initing ctor
    SCOPIRAUI_EXPORT explicit widget_context(GtkWidget *w);
    /// initing ctor
    SCOPIRAUI_EXPORT explicit widget_context(widget *w);
    /// initing ctor
    SCOPIRAUI_EXPORT explicit widget_context(widget_canvas &wc);

    /// sets by widget
    SCOPIRAUI_EXPORT void set_gtk_widget(GtkWidget *w);
    /// sets by widget
    SCOPIRAUI_EXPORT void set_widget(widget *w);

  private:
    // disabled, not implemented:
    widget_context(const widget_context&);
    widget_context & operator=(const widget_context&);
};

/**
 * a canvas, thats based on a widget (offers some default
 * contexts)
 *
 * @author Aleksander Demko
 */
class scopira::coreui::widget_canvas : public scopira::coreui::canvas
{
  private:
    GtkWidget *dm_widget;
  public:
    SCOPIRAUI_EXPORT widget_canvas(void);
    SCOPIRAUI_EXPORT explicit widget_canvas(widget *w);
    SCOPIRAUI_EXPORT explicit widget_canvas(GtkWidget *w);

    /// sets by widget
    SCOPIRAUI_EXPORT void set_widget(widget *w);
    /// gets the associated widget
    GtkWidget * get_gtk_widget(void) const { return dm_widget; }

    SCOPIRAUI_EXPORT context white_context(void);
    SCOPIRAUI_EXPORT context black_context(void);
    SCOPIRAUI_EXPORT context background_context(void);
};

/**
 * a context wired to a pixmap
 *
 * @author Aleksander Demko
 */ 
class scopira::coreui::pixmap_context : public scopira::coreui::context
{
  public:
    /// ctor
    SCOPIRAUI_EXPORT pixmap_context(void);
    /// ctor + set_pixmap call
    SCOPIRAUI_EXPORT explicit pixmap_context(pixmap_canvas &pcan);
    /// dtor
    SCOPIRAUI_EXPORT ~pixmap_context();

    /// associates the given pixmap, you must call this
    SCOPIRAUI_EXPORT void set_pixmap(pixmap_canvas &pcan);
};

/**
 * a canvas based on a pixmap
 *
 * @author Aleksander Demko
 */
class scopira::coreui::pixmap_canvas : public scopira::coreui::canvas
{
  private:
    GtkWidget *dm_widget;
    GdkPixmap *dm_pixmap;
  public:
    /// ctor
    SCOPIRAUI_EXPORT pixmap_canvas(void);
    /// ctor
    SCOPIRAUI_EXPORT explicit pixmap_canvas(widget *wid, int w, int h);
    /// dtor
    SCOPIRAUI_EXPORT ~pixmap_canvas();

    /// resize
    SCOPIRAUI_EXPORT void resize(GtkWidget *wid, int w, int h);
    /// resize
    void resize(widget *wid, int w, int h) { resize(wid->get_widget(), w, h); }

    friend class pixmap_context;
};

/**
 * a vector of colours, mostly for palettes
 *
 * @author Aleksander Demko
 */ 
class scopira::coreui::color_vec : public scopira::tool::basic_array<GdkColor>
{
  public:
    enum {
      black_c = 0,
      white_c = 0xFFFFFF,
      red_c = 0xFF0000,
      green_c = 0x00FF00,
      blue_c = 0x0000FF
    };

    /// ctor
    SCOPIRAUI_EXPORT explicit color_vec(size_t len);

    /// sets by rgb
    SCOPIRAUI_EXPORT void set_rgb(size_t i, int r, int g, int b);

    /// set as 0xRRGGBB
    SCOPIRAUI_EXPORT void set_rgb(size_t i, int pcol);
    /// get as 0xRRGGBB
    SCOPIRAUI_EXPORT int get_rgb(size_t i) const;

    /// set colors, suitable for picking
    SCOPIRAUI_EXPORT void set_pick(size_t i, size_t len);
    /// set a gradient
    SCOPIRAUI_EXPORT void set_gradient(size_t i, size_t len, int startpcol, int endpcol);

    // packing routine
    static int rgb(int r, int g, int b) {
      return (r<<16) | (g<<8) | b;
    }
    static int red(int packedcol) {
      return (packedcol&0xFF0000)>>16;
    }
    static int green(int packedcol) {
      return (packedcol&0x00FF00)>>8;
    }
    static int blue(int packedcol) {
      return (packedcol&0x0000FF);
    }

    // copy from an array of packed cols (template for various reasons, in particular cuz i wont want to refer to basekit from coreui)
    template <class L> void copy(const L &src);
};
template <class L> void scopira::coreui::color_vec::copy(const L &src)
{
  size_t mx = src.size();
  if (mx>size())
    mx = size();
  for (size_t ii=0; ii<mx; ++ii)
    set_rgb(ii, src[ii]);
}

/**
 * RGB images, useful for back buffers and the like
 *
 * @author Aleksander Demko
 */ 
class scopira::coreui::rgb_image : protected scopira::tool::basic_array<unsigned char>
{
  public:
    struct color {
      unsigned char r, g, b;
      /// sets the rgb_image by "packed color"
      void set_by_packed(int packedcol) {
        r = (packedcol&0xFF0000)>>16;
        g = (packedcol&0x00FF00)>>8;
        b = (packedcol&0x0000FF);
      }
    };

  private:
    typedef scopira::tool::basic_array<unsigned char> parent_type;

    int dm_width, dm_height;
    GdkPixbuf *dm_buff;

  public:
    /// ctor
    SCOPIRAUI_EXPORT rgb_image(void);
    /// dtor
    SCOPIRAUI_EXPORT virtual ~rgb_image();

    /// resizer
    SCOPIRAUI_EXPORT void resize(int w, int h);
    /// sets all to one colour
    SCOPIRAUI_EXPORT void set_all_black(void);

    /// width
    int width(void) const { return dm_width; }
    /// height
    int height(void) const { return dm_height; }

    const color & operator()(size_t x, size_t y) const {
      assert( (x<dm_width) && (y<dm_height) );
      return *reinterpret_cast<color*>(dm_ary+((x+y*dm_width)*sizeof(color)));
    }
    /// [] ref
    color & operator()(size_t x, size_t y) {
      assert( (x<dm_width) && (y<dm_height) );
      return *reinterpret_cast<color*>(dm_ary+((x+y*dm_width)*sizeof(color)));
    }
    
    GdkPixbuf *get_pixbuf(void) { return dm_buff; }

    friend class canvas;

  private:
    // disabled, not implemented:
    rgb_image(const rgb_image&);
    rgb_image & operator=(const rgb_image&);
};

/**
 * a zooming widget
 *
 * @author Aleksander Demko
 */ 
class scopira::coreui::zoom_widget : public scopira::coreui::widget
{
  private:
    GtkAdjustment *dm_adj;
    GtkWidget *dm_bar;

  public:
    /// ctor
    SCOPIRAUI_EXPORT zoom_widget(void);
    /// explicitly set the zoom factor
    SCOPIRAUI_EXPORT void set_factor(int newfactor);
    /// get adjustment, see cpp for value ranges
    GtkAdjustment * get_adjustment(void) { return dm_adj; }
    ///get the zoom bar
    GtkWidget * get_hscale(void) { return dm_bar; } 
    
  private:
    /// inits gui
    void init_gui(void);
};

#endif

