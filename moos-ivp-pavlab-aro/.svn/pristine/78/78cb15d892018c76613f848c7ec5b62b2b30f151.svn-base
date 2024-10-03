/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SurveyPlan_Viewer.cpp                                */
/*    DATE: August 14th 2020                                     */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <unistd.h>
#include <iostream>
#include <cmath>
#include "SurveyPlan_Viewer.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "XYFormatUtilsPoly.h"
#include "SurveyGenerator.h"


using namespace std;

//----------------------------------------------------------------
// Constructor

SurveyPlan_Viewer::SurveyPlan_Viewer(int x, int y, int wid, int hgt,
				     const char *label) :
  MarineViewer(x, y, wid, hgt, label)
{
  // Initialize config params
  m_pt_color = "yellow";
  m_pt_size  = 4;
  
  m_vshift_x = -80;
  m_vshift_y = -60;

  m_show_rehull = false;
  m_show_region = true;
  m_show_survey = true;
  
  //========================================================
  // Override some default values of MarineViewer superclass
  //========================================================
  m_zoom = 0.6;

  setParam("tiff_view", "on");
  setParam("hash_viewable", "false");
  setParam("hash_shade", -1.0);
  setParam("hash_shade", 0.75);
  setParam("back_shade", -1.0);
  setParam("back_shade", 0.85);

  m_region_edge_color = "dodger_blue";
  m_region_vert_color = "yellow";
  m_survey_edge_color = "gray50";
  m_survey_vert_color = "gray90";
}

//-------------------------------------------------------------
// Procedure: handle()

int SurveyPlan_Viewer::handle(int event)
{
  int vx, vy;
  switch(event) {
  case FL_PUSH:
    vx = Fl::event_x();
    vy = h() - Fl::event_y();
    if(Fl_Window::handle(event) != 1) {
      if(Fl::event_button() == FL_LEFT_MOUSE)
	handle_left_mouse(vx, vy);
      if(Fl::event_button() == FL_RIGHT_MOUSE)
	handle_right_mouse(vx, vy);
    }
    return(1);
    break;  
  default:
    return(Fl_Gl_Window::handle(event));
  }
}

//-------------------------------------------------------------
// Procedure: draw()

void SurveyPlan_Viewer::draw()
{
  MarineViewer::draw();

  if(m_show_rehull)
    drawRectHull();

  if(m_show_region)
    drawRegion();

  if(m_show_survey)
    drawSurvey();

  drawPoint(m_newpt);
}

//-------------------------------------------------------------
// Procedure: drawRegion()

void SurveyPlan_Viewer::drawRegion()
{
  XYPolygon region = m_spgen.getRegion();
  if(!region.is_convex())
    return;

  region.set_label("foo");
  region.set_label_color("invisible");
  region.set_edge_color(m_region_edge_color);
  region.set_vertex_color(m_region_vert_color);
  drawPolygon(region);
}

//-------------------------------------------------------------
// Procedure: drawRectHull()

void SurveyPlan_Viewer::drawRectHull()
{
  XYPolygon rehull = m_spgen.getReHull();
  cout << "RectHull: " << rehull.get_spec() << endl;

  if(!rehull.is_convex())
    return;

  rehull.set_label("rhull");
  rehull.set_label_color("invisible");
  rehull.set_edge_color("white");
  rehull.set_vertex_color("white");
  rehull.set_param("fill_color", "white");
  rehull.set_param("fill_transparency", "0.2");
  drawPolygon(rehull);
}

//-------------------------------------------------------------
// Procedure: drawSurvey()

