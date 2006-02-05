
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

#include <scopira/coreui/canvascommon.h>

#include <string>

#include <gtk/gtk.h>

//BBlibs scopira
//BBtargets libscopiraui.so

using namespace scopira::coreui;

GdkColor scopira::coreui::packed_to_gdkcolor(int packedcolour)
{
  GdkColor col;

  col.red = (packedcolour & 0xFF0000) >> 8;
  col.green = (packedcolour & 0x00FF00);
  col.blue = (packedcolour & 0x0000FF) << 8;
  col.pixel = 0;

  return col;
}

int scopira::coreui::gdkcolor_to_packed(GdkColor c)
{
  return
    ((c.red & 0xFF00) << 8) |
    (c.green & 0xFF00) |
    ((c.blue & 0xFF00) >> 8);
}

//constructor
font::font(void)
{
  dm_font = pango_font_description_from_string("Sans 10");
  assert(dm_font);
}

//constructor with font type
font::font(const char *str)
{
  dm_font = pango_font_description_from_string(str);
  assert(dm_font);
}
//diconstructor  
font::~font(void)
{
  assert(dm_font);
  pango_font_description_free(dm_font);
}

void font::set_size(int size)
{
	// pango font description deals with size relating to the PANGO_SCALE which is 1024
	// to set size from points we multiply by PANGO_SCALE
	pango_font_description_set_size(dm_font, size*PANGO_SCALE);
}

int font::get_size()
{
	int size = 0;
	// pango font description deals with size relating to the PANGO_SCALE which is 1024
	// to get size in points we divide by PANGO_SCALE
	size = pango_font_description_get_size(dm_font) / PANGO_SCALE;
	return size;
}
  
void context::set_foreground(GdkColor &col)
{
  assert(dm_gc);
  gdk_gc_set_foreground(dm_gc, &col);
}

void context::set_foreground(int packedcolour)
{
  GdkColor col;

  assert(dm_gc);

  col.red = (packedcolour & 0xFF0000) >> 8;
  col.green = (packedcolour & 0x00FF00);
  col.blue = (packedcolour & 0x0000FF) << 8;

  gdk_gc_set_rgb_fg_color(dm_gc, &col);
}

void context::set_line_attributes(int line_width, GdkLineStyle line_style, 
  GdkCapStyle cap_style, GdkJoinStyle join_style)
{
  gdk_gc_set_line_attributes(dm_gc, line_width, line_style, cap_style, join_style);   
}

layout::layout(void)
{
  dm_context = create_pango_context();
  assert(dm_context);
  dm_layout =  pango_layout_new(dm_context);
  assert(dm_layout);
}

layout::~layout(void)
{
  assert(dm_layout);
  g_object_unref(G_OBJECT(dm_layout));
  assert(dm_layout);
  g_object_unref(G_OBJECT(dm_context));
}
    
PangoContext* layout::create_pango_context(void)
{
  PangoContext *context;
  
  context = gdk_pango_context_get();
  pango_context_set_language(context, gtk_get_default_language());

  return context;
}


canvas::canvas(GdkDrawable *drawable, GdkColormap *cmap, int w, int h)
  : dm_drawable(drawable), dm_cmap(cmap), dm_width(w), dm_height(h)
{
}

void canvas::draw_line(context c, int x, int y, int x2, int y2)
{
  assert(c.dm_gc);
  gdk_draw_line(dm_drawable, c.dm_gc, x, y, x2, y2);
}

void canvas::draw_rectangle(context c, bool filled, int x, int y, int w, int h)
{
  assert(c.dm_gc);
  gdk_draw_rectangle(dm_drawable, c.dm_gc, filled?TRUE:FALSE, x, y, w, h);
}

void canvas::draw_polygon(context c, bool filled, std::vector<point> points)
{
  GdkPoint *gdk_points;
  int i;
  assert(c.dm_gc);
  
  gdk_points = new GdkPoint[points.size()];
  for(i=0; i<points.size(); i++) {
    gdk_points[i].x = points[i].x;
    gdk_points[i].y = points[i].y;
  }
  
  gdk_draw_polygon(dm_drawable,c.dm_gc,filled?TRUE:FALSE,gdk_points,points.size());
}

