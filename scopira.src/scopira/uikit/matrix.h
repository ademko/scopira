
/*
 *  Copyright (c) 2005-2007    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_UIKIT_MATRIX_H__
#define __INCLUDED_SCOPIRA_UIKIT_MATRIX_H__

#include <scopira/coreui/entry.h>
#include <scopira/coreui/canvas.h>
#include <scopira/coreui/viewwidget.h>
#include <scopira/coreui/export.h>
#include <scopira/basekit/narray.h>
#include <scopira/basekit/stringvector.h>

#include <scopira/uikit/models.h>

//
//
// This is the ported over (from Scopira v0) "Matrix Master" excel-like
// editor. It should be modernized or perhaps replaced by something
// more general and UI-comfortable.
//
// This code was hastely ported over from the old code. It shouldn't
// be used as any kind of guide to new code.
//

namespace scopira
{
  namespace uikit
  {
    class matrix_viewer_base;

    class basic_matrix_data_i;
    class basic_matrix_viewer_v;

    class stringvec_viewer_v;
    class vec_viewer_v;
    class int_vec_viewer_v;
    class matrix_viewer_v;
    class float_matrix_viewer_v;
    class char_matrix_viewer_v;
    class int_matrix_viewer_v;

    class matrix_editor_base;
    //class vector_editor_v;
    class matrix_editor_v;

    //class load_matrix_v;
    //class save_matrix_v;
  }
}

//
// TODO this should all be converted to templates
//

/**
 * Base class for all proponents that display matrix-like data
 * @author Aleksander Demko
 */
class scopira::uikit::matrix_viewer_base : public scopira::coreui::scrolled_canvas_base,
  public virtual scopira::core::view_i
{
  protected:

    enum {
      textcol_c = 0x0,
      backcol_c = 0xFFFFFF,
      // measurements
      basex_c = 30,
      basey_c = 45,
      titley_c = 10,
      cellw_c = 100,
      cellh_c = 20,
    };

    scopira::coreui::font dm_font;

    std::string dm_titleline;

  public:
    /// handle resize
    SCOPIRAUI_EXPORT virtual void handle_resize(scopira::coreui::widget_canvas &v, coord &crd);
    /// repaint handler
    SCOPIRAUI_EXPORT virtual void handle_repaint(scopira::coreui::widget_canvas &v, coord &crd);
    /// handle scroll
    SCOPIRAUI_EXPORT virtual void handle_scroll(scopira::coreui::widget_canvas &v, coord &crd);

    /// is the data empty?
    bool is_data_empty(void) const { return get_data_width() == 0 || get_data_height() == 0; }
    /// descedants provide this
    SCOPIRAUI_EXPORT virtual int get_data_width(void) const = 0;
    /// descedants provide this
    SCOPIRAUI_EXPORT virtual int get_data_height(void) const = 0;
    /// descedants provide this
    SCOPIRAUI_EXPORT virtual void get_data_text(int x, int y, std::string &out) const = 0;
    /// descedants provide this. default one is provided
    SCOPIRAUI_EXPORT virtual void get_data_stat(std::string &out) const;
    /// gets the header for the column. default implementation is numerical
    SCOPIRAUI_EXPORT virtual void get_data_header(int x, std::string &out) const;
    /// gets the header for the row. default implementation is numerical
    SCOPIRAUI_EXPORT virtual void get_data_side(int y, std::string &out) const;

  protected:
    /**
     * Protected ctor. Remember, decendants must call init_gui.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT matrix_viewer_base(void);
    /**
      * decendant classes may override this.
      * this paints one single cell
      * @author Aleksander Demko
      */
    SCOPIRAUI_EXPORT virtual void paint_cell(scopira::coreui::widget_canvas &v, scopira::coreui::widget_context &pen,
        int cellx, int celly, int datax, int datay);

    // called after new data comes in... sets the titleline
    SCOPIRAUI_EXPORT void prep_display(void);

    SCOPIRAUI_EXPORT void convert_mouse_to_xy(scopira::coreui::widget_canvas &v,
        coord &crd, const scopira::coreui::mouse_event &mevt, int &outx, int &outy,
        bool *xok = 0, bool *yok = 0);

  private:
    void paint(scopira::coreui::widget_canvas &v, coord &crd);
};

