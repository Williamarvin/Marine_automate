
/* **************************************************************
  NAME: Raymond Turrisi
  ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA
  FILE: BHV_TaskFormationLeader.h
  CIRC: November 2023
  DESC:
    Constructs a bid for the convoy ordering problem with a dubins
    turn. The total odometry of this proposed dubins turn / path
    is the bid.

  LICENSE:
    This is unreleased BETA code. No permission is granted or
    implied to use, copy, modify, and distribute this software
    except by the author(s), or those designated by the author.
************************************************************** */

#ifndef BHV_TASK_WAYPOINT_HEADER
#define BHV_TASK_WAYPOINT_HEADER

#include "IvPTaskBehavior.h"
#include "dubin.h"
#include <string>
#include <vector>

class IvPDomain;
class BHV_TaskFormationLeader : public IvPTaskBehavior {
public:
  BHV_TaskFormationLeader(IvPDomain);
  ~BHV_TaskFormationLeader() {}

  // virtuals defined
  void onHelmStart();
  double getTaskBid();
  bool setParam(std::string, std::string);

  vector<VarDataPair> applyFlagMacros(std::vector<VarDataPair>);

  void onIdleState();
  IvPFunction *onRunState();

protected:
  bool updatePlatformInfo();

protected: // Configuration Parameters
  double m_ptx;
  double m_pty;
  double m_turn_radius;

  bool m_ptx_set;
  bool m_pty_set;
  bool m_turn_radius_set;

  bool m_consider_contacts;

protected: // State Variables
  DubinsPath m_dubin_generator;
};

#ifdef WIN32
// Windows needs to explicitly specify functions to export from a dll
#define IVP_EXPORT_FUNCTION __declspec(dllexport)
#else
#define IVP_EXPORT_FUNCTION
#endif

extern "C" {
IVP_EXPORT_FUNCTION IvPBehavior *createBehavior(std::string name,
                                                IvPDomain domain) {
  return new BHV_TaskFormationLeader(domain);
}
}
#endif
