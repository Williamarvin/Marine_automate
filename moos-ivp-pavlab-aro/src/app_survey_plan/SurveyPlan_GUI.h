/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SurveyPlan_GUI.h                                     */
/*    DATE: Aug 14th 2020                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef SURVEY_PLAN_GUI_HEADER
#define SURVEY_PLAN_GUI_HEADER

#include "MarineGUI.h"
#include "SurveyPlan_Viewer.h"
#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>
#include <vector>

class SurveyPlan_GUI : public MarineGUI {
public:
  SurveyPlan_GUI(int w, int h, const char *l = 0);
  ~SurveyPlan_GUI(){};

  void resize(int, int, int, int);
  void updateXY();
  int handle(int);

  bool addPoly(std::string);

  void addConfigParam(std::string);
  void addPostConfigParam(std::string);

protected:
  void augmentMenu();
  void initWidgets();
  void resizeWidgetsShape();
  void resizeWidgetsText();

public:
  SurveyPlan_Viewer *m_wt_viewer;

protected:
  // Panel - Column ONE
  Fl_Output *m_fld_pspec;
  Fl_Output *m_fld_vspec;
  Fl_Output *m_fld_lane_ang;

  // Panel - Column TWO
  Fl_Output *m_fld_lane_wid;

  // Panel - Column THREE
  Fl_Output *m_fld_lane_cnt;

  // Panel - Column FOUR
  Fl_Output *m_fld_survey_len;

  // Panel - Column FIVE
  Fl_Output *m_fld_pack_lanes;

  // Panel - Column SIX
  Fl_Output *m_fld_core_wid;

  // Panel - Column SEVEN
  Fl_Output *m_fld_drop_lane;

  // Panel - Column EIGHT
  Fl_Output *m_fld_auto_dense;

  // Panel - Column NINE
  Fl_Output *m_fld_auto_shift;

  //=================================================
  // BOTTOM Panel
  //=================================================
private:
  inline void cb_AltLaneAngle_i(int);
  static void cb_AltLaneAngle(Fl_Widget *, int);

  inline void cb_CenterView_i();
  static void cb_CenterView(Fl_Widget *);

  inline void cb_RotateRegion_i(int);
  static void cb_RotateRegion(Fl_Widget *, int);

  inline void cb_AltRegionX_i(int);
  static void cb_AltRegionX(Fl_Widget *, int);

  inline void cb_AltRegionY_i(int);
  static void cb_AltRegionY(Fl_Widget *, int);

  inline void cb_AltLaneWidth_i(int);
  static void cb_AltLaneWidth(Fl_Widget *, int);

  inline void cb_ToggleLabels_i();
  static void cb_ToggleLabels(Fl_Widget *);

  inline void cb_ToggleShowHull_i();
  static void cb_ToggleShowHull(Fl_Widget *);

  inline void cb_ToggleShowRegion_i();
  static void cb_ToggleShowRegion(Fl_Widget *);

  inline void cb_ToggleShowSurvey_i();
  static void cb_ToggleShowSurvey(Fl_Widget *);

  inline void cb_ToggleAutoFit_i();
  static void cb_ToggleAutoFit(Fl_Widget *);

  inline void cb_TogglePackLanes_i();
  static void cb_TogglePackLanes(Fl_Widget *);

  inline void cb_ToggleDropLane_i();
  static void cb_ToggleDropLane(Fl_Widget *);

  inline void cb_ToggleCoreWidth_i();
  static void cb_ToggleCoreWidth(Fl_Widget *);

  inline void cb_ToggleAutoShift_i();
  static void cb_ToggleAutoShift(Fl_Widget *);

  inline void cb_ToggleAutoDense_i();
  static void cb_ToggleAutoDense(Fl_Widget *);

  int m_start_wid;
  int m_start_hgt;
};
#endif