void canvas::draw_segments(context c, std::vector<point> points)
{
  GdkSegment *segs;
  int i, j, nsegs;
  assert(c.dm_gc);
  
  nsegs = points.size()/2;
  segs = new GdkSegment[nsegs];
  for(i=0,j=0; i<points.size(); i+=2, j++) {
    segs[j].x1 = points[i].x;
    segs[j].y1 = points[i].y;
    segs[j].x2 = points[i+1].x;
    segs[j].y2 = points[i+1].y;
  }
  
  gdk_draw_segments(dm_drawable, c.dm_gc, segs, nsegs);
}


void canvas::draw_point(context c, int x, int y)
{
  assert(c.dm_gc);
  gdk_draw_point(dm_drawable, c.dm_gc, x, y);
}

void canvas::draw_circle(context c, bool filled, int x, int y, int w, int h)
{
  assert(c.dm_gc);
  gdk_draw_arc(dm_drawable, c.dm_gc, filled?TRUE:FALSE, x, y, w, h, 0, 64*360);
}

void canvas::draw_text(context c, font &f, int x, int y, const std::string &s)
{
  assert(c.dm_gc);
  assert(f.dm_font);
  layout dm_layout;
  
  //set font
  pango_layout_set_font_description(dm_layout.get_layout(), f.dm_font);
  //set text in layout
  pango_layout_set_text(dm_layout.get_layout(), s.c_str(), -1);

  gdk_draw_layout(dm_drawable, c.dm_gc, x, y, dm_layout.get_layout()); 
}

void canvas::draw_text_right(context c, font &f, int x, int y, int width, const std::string &s)
{
  assert(c.dm_gc);
  assert(f.dm_font);
  layout dm_layout;  
  int text_width;
 
  //set font
  pango_layout_set_font_description(dm_layout.get_layout(), f.dm_font);
  pango_layout_set_wrap(dm_layout.get_layout(), PANGO_WRAP_WORD_CHAR);
   
  //set text in layout
  pango_layout_set_text(dm_layout.get_layout(), s.c_str(), -1);
  //get the string's pixel width
  pango_layout_get_pixel_size(dm_layout.get_layout(), &text_width, 0);
  
  //set justification for overflowing text.  For some reason, right justified 
  //  means left justified for overflows in pango 1.4.  Numerals might be
  //  handled differently from letters, too.
  pango_layout_set_alignment(dm_layout.get_layout(), PANGO_ALIGN_RIGHT);
  //set width to which the lines of the PangoLayout should be wrapped, only if 
  //  there's an overflow.  Otherwise don't wrap
  if ( text_width > width ) {
    pango_layout_set_width(dm_layout.get_layout(), width * PANGO_SCALE);
    gdk_draw_layout(dm_drawable, c.dm_gc, x - width, y, dm_layout.get_layout());
  }
  else {
    pango_layout_set_width(dm_layout.get_layout(), -1);
    gdk_draw_layout(dm_drawable, c.dm_gc, x - text_width, y, dm_layout.get_layout());
  }
}

void canvas::draw_canvas(context c, int xsrc, int ysrc, canvas &src,
  int xdest, int ydest, int w, int h)
{
  assert(dm_drawable);
  assert(c.dm_gc);
  assert(src.dm_drawable);
  gdk_draw_drawable(dm_drawable, c.dm_gc, src.dm_drawable, xsrc, ysrc, xdest, ydest, w,h);
}

void canvas::draw_canvas(context c, canvas &src)
{
  assert(dm_drawable);
  assert(c.dm_gc);
  assert(src.dm_drawable);
  assert(width() == src.width());
  assert(height() == src.height());
  gdk_draw_drawable(dm_drawable, c.dm_gc, src.dm_drawable, 0, 0, 0, 0, width(), height());
}

void canvas::draw_image(context c, int xsrc, int ysrc , rgb_image &src,
  int xdest, int ydest, int w, int h)
{
  assert(c.dm_gc);
  gdk_pixbuf_render_to_drawable(src.dm_buff, dm_drawable, c.dm_gc,
    xsrc, ysrc, xdest, ydest, w, h, GDK_RGB_DITHER_NONE, 0, 0);
}

