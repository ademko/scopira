
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

#include <scopira/coregl/glwidget.h>

#include <math.h>

#include <scopira/tool/platform.h>
#include <scopira/tool/flow.h>
#include <scopira/tool/output.h>
#include <scopira/basekit/color.h>
#include <scopira/basekit/vectormath.h>

#include <gtk/gtk.h>

//BBlibs scopiraui gtkglext-1.0
//BBtargets libscopiragl.so

using namespace scopira::tool;
using namespace scopira::coreui;
using namespace scopira::coregl;

//
//
// glfont
//
//


glfont::glfont(void)
  : dm_inited(false)
{
}

glfont::~glfont()
{
  //assert(!dm_inited);
}

void glfont::init(void)
{
  init("Courier 10");
}

void glfont::init(const std::string &fontname)
{
  PangoFontDescription *font_desc;
  PangoFont *font;
  PangoFontMetrics *font_metrics;

  assert(!dm_inited);
  dm_inited = true;

  dm_list_base = glGenLists(num_lists_c);

  font_desc = pango_font_description_from_string(fontname.c_str());
  font = gdk_gl_font_use_pango_font(font_desc, 0, num_lists_c, dm_list_base);
  assert("[GL font loading trouble]" && font);

  font_metrics = pango_font_get_metrics(font, 0);
  dm_font_height = PANGO_PIXELS(pango_font_metrics_get_ascent(font_metrics) +
      pango_font_metrics_get_descent(font_metrics));
  pango_font_metrics_unref(font_metrics);

  pango_font_description_free(font_desc);
}

void glfont::close(void)
{
  assert(dm_inited);
  dm_inited = false;

  glDeleteLists(dm_list_base, num_lists_c);
}

void glfont::render_string(const std::string &v)
{
  assert(dm_inited);
  glListBase(dm_list_base);
  glCallLists(v.size(), GL_UNSIGNED_BYTE, v.c_str());
}

void glfont::render_string(const char *c)
{
  assert(dm_inited);
  if (!c)
    return;
  glListBase(dm_list_base);
  glCallLists(::strlen(c), GL_UNSIGNED_BYTE, c);
}

//
//
// glwidget
//
//

// constructor for glview class
glwidget::glwidget(void)
{
  dm_glconfig = 0;
  dm_glw = 0;
  dm_perspective = true;
  dm_camera_scale = 1;
  dm_basecmd = 0;
  dm_inited = false;
  dm_width = dm_height = 0;
  dm_dirty = true;

  dm_mousedown = false;
  dm_selectdown = false;
}

glwidget::~glwidget()
{
  assert(dm_glconfig); /// no idea who deletes this TODO FIXME
}

widget * glwidget::make_drawing_area(bool adddefaultbuts)
{
  return new scopira::coreui::widget(make_drawing_area_impl(adddefaultbuts));
}

