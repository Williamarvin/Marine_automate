/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: NodeMessage.h                                        */
/*    DATE: Jan 7th 2011                                         */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef UFLD_NODE_MESSAGE_HEADER
#define UFLD_NODE_MESSAGE_HEADER

#include <string>

class NodeMessage {
public:
  NodeMessage(std::string src = "", std::string dest = "",
              std::string var = "");
  ~NodeMessage() {}

  void setSourceNode(const std::string &s) { m_src_node = s; }
  void setDestNode(const std::string &s) { m_dest_node = s; }
  void setDestGroup(const std::string &s) { m_dest_group = s; }
  void setVarName(const std::string &s) { m_var_name = s; }
  void setStringVal(const std::string &);
  void setColor(const std::string &);
  void setSourceApp(const std::string &s) { m_src_app = s; }
  void setSourceBehavior(const std::string &s) { m_src_bhv = s; }
  void setMessageID(const std::string &s) { m_msg_id = s; }
  void setAckRequested(bool v = true) { m_ack_requested = v; }

  void setDoubleVal(double v) {
    m_double_val = v;
    m_double_val_set = true;
  }

  std::string getSourceNode() const { return (m_src_node); }
  std::string getDestNode() const { return (m_dest_node); }
  std::string getDestGroup() const { return (m_dest_group); }
  std::string getVarName() const { return (m_var_name); }
  double getDoubleVal() const { return (m_double_val); }
  std::string getColor() const { return (m_color); }
  std::string getSourceApp() const { return (m_src_app); }
  std::string getSourceBehavior() const { return (m_src_bhv); }
  std::string getMessageID() const { return (m_msg_id); }
  std::string getStringVal() const { return (m_string_val); }
  std::string getStringValX() const;

  bool getAckRequested() const { return (m_ack_requested); }

  unsigned int length() const;

  bool valid() const;

  std::string getSpec(bool include_ack_info = true) const;

protected:
  std::string m_src_node;
  std::string m_dest_node;
  std::string m_dest_group;
  std::string m_var_name;
  std::string m_string_val;
  double m_double_val;
  bool m_double_val_set;
  std::string m_src_app;
  std::string m_src_bhv;
  std::string m_color;
  std::string m_msg_id;
  bool m_ack_requested;
};

#endif
