/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: FldNodeComms.h                                       */
/*    DATE: Dec 4th 2011                                         */
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

#ifndef FLD_NODE_COMMS_HEADER
#define FLD_NODE_COMMS_HEADER

#include "AckMessage.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "NodeMessage.h"
#include "NodeRecord.h"
#include <map>
#include <string>

class FldNodeComms : public AppCastingMOOSApp {
public:
  FldNodeComms();
  virtual ~FldNodeComms() {}

public: // Standard MOOSApp functions to overload
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

protected: // Standard AppCastingMOOSApp function to overload
  bool buildReport();

protected:
  void registerVariables();
  void checkForDeletableNodes();
  bool handleMailNodeReport(const std::string &str, std::string &whynot);
  bool handleMailNodeMessage(const std::string &str, const std::string &src);
  bool handleMailAckMessage(const std::string &str);
  bool handleMailCommsRange(double);
  bool handleMailFullReportReq(std::string);
  bool handleStealth(const std::string &);
  bool handleEarange(const std::string &);
  bool handleEnableSharedNodeReports(std::string);

  void distributeNodeReportInfo(const std::string &uname);
  void localShareNodeReportInfo(const std::string &uname);
  void distributeNodeMessageInfo(const std::string &uname);
  void distributeNodeMessageInfo(std::string src, NodeMessage msg);
  void distributeAckMessageInfo(const std::string &uname);
  void distributeAckMessageInfo(std::string src, AckMessage msg);

  void clearStaleNodes();
  bool meetsDropPercentage();
  bool meetsRangeThresh(const std::string &v1, const std::string &v2);
  bool meetsReportRateThresh(const std::string &v1, const std::string &v2);
  bool meetsCriticalRangeThresh(const std::string &, const std::string &);
  void postViewCommsPulse(const std::string &v1, const std::string &v2,
                          const std::string &pulse_type = "nrep",
                          const std::string &color = "auto",
                          double fill_opaqueness = 0.35);

protected: // Configuration variables
  bool m_verbose;
  bool m_debug;
  bool m_apply_groups;
  bool m_apply_groups_msgs;
  bool m_view_node_rpt_pulses;

  // Default range to source threshold for vehicle to receive
  // node report from a source vehicle.
  double m_comms_range;
  double m_critical_range;
  double m_default_stealth;
  double m_default_earange;
  double m_min_stealth;
  double m_max_earange;
  double m_drop_pct;

  // Added Aug 20th, 2021
  std::string m_ignore_group;

  // The duration in seconds associated the comms pulses posted. A
  // duration of zero means no pulses are to be posted.
  double m_pulse_duration;

  // Seconds after which a vehicle does not received node reports
  // or messages unless a node report has been received from it.
  double m_stale_time;
  double m_stale_forget; // always 20 * m_stale_time

  // Minimum time between message sends (doesn't affect node reports)
  double m_min_msg_interval;
  // Minimum time between message sends (doesn't affect messages)
  double m_min_rpt_interval;

  // Min time between reposting node reports locally, if enabled
  double m_min_share_interval;

  // Maximum length of strings allowed in node messages
  unsigned int m_max_msg_length;

  // Total number of stale vehicles detected and dropped
  unsigned int m_stale_dropped;

  std::map<std::string, double> m_map_stealth; // key:vname
  std::map<std::string, double> m_map_earange; // key:vname

protected: // State variables
  // Holds last node report received for vehicle vname
  std::map<std::string, NodeRecord> m_map_record;
  // Holds last time posted local share, if enabled, for each vname
  std::map<std::string, double> m_map_lshare_tstamp;
  // Holds last node messsages received for vehicle vname
  std::map<std::string, std::vector<NodeMessage>> m_map_message;
  // Holds last ack messsages received for vehicle vname
  std::map<std::string, std::vector<AckMessage>> m_map_ack_message;

  // True if last node report for vehicle vname has not been sent out
  std::map<std::string, bool> m_map_newrecord;
  // True if last node message for vehicle vname has not been sent out
  std::map<std::string, bool> m_map_newmessage;
  // True if last ack message for vehicle vname has not been sent out
  std::map<std::string, bool> m_map_newackmessage;

  // Time node report was last received for vehicle vname
  std::map<std::string, double> m_map_time_nreport;
  // Time node message was last received for vehicle vname
  std::map<std::string, double> m_map_time_nmessage;
  // Time ack message was last received for vehicle vname
  std::map<std::string, double> m_map_time_amessage;

  // Unique index associated with vehicle vname
  std::map<std::string, unsigned int> m_map_vindex;
  // Name of group if any for vehicle vname according to node reports
  std::map<std::string, std::string> m_map_vgroup;

  // Time last node report send from Vehicle A to B. Key is vname:vname2
  std::map<std::string, double> m_map_last_rpt_tstamp;

  std::string m_full_rpt_vname;
  std::set<std::string> m_full_rpt_waiting;

  std::string m_msg_color;
  std::string m_msg_repeat_color;

protected: // State (statistics) variables
  unsigned int m_total_reports_rcvd;
  unsigned int m_total_reports_sent;
  unsigned int m_total_messages_rcvd;
  unsigned int m_total_messages_sent;
  unsigned int m_total_ack_messages_rcvd;
  unsigned int m_total_ack_messages_sent;

  std::map<std::string, unsigned int> m_map_reports_rcvd;
  std::map<std::string, unsigned int> m_map_reports_sent;
  std::map<std::string, unsigned int> m_map_messages_rcvd;
  std::map<std::string, unsigned int> m_map_messages_sent;
  std::map<std::string, unsigned int> m_map_ack_messages_rcvd;
  std::map<std::string, unsigned int> m_map_ack_messages_sent;

  unsigned int m_blk_msg_invalid;
  unsigned int m_blk_msg_toostale;
  unsigned int m_blk_msg_tooquick;
  unsigned int m_blk_msg_toolong;
  unsigned int m_blk_msg_toofar;
  unsigned int m_blk_msg_noinfo;

  std::list<std::string> m_last_messages;

  std::list<std::string> m_recent_ackids;
};

#endif
