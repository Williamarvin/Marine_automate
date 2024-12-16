/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SurveyPlan_GUI.cpp                                   */
/*    DATE: Aug 14th, 2020                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include "SurveyPlan_GUI.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "MBUtils.h"
#include <cstdlib>
#include <iostream>

using namespace std;

//----------------------------------------------------------------
// Constructor

SurveyPlan_GUI::SurveyPlan_GUI(int wid, int hgt, const char *label)
    : MarineGUI(wid, hgt, label) {
  this->user_data((void *)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();
  // size_range(minw,minh, maxw=0,maxh=0)
  this->size_range(1000, 800, 2500, 1800);

  m_start_hgt = hgt;
  m_start_wid = wid;

  augmentMenu();
  setMenuItemColors();

  initWidgets();
  resizeWidgetsShape();
  resizeWidgetsText();

  this->end();
  this->resizable(this);
  this->show();
}

//----------------------------------------------------------------
// Procedure: addConfigParam()

void SurveyPlan_GUI::addConfigParam(string param) {
  if (!m_wt_viewer)
    return;

  m_wt_viewer->addConfigParam(param);
}

//----------------------------------------------------------------
// Procedure: addPostConfigParam()

void SurveyPlan_GUI::addPostConfigParam(string param) {
  if (!m_wt_viewer)
    return;

  m_wt_viewer->addPostConfigParam(param);
}

//---------------------------------------------------------------
// Procedure: initWidgets()

void SurveyPlan_GUI::initWidgets() {
  Fl_Color fcolor_blue = fl_rgb_color(140, 140, 220);
  Fl_Color fcolor_beige = fl_rgb_color(223, 219, 191);
  Fl_Color fcolor_green = fl_rgb_color(200, 230, 190);
  Fl_Color fcolor_red = fl_rgb_color(200, 140, 140);
  Fl_Color fcolor_dark_goldenrod = fl_rgb_color(184, 136, 11);
  // Fl_Color fcolor_dark_goldenrodx = fl_rgb_color(136, 136, 11);

  //=================================================
  // Main Survey Plan Test Viewer
  //=================================================
  m_wt_viewer = new SurveyPlan_Viewer(1, 1, 1, 1);
  m_mviewer = m_wt_viewer;

  //=================================================
  // Column One
  m_fld_pspec = new Fl_Output(0, 0, 1, 1, "Region:");
  m_fld_pspec->color(fcolor_dark_goldenrod);
  m_fld_pspec->clear_visible_focus();

  m_fld_vspec = new Fl_Output(0, 0, 1, 1, "Survey:");
  m_fld_vspec->color(fcolor_dark_goldenrod);
  m_fld_vspec->clear_visible_focus();

  m_fld_lane_ang = new Fl_Output(0, 0, 1, 1, "LaneAng:");
  m_fld_lane_ang->color(fcolor_beige);
  m_fld_lane_ang->clear_visible_focus();

  //=================================================
  // Column Two
  m_fld_lane_wid = new Fl_Output(0, 0, 1, 1, "LaneWid:");
  m_fld_lane_wid->color(fcolor_beige);
  m_fld_lane_wid->clear_visible_focus();

  //=================================================
  // Column Three
  m_fld_lane_cnt = new Fl_Output(0, 0, 1, 1, "Lanes:");
  m_fld_lane_cnt->color(fcolor_beige);
  m_fld_lane_cnt->clear_visible_focus();

  //=================================================
  // Column Four
  m_fld_survey_len = new Fl_Output(0, 0, 1, 1, "Length:");
  m_fld_survey_len->color(fcolor_beige);
  m_fld_survey_len->clear_visible_focus();

  //=================================================
  // Column Five
  // m_fld_auto_fit = new Fl_Output(0, 0, 1, 1, "AutoFit:");
  // m_fld_auto_fit->color(fcolor_beige);
  // m_fld_auto_fit->clear_visible_focus();

  //=================================================
  // Column Five
  m_fld_pack_lanes = new Fl_Output(0, 0, 1, 1, "PackLanes:");
  m_fld_pack_lanes->color(fcolor_beige);
  m_fld_pack_lanes->clear_visible_focus();

  //=================================================
  // Column Six
  m_fld_core_wid = new Fl_Output(0, 0, 1, 1, "CoreWid:");
  m_fld_core_wid->color(fcolor_beige);
  m_fld_core_wid->clear_visible_focus();

  //=================================================
  // Column Seven
  m_fld_drop_lane = new Fl_Output(0, 0, 1, 1, "DropLane:");
  m_fld_drop_lane->color(fcolor_beige);
  m_fld_drop_lane->clear_visible_focus();

  //=================================================
  // Column Eight
  m_fld_auto_dense = new Fl_Output(0, 0, 1, 1, "AutoDense:");
  m_fld_auto_dense->color(fcolor_beige);
  m_fld_auto_dense->clear_visible_focus();

  //=================================================
  // Column Nine
  m_fld_auto_shift = new Fl_Output(0, 0, 1, 1, "AutoShift:");
  m_fld_auto_shift->color(fcolor_beige);
  m_fld_auto_shift->clear_visible_focus();
}

//----------------------------------------------------------------------
// Procedure: resizeWidgetsShape()

void SurveyPlan_GUI::resizeWidgetsShape() {
  int extra_wid = w() - m_start_wid;
  if (extra_wid < 0)
    extra_wid = 0;

  int num_columns = 9;
  extra_wid = (extra_wid / num_columns);

  int field_hgt = 20;

  int row0 = h() - 80;
  int row1 = row0 + 25;
  int row2 = row1 + 25;

  int col1_pos = 60;
  int col1_wid = 35 + (extra_wid);

  int col2_pos = col1_pos + col1_wid + 60;
  int col2_wid = 55 + (extra_wid);

  int col3_pos = col2_pos + col2_wid + 50;
  int col3_wid = 35 + (extra_wid);

  int col4_pos = col3_pos + col3_wid + 60;
  int col4_wid = 50 + (extra_wid);

  int col5_pos = col4_pos + col4_wid + 80;
  int col5_wid = 45 + (extra_wid);

  int col6_pos = col5_pos + col5_wid + 65;
  int col6_wid = 40 + (extra_wid);

  int col7_pos = col6_pos + col6_wid + 70;
  int col7_wid = 40 + (extra_wid);

  int col8_pos = col7_pos + col7_wid + 75;
  int col8_wid = 40 + (extra_wid);

  int col9_pos = col8_pos + col8_wid + 70;
  int col9_wid = 40 + (extra_wid);

  //===================================================
  // Main Viewer
  //===================================================
  m_wt_viewer->resize(0, 30, w(), h() - 120);

  // Column 1  --------------------------
  int psp_x = col1_pos;
  int psp_y = row0;
  int psp_wid = w() - 70;
  m_fld_pspec->resize(psp_x, psp_y, psp_wid, field_hgt);

  int vsp_x = col1_pos;
  int vsp_y = row1;
  int vsp_wid = w() - 70;
  m_fld_vspec->resize(vsp_x, vsp_y, vsp_wid, field_hgt);

  int lang_x = col1_pos;
  int lang_y = row2;
  int lang_wid = col1_wid;
  m_fld_lane_ang->resize(lang_x, lang_y, lang_wid, field_hgt);

  // Column 2  --------------------------
  int lwid_x = col2_pos;
  int lwid_y = row2;
  int lwid_wid = col2_wid;
  m_fld_lane_wid->resize(lwid_x, lwid_y, lwid_wid, field_hgt);

  // Column 3  --------------------------
  int lcnt_x = col3_pos;
  int lcnt_y = row2;
  int lcnt_wid = col3_wid;
  m_fld_lane_cnt->resize(lcnt_x, lcnt_y, lcnt_wid, field_hgt);

  // Column 4  --------------------------
  int slen_x = col4_pos;
  int slen_y = row2;
  int slen_wid = col4_wid;
  m_fld_survey_len->resize(slen_x, slen_y, slen_wid, field_hgt);

  // Column 5  --------------------------
  int pack_x = col5_pos;
  int pack_y = row2;
  int pack_wid = col5_wid;
  m_fld_pack_lanes->resize(pack_x, pack_y, pack_wid, field_hgt);

  // Column 6  --------------------------
  int cow_x = col6_pos;
  int cow_y = row2;
  int cow_wid = col6_wid;
  m_fld_core_wid->resize(cow_x, cow_y, cow_wid, field_hgt);

  // Column 7  --------------------------
  int dl_x = col7_pos;
  int dl_y = row2;
  int dl_wid = col7_wid;
  m_fld_drop_lane->resize(dl_x, dl_y, dl_wid, field_hgt);

  // Column 8  --------------------------
  int den_x = col8_pos;
  int den_y = row2;
  int den_wid = col8_wid;
  m_fld_auto_dense->resize(den_x, den_y, den_wid, field_hgt);

  // Column 9  --------------------------
  int shft_x = col9_pos;
  int shft_y = row2;
  int shft_wid = col9_wid;
  m_fld_auto_shift->resize(shft_x, shft_y, shft_wid, field_hgt);
}

//----------------------------------------------------------------------
// Procedure: resizeWidgetsText()

void SurveyPlan_GUI::resizeWidgetsText() {
  int text_size = 12;
  int label_size = 12;

  // Column One ------------------------
  m_fld_pspec->textsize(text_size);
  m_fld_pspec->labelsize(label_size);

  m_fld_vspec->textsize(text_size);
  m_fld_vspec->labelsize(label_size);

  m_fld_lane_ang->textsize(text_size);
  m_fld_lane_ang->labelsize(label_size);

  // Column Two -------------------------
  m_fld_lane_wid->textsize(text_size);
  m_fld_lane_wid->labelsize(label_size);

  // Column Three -----------------------
  m_fld_lane_cnt->textsize(text_size);
  m_fld_lane_cnt->labelsize(label_size);

  // Column Four ------------------------
  m_fld_survey_len->textsize(text_size);
  m_fld_survey_len->labelsize(label_size);

  // Column Five -------------------------
  m_fld_pack_lanes->textsize(text_size);
  m_fld_pack_lanes->labelsize(label_size);

  // Column Six -----------------------
  m_fld_core_wid->textsize(text_size);
  m_fld_core_wid->labelsize(label_size);

  // Column Seven -----------------------
  m_fld_drop_lane->textsize(text_size);
  m_fld_drop_lane->labelsize(label_size);

  // Column Eight -----------------------
  m_fld_auto_dense->textsize(text_size);
  m_fld_auto_dense->labelsize(label_size);

  // Column Nine ------------------------
  m_fld_auto_shift->textsize(text_size);
  m_fld_auto_shift->labelsize(label_size);
}

//----------------------------------------------------------
// Procedure: resize

void SurveyPlan_GUI::resize(int lx, int ly, int lw, int lh) {
  Fl_Window::resize(lx, ly, lw, lh);
  resizeWidgetsShape();
  resizeWidgetsText();
}

//----------------------------------------------------------------
// Procedure: augmentMenu

void SurveyPlan_GUI::augmentMenu() {
  //==============================================================
  // The File SubMenu
  //==============================================================
  // m_menubar->add("File/dump cmdline args", 'd',
  //		 (Fl_Callback*)SurveyPlan_GUI::cb_DumpCmdLineArgs, (void*)0,
  //		 FL_MENU_DIVIDER);

  //==============================================================
  // The BackView SubMenu
  //==============================================================
  // First we remove some items at the superclass level so we can use the
  // hot keys differently.

  removeMenuItem("BackView/Zoom Reset");

  removeMenuItem("BackView/Pan Up (v. slow) ");
  removeMenuItem("BackView/Pan Down (v. slow) ");
  removeMenuItem("BackView/Pan Left (v. slow) ");
  removeMenuItem("BackView/Pan Right (v. slow)");

  //====================================================================
  // The Turn SubMenu
  //====================================================================

  //====================================================================
  // The Region Polygon SubMenu
  //====================================================================
  m_menubar->add("Region/RotateLeft", '[',
                 (Fl_Callback *)SurveyPlan_GUI::cb_RotateRegion, (void *)-1, 0);
  m_menubar->add("Region/RotateRight", ']',
                 (Fl_Callback *)SurveyPlan_GUI::cb_RotateRegion, (void *)1,
                 FL_MENU_DIVIDER);
  m_menubar->add("Region/RotateLeft Fast", FL_ALT + '[',
                 (Fl_Callback *)SurveyPlan_GUI::cb_RotateRegion, (void *)-5, 0);
  m_menubar->add("Region/RotateRight Fast", FL_ALT + ']',
                 (Fl_Callback *)SurveyPlan_GUI::cb_RotateRegion, (void *)5,
                 FL_MENU_DIVIDER);

  m_menubar->add("Region/Up", FL_SHIFT + FL_Up,
                 (Fl_Callback *)SurveyPlan_GUI::cb_AltRegionY, (void *)10, 0);
  m_menubar->add("Region/Down", FL_SHIFT + FL_Down,
                 (Fl_Callback *)SurveyPlan_GUI::cb_AltRegionY, (void *)-10, 0);
  m_menubar->add("Region/Right", FL_SHIFT + FL_Right,
                 (Fl_Callback *)SurveyPlan_GUI::cb_AltRegionX, (void *)10, 0);
  m_menubar->add("Region/Left", FL_SHIFT + FL_Left,
                 (Fl_Callback *)SurveyPlan_GUI::cb_AltRegionX, (void *)-10,
                 FL_MENU_DIVIDER);

  m_menubar->add("Region/Toggle Show Hull", 'u',
                 (Fl_Callback *)SurveyPlan_GUI::cb_ToggleShowHull, (void *)0,
                 0);

  m_menubar->add("Region/Toggle Show Region", 'r',
                 (Fl_Callback *)SurveyPlan_GUI::cb_ToggleShowRegion, (void *)0,
                 0);

  //====================================================================
  // The Survey SubMenu
  //====================================================================
  m_menubar->add("Survey/Toggle PackLanes", 'p',
                 (Fl_Callback *)SurveyPlan_GUI::cb_TogglePackLanes, (void *)0,
                 0);

  m_menubar->add("Survey/Toggle DropLane", 'd',
                 (Fl_Callback *)SurveyPlan_GUI::cb_ToggleDropLane, (void *)0,
                 0);

  m_menubar->add("Survey/Toggle CoreWidth", 'w',
                 (Fl_Callback *)SurveyPlan_GUI::cb_ToggleCoreWidth, (void *)0,
                 0);

  m_menubar->add("Survey/Toggle AutoShift", 's',
                 (Fl_Callback *)SurveyPlan_GUI::cb_ToggleAutoShift, (void *)0,
                 0);

  m_menubar->add("Survey/Toggle AutoDense", 'e',
                 (Fl_Callback *)SurveyPlan_GUI::cb_ToggleAutoDense, (void *)0,
                 FL_MENU_DIVIDER);

  m_menubar->add("Survey/LaneWidth ++", ')',
                 (Fl_Callback *)SurveyPlan_GUI::cb_AltLaneWidth, (void *)1, 0);
  m_menubar->add("Survey/LaneWidth --", '(',
                 (Fl_Callback *)SurveyPlan_GUI::cb_AltLaneWidth, (void *)-1,
                 FL_MENU_DIVIDER);

  m_menubar->add("Survey/LaneAngle ++", 'a',
                 (Fl_Callback *)SurveyPlan_GUI::cb_AltLaneAngle, (void *)1, 0);
  m_menubar->add("Survey/LaneAngle --", 'z',
                 (Fl_Callback *)SurveyPlan_GUI::cb_AltLaneAngle, (void *)-1,
                 FL_MENU_DIVIDER);

  m_menubar->add("Region/Toggle Show Survey", 's',
                 (Fl_Callback *)SurveyPlan_GUI::cb_ToggleShowSurvey, (void *)0,
                 0);
}

//----------------------------------------------------------
// Procedure: handle

int SurveyPlan_GUI::handle(int event) {
  switch (event) {
  case FL_PUSH:
    Fl_Window::handle(event);
    updateXY();
    return (1);
    break;
  default:
    return (Fl_Window::handle(event));
  }
}

//----------------------------------------- AltLaneAngle
inline void SurveyPlan_GUI::cb_AltLaneAngle_i(int val) {
  m_wt_viewer->modLaneAngle(val);
  m_wt_viewer->redraw();
  updateXY();
}
void SurveyPlan_GUI::cb_AltLaneAngle(Fl_Widget *o, int v) {
  ((SurveyPlan_GUI *)(o->parent()->user_data()))->cb_AltLaneAngle_i(v);
}

//----------------------------------------- RotateRegion
inline void SurveyPlan_GUI::cb_RotateRegion_i(int amt) {
  m_wt_viewer->rotateRegion(amt);
  m_wt_viewer->redraw();
  updateXY();
}
void SurveyPlan_GUI::cb_RotateRegion(Fl_Widget *o, int v) {
  ((SurveyPlan_GUI *)(o->parent()->user_data()))->cb_RotateRegion_i(v);
}

//----------------------------------------- AltRegionX
inline void SurveyPlan_GUI::cb_AltRegionX_i(int amt) {
  m_wt_viewer->modRegionX(amt);
  m_wt_viewer->redraw();
  updateXY();
}
void SurveyPlan_GUI::cb_AltRegionX(Fl_Widget *o, int v) {
  ((SurveyPlan_GUI *)(o->parent()->user_data()))->cb_AltRegionX_i(v);
}

//----------------------------------------- AltStartY
inline void SurveyPlan_GUI::cb_AltRegionY_i(int amt) {
  m_wt_viewer->modRegionY(amt);
  m_wt_viewer->redraw();
  updateXY();
}
void SurveyPlan_GUI::cb_AltRegionY(Fl_Widget *o, int v) {
  ((SurveyPlan_GUI *)(o->parent()->user_data()))->cb_AltRegionY_i(v);
}

//----------------------------------------- AltLaneWidth
inline void SurveyPlan_GUI::cb_AltLaneWidth_i(int amt) {
  m_wt_viewer->modLaneWidth(amt);
  m_wt_viewer->redraw();
  updateXY();
}
void SurveyPlan_GUI::cb_AltLaneWidth(Fl_Widget *o, int v) {
  ((SurveyPlan_GUI *)(o->parent()->user_data()))->cb_AltLaneWidth_i(v);
}

//----------------------------------------- ToggleLabels
inline void SurveyPlan_GUI::cb_ToggleLabels_i() {
  // m_wt_viewer->toggleLabels();
  m_wt_viewer->redraw();
  updateXY();
}
void SurveyPlan_GUI::cb_ToggleLabels(Fl_Widget *o) {
  ((SurveyPlan_GUI *)(o->parent()->user_data()))->cb_ToggleLabels_i();
}

//----------------------------------------- ToggleShowHull
inline void SurveyPlan_GUI::cb_ToggleShowHull_i() {
  m_wt_viewer->toggleShowHull();
  m_wt_viewer->redraw();
  updateXY();
}
void SurveyPlan_GUI::cb_ToggleShowHull(Fl_Widget *o) {
  ((SurveyPlan_GUI *)(o->parent()->user_data()))->cb_ToggleShowHull_i();
}

//----------------------------------------- ToggleShowRegion
inline void SurveyPlan_GUI::cb_ToggleShowRegion_i() {
  m_wt_viewer->toggleShowRegion();
  m_wt_viewer->redraw();
  updateXY();
}
void SurveyPlan_GUI::cb_ToggleShowRegion(Fl_Widget *o) {
  ((SurveyPlan_GUI *)(o->parent()->user_data()))->cb_ToggleShowRegion_i();
}

//----------------------------------------- ToggleShowSurvey
inline void SurveyPlan_GUI::cb_ToggleShowSurvey_i() {
  m_wt_viewer->toggleShowSurvey();
  m_wt_viewer->redraw();
  updateXY();
}
void SurveyPlan_GUI::cb_ToggleShowSurvey(Fl_Widget *o) {
  ((SurveyPlan_GUI *)(o->parent()->user_data()))->cb_ToggleShowSurvey_i();
}

//----------------------------------------- TogglePackLanes
inline void SurveyPlan_GUI::cb_TogglePackLanes_i() {
  m_wt_viewer->togglePackLanes();
  m_wt_viewer->updateSurveyPlan();
  m_wt_viewer->redraw();
  updateXY();
}
void SurveyPlan_GUI::cb_TogglePackLanes(Fl_Widget *o) {
  ((SurveyPlan_GUI *)(o->parent()->user_data()))->cb_TogglePackLanes_i();
}

//----------------------------------------- ToggleDropLane
inline void SurveyPlan_GUI::cb_ToggleDropLane_i() {
  m_wt_viewer->toggleDropLane();
  m_wt_viewer->updateSurveyPlan();
  m_wt_viewer->redraw();
  updateXY();
}
void SurveyPlan_GUI::cb_ToggleDropLane(Fl_Widget *o) {
  ((SurveyPlan_GUI *)(o->parent()->user_data()))->cb_ToggleDropLane_i();
}

//----------------------------------------- ToggleCoreWidth
inline void SurveyPlan_GUI::cb_ToggleCoreWidth_i() {
  m_wt_viewer->toggleCoreWidth();
  m_wt_viewer->updateSurveyPlan();
  m_wt_viewer->redraw();
  updateXY();
}
void SurveyPlan_GUI::cb_ToggleCoreWidth(Fl_Widget *o) {
  ((SurveyPlan_GUI *)(o->parent()->user_data()))->cb_ToggleCoreWidth_i();
}

//----------------------------------------- ToggleAutoShift
inline void SurveyPlan_GUI::cb_ToggleAutoShift_i() {
  m_wt_viewer->toggleAutoShift();
  m_wt_viewer->updateSurveyPlan();
  m_wt_viewer->redraw();
  updateXY();
}
void SurveyPlan_GUI::cb_ToggleAutoShift(Fl_Widget *o) {
  ((SurveyPlan_GUI *)(o->parent()->user_data()))->cb_ToggleAutoShift_i();
}

//----------------------------------------- ToggleAutoDense
inline void SurveyPlan_GUI::cb_ToggleAutoDense_i() {
  m_wt_viewer->toggleAutoDense();
  m_wt_viewer->updateSurveyPlan();
  m_wt_viewer->redraw();
  updateXY();
}
void SurveyPlan_GUI::cb_ToggleAutoDense(Fl_Widget *o) {
  ((SurveyPlan_GUI *)(o->parent()->user_data()))->cb_ToggleAutoDense_i();
}

//------------------------------------------------------
// Procedure  UpdateXY()

void SurveyPlan_GUI::updateXY() {
  Fl_Color fcolor_green = fl_rgb_color(200, 230, 190);
  Fl_Color fcolor_red = fl_rgb_color(200, 140, 140);
  Fl_Color fcolor_blue = fl_rgb_color(140, 140, 240);
  Fl_Color fcolor_beige = fl_rgb_color(223, 219, 191);

  string sval;
  // Column (1)  --------------------------
  sval = m_wt_viewer->getRegionSpec();
  m_fld_pspec->value(sval.c_str());

  sval = m_wt_viewer->getSurveySpec();
  m_fld_vspec->value(sval.c_str());

  double lane_angle = m_wt_viewer->getLaneAngle();
  sval = doubleToStringX(lane_angle, 1);
  m_fld_lane_ang->value(sval.c_str());

  // Column (2) Lane Width ---------
  double lane_width = m_wt_viewer->getLaneWidth();
  double lane_widthx = m_wt_viewer->getLaneWidthX();
  sval = doubleToStringX(lane_width, 1);
  if (lane_width != lane_widthx) {
    sval += "/";
    sval += doubleToStringX(lane_widthx, 2);
    m_fld_lane_wid->color(fcolor_blue);
  } else
    m_fld_lane_wid->color(fcolor_beige);

  m_fld_lane_wid->value(sval.c_str());

  // Column (3) Lane Count ---------
  double lane_count = m_wt_viewer->getLaneCount();
  sval = doubleToStringX(lane_count, 1);
  m_fld_lane_cnt->value(sval.c_str());

  // Column (4) Survey Length -----------
  double survey_len = m_wt_viewer->getSurveyLen();
  sval = doubleToStringX(survey_len, 1);
  m_fld_survey_len->value(sval.c_str());

  // Column (5)  -----------
  bool pack_lanes = m_wt_viewer->getPackLanes();
  sval = boolToString(pack_lanes);
  m_fld_pack_lanes->value(sval.c_str());
  if (pack_lanes)
    m_fld_pack_lanes->color(fcolor_green);
  else
    m_fld_pack_lanes->color(fcolor_red);

  // Column (6)  -----------
  bool core_wid = m_wt_viewer->getCoreWidth();
  sval = boolToString(core_wid);
  m_fld_core_wid->value(sval.c_str());
  if (core_wid)
    m_fld_core_wid->color(fcolor_green);
  else
    m_fld_core_wid->color(fcolor_red);

  // Column (7)  -----------
  bool drop_lane = m_wt_viewer->getDropLane();
  sval = boolToString(drop_lane);
  m_fld_drop_lane->value(sval.c_str());
  if (drop_lane)
    m_fld_drop_lane->color(fcolor_green);
  else
    m_fld_drop_lane->color(fcolor_red);

  // Column (8)  -----------
  bool auto_dense = m_wt_viewer->getAutoDense();
  sval = boolToString(auto_dense);
  m_fld_auto_dense->value(sval.c_str());
  if (auto_dense)
    m_fld_auto_dense->color(fcolor_green);
  else
    m_fld_auto_dense->color(fcolor_red);

  // Column (9) AutoShift ---------
  bool auto_shift = m_wt_viewer->getAutoShift();
  sval = boolToString(auto_shift);
  m_fld_auto_shift->value(sval.c_str());
  if (auto_shift)
    m_fld_auto_shift->color(fcolor_green);
  else
    m_fld_auto_shift->color(fcolor_red);
}