GtkWidget * glwidget::make_drawing_area_impl(bool adddefaultbuts)
{
  gboolean b;
  GtkWidget *butbox=0, *retwid;
  GtkWidget *but1, *but2;

  dm_glconfig = gdk_gl_config_new_by_mode(static_cast<GdkGLConfigMode>(GDK_GL_MODE_RGB | GDK_GL_MODE_DEPTH | GDK_GL_MODE_DOUBLE));
  assert(dm_glconfig);

  dm_glw = gtk_drawing_area_new();
  gtk_widget_set_size_request(dm_glw, 300, 200);

  b = gtk_widget_set_gl_capability(dm_glw, dm_glconfig, 0, TRUE, GDK_GL_RGBA_TYPE);
  assert(b);

  gtk_widget_add_events(dm_glw, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_BUTTON_MOTION_MASK);

  g_signal_connect(G_OBJECT(dm_glw), "realize", G_CALLBACK(h_realize), this);
  g_signal_connect(G_OBJECT(dm_glw), "configure_event", G_CALLBACK(h_configure_event), this);
  g_signal_connect(G_OBJECT(dm_glw), "expose_event", G_CALLBACK(h_expose_event), this);
  g_signal_connect(G_OBJECT(dm_glw), "button_press_event", G_CALLBACK(h_mouse), this);
  g_signal_connect(G_OBJECT(dm_glw), "button_release_event", G_CALLBACK(h_mouse), this);
  g_signal_connect(G_OBJECT(dm_glw), "motion_notify_event", G_CALLBACK(h_mouse_motion), this);
  //g_signal_connect(G_OBJECT(dm_glw), "unrealize", G_CALLBACK(h_destroy), this);

  if (adddefaultbuts) {
    butbox = gtk_hbox_new(FALSE,0);
    
    //create button1
    but1 = gtk_button_new_with_label("Perspective/Orthographic");
    g_signal_connect(G_OBJECT(but1), "clicked", G_CALLBACK(h_but_per), this);
    gtk_box_pack_start(GTK_BOX(butbox), but1, TRUE, TRUE, 0);
    gtk_widget_show(but1);

    //create button2
    but2 = gtk_button_new_with_label("Home Display");
    g_signal_connect(G_OBJECT(but2), "clicked", G_CALLBACK(h_but_home), this);
    gtk_box_pack_start(GTK_BOX(butbox), but2, TRUE, TRUE, 0);
    gtk_widget_show(but2);
  }

  retwid = gtk_vbox_new(FALSE,2);

  gtk_container_set_border_width(GTK_CONTAINER(retwid), 2);

	gtk_box_pack_start(GTK_BOX(retwid), dm_glw, TRUE, TRUE, 0);
  if (adddefaultbuts)
    gtk_box_pack_start(GTK_BOX(retwid), butbox, FALSE, TRUE, 0);
	
	gtk_widget_show(dm_glw);
  if (adddefaultbuts)
    gtk_widget_show(butbox);

  assert(retwid);

  return retwid;
}

void glwidget::init_gui(bool adddefaultbuts)
{
  widget::init_gui(make_drawing_area_impl(adddefaultbuts));
}

void glwidget::setup_camera(int selx, int sely)
{
  double ratio = static_cast<double>(dm_width)/dm_height;

  if (dm_perspective) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glFrustum(-1.000, 1.000, -0.6, 0.6, 1.05, 30);
    //glFrustum(-10.000, 10.000, -6, 6, 0, 3);
    if (selx>=0) {
      fixed_array<int, 4> vp;
      glGetIntegerv(GL_VIEWPORT, vp.c_array());
      gluPickMatrix(selx, vp[3] - sely, 12, 12, vp.c_array());
    }
    glFrustum(-ratio/2, ratio/2, -0.5, 0.5, 5, 100);
    if (selx<0)
      glViewport(0, 0, dm_width, dm_height);
    glMatrixMode(GL_MODELVIEW);
    // load the current xform matrix
    glLoadIdentity();
    glTranslated(0, 0, camera_offset_c);
    glScaled(dm_camera_scale, dm_camera_scale, dm_camera_scale);
    glMultMatrixd(dm_xform.c_array());
    //glLoadIdentity();
    /*glTranslatef(0, 0, -2.5);
    glRotatef(35, 1, 0, 0);
    glTranslatef(0, 0, 1.0);*/
  } else {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (selx>=0) {
      fixed_array<int, 4> vp;
      glGetIntegerv(GL_VIEWPORT, vp.c_array());
      gluPickMatrix(selx, vp[3] - sely, 12, 12, vp.c_array());
    }
    //glOrtho(-6.000, 6.000, -4.61, 4.61, 1.05, 30);
    glOrtho(-6.000, 6.000, -4.61, 4.61, 0.05, 100);
    if (selx<0)
      glViewport(0, 0, dm_width, dm_height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(0, 0, camera_offset_c);
    glScaled(dm_camera_scale, dm_camera_scale, dm_camera_scale);
    glMultMatrixd(dm_xform.c_array());
  }
}

void glwidget::handle_init(void)
{
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glEnable(GL_DEPTH_TEST);
  //glClearDepth (1.0);

  // init dm_xform to the identity
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glGetDoublev(GL_MODELVIEW_MATRIX, dm_xform.c_array());
}

