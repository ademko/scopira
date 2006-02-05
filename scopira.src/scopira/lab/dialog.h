
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

#ifndef __INCLUDED_SCOPIRA_LAB_DIALOG_H__
#define __INCLUDED_SCOPIRA_LAB_DIALOG_H__

#include <scopira/coreui/export.h>
#include <scopira/lab/window.h>
#include <scopira/coreui/label.h>
#include <scopira/coreui/filewindow.h>

//BBtargets libscopiraui.so

namespace scopira
{
  namespace lab
  {
    // simple, one line label
    class msg_window;
    // ask the user for some text
    class query_window;
    // ask for a filename
    class filename_window;
    // a window that can be used to quickly close all other windows (useful to force a quit)
    class quit_window;
  }
};

/**
 * A simple lab widget that shows a status line, which the programmer may change.
 *
 * @author Aleksander Demko
 */
class scopira::lab::msg_window : public scopira::lab::lab_window
{
  private:
    GtkWidget *dm_label;

  public:
    /**
     * Constructor.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT msg_window(void);
    /**
     * Alternate constructor. You can specify additional buttons to
     * display in the dialog.
     *
     * @param buttons a | bitmask of button_*_c constants
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT msg_window(int buttons);

    /**
     * Sets the current diplayed status label.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT void set_msg(const std::string &label);

    /**
     * A nice helper/wrapper function.
     * Basically, this function will load, init, show the window in question, then
     * perhaps wait for some input from the user (if that makes sense for the window in question).
     *
     * The 2nd parameter may be "true" to cause this operation to wait until the
     * user closes the window.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT static void popup(const std::string &label, bool waitforreply = false);

    /**
     * This variant of popup waits for the user to press a button and returns
     * its action code. You must specify the bitmask of button_*_c you want.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT static int popup(const std::string &label, int buttons);

  protected:
    SCOPIRAUI_EXPORT void init_gui(int buttons);
};

/**
 * A lab widget that has a label and a text entry box. Useful for asking questions.
 *
 * Perhaps add checkers/different widget types in the future?
 *
 * @author Aleksander Demko
 */ 
class scopira::lab::query_window : public scopira::lab::lab_window
{
  private:
    GtkWidget *dm_label, *dm_entry;

  public:
    /**
     * Create a query window with the given buttons
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT query_window(int buttons = button_close_c);

    /**
     * Sets the msg in the prompt
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void set_question(const std::string &label);

    /**
     * Sets the entry box
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void set_answer(const std::string &txt);

    /**
     * Gets the current entry box value
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT std::string get_answer(void) const;

    /**
     * Prompts the user with the given question and default answer
     * and returns their answer.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT static std::string popup(const std::string &question, const std::string &default_answer = "");

  protected:
    SCOPIRAUI_EXPORT void init_gui(int buttons);
};

/**
 * A dialog window that obtains a file name from the user.
 *
 * @author Aleksander Demko
 */
class scopira::lab::filename_window : public scopira::lab::lab_window
{
  private:
    scopira::tool::count_ptr<scopira::coreui::label> dm_label;
    scopira::tool::count_ptr<scopira::coreui::fileentry> dm_fileentry;

  public:
    /**
     * Create a filewindow window with the given buttons
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT filename_window(int buttons = button_close_c);

    /**
     * Sets the msg in the prompt
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void set_question(const std::string &label);

    /**
     * Sets the filename box
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void set_filename(const std::string &filename);

    /**
     * Gets the current filename value
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT std::string get_filename(void) const;

    /**
     * Prompts the user with a file entry dialog with the given question and default answer
     * and returns their answer.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT static std::string popup(const std::string &question, const std::string &default_filename = "");

  protected:
    SCOPIRAUI_EXPORT void init_gui(int buttons);
};

/**
 * A dialog box that when closed, kills all the other open windows.
 *
 * @author Aleksander Demko
 */
class scopira::lab::quit_window : public scopira::lab::msg_window
{
  public:
    /**
     * Constructor.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT quit_window(void);

    /**
     * Pops up a quit_window for you
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT static void popup(void);

  protected:
    SCOPIRAUI_EXPORT virtual bool on_button(int actionID);
    
  private:
    bool dm_clickedyes;
};

#endif

