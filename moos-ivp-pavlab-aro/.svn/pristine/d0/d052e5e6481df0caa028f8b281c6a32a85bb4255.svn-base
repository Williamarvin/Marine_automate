/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SurveyPlan_Viewer.h                                  */
/*    DATE: July 26th, 2020                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef SURVEY_PLAN_VIEWER_HEADER
#define SURVEY_PLAN_VIEWER_HEADER

#include <vector>
#include <string>
#include "FL/Fl.H"
#include "FL/fl_draw.H"
#include "BackImg.h"
#include "MarineViewer.h"
#include "XYPolygon.h"
#include "SurveyGenerator.h"

class SurveyPlan_GUI;

class SurveyPlan_Viewer : public MarineViewer
{
 friend class SurveyPlan_GUI;
 public:
  SurveyPlan_Viewer(int x, int y, int w, int h, const char *l=0);
  ~SurveyPlan_Viewer() {};

  // Pure virtuals that needs to be defined
  void   modColorScheme() {};

  // Virtuals defined
  void   draw();
  int    handle(int);
  void   handle_left_mouse(int, int);
  void   handle_right_mouse(int, int);

  void   addPostConfigParam(std::string);
  void   addConfigParam(std::string);

  void   drawRegion();
  void   drawSurvey();
  void   drawRectHull();
  
  void   toggleShowHull()   {m_show_rehull = !m_show_rehull;}
  void   toggleShowRegion() {m_show_region = !m_show_region;}
  void   toggleShowSurvey() {m_show_survey = !m_show_survey;}

  bool   handlePostConfigParams();
  bool   handleConfigParams();

  bool   setPointColor(std::string);
  bool   setPointSize(std::string);

  void   updateSurveyPlan();

  void   rotateRegion(double v);

  
 public: // SurveryGenerator Modifiers/Getters 
  void   setLaneWidth(double);
  void   setLaneAngle(double);
  void   modLaneWidth(double);
  void   modLaneAngle(double);
  void   modRegionX(double);
  void   modRegionY(double);

  void   togglePackLanes()  {m_spgen.togglePackLanes();}
  void   toggleDropLane()   {m_spgen.toggleDropLane();}
  void   toggleCoreWidth()  {m_spgen.toggleCoreWidth();}
  void   toggleAutoShift()  {m_spgen.toggleAutoShift();}
  void   toggleAutoDense()  {m_spgen.toggleAutoDense();}

  std::string  getRegionSpec() const {return(m_spgen.getRegionSpec());}
  std::string  getSurveySpec() const {return(m_spgen.getSurveySpec());}
  double       getSurveyLen()  const {return(m_spgen.getSurveyLen());}
  unsigned int getLaneCount()  const {return(m_spgen.getLaneCount());}
  double       getLaneAngle()  const {return(m_spgen.getLaneAngle());}
  double       getLaneWidth()  const {return(m_spgen.getLaneWidth());}
  double       getLaneWidthX() const {return(m_spgen.getLaneWidthX());}
  
  bool getPackLanes() const {return(m_spgen.getPackLanes());}
  bool getDropLane() const  {return(m_spgen.getDropLane());}
  bool getCoreWidth() const {return(m_spgen.getCoreWidth());}
  bool getAutoShift() const {return(m_spgen.getAutoShift());}
  bool getAutoDense() const {return(m_spgen.getAutoDense());}
  
 private: // State vars
  SurveyGenerator m_spgen;
  
 private: // Config vars

  std::vector<std::string> m_config_params;
  std::vector<std::string> m_post_config_params;

  bool   m_show_rehull;
  bool   m_show_region;
  bool   m_show_survey;
  
  std::string m_pt_color;
  double      m_pt_size;

  std::string m_region_edge_color;
  std::string m_region_vert_color;
  std::string m_survey_edge_color;
  std::string m_survey_vert_color;

  XYPoint m_newpt;
};

#endif 