void glwidget::handle_close(void)
{
}

void glwidget::handle_resize(int newwidth, int newheight)
{
  setup_camera();
}

void glwidget::handle_render(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glLoadIdentity();

  // axis
  glPushMatrix();
  glLoadIdentity();
  glTranslated(0, 0, camera_offset_c);

  // look at point
  glColor3d(0, 1, 0);
  glBegin(GL_POINTS);
  glVertex3d(0, 0, 0);
  glEnd();

  glPopMatrix();

  // axis
  glColor3d(0, 1, 0);
  glBegin(GL_LINES);
  glVertex3d(0, 0, 0);
  glVertex3d(3, 0, 0);
  glVertex3d(0, 0, 0);
  glVertex3d(0, 2, 0);
  glVertex3d(0, 0, 0);
  glVertex3d(0, 0, 1);
  glEnd();
  /*glBegin(GL_TRIANGLES);
  glVertex3d(0, 0, 0);
  glVertex3d(0.2, 0, 0);
  glVertex3d(0, 0.2, 0);
  glEnd();*/
}

void glwidget::handle_press(char button, int x, int y)
{
  //fixed_array<int, 4> vp;
  fixed_array<GLuint, 4096> hitlist;
  GLint numhits;

  // draw scene here
  //glGetIntegerv(GL_VIEWPORT, vp.c_array());
  glSelectBuffer(hitlist.size(), hitlist.c_array());
  glRenderMode(GL_SELECT);

  setup_camera(x, y);

  glInitNames();
  //glPushName(0);

  dm_selectdown = true;
  glPushMatrix();
  handle_render();
  glPopMatrix();
  dm_selectdown = false;

  setup_camera();

  glFlush();

  numhits = glRenderMode(GL_RENDER);
  //OUTPUT << "HIT LIST SIZE = " << numhits << '\n';
  error_flush();

  if (numhits > 0) {
    bool sendhandler = false;
    basic_array<int> hitstack;
    GLuint *idx = hitlist.c_array();
    int rec;

    for (rec=0; rec<numhits; ++rec) {
      if (*idx == 0)
        idx += 3;   // skip this empty rec
      else {
        // we found a rec with >0 names... this is what we want

        assert(*idx > 0);
        hitstack.resize(*idx);

        // skip over to the names
        idx += 3;

        // read in the stack
        for (int j=0; j<hitstack.size(); ++j) {
          hitstack[j] = *idx;
          ++idx;
        }

        glPushMatrix();
        handle_select(numhits, hitstack);
        glPopMatrix();

        sendhandler = true;
        break;  // stop this loop, no need to continue
      }
    }//for

    if (!sendhandler) {
      // we didnt find a non 0-length stack... we still
      // must send a emtpy hitstack handle_select event,
      // just incase the descendant wants those
      basic_array<int> hitstack;

      glPushMatrix();
      handle_select(numhits, hitstack);
      glPopMatrix();
    }
  }//if
}

void glwidget::handle_select(int numhits, const basic_array<int> &hitstack)
{
  if (!hitstack.empty()) {
    OUTPUT << "glwidget::handle_select(): " << numhits << " hits, stack: ";
    for (int x=0; x<hitstack.size(); ++x)
      OUTPUT << ' ' << hitstack[x];
    OUTPUT << '\n';
  }
}

void glwidget::request_render(void)
{
  // crafty
  //h_expose_event(dm_glw, 0, this);
  gtk_widget_queue_draw(dm_glw);
}

bool glwidget::error_flush(void)
{
  GLenum er = glGetError();

  if (er == GL_NO_ERROR)
    return false;

  while (er != GL_NO_ERROR) {
    OUTPUT << " !!! GL Error: " << er << '\n';
    er = glGetError();
  }
  return true;
}