void canvas::draw_scaled_image(context c, rgb_image &src,
  int xdest, int ydest, int w, int h)
{
  GdkPixbuf	*scaled_img;
  
  assert(c.dm_gc);
  assert(src.dm_buff);
  assert(dm_drawable);
  
  scaled_img = gdk_pixbuf_scale_simple(src.dm_buff, w, h, GDK_INTERP_BILINEAR);
  assert(scaled_img);
  gdk_pixbuf_ref(scaled_img);
  gdk_pixbuf_render_to_drawable(scaled_img, dm_drawable, c.dm_gc,
    0, 0, xdest, ydest, w, h, GDK_RGB_DITHER_NONE, 0, 0);

  gdk_pixbuf_unref(scaled_img);
}
                                             
void canvas::draw_scaled_image(context c, int xsrc, int ysrc, int wsrc, int hsrc, rgb_image &src,
    int xdest, int ydest, int wdest, int hdest)
{
  GdkPixbuf	*subsrc, *scaled_img;
  
  assert(c.dm_gc);
  assert(src.dm_buff);
  assert(dm_drawable);
  
  assert(xsrc>=0);
  assert(ysrc>=0);
  assert(wsrc>0);
  assert(hsrc>0);
  assert(xdest>=0);
  assert(ydest>=0);
  assert(wdest>0);
  assert(hdest>0);
  subsrc = gdk_pixbuf_new_subpixbuf(src.dm_buff, xsrc, ysrc, wsrc, hsrc);
  assert(subsrc);
  gdk_pixbuf_ref(subsrc);

  scaled_img = gdk_pixbuf_scale_simple(subsrc, wdest, hdest, GDK_INTERP_BILINEAR);
  assert(scaled_img);
  gdk_pixbuf_ref(scaled_img);

  gdk_pixbuf_render_to_drawable(scaled_img, dm_drawable, c.dm_gc,
    0, 0, xdest, ydest, wdest, hdest, GDK_RGB_DITHER_NONE, 0, 0);

  gdk_pixbuf_unref(scaled_img);
}

void canvas::map_colors(color_vec &cvec)
{
  tool::basic_array<gboolean> out(cvec.size());

  assert(!cvec.empty());
  // check return value of this call???
  gdk_colormap_alloc_colors(dm_cmap, cvec.array(), cvec.size(), FALSE, TRUE, out.array());
}

void canvas::unmap_colors(color_vec &cvec)
{
  tool::basic_array<gboolean> out(cvec.size());

  assert(!cvec.empty());
  // check return value of this call???
  gdk_colormap_free_colors(dm_cmap, cvec.array(), cvec.size());
}

widget_context::widget_context(void)
{
}

widget_context::widget_context(widget *w)
{
  assert(w);
  set_widget(w);
}

widget_context::widget_context(GtkWidget *w)
{
  assert(w);
  set_gtk_widget(w);
}

widget_context::widget_context(widget_canvas &wc)
{
  set_gtk_widget(wc.get_gtk_widget());
}

widget_context::~widget_context()
{
  set_widget(0);
}

void widget_context::set_gtk_widget(GtkWidget *w)
{
  GdkGC *newgc;

  if (w) {
    newgc = gdk_gc_new(w->window);
    // should we add a ref here? sure.
    assert(newgc);
    g_object_ref(newgc);
  } else
    newgc = 0;

  if (dm_gc)
    g_object_unref(dm_gc);

  dm_gc = newgc;
}

void widget_context::set_widget(widget *w)
{
  if (w)
    set_gtk_widget(w->get_widget());
  else
    set_gtk_widget(0);
}

widget_canvas::widget_canvas(void)
  : canvas(0, 0, 0, 0), dm_widget(0)
{
}

widget_canvas::widget_canvas(widget *w)
  : canvas(w->get_widget()->window, gtk_widget_get_colormap(w->get_widget()),
    w->get_widget()->allocation.width, w->get_widget()->allocation.height), dm_widget(w->get_widget())
{
  assert(dm_widget);
  assert(dm_widget->window);
}