/**
 * This is a pure data model class that is used by basic_matrix_viewer_v.
 * Basically, basic_matrix_viewer_v is the view, and this is the model.
 * Descendants must decent from this (which operates on its own data type,
 * or on the model provided).
 *
 * @author Aleksander Demko
 */ 
class scopira::uikit::basic_matrix_data_i : public virtual scopira::tool::object
{
  public:
    // scopira::core::view_i like functions

    SCOPIRAUI_EXPORT virtual void bind_model(scopira::core::model_i *sus) { }
    SCOPIRAUI_EXPORT virtual void react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src) { }

    // basic_matrix_viewer_v like funciotns

    SCOPIRAUI_EXPORT virtual int get_data_width(void) const = 0;
    SCOPIRAUI_EXPORT virtual int get_data_height(void) const = 0;
    SCOPIRAUI_EXPORT virtual void get_data_text(int x, int y, std::string &out) const = 0;
    SCOPIRAUI_EXPORT virtual void get_data_stat(std::string &out) const { out.clear(); }
    // default just converts the x to out via int_to_string
    SCOPIRAUI_EXPORT virtual void get_data_header(int x, std::string &out) const;
    SCOPIRAUI_EXPORT virtual void get_data_side(int y, std::string &out) const;

  protected:
    SCOPIRAUI_EXPORT basic_matrix_data_i(void);

};

/**
 * A generic matrix viewer that offloads the virtual functions to the "model" like
 * basic_matrix_data_i class.
 *
 * @author Aleksander Demko
 */ 
class scopira::uikit::basic_matrix_viewer_v : public scopira::uikit::matrix_viewer_base
{
  public:
    /// data cannot be null. will be ref counted
    SCOPIRAUI_EXPORT basic_matrix_viewer_v(basic_matrix_data_i *data);

    SCOPIRAUI_EXPORT virtual void bind_model(scopira::core::model_i *sus) { dm_datamodel->bind_model(sus); }
    SCOPIRAUI_EXPORT virtual void react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src) { dm_datamodel->react_model_update(sus, src); }

    SCOPIRAUI_EXPORT virtual int get_data_width(void) const { return dm_datamodel->get_data_width(); }
    SCOPIRAUI_EXPORT virtual int get_data_height(void) const { return dm_datamodel->get_data_height(); }
    SCOPIRAUI_EXPORT virtual void get_data_text(int x, int y, std::string &out) const { dm_datamodel->get_data_text(x, y, out); }
    SCOPIRAUI_EXPORT virtual void get_data_stat(std::string &out) const { dm_datamodel->get_data_stat(out); }
    SCOPIRAUI_EXPORT virtual void get_data_header(int x, std::string &out) const { dm_datamodel->get_data_header(x, out); }
    SCOPIRAUI_EXPORT virtual void get_data_side(int y, std::string &out) const { dm_datamodel->get_data_side(y, out); }

  private:
    scopira::tool::count_ptr<basic_matrix_data_i> dm_datamodel;
};

/**
 * viewer of string vecs
 * @author Aleksander Demko
 */
class scopira::uikit::stringvec_viewer_v : public scopira::uikit::matrix_viewer_base
{
  protected:
    scopira::tool::count_ptr<scopira::basekit::stringvector_o> dm_data;

  public:
    /// ctor
    SCOPIRAUI_EXPORT stringvec_viewer_v(void);

    /// descedants provide this
    SCOPIRAUI_EXPORT virtual int get_data_width(void) const;
    /// descedants provide this
    SCOPIRAUI_EXPORT virtual int get_data_height(void) const;
    /// descedants provide this
    SCOPIRAUI_EXPORT virtual void get_data_text(int x, int y, std::string &out) const;
    SCOPIRAUI_EXPORT /// descedants provide this
    virtual void get_data_stat(std::string &out) const;
};


