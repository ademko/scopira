
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

#ifndef _INCLUDED__PAKIT_UI_PATTERNEDIT_H__
#define _INCLUDED__PAKIT_UI_PATTERNEDIT_H__

#include <scopira/core/view.h>
#include <scopira/uikit/matrix.h>
#include <pakit/ui/models.h>

namespace pakit
{
  class pattern_editor_v;
}

/**
 * Editor of patterns
 * @author Aleksander Demko
 */
class pakit::pattern_editor_v : public scopira::uikit::matrix_editor_base,
  public virtual scopira::core::view_i,
  public scopira::coreui::button_constants,
  public virtual scopira::coreui::button_reactor_i
{
  private:
    typedef matrix_editor_base parent_type;

  public:
    /// ctor
    pattern_editor_v(void);

    virtual int get_data_width(void) const;
    virtual int get_data_height(void) const;
    virtual void get_data_text(int x, int y, std::string &out) const;
    virtual void get_data_stat(std::string &out) const;
    virtual void get_data_header(int x, std::string &out) const;

    virtual bool get_data_double(int x, int y, double &out) const;
    virtual bool get_data_string(int x, int y, std::string &out) const;
    virtual void prepare_undo(void);
    virtual void undo(void);

    virtual void bind_model(scopira::core::model_i *sus);
    virtual void react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src);
    virtual void react_button(scopira::coreui::button *source, int actionid);

  private:
    // matrix has the following form:
    // titles, training, class, data
    // height == data.height, the other 3 vectors may have a size<=data.height

    enum {
      basex_c = 3,
    };
    scopira::basekit::stringvector_o dm_titles;
    scopira::basekit::narray_o<bool,1> dm_training;
    scopira::basekit::narray_o<int,1> dm_classes;
    scopira::basekit::narray_o<double,2> dm_feat;

    // Centroid map and associated functions
    std::map<int,int> centroid_map;
    void build_centroid_map(void);
    void adjust_centroids(parm_t &p, int row);
    void compute_overall_centroid();
    void compute_centroid(int cent_class);

    scopira::core::model_ptr<pakit::patterns_m> dm_model;

    static cmd_t dm_commands[];

  private:
    /// setsup the menus
    void init_menu(popup_menu &b);

    static void cmd_sel_row(parm_t &p);
    static void cmd_sel_col(parm_t &p);

    static void cmd_set_one(parm_t &p);
    static void cmd_set(parm_t &p);

    static void cmd_collabel(parm_t &p);
    static void cmd_fliptrain(parm_t &p);
    static void cmd_coltrain(parm_t &p);
    static void cmd_colclass(parm_t &p);
    static void cmd_features(parm_t &p);

    static void cmd_labelfill(parm_t &p);

    static void cmd_resize(parm_t &p);
    static void cmd_crop(parm_t &p);
    static void cmd_delcols(parm_t &p);
    static void cmd_delrows(parm_t &p);
    static void cmd_delrow(parm_t &p);
};


#endif

