/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT                                             */
/*    FILE: JoustMgr.cpp                                    */
/*    DATE: Mar 2nd, 2024                                   */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "FileBuffer.h"
#include "NodeRecordUtils.h"      
#include "ACTable.h"
#include "JoustMgr.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

JoustMgr::JoustMgr()
{
  // Init config vars
  m_center_x = 0;
  m_center_y = 0;
  m_radius = 0;

  // Init state vars
  m_circle_posted = false;
  m_last_post_circle_utc = 0;
  m_last_post_assign_utc = 0;
  
  // Visual Hint Defaults for the ring
  m_hints.setMeasure("ring_vertex_size", 0);
  m_hints.setMeasure("ring_edge_size", 1);
  m_hints.setColor("ring_vertex_color", "off");
  m_hints.setColor("ring_edge_color", "gray50");
  m_hints.setColor("ring_label_color", "off");
}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool JoustMgr::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);
  
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    string comm  = msg.GetCommunity();
    string sval  = msg.GetString(); 
    
#if 0 // Keep these around just for template
    double dval  = msg.GetDouble();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    if(key == "NODE_REPORT") {
      string whynot;
      bool ok = handleMailNodeReport(sval, whynot);
      if(!ok)
	reportRunWarning("Unhandled NodeReport: " + whynot);
    }
    
    else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
  }
  
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool JoustMgr::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool JoustMgr::Iterate()
{
  AppCastingMOOSApp::Iterate();

  postJoustCircle();
  postAssignments();

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool JoustMgr::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "circle") 
      handled = handleConfigJoustCircle(value);

    else if(param == "joust_file") 
      handled = handleConfigJoustFile(value);

    else if(param == "updates") 
      handled = setNonWhiteVarOnString(m_update_var, value);

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables()

void JoustMgr::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("NODE_REPORT", 0);
}

//---------------------------------------------------------
// Procedure: handleConfigJoustCircle()
//   Example: circle = x=33,y=99,rad=50

bool JoustMgr::handleConfigJoustCircle(string sval)
{
  string xpos;
  string ypos;
  string radius;
  
  vector<string> svector = parseString(sval, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = tolower(biteStringX(svector[i],'='));
    string value = svector[i];

    if(param == "x")
      xpos = value;
    else if(param == "y")
      ypos = value;
    else if((param == "rad") || (param == "radius"))
      radius = value;
  }
  if(!isNumber(xpos) || !isNumber(ypos) || !isNumber(radius))
    return(false);

  double xval = atof(xpos.c_str());
  double yval = atof(ypos.c_str());
  double rval = atof(radius.c_str());

  if(rval <= 0)
    return(false);

  m_center_x = xval;
  m_center_y = yval;
  m_radius = rval;
  return(true);
}

//---------------------------------------------------------
// Procedure: handleConfigJoustFile()
//   Example: px1=51.5,py1=-50.1,px2=48.4,py2=-19.9,spd=2.1


bool JoustMgr::handleConfigJoustFile(string filename)
{
  vector<string> lines = fileBuffer(filename);
  if(lines.size() == 0)
    return(false);


  // Part 1: Look for lines like:
  //   px1=84.44,py1=-110.31,px2=15.53,py2=-69.69, \
  //   hdg=300.5,spd=1.9,vname=abe,vcolor=yellow
  
  for(unsigned int i=0; i<lines.size(); i++) {
    string line = lines[i];
    string px1 = tokStringParse(line, "px1");
    string py1 = tokStringParse(line, "py1");
    string px2 = tokStringParse(line, "px2");
    string py2 = tokStringParse(line, "py2");
    string spd = tokStringParse(line, "spd");
    string nam = tokStringParse(line, "vname");
    if(!isNumber(px1) || !isNumber(py1) ||
       !isNumber(px2) || !isNumber(py2) ||
       !isNumber(spd))
      continue;

    m_px1.push_back(atof(px1.c_str()));
    m_py1.push_back(atof(py1.c_str()));
    m_px2.push_back(atof(px2.c_str()));
    m_py2.push_back(atof(py2.c_str()));
    m_spd.push_back(atof(spd.c_str()));
    m_hint.push_back(nam);
    m_set.push_back("");
  }

  // Part 2: Look for lines like:
  // cx=50,cy=-90,rad=40
  for(unsigned int i=0; i<lines.size(); i++) {
    string line = lines[i];
    string cx  = tokStringParse(line, "cx");
    string cy  = tokStringParse(line, "cy");
    string rad = tokStringParse(line, "rad");
    if(!isNumber(cx) || !isNumber(cy) ||
       !isNumber(rad))
      continue;

    m_center_x = atof(cx.c_str());
    m_center_y = atof(cy.c_str());
    m_radius = atof(rad.c_str());
  }
  
  return(true);
}

//---------------------------------------------------------
// Procedure: postJoustCircle()

void JoustMgr::postJoustCircle(bool active)
{
  double elapsed = m_curr_time - m_last_post_circle_utc;
  if(m_circle_posted && (elapsed < 15))
    return;

  XYPolygon ringpoly;
  ringpoly.setRadial(m_center_x, m_center_y, m_radius, 36, 0.01);
  ringpoly.set_label("center_joust");
  
  if(!active)
    Notify("VIEW_POLYGON", ringpoly.get_spec_inactive());
  else {
    applyHints(ringpoly, m_hints, "ring");
    Notify("VIEW_POLYGON", ringpoly.get_spec());
  }
  
  m_circle_posted = true;
  m_last_post_circle_utc = m_curr_time;
}


