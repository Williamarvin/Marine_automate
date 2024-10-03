/************************************************************/
/*    NAME: Filip Stromstad                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: DynamicTrafficLight.cpp                                        */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#include <iterator>
#include <algorithm>
#include "MBUtils.h"
#include "ACTable.h"
#include "DynamicTrafficLight.h"
#include "NodeRecord.h"
#include "NodeMessage.h" // In the lib_ufield library
#include "XYFormatUtilsSegl.h"
#include "XYFormatUtilsPoly.h"
#include "XYVector.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

DynamicTrafficLight::DynamicTrafficLight()
{
  m_name_os = "";
  m_precision = 2;
  m_default_speed = 1.0;

  m_max_safety_dist = 4;
  m_min_safety_dist = 2;
  m_max_horizon_meters = 10;
  m_min_horizon_meters = 2;
  m_show_visualization = false;
  m_synchronize = false;

  m_trajectory_map = {};
  m_speed_map = {};
  m_block_matrix = {};
  
  m_handshakes = {};
  m_nodes_in_proximity = {};
  m_proximity_range = 10;
  m_os_trajectory_number = 0;
  m_known_traj_numbers = {};

  m_nav_x = 0;
  m_nav_y = 0;
  m_nav_heading = 0;
  m_compass_heading = 0;
  m_compass_declination = 0;
  m_use_compass_heading = false;
  m_nav_speed = 0;
  
  m_potential_permablock_timer = -1;
  m_potential_deadlock_timer = -1;
  m_deadlock_timer = -1;

  m_reverse_thrust = -30;
}

//---------------------------------------------------------
// Destructor