widget_canvas::widget_canvas(GtkWidget *w)
  : canvas(w->window, gtk_widget_get_colormap(w), w->allocation.width, w->allocation.height), dm_widget(w)
{
  assert(dm_widget);
  assert(dm_widget->window);
}

void widget_canvas::set_widget(widget *w)
{
  dm_widget = w->get_widget();

  dm_drawable = dm_widget->window;
  dm_cmap = gtk_widget_get_colormap(dm_widget);
  dm_width = w->get_widget()->allocation.width;
  dm_height = w->get_widget()->allocation.height;

  assert(dm_widget);
  assert(dm_widget->window);
}

context widget_canvas::white_context(void)
{
  assert(dm_widget);
  assert(GTK_IS_WIDGET(dm_widget));
  return context(dm_widget->style->white_gc);
}

context widget_canvas::black_context(void)
{
  assert(dm_widget);
  assert(GTK_IS_WIDGET(dm_widget));
  return context(dm_widget->style->black_gc);
}

context widget_canvas::background_context(void)
{
  assert(dm_widget);
  assert(GTK_IS_WIDGET(dm_widget));
  return context(dm_widget->style->bg_gc[GTK_STATE_NORMAL]);
}

pixmap_context::pixmap_context(void)
{
}

pixmap_context::~pixmap_context()
{
  if (dm_gc)
    g_object_unref(dm_gc);
}

void pixmap_context::set_pixmap(pixmap_canvas &pcan)
{
  if (dm_gc)
    g_object_unref(dm_gc);
  dm_gc = gdk_gc_new(pcan.dm_drawable);
}

pixmap_canvas::pixmap_canvas(void)
  : canvas(0, 0, 0, 0), dm_widget(0), dm_pixmap(0)
{
}

pixmap_canvas::pixmap_canvas(widget *wid, int w, int h)
  : canvas(0, gtk_widget_get_colormap(wid->get_widget()), w, h),
    dm_widget(wid->get_widget())
{
  assert(dm_widget);
  assert(dm_widget->window);
  assert(w>0);
  assert(h>0);
  dm_pixmap = gdk_pixmap_new(dm_widget->window, w, h, -1);
  assert(dm_pixmap);
  dm_drawable = dm_pixmap;
}

pixmap_canvas::~pixmap_canvas()
{
  if (dm_pixmap)
    g_object_unref(dm_pixmap);
}

void pixmap_canvas::resize(GtkWidget *wid, int w, int h)
{
  if (dm_pixmap)
    g_object_unref(dm_pixmap);
  if (wid && w!=0 && h!=0) {
		dm_widget = wid;
		dm_width = w;
		dm_height = h;

    dm_cmap = gtk_widget_get_colormap(dm_widget);
    dm_pixmap = gdk_pixmap_new(dm_widget->window, w, h, -1);
    assert(dm_pixmap);
    dm_drawable = dm_pixmap;
  } else {
    dm_pixmap = 0;
    dm_drawable = 0;
    dm_width = 0;
    dm_height = 0;
  }
}

color_vec::color_vec(size_t len)
  : scopira::tool::basic_array<GdkColor>(len)
{
  assert(dm_len>0);
}

void color_vec::set_rgb(size_t i, int r, int g, int b)
{
  assert(i<dm_len);
  dm_ary[i].red = r << 8;
  dm_ary[i].green = g << 8;
  dm_ary[i].blue = b << 8;
}

void color_vec::set_rgb(size_t i, int col)
{
  assert(i<dm_len);
  dm_ary[i].red = (col & 0xFF0000) >> 8;
  dm_ary[i].green = (col & 0x00FF00);
  dm_ary[i].blue = (col & 0x0000FF) << 8;
}

int color_vec::get_rgb(size_t i) const
{
  assert(i<dm_len);
  return ((dm_ary[i].red & 0xFF00) << 8) |
    ((dm_ary[i].green & 0xFF00) | ((dm_ary[i].blue & 0xFF00) >> 8));
}