/**
 * viewer of double vectors
 * @author Aleksander Demko
 */
class scopira::uikit::vec_viewer_v : public scopira::uikit::matrix_viewer_base
{
  protected:
    scopira::tool::count_ptr<scopira::basekit::narray_o<double,1> > dm_data;
    scopira::core::model_ptr<scopira::uikit::narray_m<double,1> > dm_model;

  public:
    /// ctor
    SCOPIRAUI_EXPORT vec_viewer_v(void);

    SCOPIRAUI_EXPORT virtual void set_vector(scopira::basekit::narray_o<double,1>  *indata);

    SCOPIRAUI_EXPORT virtual void bind_model(scopira::core::model_i *sus);
    SCOPIRAUI_EXPORT virtual void react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src);

    /// descedants provide this
    SCOPIRAUI_EXPORT virtual int get_data_width(void) const;
    /// descedants provide this
    SCOPIRAUI_EXPORT virtual int get_data_height(void) const;
    /// descedants provide this
    SCOPIRAUI_EXPORT virtual void get_data_text(int x, int y, std::string &out) const;
    /// descedants provide this
    SCOPIRAUI_EXPORT virtual void get_data_stat(std::string &out) const;
};

/**
 * viewer of int vectors
 * @author Aleksander Demko
 */
class scopira::uikit::int_vec_viewer_v : public scopira::uikit::matrix_viewer_base
{
  protected:
    scopira::tool::count_ptr<scopira::basekit::narray_o<int,1> > dm_data;
    scopira::core::model_ptr<scopira::uikit::narray_m<int,1> > dm_model;

  public:
    /// ctor
    SCOPIRAUI_EXPORT int_vec_viewer_v(void);

    SCOPIRAUI_EXPORT virtual void bind_model(scopira::core::model_i *sus);
    SCOPIRAUI_EXPORT virtual void react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src);

    /// descedants provide this
    SCOPIRAUI_EXPORT virtual int get_data_width(void) const;
    /// descedants provide this
    SCOPIRAUI_EXPORT virtual int get_data_height(void) const;
    /// descedants provide this
    SCOPIRAUI_EXPORT virtual void get_data_text(int x, int y, std::string &out) const;
    /// descedants provide this
    SCOPIRAUI_EXPORT virtual void get_data_stat(std::string &out) const;
};


/**
 * viewer of double matrices
 * @author Aleksander Demko
 */
class scopira::uikit::matrix_viewer_v : public scopira::uikit::matrix_viewer_base
{
  protected:
    scopira::tool::count_ptr<scopira::basekit::narray_o<double,2> > dm_data;
    scopira::core::model_ptr<scopira::uikit::narray_m<double,2> > dm_model;

  public:
    /// ctor
    SCOPIRAUI_EXPORT matrix_viewer_v(void);

    SCOPIRAUI_EXPORT virtual void set_matrix(scopira::basekit::narray_o<double,2> *indata);

    SCOPIRAUI_EXPORT virtual void bind_model(scopira::core::model_i *sus);
    SCOPIRAUI_EXPORT virtual void react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src);

    /// descedants provide this
    SCOPIRAUI_EXPORT virtual int get_data_width(void) const;
    /// descedants provide this
    SCOPIRAUI_EXPORT virtual int get_data_height(void) const;
    /// descedants provide this
    SCOPIRAUI_EXPORT virtual void get_data_text(int x, int y, std::string &out) const;
    /// descedants provide this
    SCOPIRAUI_EXPORT virtual void get_data_stat(std::string &out) const;
};

/**
 * Viewer of INT matrices.
 * Perhaps this should be unified with the double one?
 * @author Aleksander Demko
 */
class scopira::uikit::float_matrix_viewer_v : public scopira::uikit::matrix_viewer_base
{
  protected:
    scopira::tool::count_ptr<scopira::basekit::narray_o<float,2> > dm_data;
    scopira::core::model_ptr<scopira::uikit::narray_m<float,2> > dm_model;