DynamicTrafficLight::~DynamicTrafficLight()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool DynamicTrafficLight::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    string sval  = msg.GetString(); 
    double mtime = msg.GetTime();
    double dval  = msg.GetDouble();

    if(key == "TRAJECTORY_MSG_LOCAL") {
      XYSegList temp_seglist = string2SegList(sval);
      string label = temp_seglist.get_label(); //label set previously to "osname_dubin_#oftrajectories"
      int traj_number = stoi(label.substr(label.find_last_of("_") + 1));

      //Higher trajectory number --> newer trajectory
      if (traj_number > m_os_trajectory_number) {
        m_os_trajectory_number = traj_number;
        m_handshakes.clear();
      }

      m_trajectory_map[m_name_os] = temp_seglist;

      NodeMessage node_msg;
      node_msg.setSourceNode(m_name_os);
      node_msg.setDestNode("all");
      node_msg.setVarName("TRAJECTORY_MSG");
      node_msg.setStringVal(temp_seglist.get_spec());
      Notify("NODE_MESSAGE_LOCAL", node_msg.getSpec());
     } 
     else if(key == "TRAJECTORY_MSG") {
       XYSegList temp_seglist = string2SegList(sval);
       string traj_label = temp_seglist.get_label(); //label on the form "osname_dubin_#oftrajectories"
       string sender_name = traj_label.substr(0, traj_label.find_first_of("_"));
       string traj_number = traj_label.substr(traj_label.find_last_of("_") + 1);

       m_trajectory_map[sender_name] = temp_seglist;
       m_known_traj_numbers[sender_name] = traj_number;

      //  Check if sender is in proximity, position is the first point in the trajectory
       XYPoint position = temp_seglist.get_first_point();
       double dist = sqrt(pow(position.x()-m_nav_x, 2) + pow(position.y()-m_nav_y, 2));
       if (dist < m_proximity_range && find(m_nodes_in_proximity.begin(), m_nodes_in_proximity.end(), sender_name) == m_nodes_in_proximity.end()) {
         m_nodes_in_proximity.push_back(sender_name);
       } else if (dist >= m_proximity_range && find(m_nodes_in_proximity.begin(), m_nodes_in_proximity.end(), sender_name) != m_nodes_in_proximity.end()) {
         m_nodes_in_proximity.erase(remove(m_nodes_in_proximity.begin(), m_nodes_in_proximity.end(), sender_name), m_nodes_in_proximity.end());
       }
     } 
     else if (key == "TRAJECTORY_HANDSHAKE"){
        string sender_name = parseString(sval, ',')[0];
        int traj_number = stoi(parseString(sval, ',')[1]);
        bool sender_in_handshakes = find(m_handshakes.begin(), m_handshakes.end(), sender_name) != m_handshakes.end();
        bool traj_number_most_recent = (traj_number >= m_os_trajectory_number); //should not be possible to be greater than

        if (!sender_in_handshakes && traj_number_most_recent) {
          m_handshakes.push_back(sender_name);
        }
     }
     else if(key == "SPEED_MSG") {
        vector<string> parts = parseString(sval, ',');
        m_speed_map[parts[0]] = stod(parts[1]);
     }
     else if(key == "NAV_X") {
       m_nav_x = dval;
     } 
     else if(key == "NAV_Y") {
       m_nav_y = dval;
     }
     else if (key == "NAV_HEADING") {
       m_nav_heading = dval;
     }
     else if (key == "COMPASS_HEADING_RAW") {
       m_compass_heading = dval + m_compass_declination;
       
       if (m_compass_heading < 0) {
         m_compass_heading += 360;
       } else if (m_compass_heading >= 360) {
         m_compass_heading -= 360;
       }
       
     }
     else if(key == "NAV_SPEED") {
       m_nav_speed = dval;
       m_speed_map[m_name_os] = dval;

       NodeMessage node_msg;
       node_msg.setSourceNode(m_name_os);
       node_msg.setDestNode("all");
       node_msg.setVarName("SPEED_MSG");
       node_msg.setStringVal(m_name_os + "," + to_string(dval));
       Notify("NODE_MESSAGE_LOCAL", node_msg.getSpec());
     }
     else if(key == "DEMUSTER_CONFIG"){
        if (sval == "synch_toggle"){
          m_synchronize = !m_synchronize;
        } 
        else if (sval == "safety_distance_increase"){
          m_max_safety_dist += 1;
        } 
        else if (sval == "safety_distance_decrease"){
          m_max_safety_dist -= 1;
        }
        else if (sval == "safety_horizon_increase"){
          m_max_horizon_meters += 1;
        } 
        else if (sval == "safety_horizon_decrease"){
          m_max_horizon_meters -= 1;
        }
        else if (sval == "speed_increase"){
          m_default_speed += 0.1;
        } 
        else if (sval == "speed_decrease"){
          m_default_speed -= 0.1;
        }
     }
     else if(key != "APPCAST_REQ"){ // handled by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
     }
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool DynamicTrafficLight::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool DynamicTrafficLight::Iterate()
{
  AppCastingMOOSApp::Iterate();

  //Send out handshakes

  for (auto const& node : m_known_traj_numbers) { //TODO: could reduce the frequency of this
      NodeMessage node_msg;
      node_msg.setSourceNode(m_name_os);
      node_msg.setDestNode(node.first);
      node_msg.setVarName("TRAJECTORY_HANDSHAKE");
      node_msg.setStringVal(m_name_os + "," + node.second);
      Notify("NODE_MESSAGE_LOCAL", node_msg.getSpec());
  }

  bool break_early = false;
  if (m_trajectory_map.find(m_name_os) == m_trajectory_map.end()){ //If no trajectory, stop
    break_early = true;
  } 
  else if (m_trajectory_map[m_name_os].size() == 0) { //No os trajectory, stop
    break_early = true;
  }
  // else if (m_handshakes.size() < m_known_traj_numbers.size()) { //Wait until all handshook
  //   break_early = true;
  // }
  else if (m_trajectory_map[m_name_os].size() <= 1) {
    //os has arrived at the last node or has no nodes
    break_early = true;
  }

  // check if all nodes in proximity have handshook
  for (auto const& node : m_nodes_in_proximity) {
    if (find(m_handshakes.begin(), m_handshakes.end(), node) == m_handshakes.end()) {
      break_early = true;
      break;
    }
  }

  if (break_early){
    Notify("DUBIN_UPDATE", "speed=0");

    if (m_show_visualization){
      string label = "safety_" + m_name_os;
      XYPolygon poly = XYPolygon(0, 0, 10, 10, label);
      poly.set_active(false);
      Notify("VIEW_POLYGON", poly.get_spec());
      XYSegList viz_seglist = XYSegList("traj_" + m_name_os);
      Notify("VIEW_SEGLIST", viz_seglist.get_spec_inactive());
    }

    AppCastingMOOSApp::PostReport();
    return(true);
  }


  // Deadlock handling
  if (m_deadlock_timer != -1){
    if (MOOSTime() - m_deadlock_timer < 10){
      //Deadlocked

      string label = "safety_" + m_name_os;
      // Make a squarepolygon right behind ownship
      double poly_radius = 10;

      double os_heading_rad = 0;
      double vector_angle = 0;

      if (m_use_compass_heading){
        os_heading_rad = (90 - m_compass_heading) * M_PI / 180;
        vector_angle = m_compass_heading;
      } else {
        os_heading_rad = (90 - m_nav_heading) * M_PI / 180;
        vector_angle = m_nav_heading;
      }

      // TODO: Unify this with the other arrow...
      XYVector os_heading_vec = XYVector(m_nav_x, m_nav_y, 5, vector_angle);
      os_heading_vec.set_label(m_name_os + "_heading");
      os_heading_vec.setHeadSize(1);
      os_heading_vec.set_color("edge", "magenta");
      os_heading_vec.set_color("fill", "magenta");
      os_heading_vec.set_color("vertex", "magenta");
      Notify("VIEW_VECTOR", os_heading_vec.get_spec());


      // double lang = m_nav_heading -180 + 80;
      // double rang = m_nav_heading -180 - 80;

      // XYPolygon poly = string2Poly("format=wedge, x=0, y=0, lang=40, rang=60, range=10, snap=1");
      // XYPolygon poly = string2Poly("format=wedge, x=" + to_string(m_nav_x) + ", y=" + to_string(m_nav_y) + ", range=" + to_string(poly_radius) + ", lang=" + to_string(lang) + ", rang=" + to_string(rang) + ", pts=10");
      XYPolygon poly = XYPolygon();
      poly.set_label(label);
      poly.add_vertex(m_nav_x, m_nav_y);

      int lim = 8;
      for (int i = -lim; i <= lim; i++){ //Between -lim*10 and lim*10 degrees
        double relative_angle = 10 * i * M_PI / 180;
        double angle_1 = os_heading_rad - M_PI + relative_angle;
        double poly_x_1 = m_nav_x + poly_radius * cos(angle_1);
        double poly_y_1 = m_nav_y + poly_radius * sin(angle_1);
        poly.add_vertex(poly_x_1, poly_y_1);
      }

      if (!poly.is_convex()){
        //Will not be convex if relative angle is >=90 (= beacuse of rounding errors)
        reportRunWarning("Polygon is not convex");
      }

      //Check if any of the other vehicles are in the polygon
      bool blocked = false;
      for (auto const& vehicle : m_trajectory_map) {
        string name = vehicle.first;
        if (name == m_name_os) {
          continue;
        }
        XYSegList seglist = vehicle.second;
        XYPoint position = seglist.get_first_point();
        if (poly.contains(position.x(), position.y())) {
          blocked = true;
          break;
        }
      }


      string viz_color = "";
      if (blocked){
        //Reset the deadlock timer
        Notify("DESIRED_THRUST", 0.0);
        // m_deadlock_timer = MOOSTime();
        viz_color = "red";
      } 
      else {
        Notify("DESIRED_THRUST", m_reverse_thrust);
        viz_color = "green";
      }

      if (m_show_visualization){
        poly.set_edge_color(viz_color);
        poly.set_vertex_color(viz_color);
        poly.set_label_color("transparent");
        Notify("VIEW_POLYGON", poly.get_spec());
      }


    } else  {
      //Remove os trajectory from trajectory map
      m_trajectory_map.erase(m_name_os);
      m_deadlock_timer = -1;
      Notify("MOOS_MANUAL_OVERRIDE", "false");
      Notify("DUBIN_UPDATE", "regenerate_path=true");
    }

    AppCastingMOOSApp::PostReport();
    return(true);
  }

  //TODO:
  // Only check os first
  // If total status is blocked or unblocked, no need to check the rest...
  // If total status is temp_blocked, check all other vehicles to make final decision

  //double for-loop to compare all trajectories to each other
  string horizon_color = "green";
  for (auto const& vehicle_i : m_trajectory_map) { //For all vehicles...
    string name_i = vehicle_i.first;
    XYSegList seglist_i = vehicle_i.second;
    double speed_i = m_speed_map[name_i];
    double dynamic_safe_dist_i = m_min_safety_dist + min(pow((speed_i/m_default_speed),2),1.0) * (m_max_safety_dist - m_min_safety_dist);
    double dynamic_horizon_i_meters = m_min_horizon_meters + min(speed_i/m_default_speed, 1.0) * (m_max_horizon_meters - m_min_horizon_meters);
    int dynamic_horizon_i_points = round(dynamic_horizon_i_meters / m_precision);
    int horizon_i = min(dynamic_horizon_i_points, int(m_trajectory_map[name_i].size()));

    for (auto const& vehicle_j : m_trajectory_map) { //..check block status with all other vehicles
      string name_j = vehicle_j.first;
      if (name_i == name_j) {
        m_block_matrix[name_i][name_j] = UNBLOCKED; //Cannot be blocked by itself
        continue;
      }
      XYSegList seglist_j = vehicle_j.second;
      double speed_j = m_speed_map[name_j];
      double dynamic_safe_dist_j = m_min_safety_dist + min(pow((speed_j/m_default_speed),2),1.0)* (m_max_safety_dist - m_min_safety_dist);
      double largest_safe_dist = max(dynamic_safe_dist_i, dynamic_safe_dist_j);
      double dynamic_horizon_j_meters = m_min_horizon_meters + min(speed_j/m_default_speed, 1.0) * (m_max_horizon_meters - m_min_horizon_meters);
      int dynamic_horizon_j_points = round(dynamic_horizon_j_meters / m_precision);
      int horizon_j = min(dynamic_horizon_j_points, int(m_trajectory_map[name_j].size()));

      //----------1. Check if vehicle i is blocked by vehicle j----------
      bool physically_blocked = false;
      XYPoint position_j = seglist_j.get_first_point();

      for (int i = 0; i < horizon_i; i++) { //For all points in trajectory i
        XYPoint point_i = seglist_i.get_point(i);
        double dist = sqrt(pow(point_i.x()-position_j.x(), 2) + pow(point_i.y()-position_j.y(), 2));
        if (dist < largest_safe_dist) {
          physically_blocked = true;
          break;
        }
      }

      if (physically_blocked) {
        m_block_matrix[name_i][name_j] = PHYSICALLY_BLOCKED;
        if (name_i == m_name_os) {
          horizon_color = "red";
        }
        continue;
      }

      //----------2. Check if vehicle i is temporarily blocked by vehicle j----------
      bool temp_blocked = false;
      for (int i = 0; i < horizon_i; i++) { //For all points in trajectory i
        XYPoint point_i = seglist_i.get_point(i);
        for (int j = 0; j < horizon_j; j++) { //For all points in trajectory j
          XYPoint point_j = seglist_j.get_point(j);
          double dist = sqrt(pow(point_i.x()-point_j.x(), 2) + pow(point_i.y()-point_j.y(), 2));
          if (dist < largest_safe_dist) {
            temp_blocked = true;
            break;
          }
        }
        if (temp_blocked) {
          break;
        }
      }

      if (temp_blocked) {
          m_block_matrix[name_i][name_j] = TEMP_BLOCKED;
          if (horizon_color != "red" && name_i == m_name_os) { //Phys. blocked has priority
            horizon_color = "orange";
          }
          //Decide on the actual blockage here...
      } else {
        m_block_matrix[name_i][name_j] = UNBLOCKED;
      }

    }
  }

  //----------3. For all temp blocked, decide on the actual blockage----------
  for (auto const& block_map : m_block_matrix) {
    string current = block_map.first;
    for (auto const& blockage : block_map.second) {
      string other = blockage.first;
      if (other == "total" || other == current) {
        continue;
      }
      if (blockage.second != TEMP_BLOCKED) {
        continue;
      }
      
      BlockStatus current_to_other = UNBLOCKED;
      if (m_block_matrix[other][current] == PHYSICALLY_BLOCKED) { //other one is physically blocked (or previously set to blocked bc of priority)
        //Current one is unblocked by the other one because it is physically blocking it
      } 
      else {
        int decision_var = current.compare(other);
        if (decision_var < 0) {
          //Current one is unblocked by the other one because of priority
        } 
        else {
          //Current one is blocked by the other one because of priority
          current_to_other = PHYSICALLY_BLOCKED;
        }
      }
      m_block_matrix[current][other] = current_to_other;
    }
  }  

  //----------4. Decide on the total blockstatus of each ----------
  //TODO: ONLY NEED TO FIND MY OWN TOTAL STATUS? COULD REMMOVE FROM MATRIX AND JUST HAVE A VARIABLE?
  for (auto const& block_map : m_block_matrix) {
    string name = block_map.first;
    BlockStatus total_status = UNBLOCKED;
    for (auto const& blockage : block_map.second) {
      if (blockage.first == "total") {
        continue;
      }

      BlockStatus status = blockage.second;
      if (status == PHYSICALLY_BLOCKED) {
        total_status = PHYSICALLY_BLOCKED;
        break;
      } else if (status == TEMP_BLOCKED) {
        total_status = TEMP_BLOCKED;
      }
    }
    m_block_matrix[name]["total"] = total_status;
  }  

  // if (m_block_matrix[m_name_os]["total"] == TEMP_BLOCKED) {
  //   m_block_matrix[m_name_os]["total"] = UNBLOCKED; //assume unblocked until proven otherwise
  //   for (auto const& blockage : m_block_matrix[m_name_os]) {
  //     if (blockage.first == "total") {
  //       continue;
  //     }
  //     if (blockage.second != TEMP_BLOCKED) {
  //       continue;
  //     }

  //     if (m_block_matrix[blockage.first][m_name_os] == PHYSICALLY_BLOCKED) { //they are physically blocked by me
  //       //continue assuming unblocked
  //     } 
  //     else {
  //       int decision_var = m_name_os.compare(blockage.first);
  //       if (decision_var < 0) {
  //         // continue assuming unblocked
  //       } else {
  //         m_block_matrix[m_name_os]["total"] = PHYSICALLY_BLOCKED;
  //         m_block_matrix[m_name_os][blockage.first] = PHYSICALLY_BLOCKED;
  //         //Also set the other to physically blocked for deadlock detection
  //         // m_block_matrix[blockage.first][m_name_os] = PHYSICALLY_BLOCKED;
  //         break;
  //       }
  //     }
  //   }
  // }

  double synch_factor = 1;
  if (m_synchronize) {
    //find the average total pathlength left for each vehicle
    double total_pathlength = 0;
    for (auto const& vehicle : m_trajectory_map) {
      XYSegList seglist = vehicle.second;
      total_pathlength += seglist.length();
    }
    double average_pathlength = total_pathlength / m_trajectory_map.size();
    double os_pathlength = m_trajectory_map[m_name_os].length();
    synch_factor =  min(os_pathlength / average_pathlength, 1.4);
    synch_factor =  max(synch_factor, 0.1);
  }

  //----------5. Send speed command to vehicles based on blockstatus----------
  string viz_color = ""; //TODO: MOVE THIS UP SO ITS GLOBAL AND DECIDE IN THE MAIN LOOP. START GREEN, IF TEMP-ORANGE, IF PHYSICALLY-RED
  if (m_block_matrix[m_name_os]["total"] == PHYSICALLY_BLOCKED) {
    Notify("DUBIN_UPDATE", "speed=0");
    viz_color = "red";
  } else {
    Notify("DUBIN_UPDATE", "speed="+doubleToString(m_default_speed*synch_factor)); //Unblocked
    viz_color = "green";
  }
  //Should not be possible to be temp_blocked here

  //Deadlock detection
  bool potential_permablock = false;
  bool potential_deadlock = false;
  if (m_block_matrix[m_name_os]["total"] == PHYSICALLY_BLOCKED) {
    vector<string> visited;
    vector<string> recursive_stack;
    potential_deadlock = depthFirstSearch(m_name_os, m_block_matrix, visited, recursive_stack);
    potential_permablock = isPermablocked(m_name_os, m_block_matrix, m_trajectory_map);
  }

  if (potential_permablock){
    potential_deadlock = false; //Permablocked has priority
    if (m_potential_permablock_timer == -1){
      m_potential_permablock_timer = MOOSTime();
    } 
    else if (MOOSTime() - m_potential_permablock_timer > 10) {
      //Notify shoreside
      Notify("PERMANENTLY_BLOCKED", "true");
      Notify("DUBIN_UPDATE", "speed=0");

      XYSegList output_seglist;
      output_seglist.add_vertex(m_nav_x, m_nav_y);
      output_seglist.set_label(m_name_os + "_dubin_" + to_string(m_os_trajectory_number+1));

      NodeMessage node_msg;
      node_msg.setSourceNode(m_name_os);
      node_msg.setDestNode("all");
      node_msg.setVarName("TRAJECTORY_MSG");
      node_msg.setStringVal(output_seglist.get_spec());
      Notify("NODE_MESSAGE_LOCAL", node_msg.getSpec());
    }
  } else { 
    m_potential_permablock_timer = -1;
  }

  if (potential_deadlock){
    if (m_potential_deadlock_timer == -1){
      m_potential_deadlock_timer = MOOSTime();
    } 
    else if (MOOSTime() - m_potential_deadlock_timer > 10) {
      m_deadlock_timer = MOOSTime();
      Notify("MOOS_MANUAL_OVERRIDE", "true");
    }
  } else {
    m_potential_deadlock_timer = -1;
  }

  
  if (m_show_visualization){
    //Vizualise safety radius
    string label = "safety_" + m_name_os;
    double dynamic_safety_dist = m_min_safety_dist + min(pow((m_nav_speed/m_default_speed),2),1.0) * (m_max_safety_dist - m_min_safety_dist);
    XYPolygon poly = XYPolygon(m_nav_x, m_nav_y, dynamic_safety_dist, 10, label);
    poly.set_edge_color(viz_color);
    poly.set_vertex_color(viz_color);
    poly.set_label_color("transparent");
    Notify("VIEW_POLYGON", poly.get_spec());

    //Visualize the horizon trajectory
    double dynamic_horizon_os_meters = m_min_horizon_meters + min(m_nav_speed/m_default_speed, 1.0) * (m_max_horizon_meters - m_min_horizon_meters);
    int dynamic_horizon_os_points = round(dynamic_horizon_os_meters / m_precision);
    int horizon = min(dynamic_horizon_os_points, int(m_trajectory_map[m_name_os].size()));
    XYSegList viz_seglist = XYSegList("traj_" + m_name_os);
    for (int i = 1; i < horizon; i++) {
      viz_seglist.add_vertex(m_trajectory_map[m_name_os].get_point(i));
    }

    viz_seglist.set_edge_color(horizon_color);
    viz_seglist.set_edge_size(3);
    Notify("VIEW_SEGLIST", viz_seglist.get_spec());
  }

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool DynamicTrafficLight::OnStartUp()
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
    if(param == "vname") {
      m_name_os = value;
      handled = true;
    } else if (param == "default_speed") {
      m_default_speed = stod(value);
      handled = true;
    } else if (param == "precision") {
      m_precision = stod(value);
      handled = true;
    } else if (param == "max_safety_distance") {
      m_max_safety_dist = stoi(value);
      handled = true;
    } else if (param == "min_safety_distance") {
      m_min_safety_dist = stoi(value);
      handled = true;
    } else if (param == "max_horizon") {
      m_max_horizon_meters = stoi(value);
      handled = true;
    } else if (param == "min_horizon") {
      m_min_horizon_meters = stoi(value);
      handled = true;
    } else if (param == "show_visualization") {
      m_show_visualization = (value == "true");
      handled = true;
    } else if (param == "synchronize") {
      m_synchronize = (value == "true");
      handled = true;
    } else if (param == "reverse_thrust"){
      m_reverse_thrust = stod(value);
      handled = true;
    } else if (param == "compass_declination"){
      m_compass_declination = stod(value);
      handled = true;
    } else if (param == "use_compass_heading"){
      m_use_compass_heading = (value == "true");
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables()

void DynamicTrafficLight::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("TRAJECTORY_MSG_LOCAL", 0);
  Register("TRAJECTORY_MSG", 0);
  Register("TRAJECTORY_HANDSHAKE", 0);
  Register("SPEED_MSG", 0);
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
  Register("NAV_HEADING", 0);
  Register("COMPASS_HEADING_RAW", 0);
  Register("NAV_SPEED", 0);
  Register("DEMUSTER_CONFIG", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool DynamicTrafficLight::buildReport() 
{
  m_msgs << "============================================" << endl;
  m_msgs << "File: DynamicTrafficLight.cpp               " << endl;
  m_msgs << "============================================" << endl;

  m_msgs << "Configuration parameters for " << m_name_os << ":" << endl;
  m_msgs << "  Default speed: " << m_default_speed << endl;
  m_msgs << "  Actual speed: " << m_nav_speed << endl;
  m_msgs << "  Precision: " << m_precision << endl;
  m_msgs << "  Max safety distance: " << m_max_safety_dist << endl;
  m_msgs << "  Min safety distance: " << m_min_safety_dist << endl;
  double dynamic_safety_dist = m_min_safety_dist + min(pow((m_nav_speed/m_default_speed),2),1.0) * (m_max_safety_dist - m_min_safety_dist);
  m_msgs << "  Dynamic safety distance: " << dynamic_safety_dist << endl;
  m_msgs << "  Max horizon: " << m_max_horizon_meters << endl;
  m_msgs << "  Min horizon: " << m_min_horizon_meters << endl;
  double dynamic_horizon_os_meters = m_min_horizon_meters + min(m_nav_speed/m_default_speed, 1.0) * (m_max_horizon_meters - m_min_horizon_meters);
  m_msgs << "  Dynamic horizon: " << dynamic_horizon_os_meters << endl;
  m_msgs << "  Show visualization: " << m_show_visualization << endl;
  m_msgs << "  Synchronize: " << m_synchronize << endl;

  m_msgs << "============================================" << endl;
  string handshake_nodes = "";
  for (auto const& x : m_handshakes)
  {
    handshake_nodes += x + " ";
  }
  m_msgs << "Handshakes: " << handshake_nodes << endl;
  string proximity_nodes = "";
  for (auto const& x : m_nodes_in_proximity)
  {
    proximity_nodes += x + " ";
  }
  m_msgs << "Nodes in proximity: " << proximity_nodes << endl;
  m_msgs << "============================================" << endl;
  m_msgs << "Own Trajectory: " << m_trajectory_map[m_name_os].get_spec() << endl;
  m_msgs << "Other Trajectories: " << endl;
  m_msgs << "Other trajectories length: " << m_trajectory_map.size() << endl;
  for (auto const& x : m_trajectory_map)
  {
    m_msgs << x.first << " --> " << x.second.get_spec() << endl;

  }

  m_msgs << "============================================" << endl;
  m_msgs << "Block matrix: " << endl;
  for (auto const& x : m_block_matrix)
  {
    m_msgs << x.first << " --> ";
    for (auto const& y : x.second)
    {
      m_msgs << y.first << ": " << y.second << " ";
    }
    m_msgs << endl;
  }

  m_msgs << "============================================" << endl;
  m_msgs << "Potential permablock timer: " << m_potential_permablock_timer << endl;
  m_msgs << "Potential deadlock timer: " << m_potential_deadlock_timer << endl;

  m_msgs << "============================================" << endl;
  m_msgs << "Use compass heading: " << m_use_compass_heading << endl;
  m_msgs << "Compass heading: " << m_compass_heading << endl;
  m_msgs << "Compass declination: " << m_compass_declination << endl;


  return(true);
}

bool depthFirstSearch(string name, map<string, map<string, BlockStatus>> &block_matrix, vector<string> &visited, vector<string> &recursive_stack){
    // Mark the current node as visited and add to the recursion stack
    visited.push_back(name);
    recursive_stack.push_back(name);

    // Iterate through all the nodes that the current node is blocked by
    for (auto const& blockage : block_matrix[name]) {
      if (blockage.first == "total") {
        continue;
      }
      
      if (blockage.first == "name") { //Should not be necessary, as a node cannot be blocked by itself
        continue;
      }

      if (blockage.second == PHYSICALLY_BLOCKED){
        bool already_visited = find(visited.begin(), visited.end(), blockage.first) != visited.end();
        if (!already_visited && depthFirstSearch(blockage.first, block_matrix, visited, recursive_stack)){
          return true;
        } else if (find(recursive_stack.begin(), recursive_stack.end(), blockage.first) != recursive_stack.end()){
          return true;
        }
      }
    }

    //Remove the node "name" from the recursion stack
    recursive_stack.erase(remove(recursive_stack.begin(), recursive_stack.end(), name), recursive_stack.end());

    return false;
}


bool isPermablocked(std::string name, std::map<std::string, std::map<std::string, BlockStatus>> &block_matrix, std::map<std::string, XYSegList> &trajectory_map){
  bool permablocked = false;

  for (auto const& blockage : block_matrix[name]) {
    if (blockage.first == "total") {
      continue;
    }

    if (blockage.first == "name") { //Should not be necessary, as a node cannot be blocked by itself
      continue;
    }

    if (blockage.second == PHYSICALLY_BLOCKED){
      int points_left_blocker = int(trajectory_map[blockage.first].size());
      if (points_left_blocker <= 1) {
        permablocked = true;
        break;
      }
    }
  }

  return permablocked;
}