//------------------------------------------------------------
// Procedure: handleMailNodeReport() 

bool JoustMgr::handleMailNodeReport(string str, string& whynot)
{
  NodeRecord new_record = string2NodeRecord(str);
  if(!new_record.valid("x,y,name", whynot))
    return(false);

  string vname = tolower(new_record.getName());

  Notify("DEBUG1", "In handleMailNodeReport()" + vname);
  
  cout << "In handleMailNodeReport() " << vname << endl;
  
#if 0
  string gname = tolower(new_record.getGroup());

  if(m_ignore_group != "") {
    if(toupper(m_ignore_group) == grp_name)
      return(true);
  }
#endif
  
  makeAssignment(vname);
  return(true);
}

//------------------------------------------------------------
// Procedure: makeAssigment() 

void JoustMgr::makeAssignment(string vname)
{
  //cout << "In makeAssigment: " << vname << endl;
  
  // If the names has already been handled, just ignore
  if(m_known_vnames.count(vname) != 0)
    return;

  // Othewise add to the list of known/assigned names
  m_known_vnames.insert(vname);

  // Go through the table/set of vnames, assign to the first
  // untaken slot.
  int ix = -1;

  // Pass 1: Only assign if it matches a hint
  for(unsigned int i=0; i<m_set.size() && (ix<0); i++) {
    if((m_hint[i] == vname) && (m_set[i] == "")) {
      cout << "AAssigning " << vname << ", index " << i << endl;
      m_set[i] = vname;
      ix = i;
    }
  }

  // Pass 2: Assign to first unset index that has no hint
  for(unsigned int i=0; i<m_set.size() && (ix<0); i++) {
    if((m_hint[i] == "") && (m_set[i] == "")) {
      cout << "BAssigning " << vname << ", index " << i << endl;
      m_set[i] = vname;
      ix = i;
    }
  }

  // Pass 2: Assign to first unset index
  for(unsigned int i=0; i<m_set.size() && (ix<0); i++) {
    if(m_set[i] == "") {
      cout << "CAssigning " << vname << ", index " << i << endl;
      m_set[i] = vname;
      ix = i;
    }
  }

  bool successful_assignment = false;
  if(ix >= 0) 
    successful_assignment = true;

  if(successful_assignment) {
    m_last_post_assign_utc = 0; // trigger immediate posting

#if 0
    unsigned int index = (unsigned int)(ix);
    if(m_update_var != "") {
      string msg1 = "vx1=";
      msg1 += "x=" + doubleToStringX(m_px1[index],3);
      msg1 += ",y=" + doubleToStringX(m_py1[index],3);
      string msg2 = "vx2=";
      msg2 += "x=" + doubleToStringX(m_px2[index],3);
      msg2 += ",y=" + doubleToStringX(m_py2[index],3);
      string msg = msg1 + "#" + msg2;
      
      string var = m_update_var + "_" + toupper(vname);
      
      Notify(var, msg);
    }
#endif
  }
  else {
    reportRunWarning("Failed to assign: " + vname);
    reportEvent("Failed to assign: " + vname);
  }
}


//------------------------------------------------------------
// Procedure: postAssigments() 

void JoustMgr::postAssignments()
{
  double elapsed = m_curr_time - m_last_post_assign_utc;
  if(elapsed < 15)
    return; 
  
  // sanity check
  if(m_update_var == "")
    return;
  
  for(unsigned int i=0; i<m_px1.size(); i++) {
    string vname = m_set[i];
    if(vname == "")
      continue;

    string msg1 = "vx1=";
    msg1 += "x=" + doubleToStringX(m_px1[i],3);
    msg1 += ",y=" + doubleToStringX(m_py1[i],3);
    string msg2 = "vx2=";
    msg2 += "x=" + doubleToStringX(m_px2[i],3);
    msg2 += ",y=" + doubleToStringX(m_py2[i],3);
    string msg = msg1 + "#" + msg2;
    
    string var = m_update_var + "_" + toupper(vname);
    
    Notify(var, msg);
  }
  m_last_post_assign_utc = m_curr_time;
}


//------------------------------------------------------------
// Procedure: buildReport()

bool JoustMgr::buildReport() 
{
  string circle = "x=" + doubleToStringX(m_center_x,2);
  circle += ", y=" + doubleToStringX(m_center_y,2);
  circle += ", rad=" + doubleToStringX(m_radius,2);
  
  m_msgs << "Configuration:                               " << endl;
  m_msgs << "============================================ " << endl;
  m_msgs << "Ring:       " << circle                        << endl;
  m_msgs << "Update Var: " << m_update_var                  << endl;
  m_msgs << "                                             " << endl;
  m_msgs << "State:                                       " << endl;
  m_msgs << "============================================ " << endl;
  m_msgs << "Known VNames: " << stringSetToString(m_known_vnames) << endl;

  ACTable actab(7,2);
  actab << "VName | Set | Spd | PX1 | PY1 | PX2 | PY2";
  actab.addHeaderLines();
  for(unsigned int i=0; i<m_px1.size(); i++) {
    string nam = m_hint[i];
    string set = m_set[i];
    string spd = doubleToString(m_spd[i],1);
    string px1 = doubleToString(m_px1[i],1);
    string py1 = doubleToString(m_py1[i],1);
    string px2 = doubleToString(m_px2[i],1);
    string py2 = doubleToString(m_py2[i],1);
    actab << nam << set << spd << px1 << py1 << px2 << py2;
  }
  m_msgs << actab.getFormattedString();
    
  return(true);
}