  public:
    /// ctor
    SCOPIRAUI_EXPORT float_matrix_viewer_v(void);

    SCOPIRAUI_EXPORT virtual void set_matrix(scopira::basekit::narray_o<float,2> *indata);

    SCOPIRAUI_EXPORT virtual void bind_model(scopira::core::model_i *sus);
    SCOPIRAUI_EXPORT virtual void react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src);

    /// descedants provide this
    SCOPIRAUI_EXPORT virtual int get_data_width(void) const;
    /// descedants provide this
    SCOPIRAUI_EXPORT virtual int get_data_height(void) const;
    /// descedants provide this
    SCOPIRAUI_EXPORT virtual void get_data_text(int x, int y, std::string &out) const;
    /// descedants provide this
    SCOPIRAUI_EXPORT virtual void get_data_stat(std::string &out) const;
};

/**
 * Viewer of INT matrices.
 * Perhaps this should be unified with the double one?
 * @author Aleksander Demko
 */
class scopira::uikit::char_matrix_viewer_v : public scopira::uikit::matrix_viewer_base
{
  protected:
    scopira::tool::count_ptr<scopira::basekit::narray_o<char,2> > dm_data;
    scopira::core::model_ptr<scopira::uikit::narray_m<char,2> > dm_model;

  public:
    /// ctor
    SCOPIRAUI_EXPORT char_matrix_viewer_v(void);

    SCOPIRAUI_EXPORT virtual void set_matrix(scopira::basekit::narray_o<char,2> *indata);

    SCOPIRAUI_EXPORT virtual void bind_model(scopira::core::model_i *sus);
    SCOPIRAUI_EXPORT virtual void react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src);

    /// descedants provide this
    SCOPIRAUI_EXPORT virtual int get_data_width(void) const;
    /// descedants provide this
    SCOPIRAUI_EXPORT virtual int get_data_height(void) const;
    /// descedants provide this
    SCOPIRAUI_EXPORT virtual void get_data_text(int x, int y, std::string &out) const;
    /// descedants provide this
    SCOPIRAUI_EXPORT virtual void get_data_stat(std::string &out) const;
};

/**
 * Viewer of INT matrices.
 * Perhaps this should be unified with the double one?
 * @author Aleksander Demko
 */
class scopira::uikit::int_matrix_viewer_v : public scopira::uikit::matrix_viewer_base
{
  protected:
    scopira::tool::count_ptr<scopira::basekit::narray_o<int,2> > dm_data;
    scopira::core::model_ptr<scopira::uikit::narray_m<int,2> > dm_model;

  public:
    /// ctor
    SCOPIRAUI_EXPORT int_matrix_viewer_v(void);

    SCOPIRAUI_EXPORT virtual void set_matrix(scopira::basekit::narray_o<int,2> *indata);

    SCOPIRAUI_EXPORT virtual void bind_model(scopira::core::model_i *sus);
    SCOPIRAUI_EXPORT virtual void react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src);

    /// descedants provide this
    SCOPIRAUI_EXPORT virtual int get_data_width(void) const;
    /// descedants provide this
    SCOPIRAUI_EXPORT virtual int get_data_height(void) const;
    /// descedants provide this
    SCOPIRAUI_EXPORT virtual void get_data_text(int x, int y, std::string &out) const;
    /// descedants provide this
    SCOPIRAUI_EXPORT virtual void get_data_stat(std::string &out) const;
};

/**
 * base for all matrix-list editors
 * @author Aleksander Demko
 */