void color_vec::set_pick(size_t i, size_t len)
{
  int x, endx, *p;
  static int loop[] =
      //Rays's colors
      {0xC42141, 0x2D3796, 0x00FFFF, 0x00FF00,     //red, dark blue, cyan ,green
       //0x000000, 0xBBBBBB, 0x000000, 0xBBBBBB,//black and white
       //0xFF7700, 0xFF00FF, 0x00FFFF, 0x00FF00, //orange, purple, blue,green
       0xFFFF00, 0xFF00FF, 0xFFFF77, 0xFF77FF, 0xFF7700, -1}; //yellow, magenta, light yellow, orange
       //Old colors
       /*
       {0xFF7700, 0xFF00FF, 0x00FFFF, 0x00FF00,
       0xFFFF00, 0xFF00FF, 0xFFFF77, 0xFF77FF, 0x77FFFF, -1};
       0x0000FF, 0x00FF00,
       0xFFFF77, 0xFF77FF, 0x77FFFF, 0xFF7700, 0x7700FF, 0x00FF77,
       0x77FF00, 0xFF0077, 0x0077FF, 0x770000, 0x000077, 0x007700, -1};
	*/                                             

  assert(len>0);

  p = loop;
  endx = i+len;
  for (x=i; x<endx; ++x) {
    if (*p == -1)
      p = loop;
    set_rgb(x, *p);
    p++;
  }
}

void color_vec::set_gradient(size_t i, size_t len, int startpcol, int endpcol)
{
  int x, endx, r, g, b;
  double rr, rg, rb;

  assert(len>0);

  // use gdk's 16bit precision for a better gradient
  // use int math instead?

  r = red(startpcol) << 8;
  g = green(startpcol) << 8;
  b = blue(startpcol) << 8;
  rr = ((red(endpcol)<<8) - static_cast<double>(r)) / len;
  rg = ((green(endpcol)<<8) - static_cast<double>(g)) / len;
  rb = ((blue(endpcol)<<8) - static_cast<double>(b)) / len;

  endx = i+len;
  for (x=i; x<endx; ++x) {
    (*this)[x].red = static_cast<int>(r+(x-i)*rr);
    (*this)[x].green = static_cast<int>(g+(x-i)*rg);
    (*this)[x].blue = static_cast<int>(b+(x-i)*rb);
  }
}


//
// rgb_image
//

rgb_image::rgb_image(void)
{
  dm_height = dm_width = 0;
  dm_buff = 0;
}

rgb_image::~rgb_image()
{
  if (dm_buff)
    gdk_pixbuf_unref(dm_buff);
}

void rgb_image::resize(int w, int h)
{
  if (dm_buff)
    gdk_pixbuf_unref(dm_buff);
  dm_buff = 0;
  dm_width = w;
  dm_height = h;
  parent_type::resize(dm_width * dm_height * sizeof(color));

  if (dm_width == 0 && dm_height == 0)
    return;
    
  if (dm_width > 0 && dm_height > 0) {
    dm_buff = gdk_pixbuf_new_from_data(dm_ary, GDK_COLORSPACE_RGB,
      FALSE, 8, dm_width, dm_height, dm_width*sizeof(color), 0, 0);
    assert(dm_buff);
  }
  
  assert(GDK_IS_PIXBUF(dm_buff));
}

void rgb_image::set_all_black(void)
{
  parent_type::clear_zero();
}


//
// zoom_widget
//

zoom_widget::zoom_widget(void)
{
  init_gui();
}

void zoom_widget::set_factor(int newfactor)
{
  gtk_adjustment_set_value(GTK_ADJUSTMENT(dm_adj), 20.0 * newfactor/1000 - 10);
}

void zoom_widget::init_gui(void)
{
  GtkWidget *la;
  
  dm_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0, -10, 10, 1, 1, 0));
  dm_bar = gtk_hscale_new(dm_adj);
  la = gtk_label_new("Zoomer:");
  
  dm_widget = gtk_hbox_new(FALSE, 2);
  gtk_box_pack_start(GTK_BOX(dm_widget), la, FALSE, TRUE, 2);
  gtk_widget_show(la);
  gtk_box_pack_start(GTK_BOX(dm_widget), dm_bar, TRUE, TRUE, 2);
  gtk_widget_show(dm_bar);

  widget::init_gui();
}