void glwidget::save_dirty_buffer(GtkWidget *widget)
{
  // have they asked to save the screen?
  if (!dm_dirty) {
    //OUTPUT << "gl.render.save\n";
    widget_canvas wc(widget);
    if (dm_dirty_buffer.width() != widget->allocation.width || dm_dirty_buffer.height() != widget->allocation.height)
      dm_dirty_buffer.resize(widget, widget->allocation.width, widget->allocation.height);
    dm_dirty_buffer.draw_canvas(wc.black_context(), wc);
  }
}

double glwidget::adjust_camera(double oldcamera, double pixdelta)
{
  double newcamera;

  newcamera = oldcamera + 0.005 * pixdelta;
  if (newcamera <= 0)
    return 0.02;

  newcamera = ::sqrt(newcamera);
  newcamera *= newcamera;

  if (newcamera < 0.02)
    return 0.02;

  return newcamera;
}

void glwidget::h_realize(GtkWidget *widget, gpointer data)
{
  glwidget *here = reinterpret_cast<glwidget*>(data);

  if (here->dm_inited)
    return; // no need to do it again

  here->dm_inited = true;

  // some stuff taken from http://cvs.sourceforge.net/cgi-bin/viewcvs.cgi/gtkglext/gtkglext/examples/logo.c?rev=HEAD&content-type=text/vnd.viewcvs-markup
  gl_lock LK(here);

  assert(here);
  //OUTPUT << "gl.init.begin\n";
  here->handle_init();
}

gboolean glwidget::h_configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
  glwidget *here = reinterpret_cast<glwidget*>(data);
  gl_lock LK(here);

  //GLfloat w = widget->allocation.width;
  //GLfloat h = widget->allocation.height;
  //GLfloat aspect;

  here->dm_width = static_cast<int>(widget->allocation.width);
  here->dm_height = static_cast<int>(widget->allocation.height);
  here->dm_dirty = true;

  assert(here);
  if (!here->dm_inited) {
    here->dm_inited = true;
    //OUTPUT << "gl.init.begin\n";
    here->handle_init();
  }
  //OUTPUT << "gl.resize.begin\n";

  here->handle_resize(here->dm_width, here->dm_height);

  return TRUE;
}

gboolean glwidget::h_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  glwidget *here = reinterpret_cast<glwidget*>(data);

  // can we use the saved screen?
  if (!here->dm_dirty) {
    //OUTPUT << "gl.render.cache\n";
    widget_canvas wc(widget);
    assert(wc.width() == here->dm_dirty_buffer.width());
    assert(wc.height() == here->dm_dirty_buffer.height());
    wc.draw_canvas(wc.black_context(), here->dm_dirty_buffer);
    return TRUE;
  }

  // GL zone
  {
    gl_lock LK(here);

    assert(here);
    glPushMatrix();
    //OUTPUT << "gl.render.begin\n";
    here->handle_render();
    //OUTPUT << "gl.render.end\n";
    glPopMatrix();

    LK.flush();
  }

  here->save_dirty_buffer(widget);

  return TRUE;
}

gboolean glwidget::h_mouse(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  glwidget *here = reinterpret_cast<glwidget*>(data);

  assert(event);
  if (event->type == GDK_BUTTON_PRESS) {
    here->dm_basex = event->x;
    here->dm_basey = event->y;
    switch (event->button) {
      case 1:
        here->dm_basecmd = 1;
        break;
      case 2:
        here->dm_basecmd = 2;
        break;
      case 3:
        here->dm_basecmd = 3;
        break;
    }
    here->dm_basecmd += 100;

  } else if (event->type == GDK_BUTTON_RELEASE && here->dm_basecmd > 100) {
    // there where no motions! this is doing to be a mouse click event
    gl_lock LK(here);

    glPushMatrix();
    here->handle_press(here->dm_basecmd - 101, static_cast<int>(here->dm_basex),
        static_cast<int>(here->dm_basey));
    glPopMatrix();

    LK.flush();
  } else if (event->type == GDK_BUTTON_RELEASE) {
    here->dm_dirty = true;

    // finalize the current transform
    gl_lock LK(here);

    if (here->dm_basecmd == 3)
      here->dm_camera_scale = adjust_camera(here->dm_camera_scale, here->dm_basey - event->y);

    glLoadIdentity();
    switch (here->dm_basecmd) {
      case 1:
        glRotated(-0.5*(here->dm_basex - event->x), 0, 1, 0);
        glRotated(-0.5*(here->dm_basey - event->y), 1, 0, 0);
        break;
      case 2:
        glTranslated(-0.001*(here->dm_basex - event->x), 0.001*(here->dm_basey - event->y), 0);
        break;
    }
    glMultMatrixd(here->dm_xform.c_array());
    // save it for future use
    glGetDoublev(GL_MODELVIEW_MATRIX, here->dm_xform.c_array());

    // reapply everything now
    glLoadIdentity();
    glTranslated(0, 0, camera_offset_c);
    glScaled(here->dm_camera_scale, here->dm_camera_scale, here->dm_camera_scale);
    glMultMatrixd(here->dm_xform.c_array());

    glPushMatrix();
    here->handle_render();      // i can call this here, since I'm kinda doing what h_expose_event does
    glPopMatrix();

    LK.flush();

    here->save_dirty_buffer(widget);
  }

  return TRUE;
}

