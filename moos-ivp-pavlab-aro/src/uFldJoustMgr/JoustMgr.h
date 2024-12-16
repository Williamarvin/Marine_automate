/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT                                             */
/*    FILE: JoustMgr.h                                      */
/*    DATE: Mar 2nd, 2024                                   */
/************************************************************/

#ifndef JOUST_MGR_HEADER
#define JOUST_MGR_HEADER

#include "HintHolder.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class JoustMgr : public AppCastingMOOSApp {
public:
  JoustMgr();
  ~JoustMgr(){};

protected: // Standard MOOSApp functions to overload
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

protected: // Standard AppCastingMOOSApp function to overload
  bool buildReport();

protected:
  void registerVariables();
  bool handleConfigJoustCircle(std::string sval);
  bool handleConfigJoustFile(std::string sval);
  bool handleMailNodeReport(std::string str, std::string &whynot);

protected:
  void makeAssignment(std::string vname);
  void postJoustCircle(bool active = true);

  void postAssignments();

private: // Config vars
  double m_center_x;
  double m_center_y;
  double m_radius;

  HintHolder m_hints;
  std::string m_update_var;

private: // State vars
  bool m_circle_posted;

  std::vector<double> m_px1;
  std::vector<double> m_py1;
  std::vector<double> m_px2;
  std::vector<double> m_py2;
  std::vector<double> m_spd;

  std::vector<std::string> m_hint;
  std::vector<std::string> m_set;

  std::set<std::string> m_known_vnames;

  double m_last_post_circle_utc;
  double m_last_post_assign_utc;
};

#endif