class scopira::uikit::matrix_editor_base : public scopira::uikit::matrix_viewer_base,
  public virtual scopira::core::view_i,
  public virtual scopira::coreui::entry_reactor_i,
  public virtual scopira::coreui::button_reactor_i
{
  private:
    typedef matrix_viewer_base parent_type;

  public:
    SCOPIRAUI_EXPORT virtual void handle_menu(scopira::coreui::widget_canvas &v, coord &crd, intptr_t menuid);
    SCOPIRAUI_EXPORT virtual void handle_press(scopira::coreui::widget_canvas &v, coord &crd, const scopira::coreui::mouse_event &mevt);

    SCOPIRAUI_EXPORT virtual void react_entry(scopira::coreui::entry *source, const char *msg);
    SCOPIRAUI_EXPORT virtual void react_button(scopira::coreui::button *source, int actionid) { }  // descendants provide the meat of this

    /// called to get an double representation. true if this makes sense for
    /// this cell
    SCOPIRAUI_EXPORT virtual bool get_data_double(int x, int y, double &out) const { return false; }
    /// gets the data as a string
    SCOPIRAUI_EXPORT virtual bool get_data_string(int x, int y, std::string &out) const { return false; }

    /// called when descendants should back up their current data to their undo
    /// stack
    /// descedants should call this one after they do their stuff
    SCOPIRAUI_EXPORT virtual void prepare_undo(void);
    /// called when a descendant should performan an undo operation.
    /// descedants should call this one after they do their stuff
    SCOPIRAUI_EXPORT virtual void undo(void);

    SCOPIRAUI_EXPORT void do_execute(void);

    // calls prep_display and does a redraw
    SCOPIRAUI_EXPORT void resync(bool newsize);

  protected:
    /// a parser token
    class tok_t
    {
      public:
        typedef scopira::tool::fixed_array<char, 80> name_t;
        name_t name;
        bool isdouble;
        bool isint;
        double dvalue;
        int ivalue;

        const char * dequote_c_array(void) const {
          const char *ret = name.c_array();
          if (*ret == '"')
            return ++ret;
          return ret;
        }
    };
    /// parameter.
    class parm_t
    {
      public:
        enum sel_t { sel_set_c, sel_add_c, sel_sub_c, sel_inter_c };
        enum op_t { op_set_c, op_add_c, op_sub_c, op_mult_c, op_div_c, op_idiv_c, op_imod_c };
      public:
        tok_t *token;
        sel_t selmode;    // reset by merge_sel
        op_t opmode;      // reset by reset_op
        bool changed_sel; // set by merge_sel
        const char *error;
        matrix_editor_base *here_base;      // the this object

        template <class L>
          L * get_here(void) const {
            assert(here_base);
            assert(here_base->is_alive_object());
            assert(dynamic_cast<L*>(here_base)->is_alive_object());
            return dynamic_cast<L*>(here_base);
          }

        SCOPIRAUI_EXPORT void merge_sel(const scopira::basekit::narray_o<bool,2> &newsel);

        SCOPIRAUI_EXPORT void apply_op(double src, double &out);
        SCOPIRAUI_EXPORT void apply_op(int src, int &out);

        void reset_op(void) { opmode = op_set_c; }
    };

    class parser; // used internally in the cpp file

    struct cmd_t {
      const char *keyword;
      int num_parm;
      void (*func)(parm_t &p);
    };

    /// ctor
    SCOPIRAUI_EXPORT matrix_editor_base(cmd_t *commands, matrix_editor_base *here);

    SCOPIRAUI_EXPORT virtual void paint_cell(scopira::coreui::widget_canvas &v, scopira::coreui::widget_context &pen,
        int cellx, int celly, int datax, int datay);

    SCOPIRAUI_EXPORT void init_gui(void);

    /// descendants should call this at the end of their set_data
    SCOPIRAUI_EXPORT void prep_data(void);

  private:
    /// initer
    /// menu handler
    static void h_menu(GtkWidget *widget, const char *cmd);

    /// called by view
    void handle_mouse_rect(bool immediate, int x, int y, int w, int h);

    /// clears the cursor
    void clear_cursor(void) { set_cursor(-1, -1); }
    /// reset cursor, called on dm_sel mods
    void reset_cursor(void);
    /// increment cursor to the next valid location
    void inc_cursor(void);
    /// sets the cursor location
    void set_cursor(int x, int y);
    /// loads the current cursot to edit box
    void edit_cursor(void);
    /// is the current cursor valid?
    bool valid_cursor(void) const { return dm_xcur>=0 && dm_xcur<dm_sel.width() && dm_ycur>=0 && dm_ycur<dm_sel.height(); }

  protected:
    // data
    /// command list
    cmd_t *dm_commands;
    matrix_editor_base *dm_here;
    /// selection matrix
    scopira::basekit::narray_o<bool,2> dm_sel;
    /// undo info for the selection matrix
    scopira::basekit::narray<bool,2> dm_undosel;
    /// cursor location, -1 for none
    int dm_xcur, dm_ycur;

    // gui data
    // h is for "highlight" (yeah, im stretchin' here)
    enum {
      h_sel_col_c = 0xDDDD00,  //yello (darker) // highlet/selected colour
      h_mark_col_c = 0xFFFF00,  //yello // mouse mark colour
      h_cur_col_c = 0xB8C9FB,  //blue // current cursor colour
    };

    scopira::tool::count_ptr<scopira::coreui::entry> dm_edit;
    popup_menu dm_menu;

    /// used in 2-hop region selection
    int dm_xsel, dm_ysel;

    /// core widget, as its not trivial
    scopira::tool::count_ptr<scopira::coreui::widget> dm_basewidget;

  protected:
    SCOPIRAUI_EXPORT static void cmd_mod_add(parm_t &p);
    SCOPIRAUI_EXPORT static void cmd_mod_sub(parm_t &p);
    SCOPIRAUI_EXPORT static void cmd_mod_mul(parm_t &p);
    SCOPIRAUI_EXPORT static void cmd_mod_div(parm_t &p);
    SCOPIRAUI_EXPORT static void cmd_mod_idiv(parm_t &p);
    SCOPIRAUI_EXPORT static void cmd_mod_imod(parm_t &p);

    SCOPIRAUI_EXPORT static void cmd_sel_add(parm_t &p);
    SCOPIRAUI_EXPORT static void cmd_sel_sub(parm_t &p);
    SCOPIRAUI_EXPORT static void cmd_sel_inter(parm_t &p);

    SCOPIRAUI_EXPORT static void cmd_sel(parm_t &p);
    SCOPIRAUI_EXPORT static void cmd_sel_rect(parm_t &p);
    SCOPIRAUI_EXPORT static void cmd_sel_invert(parm_t &p);
    SCOPIRAUI_EXPORT static void cmd_sel_all(parm_t &p);
    SCOPIRAUI_EXPORT static void cmd_sel_none(parm_t &p);

    SCOPIRAUI_EXPORT static void cmd_equals(parm_t &p);
    SCOPIRAUI_EXPORT static void cmd_notequals(parm_t &p);
    SCOPIRAUI_EXPORT static void cmd_greaterthan(parm_t &p);
    SCOPIRAUI_EXPORT static void cmd_greaterthan_equal(parm_t &p);
    SCOPIRAUI_EXPORT static void cmd_lessthan(parm_t &p);
    SCOPIRAUI_EXPORT static void cmd_lessthan_equal(parm_t &p);

    SCOPIRAUI_EXPORT static void cmd_expandcols(parm_t &p);
    SCOPIRAUI_EXPORT static void cmd_expandrows(parm_t &p);
    SCOPIRAUI_EXPORT static void cmd_expandrect(parm_t &p);
};

