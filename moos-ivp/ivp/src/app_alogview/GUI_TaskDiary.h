/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: GUI_TaskDiary.h                                      */
/*    DATE: Nov 27th, 2021                                       */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef GUI_TASK_DIARY_HEADER
#define GUI_TASK_DIARY_HEADER

#include "ALogDataBroker.h"
#include "FL/Fl_Button.H"
#include "ModelTaskDiary.h"
#include <FL/Fl.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Window.H>
#include <string>

class REPLAY_GUI;

class GUI_TaskDiary : public Fl_Window {
public:
  GUI_TaskDiary(int w, int h, const char *l = 0);
  ~GUI_TaskDiary();

  static Fl_Menu_Item menu_[];
  Fl_Menu_Bar *mbar;

  void initWidgets();

  void setParentGUI(REPLAY_GUI *gui) { m_parent_gui = gui; }
  void setDataBroker(ALogDataBroker);
  void setCurrTime(double = -1);
  void setReplayWarpMsg(std::string s) {
    m_replay_warp_msg = s;
    updateXY();
  }

  void resize(int, int, int, int);
  int handle(int);

protected:
  void resizeWidgetsShape();
  void resizeWidgetsText();
  void updateBrowsers();
  void updateXY();
  void updateMutableTextSize(std::string);

  void updateBrowser1();
  void updateBrowser2();

private:
  inline void cb_BrowserInfo_i();
  static void cb_BrowserInfo(Fl_Widget *);

  inline void cb_ButtonSeparate_i(int);
  static void cb_ButtonSeparate(Fl_Widget *, int);

  inline void cb_ButtonWrapLine_i(int);
  static void cb_ButtonWrapLine(Fl_Widget *, int);

  inline void cb_ButtonFuture_i(int);
  static void cb_ButtonFuture(Fl_Widget *, int);

  inline void cb_ModText_i(int);
  static void cb_ModText(Fl_Widget *, int);

  inline void cb_Step_i(int);
  static void cb_Step(Fl_Widget *, int);

public:
  ModelTaskDiary m_tdmodel;
  REPLAY_GUI *m_parent_gui;
  ALogDataBroker m_dbroker;

protected:
  Fl_Output *m_fld_time;

  Fl_Check_Button *m_but_separate;
  Fl_Check_Button *m_but_wrapline;
  Fl_Check_Button *m_but_future;

  Fl_Browser *m_brw_info1;
  Fl_Browser *m_brw_info2;

  Fl_Button *m_but_text_more;
  Fl_Button *m_but_text_less;

  std::string m_replay_warp_msg;
  std::string m_vname;
  std::string m_app_name;

  int m_mutable_text_size;
};

#endif
