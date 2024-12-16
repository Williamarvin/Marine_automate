/************************************************************/
/*    NAME: Filip Stromstad                                 */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: DemusterAssign.cpp                              */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#include "DemusterAssign.h"
#include "ACTable.h"
#include "MBUtils.h"
#include <iterator>

#include "MBUtils.h"
#include "NodeRecordUtils.h"

#include "Hungarian.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

DemusterAssign::DemusterAssign() {
  m_type = "line";
  m_CG = Point(0, 0);
  m_anchor = Point(0, 0);
  m_distance = 100;
  m_heading = 150;
  m_margin = 10;

  m_circle_radius = m_distance;
  m_arrow_angle = 90;

  m_assignment_algorithm = "random";
  m_assignment_metric = "distance";

  m_dubin_turn_radius = 10;

  m_calculate_formation = false;
  m_formations_calculated = 0;
  m_formation = {};
  m_node_poses = {};
  m_node_destinations = {};

  m_cost_matrix = {};
  m_total_cost = 0;

  m_arrived_nodes = {};
  m_complete_nodes = {};
  m_complete_timer = -1;

  m_node_deadlocks = {};
  m_node_permablocks = {};
}

//---------------------------------------------------------
// Destructor

DemusterAssign::~DemusterAssign() {}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool DemusterAssign::OnNewMail(MOOSMSG_LIST &NewMail) {
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for (p = NewMail.begin(); p != NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key = msg.GetKey();
    string sval = msg.GetString();
    double dval = msg.GetDouble();
    string comm = msg.GetCommunity();

    if (key == "NODE_REPORT") {
      NodeRecord new_record = string2NodeRecord(sval);
      string unvalid_reason = "";
      if (!new_record.valid("x,y,name,hdg", unvalid_reason)) {
        reportRunWarning("Invalid node report: " + unvalid_reason);
        continue;
      }
      m_node_poses[new_record.getName()] = Pose(
          Point(new_record.getX(), new_record.getY()), new_record.getHeading());
    } else if (key == "DEMUSTER_ASSIGN") {
      m_calculate_formation = true;
    } else if (key == "DUBIN_POINTS_LEFT") {
      if (m_type != "convoy" && m_type != "herringbone") {
        return (true);
      }

      if (dval > 6) {
        return (true);
      }
      // If there are less than x points left, we need to update the convoy
      // formation
      //...find a better solution for this...

      // Check if node already exists in m_arrived_nodes
      string vname = comm;
      if (find(m_arrived_nodes.begin(), m_arrived_nodes.end(), vname) !=
          m_arrived_nodes.end()) {
        return (true);
      }

      // Node does not exist in m_arrived_nodes, add it and update its position
      m_arrived_nodes.push_back(vname);
      int nodes_arrived = m_arrived_nodes.size();

      // If we are making a convoy, assign nodes to new positions when they
      // arrive to the convoy starting point. New points are extended from the
      // CG in the direction of the convoy heading
      Point c_start = m_formation[0].p;
      double c_heading = heading2Radians(m_formation[0].h);
      int N = m_node_poses.size();
      double added_distance = m_margin * (N + 1 - nodes_arrived);
      double new_x = c_start.x + added_distance * cos(c_heading);
      double new_y = c_start.y + added_distance * sin(c_heading);
      string position_update =
          "goal_point=x=" + to_string(new_x) + ",y=" + to_string(new_y);
      string notify_name = "DUBIN_UPDATE_" + toupper(vname);
      Notify(notify_name, position_update);

      if (m_type == "herringbone") {
        // If we are making a herringbone, we need to update the heading as well
        // but not for the first one
        double new_heading = m_formation[0].h;
        if (nodes_arrived > 1) {
          double herring_angle = 60;
          if (nodes_arrived % 2 == 0) {
            new_heading = fmod(new_heading + herring_angle, 360);
          } else {
            new_heading = fmod(new_heading - herring_angle, 360);
          }
        }
        string heading_update = "goal_heading=" + to_string(new_heading);
        Notify(notify_name, heading_update);
      }

      Notify(notify_name, "regenerate_path=true");
    } else if (key == "NEW_ANCHOR_POINT") {
      double x = stod(biteStringX(sval, ','));
      double y = stod(sval);
      m_anchor = Point(x, y);
    } else if (key == "DEMUSTER_CONFIG") {
      if (sval == "type_toggle") {
        // vector<string> types = {"line", "circle", "return", "arrow",
        // "convoy", "herringbone", "random"};
        vector<string> types = {"circle", "random"};
        int index = find(types.begin(), types.end(), m_type) - types.begin();
        m_type = types[(index + 1) % types.size()];
      } else if (sval == "type_circle") {
        m_type = "circle";
      } else if (sval == "type_random") {
        m_type = "random";
      } else if (sval == "algorithm_toggle") {
        vector<string> algorithms = {"random", "greedy", "hungarian_ext"};
        int index =
            find(algorithms.begin(), algorithms.end(), m_assignment_algorithm) -
            algorithms.begin();
        m_assignment_algorithm = algorithms[(index + 1) % algorithms.size()];
      } else if (sval == "metric_toggle") {
        vector<string> metrics = {"distance", "heading"};
        int index = find(metrics.begin(), metrics.end(), m_assignment_metric) -
                    metrics.begin();
        m_assignment_metric = metrics[(index + 1) % metrics.size()];
      } else if (sval == "margin_increase") {
        m_margin += 1;
      } else if (sval == "margin_decrease") {
        m_margin -= 1;
      } else if (sval == "turning_radius_increase") {
        m_dubin_turn_radius += 1;
      } else if (sval == "turning_radius_decrease") {
        m_dubin_turn_radius -= 1;
      }
    } else if (key == "DEADLOCK") {
      if (sval == "1") {
        m_node_deadlocks[comm] = true;
      } else {
        m_node_deadlocks[comm] = false;
      }
    } else if (key == "PERMANENTLY_BLOCKED") {
      if (find(m_node_permablocks.begin(), m_node_permablocks.end(), comm) ==
          m_node_permablocks.end()) {
        m_node_permablocks.push_back(comm);
      }
    } else if (key != "APPCAST_REQ") // handled by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
  }

  return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool DemusterAssign::OnConnectToServer() {
  registerVariables();
  return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool DemusterAssign::Iterate() {
  AppCastingMOOSApp::Iterate();

  if (!m_calculate_formation) {
    if (m_complete_timer > 0) {
      // Already completed, but wait a bit before resuming
      if (MOOSTime() - m_complete_timer > 10) {
        if (m_type == "random") {
          // m_anchor = Point(40, -60);
          m_anchor = Point(0, 0);
          m_heading = 150;
          m_type = "circle";
          Notify("DUBIN_UPDATE_ALL", "only_right_turns=false");
          Notify("DEMUSTER_CONFIG_ALL", "synch_toggle");
          Notify("DEMUSTER_BEGIN", "true");
        } else if (m_type == "circle") {
          m_anchor = Point(20, -30);
          m_type = "random";
          Notify("DUBIN_UPDATE_ALL", "only_right_turns=true");
          Notify("DEMUSTER_CONFIG_ALL", "synch_toggle");
          Notify("DEMUSTER_COMPLETE", "true");
        }
        m_complete_timer = -1;
        m_calculate_formation = true;
      }
      AppCastingMOOSApp::PostReport();
      return (true);
    }

    // Check for arrived nodes
    double arrived_distance = 4;
    for (auto const &node : m_node_destinations) {
      if (find(m_complete_nodes.begin(), m_complete_nodes.end(), node.first) !=
          m_complete_nodes.end()) {
        continue;
      }
      Point node_position = m_node_poses[node.first].p;
      Point node_destination = node.second.p;
      double distance = sqrt(pow(node_position.x - node_destination.x, 2) +
                             pow(node_position.y - node_destination.y, 2));
      if (distance < arrived_distance) {
        m_complete_nodes.push_back(node.first);
      }
    }

    // Check if all nodes have arrived or are permanently blocked
    if ((m_complete_nodes.size() + m_node_permablocks.size()) >=
            m_node_poses.size() &&
        m_complete_nodes.size() > 0) {
      Notify("DEMUSTER_ALL", "false");
      Notify("STATION_KEEP_ALL", "true");
      m_complete_timer = MOOSTime();
    }

    AppCastingMOOSApp::PostReport();
    return (true);
  }

  m_calculate_formation = false;
  m_formations_calculated++;
  m_complete_nodes = {};
  m_node_permablocks = {};
  m_arrived_nodes = {};
  m_node_destinations = {};

  vector<Point> points;
  for (auto const &node : m_node_poses) {
    points.push_back(node.second.p);
  }

  m_formation = calculateFormation(points, m_distance, m_heading, m_margin,
                                   m_circle_radius, m_arrow_angle, m_type, m_CG,
                                   m_anchor);

  m_total_cost = 0;

  // Assign each node to a position in the formation and notify them
  if (m_assignment_algorithm == "random") {
    int i = 0;
    for (auto const &node : m_node_poses) {
      Point node_position = m_formation[i].p;
      double node_heading = m_formation[i].h;
      m_node_destinations[node.first] = Pose(node_position, node_heading);
      string position_update = "goal_point=x=" + to_string(node_position.x) +
                               ",y=" + to_string(node_position.y);
      string heading_update = "goal_heading=" + to_string(node_heading);
      string notify_name = "DUBIN_UPDATE_" + toupper(node.first);
      Notify(notify_name, position_update);
      Notify(notify_name, heading_update);
      i++;
    }
  } else if (m_assignment_algorithm == "greedy") {
    std::map<std::string, Pose> unassigned_nodes = m_node_poses;

    for (int i = 0; i < m_formation.size(); i++) {
      double min_dist = 1000000;
      string closest_node = "";
      for (auto const &node : unassigned_nodes) {
        Point start_point = node.second.p;
        double start_heading = node.second.h;
        Point end_point = m_formation[i].p;
        double end_heading = m_formation[i].h;
        double r = m_dubin_turn_radius;
        vector<string> illegal_paths = {""};

        DubinsPath dp = DubinsPath();
        dp.findOptimalPath(start_point, start_heading, end_point, end_heading,
                           r, r, r, illegal_paths);
        double dist = dp.m_length;

        if (dist < min_dist) {
          min_dist = dist;
          closest_node = node.first;
        }
      }
      unassigned_nodes.erase(closest_node);
      m_total_cost += min_dist;

      Point node_position = m_formation[i].p;
      double node_heading = m_formation[i].h;
      m_node_destinations[closest_node] = Pose(node_position, node_heading);
      string position_update = "goal_point=x=" + to_string(node_position.x) +
                               ",y=" + to_string(node_position.y);
      string heading_update = "goal_heading=" + to_string(node_heading);
      string notify_name = "DUBIN_UPDATE_" + toupper(closest_node);
      Notify(notify_name, position_update);
      Notify(notify_name, heading_update);
    }
  } else if (m_assignment_algorithm == "hungarian_ext") {
    vector<vector<double>> costMatrix;
    vector<string> node_names_ordered;
    for (auto const &node : m_node_poses) {
      vector<double> row;
      node_names_ordered.push_back(node.first);
      for (int i = 0; i < m_formation.size(); i++) {
        if (m_assignment_metric == "distance") {
          Point start_point = node.second.p;
          double start_heading = node.second.h;
          Point end_point = m_formation[i].p;
          double end_heading = m_formation[i].h;
          double r = m_dubin_turn_radius;
          vector<string> illegal_paths = {""};

          DubinsPath dp = DubinsPath();
          dp.findOptimalPath(start_point, start_heading, end_point, end_heading,
                             r, r, r, illegal_paths);
          double dist = dp.m_length;
          row.push_back(dist);
        } else if (m_assignment_metric == "heading") {
          double start_heading = node.second.h;
          double heading_btw_start_end =
              atan2(m_formation[i].p.y - node.second.p.y,
                    m_formation[i].p.x - node.second.p.x) *
              180 / M_PI;
          double heading_diff = abs(start_heading - heading_btw_start_end);
          if (heading_diff > 180) {
            heading_diff = 360 - heading_diff;
          }
          row.push_back(heading_diff);
        }
      }
      costMatrix.push_back(row);
    }

    m_cost_matrix = costMatrix;

    HungarianAlgorithm HungAlgo = HungarianAlgorithm();
    vector<int> assignment;
    double cost = HungAlgo.Solve(costMatrix, assignment);

    for (unsigned int x = 0; x < node_names_ordered.size(); x++) {
      m_total_cost += costMatrix[x][assignment[x]];
      Point node_position = m_formation[assignment[x]].p;
      double node_heading = m_formation[assignment[x]].h;
      m_node_destinations[node_names_ordered[x]] =
          Pose(node_position, node_heading);
      string position_update = "goal_point=x=" + to_string(node_position.x) +
                               ",y=" + to_string(node_position.y);
      string heading_update = "goal_heading=" + to_string(node_heading);
      string notify_name = "DUBIN_UPDATE_" + toupper(node_names_ordered[x]);
      Notify(notify_name, position_update);
      Notify(notify_name, heading_update);
      Notify(notify_name, "regenerate_path=true");
    }
  }

  Notify("DEMUSTER_ALL", "true");
  Notify("STATION_KEEP_ALL", "false");
  Notify("MOOS_MANUAL_OVERRIDE_ALL", "false");
  // Notify("DEMUSTER_BEGIN", "true");

  AppCastingMOOSApp::PostReport();
  return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool DemusterAssign::OnStartUp() {
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if (!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for (p = sParams.begin(); p != sParams.end(); p++) {
    string orig = *p;
    string line = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if (param == "distance") {
      m_distance = stod(value);
      handled = true;
    } else if (param == "heading") {
      m_heading = stod(value);
      handled = true;
    } else if (param == "margin") {
      m_margin = stod(value);
      handled = true;
    } else if (param == "circle_radius") {
      m_circle_radius = stod(value);
      handled = true;
    } else if (param == "arrow_angle") {
      m_arrow_angle = stod(value);
      handled = true;
    } else if (param == "type") {
      m_type = value;
      handled = true;
    } else if (param == "assignment_algorithm") {
      m_assignment_algorithm = value;
      handled = true;
    } else if (param == "assignment_metric") {
      m_assignment_metric = value;
      handled = true;
    } else if (param == "origin") {
      m_CG = Point(stod(toupper(biteStringX(value, ','))), stod(value));
      handled = true;
    } else if (param == "demuster_point") {
      m_anchor = Point(stod(toupper(biteStringX(value, ','))), stod(value));
      handled = true;
    } else if (param == "dubin_turn_radius") {
      m_dubin_turn_radius = stod(value);
      handled = true;
    }

    if (!handled)
      reportUnhandledConfigWarning(orig);
  }

  registerVariables();
  return (true);
}

//---------------------------------------------------------
// Procedure: registerVariables()

void DemusterAssign::registerVariables() {
  AppCastingMOOSApp::RegisterVariables();
  Register("NODE_REPORT", 0);
  Register("DEMUSTER_ASSIGN", 0);
  Register("DUBIN_POINTS_LEFT", 0);
  Register("NEW_ANCHOR_POINT", 0);
  Register("DEMUSTER_CONFIG", 0);
  Register("DEADLOCK", 0);
  Register("PERMANENTLY_BLOCKED", 0);
}

//------------------------------------------------------------
// Procedure: buildReport()

bool DemusterAssign::buildReport() {
  m_msgs << "============================================" << endl;
  m_msgs << "File: DemusterAssign.cpp                    " << endl;
  m_msgs << "============================================" << endl;

  m_msgs << "Configuration parameters: " << endl;
  m_msgs << "  Type: " << m_type << endl;
  m_msgs << "  Margin: " << m_margin << endl;
  m_msgs << "  Center of gravity: " << m_CG.x << ", " << m_CG.y << endl;
  m_msgs << "  Anchor point: " << m_anchor.x << ", " << m_anchor.y << endl;
  m_msgs << "Default parameters: " << endl;
  m_msgs << "  Default distance: " << m_distance << endl;
  m_msgs << "  Default Heading: " << m_heading << endl;
  m_msgs << "Type-specific parameters: " << endl;
  m_msgs << "  Circle radius: " << m_circle_radius << endl;
  m_msgs << "  Arrow angle: " << m_arrow_angle << endl;

  m_msgs << "============================================" << endl;
  m_msgs << "Assignment algorithm: " << m_assignment_algorithm << endl;
  m_msgs << "Assignment metric: " << m_assignment_metric << endl;
  m_msgs << "Turn radius: " << m_dubin_turn_radius << endl;
  m_msgs << "Formation calculated: " << m_formations_calculated << endl;
  // m_msgs << "Total cost: " << m_total_cost << endl;
  // m_msgs << "Cost matrix: " << endl;
  // for(int i = 0; i < m_cost_matrix.size(); i++) {
  //   for(int j = 0; j < m_cost_matrix[i].size(); j++) {
  //     m_msgs << m_cost_matrix[i][j] << " ";
  //   }
  //   m_msgs << endl;
  // }

  m_msgs << "============================================" << endl;
  m_msgs << "Formation: " << endl;
  for (int i = 0; i < m_formation.size(); i++) {
    m_msgs << "  " << m_formation[i].p.x << ", " << m_formation[i].p.y << ", "
           << m_formation[i].h << endl;
  }

  m_msgs << "============================================" << endl;
  m_msgs << "Deadlocks: " << endl;
  for (auto const &deadlock : m_node_deadlocks) {
    m_msgs << "  " << deadlock.first << ": " << deadlock.second << endl;
  }

  m_msgs << "============================================" << endl;
  m_msgs << "Completed nodes: " << endl;
  for (int i = 0; i < m_complete_nodes.size(); i++) {
    m_msgs << "  " << m_complete_nodes[i] << ", ";
  }
  m_msgs << endl;
  m_msgs << "Permanently blocked nodes: " << endl;
  for (int i = 0; i < m_node_permablocks.size(); i++) {
    m_msgs << "  " << m_node_permablocks[i] << ", ";
  }
  m_msgs << endl;
  m_msgs << "Completed timer: " << m_complete_timer << endl;

  return (true);
}