/**
 * editor of double matricies
 * @author Aleksander Demko
 */
class scopira::uikit::matrix_editor_v : public scopira::uikit::matrix_editor_base
{
  private:
    typedef matrix_editor_base parent_type;

  public:
    /// ctor
    SCOPIRAUI_EXPORT matrix_editor_v(void);

    SCOPIRAUI_EXPORT virtual void bind_model(scopira::core::model_i *sus);
    SCOPIRAUI_EXPORT virtual void react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src);
    SCOPIRAUI_EXPORT virtual void react_button(scopira::coreui::button *source, int actionid);

    SCOPIRAUI_EXPORT virtual int get_data_width(void) const;
    SCOPIRAUI_EXPORT virtual int get_data_height(void) const;
    SCOPIRAUI_EXPORT virtual void get_data_text(int x, int y, std::string &out) const;
    SCOPIRAUI_EXPORT virtual void get_data_stat(std::string &out) const;

    SCOPIRAUI_EXPORT virtual bool get_data_double(int x, int y, double &out) const;
    SCOPIRAUI_EXPORT virtual void prepare_undo(void);
    SCOPIRAUI_EXPORT virtual void undo(void);

  private:
    /// main data matrix
    scopira::basekit::narray_o<double,2> dm_data;
    /// clipboard
    scopira::basekit::narray<double> dm_clipboard;
    /// undo data
    scopira::basekit::narray<double,2> dm_undodata;
    /// the record_i, if any
    scopira::core::model_ptr<scopira::uikit::narray_m<double,2> > dm_model;

    static cmd_t dm_commands[];

  private:
    /// setsup the menus
    void init_menu(popup_menu &b);

    static void cmd_resize(parm_t &p);
    static void cmd_alter(parm_t &p);
    static void cmd_crop(parm_t &p);
    static void cmd_delcols(parm_t &p);
    static void cmd_delrows(parm_t &p);
    static void cmd_copy(parm_t &p);
    static void cmd_vcopy(parm_t &p);
    static void cmd_paste(parm_t &p);
    static void cmd_vpaste(parm_t &p);
    static void cmd_set_one(parm_t &p);
    static void cmd_set(parm_t &p);
    static void cmd_fill(parm_t &p);
    static void cmd_random(parm_t &p);
    static void cmd_int(parm_t &p);
    static void cmd_recip(parm_t &p);
    static void cmd_sign(parm_t &p);
    //static void cmd_sel_set(parm_t &p);
    static void cmd_trans(parm_t &p);
    static void cmd_diag(parm_t &p);
    static void cmd_udiag(parm_t &p);
    static void cmd_ldiag(parm_t &p);
    static void cmd_oddcols(parm_t &p);
    static void cmd_evencols(parm_t &p);
    static void cmd_oddrows(parm_t &p);
    static void cmd_evenrows(parm_t &p);
};