void SurveyPlan_Viewer::drawSurvey()
{
  XYSegList survey = m_spgen.getSurvey();
  unsigned int vsize = survey.size();
  if(vsize == 0)
    return;
  
  survey.set_label("survey");
  survey.set_label_color("invisible");
  survey.set_edge_color(m_survey_edge_color);
  survey.set_vertex_color(m_survey_vert_color);
  drawSegList(survey);

  double sx = survey.get_vx(0);
  double sy = survey.get_vy(0);

  XYPoint start_pt(sx, sy);
  start_pt.set_vertex_size(7);
  start_pt.set_vertex_color("green");
  start_pt.set_label("startpt");
  start_pt.set_label_color("invisible");
  drawPoint(start_pt);

  if(vsize <= 1)
    return;
  
  double ex = survey.get_vx(vsize-1);
  double ey = survey.get_vy(vsize-1);

  XYPoint end_pt(ex, ey);
  end_pt.set_vertex_size(7);
  end_pt.set_vertex_color("pink");
  end_pt.set_label("endpt");
  end_pt.set_label_color("invisible");
  drawPoint(end_pt);
}

//-------------------------------------------------------------
// Procedure: handle_left_mouse()

void SurveyPlan_Viewer::handle_left_mouse(int vx, int vy)
{
  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  double sx = snapToStep(mx, 0.1);
  double sy = snapToStep(my, 0.1);
  string strx = doubleToStringX(sx,1);
  string stry = doubleToStringX(sy,1);
  
  cout << "sx: " << strx << ", sy: " << stry << endl;

  XYPolygon region = m_spgen.getRegion();
  
  if(Fl::event_state(FL_SHIFT))
    region.delete_vertex(sx, sy);
  else if(Fl::event_state(FL_ALT))
    region.delete_vertex(sx, sy);
  else {
    m_newpt.set_vertex(sx, sy);
    m_newpt.set_vertex_color("yellow");
    m_newpt.set_label("newp");
    m_newpt.set_label_color("invisible");
    m_newpt.set_vertex_size(5);
    
    XYPolygon test_poly = region;

    cout << "spec: " << test_poly.get_spec_pts() << endl;
    
    test_poly.insert_vertex(sx, sy, true);
    if(test_poly.is_convex())
      region = test_poly;
  }

  m_spgen.setRegion(region);
  updateSurveyPlan();
  redraw();
}


//-------------------------------------------------------------
// Procedure: handle_right_mouse()

void SurveyPlan_Viewer::handle_right_mouse(int vx, int vy)
{
  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  double sx = snapToStep(mx, 0.1);
  double sy = snapToStep(my, 0.1);

  string strx = doubleToStringX(sx,1);
  string stry = doubleToStringX(sy,1);
  
  cout << "sx: " << strx << ", sy: " << stry << endl;

  if(Fl::event_state(FL_SHIFT))
    cout << "Shift Right Click" << endl;
  else if(Fl::event_state(FL_ALT))
    cout << "Alt Right Click" << endl;
  else
    m_spgen.setEntryPoint(sx, sy);

    updateSurveyPlan();
  redraw();
}


//---------------------------------------------------------
// Procedure: setPointColor()

bool SurveyPlan_Viewer::setPointColor(string colorstr)
{
  if(colorstr == "")
    return(true);

  if(!isColor(colorstr))
    return(false);

  m_pt_color = colorstr;
  
  return(true);
}

//---------------------------------------------------------
// Procedure: setPointSize()

bool SurveyPlan_Viewer::setPointSize(string psize)
{
  if(psize == "")
    return(true);

  double dval = atof(psize.c_str());

  if(dval < 1)
    dval = 1;
  if(dval > 20)
    dval = 20;

  m_pt_size = dval;

  return(true);
}

//---------------------------------------------------------
// Procedure: updateSurveyPlan()

void SurveyPlan_Viewer::updateSurveyPlan()
{
  m_spgen.generate();
}

//---------------------------------------------------------
// Procedure: rotateRegion()

void SurveyPlan_Viewer::rotateRegion(double dval)
{
  XYPolygon region = m_spgen.getRegion();
  region.rotate(dval);
  m_spgen.setRegion(region);
  updateSurveyPlan();
}

//---------------------------------------------------------
// Procedure: modRegionX()

void SurveyPlan_Viewer::modRegionX(double dval)
{
  XYPolygon region = m_spgen.getRegion();
  region.shift_horz(dval);
  m_spgen.setRegion(region);
  updateSurveyPlan();
}

//---------------------------------------------------------
// Procedure: modRegionY()