gboolean glwidget::h_mouse_motion(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
  glwidget *here = reinterpret_cast<glwidget*>(data);

  if (here->dm_basecmd == 0)
    return TRUE;

  if (here->dm_basecmd > 100)
    here->dm_basecmd -= 100;    // lob off the motion thinggie

  assert(event);

#ifndef PLATFORM_win32
  // this makes motion events MUCH faster
  // hopefully wont cause any weirdness though
  if (gdk_events_pending())
    return TRUE;
#endif

  gl_lock LK(here);
  glPushMatrix();
  glLoadIdentity();
  glTranslated(0, 0, camera_offset_c);
  if (here->dm_basecmd == 3) {
    double newscale = adjust_camera(here->dm_camera_scale, here->dm_basey - event->y);
    glScaled(newscale, newscale, newscale);
  } else
    glScaled(here->dm_camera_scale, here->dm_camera_scale, here->dm_camera_scale);
  switch (here->dm_basecmd) {
    // rotation
    case 1:
      glRotated(-0.5*(here->dm_basex - event->x), 0, 1, 0);
      glRotated(-0.5*(here->dm_basey - event->y), 1, 0, 0);
      break;
    case 2:
      glTranslated(-0.001*(here->dm_basex - event->x), 0.001*(here->dm_basey - event->y), 0);
      break;
  }
  glMultMatrixd(here->dm_xform.c_array());
  here->dm_mousedown = true;
  here->handle_render();      // i can call this here, since I'm kinda doing what h_expose_event does
  here->dm_mousedown = false;
  glPopMatrix();
  LK.flush();

  return TRUE;
}

/*void glwidget::h_destroy(GtkWidget *widget, gpointer data)
{
  glwidget *here = reinterpret_cast<glwidget*>(data);

  assert(here->is_alive_object());

  if (here->dm_inited) {
    gl_lock LK(here);

    OUTPUT << "gl.close.begin\n";
    here->handle_close();
    here->dm_inited = false;
  }
}*/

// event handler for but1
void glwidget::h_but_per(GtkWidget *widget, gpointer data)
{
  glwidget *here = reinterpret_cast<glwidget*>(data);

  here->dm_perspective = !here->dm_perspective;

  {
    gl_lock LK(here);
    here->setup_camera();
  }

  //gtk_widget_queue_draw(here->dm_glw);
  here->dm_dirty = true;
  here->request_render();
}

// event handler for but2
void glwidget::h_but_home(GtkWidget *widget, gpointer data)
{
  glwidget *here = reinterpret_cast<glwidget*>(data);

  here->dm_camera_scale = 1;

  {
    gl_lock LK(here);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glGetDoublev(GL_MODELVIEW_MATRIX, here->dm_xform.c_array());
    glTranslated(0, 0, camera_offset_c);
    glScaled(here->dm_camera_scale, here->dm_camera_scale, here->dm_camera_scale);
  }

  //gtk_widget_queue_draw(here->dm_glw);
  here->dm_dirty = true;
  here->request_render();
}