/**
 * loads a doublematrix from a text file
 * @author Shantha Ramachandran
 */
#ifdef OIJOIJOIJDE
class scopira::uikit::load_matrix_v : public scopira::coreui::applicator<scopira::uikit::narray_m<double,2> >
{
  private:
    scopira::tool::count_ptr<scopira::uikit::fileentry> dm_filename;

  public:
    /// ctro
    load_matrix_v(void);
    /// called on apply
    virtual void react_button(scopira::coreui::button *source, int actionid);
};

/**
 * saves a doublematrix to a text file
 * @author Shantha Ramachandran
 */
class scopira::uikit::save_matrix_v : public scopira::coreui::applicator<scopira::uikit::narray_m<double,2> >
{
  private:
    scopira::tool::count_ptr<scopira::uikit::fileentry_p> dm_filename;

  public:
    /// ctor
    save_matrix_v(void);
    /// called on apply
    virtual void react_button(scopira::coreui::button *source, int actionid);
};
#endif

#ifdef WANT_UIKIT_MATRIX_MACROS
// helper stuff for matrix

#define ASSERT_INT(tok, msg) \
  if (!(tok)->isint) { \
    p.error = (msg); \
    return; \
  }

#define ASSERT_DOUBLE(tok, msg) \
  if (!(tok)->isdouble) { \
    p.error = (msg); \
    return; \
  }

#define ASSERT_NONNEG_INT(tok, msg) \
  if ((tok)->ivalue < 0) { \
    p.error = (msg); \
      return; \
  }

#define ASSERT_POSITIVE_INT(tok, msg) \
  if ((tok)->ivalue <= 0) { \
    p.error = (msg); \
      return; \
  }

#endif

#endif