void SurveyPlan_Viewer::modRegionY(double dval)
{
  XYPolygon region = m_spgen.getRegion();
  region.shift_vert(dval);
  m_spgen.setRegion(region);
  updateSurveyPlan();
}

//---------------------------------------------------------
// Procedure: setLaneWidth()

void SurveyPlan_Viewer::setLaneWidth(double width)
{
  m_spgen.setLaneWidth(width);
  updateSurveyPlan();
}

//---------------------------------------------------------
// Procedure: modLaneWidth()

void SurveyPlan_Viewer::modLaneWidth(double dval)
{
  double old_lane_width = m_spgen.getLaneWidth();
  double new_lane_width = old_lane_width + dval;
  m_spgen.setLaneWidth(new_lane_width);

  updateSurveyPlan();
}

//---------------------------------------------------------
// Procedure: setLaneAngle()

void SurveyPlan_Viewer::setLaneAngle(double angle)
{
  m_spgen.setLaneAngle(angle);
  updateSurveyPlan();
}

//---------------------------------------------------------
// Procedure: modLaneAngle()

void SurveyPlan_Viewer::modLaneAngle(double dval)
{
  double old_lane_angle = m_spgen.getLaneAngle();
  double new_lane_angle = old_lane_angle + dval;
  m_spgen.setLaneAngle(new_lane_angle);

  updateSurveyPlan();
}

//----------------------------------------------------------------
// Procedure: addConfigParam()

void SurveyPlan_Viewer::addConfigParam(string str)
{
  m_config_params.push_back(str);
}

//----------------------------------------------------------------
// Procedure: addPostConfigParam()

void SurveyPlan_Viewer::addPostConfigParam(string str)
{
  m_post_config_params.push_back(str);
}

//----------------------------------------------------------------
// Procedure: handleConfigParams()

bool SurveyPlan_Viewer::handleConfigParams()
{
  if(vectorContains(m_config_params, "-v") ||
     vectorContains(m_config_params, "--verbose"))
    setVerbose(true);

  cout << "Config Params:" << endl;
  for(unsigned int i=0; i<m_config_params.size(); i++){
    string orig = m_config_params[i];
    string argi = m_config_params[i];
    string left = biteStringX(argi, '=');
    string val  = argi;
    cout << "orig: " << orig << endl;

    bool ok_param = false;
    if(strEnds(left, ".tif") || strEnds(left, ".tiff"))
      ok_param = setParam("tiff_file", left);
    else if(left == "--poly") {
      XYPolygon poly = string2Poly(val);
      if(poly.is_convex()) {
	m_spgen.setRegion(poly);
	ok_param = true;
	updateSurveyPlan();
      }
      else
	cout << "Bad Region Poly:" << val << endl;
    }
    else if(left == "--lane_width") {
      setLaneWidth(atof(val.c_str()));
      ok_param = true;
    }
    else if(left == "--lane_angle") {
      setLaneAngle(atof(val.c_str()));
      ok_param = true;
    }		   
    else if((left == "-v") || (left == "--verbose"))
      ok_param = true;
      
    if(!ok_param) {
      cout << "Bad Config param: [" << orig << "]" << endl;
      return(false);
    } 
  }

  if(getTiffFileCount() == 0)
    setParam("tiff_file", "MIT_SP.tif");

  return(true);
}


//----------------------------------------------------------------
// Procedure: handlePostConfigParams()

bool SurveyPlan_Viewer::handlePostConfigParams()
{
  for(unsigned int i=0; i<m_post_config_params.size(); i++) {
    string orig = m_post_config_params[i];
    string argi = m_post_config_params[i];
    string left = biteStringX(argi, '=');
    string val  = argi;
    cout << "orig:: " << orig << endl;

    bool ok_param = false;
    if(left == "--point_color")
      ok_param = setPointColor(val);
    else if(left == "--point_size")
      ok_param = setPointSize(val);
    
    if(!ok_param) {
      cout << "Bad PostConfig param: [" << orig << "]" << endl;
      return(false);
    }
  }
  
  return(true);
}
