/************************************************************/
/*    NAME: Tyler Paine                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: BloomStormCtrl.cpp                                        */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "BloomStormCtrl.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

BloomStormCtrl::BloomStormCtrl()
{
  m_curr_option = "";
  m_storm_detected = false;
  m_bloom_detected = false;
  m_new_bloom_detected = false;
  m_no_waypoints_available_to_sample = true; 
  
  m_nav_x = -1;
  m_nav_y = -1;
  m_odometry = 0.0;

  m_send_new_ignore_set = false;

  m_min_batt = 11.5;
  m_max_batt = 15.5;
  m_battery_exhausted = m_max_batt;
  m_time_const_batt = 10.0;
  m_batt_input_gain = 50; 
  
  m_m300_batt_voltage = 0;
  m_got_real_batt_voltage = false;
  m_current_batt_voltage = 0.0;

  m_sample_radius = 15;
  m_sample_time   = 30;
  m_sample_capture_radius = 5;
  m_fixed_coalitions = false; 

  m_migration_done = false;
  m_finish_migration_val = 80;
  
  m_next_region = false;   // false = region2
  m_region_1_spec = "-300,-250:-28.9,-121.5:121,-437.8:-171.5,-566.3";
  m_region_2_spec = "125,-50:394.1,78.5:546,-237.8:253.5,-366.3";
  
  m_bloom_found_counter = 0;


  m_dist_to_closest_sample = 1000;

  m_no_sample_available_weight = 50;
  m_currently_sampling_weight  = 50;
  m_number_sampled = 0;

  m_dist_to_next_sample_max_input_value = 50;
  m_dist_to_next_sample_max_input_range = 0;
  m_dist_to_next_sample_min_input_range = 50;

  m_storm_detected_input_val = 100;
  m_finish_migration_val = 60;
  m_time_since_last_migration_thresh = 300;
  m_time_since_last_migration_resistance_val = -100;
  m_time_since_no_samples_thresh = 60;
  m_time_since_no_samples_encouragement_slope = 0.1;
  

  // To Remove::
  /*
  m_sample_active.insert("abe");
  m_sample_active.insert("ben");
  m_sample_active.insert("cal");
  m_sample_active.insert("deb");
  m_sample_active.insert("eve");
  m_sample_active.insert("fin");
  m_sample_active.insert("gil");
  m_sample_active.insert("hix");

  m_voronoi_active.insert("abe");
  m_voronoi_active.insert("ben");
  m_voronoi_active.insert("cal");
  m_voronoi_active.insert("deb");
  m_voronoi_active.insert("eve");
  m_voronoi_active.insert("fin");
  m_voronoi_active.insert("gil");
  m_voronoi_active.insert("hix");
  */
}

//---------------------------------------------------------
// Destructor

BloomStormCtrl::~BloomStormCtrl()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool BloomStormCtrl::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    std::cout << " On New Mail, key = " << key << std::endl;
    
    bool handled = false; 
    if((key == "NODE_REPORT") || (key == "NODE_REPORT_LOCAL")) {
      handled = handleMailNodeReport(msg.GetString());
      if (!handled)
	reportRunWarning("Unhandled Mail: " + key);
      
    } else if (key == "NAV_X") {
      m_nav_x = msg.GetDouble();
      m_nav_x_vec.push_back( msg.GetDouble() ); 

    } else if (key == "NAV_Y") {
      m_nav_y = msg.GetDouble();
      m_nav_y_vec.push_back( msg.GetDouble() );

    } else if (key == "CONTACTS_LIST") {
        // parse message
      std::vector<std::string> svector = parseString(msg.GetString(), ',');
      for (unsigned int i=0; i<svector.size(); i++) {
	m_contacts.insert(svector[i]);
      }
      m_send_new_ignore_set = true;

    } else if (key == "EXPLORE_LIST") {
      // Make a new set to erase the old set.
      std::set<std::string> new_voronoi_active_set;
        // parse message
      std::vector<std::string> svector = parseString(msg.GetString(), ',');
      for (unsigned int i=0; i<svector.size(); i++) {
	new_voronoi_active_set.insert(svector[i]);
      }
      // only update if not in fixed coalitions
      if(!m_fixed_coalitions)
	m_voronoi_active = new_voronoi_active_set;
      
      m_send_new_ignore_set = true;

    } else if (key == "EXPLOIT_LIST") {
      // Make a new set to erase the old set.
      std::set<std::string> new_sample_active_set;
        // parse message
      std::vector<std::string> svector = parseString(msg.GetString(), ',');
      for (unsigned int i=0; i<svector.size(); i++) {
	new_sample_active_set.insert(svector[i]);
      }
      // only update if not in fixed coalitions
      if(!m_fixed_coalitions)
	m_sample_active = new_sample_active_set;

    } else if (key == "M300_BATT_VOLTAGE") {
      m_m300_batt_voltage = msg.GetDouble();
      m_got_real_batt_voltage = true;
      m_time_last_got_batt = MOOSTime(); 

    } else if (key == "BLOOM_DETECTED") {
      handled = handleBloomMsg(msg.GetString());
      if (!handled) {
	reportRunWarning("Unhandled Mail: " + key);
      }
    } else if (key == "STORM_DETECTED") {
      handled = setBooleanOnString(m_storm_detected, msg.GetString());
      if (!handled) {
	reportRunWarning("Unhandled Mail: " + key);
      }
    } else if (key == "OPTION") {
      handled =  handleOptionMsg(msg.GetString());
      if (!handled) {
	reportRunWarning("Unhandled Mail: " + key);
      }
    } else if (key == "SAMPLE_LOC") {
      XYPoint new_point = string2Point(msg.GetString()); 
      if (new_point.valid()){
	m_bloom_records.push_back(new_point); 
      } else {
	reportRunWarning("Unhandled Mail: " + key + " Bad Point");
      }
      std::cout << " Just got mail for Sample LOC = " << msg.GetString() << std::endl;

    } else if (key == "SAMPLE_STARTING") {
      m_map_sampling_agents[msg.GetString()] = MOOSTime();

    } else if (key == "SAMPLE_FINISHING") {
      std::map<std::string, double>::iterator iter = m_map_sampling_agents.find(msg.GetString()); 
      if ( iter != m_map_sampling_agents.end()){
      	m_map_sampling_agents.erase(iter);
	if (msg.GetString() == m_vname){
	  m_number_sampled += 1;
	  Notify("NUMBER_OF_SAMPLED_BLOOMS", uintToString(m_number_sampled)); 
	}
      }
    } else if (key == "MIGRATION_DONE") {
      handled = setBooleanOnString(m_migration_done, msg.GetString());
      if (!handled) {
	reportRunWarning("Unhandled Mail: " + key);
      } else if (m_migration_done) {
	m_time_of_last_migration_completion = msg.GetTime(); 
      } else {
	m_time_of_last_migration_started = msg.GetTime(); 
      }
      
    } else if(key != "APPCAST_REQ") {// handled by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
    }
   }

  std::cout << " Exiting On New Mail: "  << std::endl;
    
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool BloomStormCtrl::OnConnectToServer()
{
   registerVariables();
   Notify("NUMBER_OF_SAMPLED_BLOOMS", uintToString(0));
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool BloomStormCtrl::Iterate()
{
 
  std::cout << " ----------- Iterating --------------- " << std::endl;

  AppCastingMOOSApp::Iterate();
  std::cout << " ----------- After AppCastingMOOSApp::Iterate--------------- " << std::endl;
  
  postDiscovery();
  updateSamplePoint(); 
  calculateInputs(MOOSTime());
  publishInputs();

  if ( m_send_new_ignore_set) {
    publishIgnoreLists(); 
  }

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool BloomStormCtrl::OnStartUp()
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
    
    if(param == "region_1_spec") {
      m_region_1_spec = value;
      m_region_1_poly = string2Poly(value);
      m_region_1_poly.grow_by_pct(1.05); 
      handled = m_region_1_poly.is_convex();
    }
    else if(param == "region_2_spec") {
      m_region_2_spec = value; 
      m_region_2_poly = string2Poly(value);
      m_region_2_poly.grow_by_pct(1.05); 
      handled = m_region_2_poly.is_convex();
    }
    else if(param == "sample_radius") {
      handled = setDoubleOnString(m_sample_radius, value);
    }
    else if(param == "sample_time") {
      handled = setDoubleOnString(m_sample_time, value);
    }
    else if(param == "max_batt") {
      handled = setPosDoubleOnString(m_max_batt, value);
    }
    else if(param == "min_batt") {
      handled = setPosDoubleOnString(m_min_batt, value);
    }
    else if(param == "batt_input_gain") {
      handled = setPosDoubleOnString(m_batt_input_gain, value);
    }
    else if(param == "sample_capture_radius") {
      handled = setDoubleOnString(m_sample_capture_radius, value);
    }
    else if(param == "no_sample_available_weight") {
      handled = setDoubleOnString(m_no_sample_available_weight, value);
    }
    else if(param == "dist_to_next_sample_max_input_value") {
      handled = setDoubleOnString(m_dist_to_next_sample_max_input_value, value);
    }
    else if(param == "dist_to_next_sample_max_input_range") {
      handled = setDoubleOnString(m_dist_to_next_sample_max_input_range, value);
    }
    else if(param == "dist_to_next_sample_min_input_range") {
      handled = setDoubleOnString(m_dist_to_next_sample_min_input_range, value);
    }
    else if(param == "currently_sampling_weight") {
      handled = setDoubleOnString(m_currently_sampling_weight, value);
    }
    else if(param == "storm_detected_input_val") {
      handled = setDoubleOnString(m_storm_detected_input_val, value);
    }
    else if(param == "finish_migration_input_val") {
      handled = setDoubleOnString(m_finish_migration_val, value);
    }
    else if(param == "time_since_last_migration_thresh") {
      handled = setDoubleOnString(m_time_since_last_migration_thresh, value);
    }
    else if(param == "time_since_last_migration_resistance_val") {
      handled = setDoubleOnString(m_time_since_last_migration_resistance_val, value);
    }
    else if(param == "time_since_no_samples_thresh") {
      handled = setDoubleOnString(m_time_since_no_samples_thresh, value);
    }
    else if(param == "time_since_no_samples_encouragement_slope") {
      handled = setDoubleOnString(m_time_since_no_samples_encouragement_slope, value);
    }
    else if(param == "cutoff_freq_batt") {
      double cutoff;
      bool ok = setPosDoubleOnString(cutoff,value);
      m_time_const_batt = 1.0 / cutoff;  // Cutoff cannot be zero.
      handled = ok;
    }

    // Taken from opinion fixer
    else if(param == "persistent_exploit_set") {
      handled = true;
      m_fixed_coalitions = true; 
      vector<string> msgs = parseString(value, ',');
      for(unsigned int i=0; i<msgs.size(); i++) {
	string msg = stripBlankEnds(msgs[i]);
	// Check if proper NMEA Header
	if(msg != "")
	  m_sample_active.insert(msg);
	else
	  handled = false;
      }
      

    }
     else if(param == "persistent_explore_set") {
      handled = true;
      m_fixed_coalitions = true; 
      vector<string> msgs = parseString(value, ',');
      for(unsigned int i=0; i<msgs.size(); i++) {
	string msg = stripBlankEnds(msgs[i]);
	// Check if proper NMEA Header
	if(msg != "")
	  m_voronoi_active.insert(msg);
	else
	  handled = false;
      }

    }


    if(!handled)
      reportUnhandledConfigWarning(orig);

  }

  // Get vname from .MOOS Mission File
  bool vnameOK = m_MissionReader.GetValue("Community", m_vname);
  if(!vnameOK){
    reportConfigWarning("Vehicle name missing in MOOS file.");
    return(false);
  }

  m_time_of_last_migration_started = MOOSTime(); 
  m_time_of_last_migration_completion = MOOSTime();
  m_time_of_last_sample_available = MOOSTime();
  
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables()

void BloomStormCtrl::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("NODE_REPORT",0);
  Register("NODE_REPORT_LOCAL",0);
  Register("OPTION",0);
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
  Register("CONTACTS_LIST", 0);
  // To uncomment
  Register("EXPLORE_LIST", 0);
  Register("EXPLOIT_LIST",0);
  Register("M300_BATT_VOLTAGE",0);
  Register("BLOOM_DETECTED",0);
  Register("STORM_DETECTED",0);
  Register("SAMPLE_LOC",0);
  Register("SAMPLE_FINISHING",0);
  Register("SAMPLE_STARTING",0);
  Register("MIGRATION_DONE",0); 

 
}


//------------------------------------------------------------
// Procedure: buildReport()

bool BloomStormCtrl::buildReport() 
{
  
 ACTable actab2(4);
  actab2 << " Current Option |  Odometry | Batt Volts | Samples Found ";
  actab2.addHeaderLines();
  actab2 << m_curr_option << doubleToString(m_odometry) << doubleToString(m_current_batt_voltage) <<  uintToString(m_bloom_found_counter); 
  m_msgs << actab2.getFormattedString();
  m_msgs << endl;

  
  if (m_map_sampling_agents.find(m_vname) != m_map_sampling_agents.end()) {
    m_msgs << "                                                           " << endl;
    m_msgs << " Sampling now for : " << doubleToString(MOOSTime() - m_map_sampling_agents[m_vname]) << std::endl; 
  }

  double dt_no_samples = MOOSTime() - m_time_of_last_sample_available;
  m_msgs << "   Debug: time since no samples avail = " << doubleToString(dt_no_samples,4) << endl;
  m_msgs << "   Debug:  m_time_of_last_sample_available = " << doubleToString( m_time_of_last_sample_available,4) << endl;
  m_msgs << "                                                           " << endl;
  m_msgs << "=  Input Values  ==========================================" << endl;
  m_msgs << "===========================================================" << endl;
  std::map<std::string, double>::iterator it;
  for (it = m_opinion_inputs.begin(); it!= m_opinion_inputs.end(); it++) {
    m_msgs << "  " << it->first << ": " <<  doubleToStringX(it->second, 4) << endl; 
  }

  m_msgs << "                                                           " << endl;
  m_msgs << "=  Sample Points: " << "(" << uintToString(m_bloom_records.size()) << ")           " << endl;
  m_msgs << "=  Distance To Closest: " << doubleToString(m_dist_to_closest_sample) << "           " << endl;
  m_msgs << "===========================================================" << endl;
  std::list<XYPoint>::iterator it2;
  for (it2 = m_bloom_records.begin(); it2!= m_bloom_records.end(); it2++) {
    m_msgs << "  *" << it2->get_spec() << endl; 
  }


  return(true);
}


//----------------------------------------------------------
// Procedure: postDiscovery();
//            Post if discovered a new bloom
void BloomStormCtrl::postDiscovery()
{
  std::cout << "   In postDiscovery()         " << std::endl;
  // if we are not in the right mode, then return
  if ( m_curr_option != "explore")
    return;

  // if we are not far enough away from any known blooms then return
  std::list<XYPoint>::iterator it;
  double dx, dy, dist;
  for ( it = m_bloom_records.begin(); it != m_bloom_records.end(); it++) {
    dx = m_nav_x - it->get_vx();
    dy = m_nav_y - it->get_vy();
    dist = sqrt( dx * dx + dy * dy);
    if (dist < 2 * m_sample_radius)
      return;
  }

  bool post_discovery = false; 
  // check if we just switched to explore from another mode, and we now
  // detect a bloom. 
  bool cond1 = (m_last_option != m_curr_option);
  bool cond2 = (m_curr_option == "explore");

  if (cond1 && cond2 && m_bloom_detected)
    post_discovery = true;

  if (m_new_bloom_detected)
    post_discovery = true;

  if (post_discovery) {
    m_bloom_found_counter += 1;
    
    // Create four points:
    std::string preamble = m_vname + "_" + uintToString(m_bloom_found_counter); 
    XYPoint north(m_nav_x, m_nav_y+m_sample_radius, preamble  + "_north");
    XYPoint east(m_nav_x+m_sample_radius, m_nav_y, preamble + "_east");
    XYPoint south(m_nav_x, m_nav_y-m_sample_radius, preamble + "_south");
    XYPoint west(m_nav_x-m_sample_radius, m_nav_y, preamble + "_west");
    XYPoint center(m_nav_x, m_nav_y, preamble + "_cent"); 
    
    // Post the four points:
    NodeMessage node_message;
    node_message.setSourceNode(m_vname);
    node_message.setDestNode("all");
    node_message.setVarName("SAMPLE_LOC");

    node_message.setStringVal(north.get_spec());
    Notify("NODE_MESSAGE_LOCAL", node_message.getSpec());
    node_message.setStringVal(east.get_spec());
    Notify("NODE_MESSAGE_LOCAL", node_message.getSpec());
    node_message.setStringVal(south.get_spec());
    Notify("NODE_MESSAGE_LOCAL", node_message.getSpec());
    node_message.setStringVal(west.get_spec());
    Notify("NODE_MESSAGE_LOCAL", node_message.getSpec());
    node_message.setStringVal(center.get_spec());
    Notify("NODE_MESSAGE_LOCAL", node_message.getSpec());
    
    // Add them to the records
    m_bloom_records.push_back(north);
    m_bloom_records.push_back(east);
    m_bloom_records.push_back(south);
    m_bloom_records.push_back(west);
    m_bloom_records.push_back(center);

    // Publish them to view
    Notify("VIEW_POINT", north.get_spec());
    Notify("VIEW_POINT", east.get_spec());
    Notify("VIEW_POINT", south.get_spec());
    Notify("VIEW_POINT", west.get_spec());
    Notify("VIEW_POINT", center.get_spec());
    
    m_new_bloom_detected = false; 
  }
    
  
  return;
}

//----------------------------------------------------------
// Procedure: updateSamplePlan();
//            Which sample point should I go to next if I am sampling?
void BloomStormCtrl::updateSamplePoint()
{
  std::cout << " UpdatingSamplePoints()" << std::endl;
  // Step 1:
  // First check if others who are sampling have reached any sample
  // point and remove it from the list.
  // AT THE SAME TIME, BUILD A VECTOR OF DISTANCES TO ALL THE POINTS
  // FROM OWN SHIP TO SEED THE SEARCH 
  std::map<std::string, NodeRecord>::iterator it_nr;
  std::list<XYPoint>::iterator it_br;
  double dist; 
  std::vector<double> distance_from_own;

  std::cout << "  Checking status of sampling and building vector " << std::endl;
  for (it_nr = m_map_node_records.begin(); it_nr != m_map_node_records.end(); it_nr++ ) {
    // continue if this vehicle is not actively sampling
    std::cout << "  -Checking vehicle: " << it_nr->second.getName() << std::endl;
    if (!m_sample_active.count(it_nr->second.getName()) && (it_nr->second.getName() != m_vname) ) {
      // This ensures that the distance vector is calculated for ownship regardless of whether it
      // is sampling or not.
      std::cout << "  --Continuing " << std::endl;
      continue; 
    }

    for (it_br = m_bloom_records.begin(); it_br != m_bloom_records.end(); ) {
      std::cout << "  -- Checking bloom = " << it_br->get_spec() << std::endl;dist = pow( (it_nr->second.getX() - it_br->get_vx()), 2) +  pow( (it_nr->second.getY() - it_br->get_vy()), 2);
      
      dist = pow( (it_nr->second.getX() - it_br->get_vx()), 2) +  pow( (it_nr->second.getY() - it_br->get_vy()), 2);
      
      std::cout << "  --dist2 "<< doubleToStringX(dist,4) << std::endl;

      bool sample_point_captured = dist < m_sample_capture_radius * m_sample_capture_radius;
      //bool agent_sampling = m_sample_active.count(it_nr->second.getName());
      bool agent_sampling = m_map_sampling_agents.find(it_nr->second.getName()) != m_map_sampling_agents.end(); 
      
      if ( sample_point_captured && agent_sampling ){
	// Assume this is done, and remove it from the list
	// and clear it for pMarineViewer
	// and break since the vehicle can only be sampling at one time.
	it_br->set_active(false);
        Notify("VIEW_POINT", it_br->get_spec());
	it_br = m_bloom_records.erase(it_br);
	std::cout << "  ---Found vehicle is sampling here, removing from the list" << doubleToStringX(dist,4) << std::endl;
	break;
	
      } else {
	// Add to vector if ownship
	if (it_nr->second.getName() == m_vname){
	  std::cout << "   ---Adding it to ownship distance vector" << std::endl;
	  distance_from_own.push_back(sqrt(dist)); 
	}
	it_br++;
	
      }
    }
  }

  
  std::cout << "-vector of distances to points = " << std::endl;
  std::cout << "[" ;
  for (unsigned int ii = 0; ii < distance_from_own.size(); ii++) {
    std::cout << doubleToStringX( distance_from_own[ii], 4) << ","; 
  }
  std::cout << std::endl;

  
  // Step 2:
  // Complete a quick sanity check of the vehicles which have reported that they are sampling
  // This clears out old values in case of communication droppout.
  
  std::map<std::string, double>::iterator it_sa;
  for (it_sa = m_map_sampling_agents.begin(); it_sa != m_map_sampling_agents.end();) {
    if ((MOOSTime() - it_sa->second) > m_sample_time) {
      it_sa = m_map_sampling_agents.erase(it_sa);
    } else {
      ++it_sa;
    }
  }
  
  
  // Step 3:
  // Now pick the next sample pont. Try the naive greedy approach to the multi-agent TSP
  // Find the closest point
  std::set<unsigned int> omit_set;  // set of indexes to omit from consideration.  These points
                                    // are better visited by other who are closer.
  double dist_to_closest;
  unsigned int index_to_closest;
  std::list<XYPoint>::iterator it_idx;  // by the end this will point to the next XYPoint to visit
  bool continue_search = true;
  unsigned int count = 0; 

  // loop until all points are allocated, or until the nearest node is found.
  while ((omit_set.size() < distance_from_own.size()) && continue_search && (count < 100)) {

    std::cout << "In while loop:" << std::endl;
    std::cout << "  omit_set.size() = " <<  uintToString(omit_set.size()) << std::endl;

    dist_to_closest = 100000;
    index_to_closest = -1;
    for (unsigned int i=0; i<distance_from_own.size(); i++){

      std::cout <<  "   checking through list, i = " << uintToString(i)
		<< ", distance = " << doubleToStringX(distance_from_own[i],4) << std::endl;
      
      if (omit_set.count(i)>0) {
	// this point has already been marked as closer to another agent
	// skip it
	std::cout << "   skipping this point since it is already marked" << std::endl;
	continue; 
      }
      
      if (distance_from_own[i] < dist_to_closest) {
	std::cout << "new closest value found" << std::endl;
	dist_to_closest = distance_from_own[i];
	index_to_closest = i;
      }
    }
		     
    // By here we will have found the closest valid point
    // Get the info about this point from the list
    double ptx;
    double pty; 
    it_idx = m_bloom_records.begin();
    std::cout << "  Found closest valid point to be index = " << uintToString(index_to_closest) << std::endl;

    // Safety check
    if ( index_to_closest < m_bloom_records.size() ) {
      std::advance(it_idx, index_to_closest);
      std::cout << "   Advanced iterator " << std::endl;
      ptx = it_idx->get_vx();
      pty = it_idx->get_vy();
    } else {
      ptx = 100.0;
      pty = -250.0; 
    }
    
    std::cout << "  Found the closest valid point at x=" << doubleToStringX(ptx,4) << ", y=" << doubleToStringX(pty,4) << std::endl;

    // Is this point closer to another vehicle (accounting for time delays)?
    double own_time_delay = 0.0; 
    if (m_map_sampling_agents.find(m_vname) != m_map_sampling_agents.end()){
      // still waiting to finish our own sampling.  How much time is left?
      own_time_delay = m_sample_time - (MOOSTime() - m_map_sampling_agents[m_vname]);	   
    }

    std::cout << "  own_time_delay = " << doubleToStringX(own_time_delay,4) << std::endl;
    double cost_to_beat = dist_to_closest + own_time_delay;
    std::cout << "  cost_to_beat = " << doubleToStringX(cost_to_beat,4) << std::endl;
    
    // Can another vehicle beat this?
    double cost_for_other_vehicle = 0.0;
    double proposed_dist = 0.0;
    double proposed_time = 0.0;
    continue_search = false;

    std::cout <<  "    Checking other vehicles" << std::endl;

  
    // reuse the iterator it_nr
    //std::map<std::string, NodeRecord>::iterator it_nr;
    for (it_nr = m_map_node_records.begin(); it_nr != m_map_node_records.end(); it_nr++ ) {

      std::cout <<  "    -> checking " << it_nr->second.getName() << std::endl;
      // continue if this vehicle is not actively sampling
      if ( !m_sample_active.count(it_nr->second.getName()) || (it_nr->second.getName() == m_vname) ) {
	std::cout <<  "    -> " << it_nr->second.getName() << " not sampling now" << std::endl;
	continue; 
      }
      
      // calc distance for this vehicle
      proposed_dist = pow( (it_nr->second.getX() - ptx), 2) +  pow( (it_nr->second.getY() - pty), 2);
      proposed_dist = sqrt(proposed_dist);
      std::cout <<  "    -> calculated proposed_dist=" << doubleToStringX(proposed_dist) << std::endl;

      // calc additional time for this vehicle
      proposed_time = 0.0;
      if (m_map_sampling_agents.find(it_nr->second.getName()) != m_map_sampling_agents.end()){
	// still waiting to finish sampling.  How much time is left?
        proposed_time = m_sample_time - (MOOSTime() - m_map_sampling_agents[it_nr->second.getName()]);	   
      }

      std::cout <<  "    -> proposed_time=" << doubleToStringX(proposed_time) << std::endl;
      
      if ( (proposed_dist + proposed_time) < cost_to_beat ){
	// This vehicle is closer to this point

	// Is this vehicle closer to a different point, which it would presumably go to first?
	bool better_point_for_other_vehicle = false;
	
	double alternate_point_dist = 0.0; 
        for (it_br = m_bloom_records.begin(); it_br != m_bloom_records.end(); it_br++) {
	  std::cout << "     -->checking if this alternate point is closer: " << it_br->get_spec() << std::endl;
	  alternate_point_dist = pow( (it_nr->second.getX() - it_br->get_vx()), 2) +  pow( (it_nr->second.getY() - it_br->get_vy()), 2);
	  alternate_point_dist = sqrt(alternate_point_dist);

	  std::cout << "      -->> alternate point dist = " << doubleToStringX(alternate_point_dist) << std::endl;
	  if (alternate_point_dist < proposed_dist) {
	    // The other vehicle will go here and the added time cost will be the same
	    // either way.  Exit
	    std::cout << " This point is closer other vehicle will go here" << std::endl;
	    better_point_for_other_vehicle = true;
	    break; 
	  }
	}

	
	if (!better_point_for_other_vehicle) {
	  std::cout <<  "    --> This vehicle is closer to the point and will go there, adding it to the set" << std::endl;
	  omit_set.insert(index_to_closest);
	  
	  // Set the flag
	  continue_search = true; 
	  // end here, no need to check further
	  break;
	}
      }
      
    } // end of for loop to check other vehicles. 
    count += 1;

    std::cout << "   End of while loop, omit_set.size() = " <<  uintToString(omit_set.size()) << std::endl;
    std::cout << "   End of while loop, distance_from_own.size() = " <<  uintToString( distance_from_own.size()) << std::endl;
    std::cout << "   End of While loop continue_search = " << continue_search << std::endl;
    std::cout << "   End of While loop count = "  <<  uintToString(count) << std::endl;
  } // end of while loop

  // There are two possibilities here:
  // 1.  A closest point was found, and it_idx points to it
  // 2.  All the points are better assigned to other vehicles
  
  std::string spec;
  if (m_next_region) {
    // region 1 is next, region 2 is active
    spec = "325,-110";
  } else {
    // region 2 is next, region 1 is active
    spec= "-100,-310";  
  }
  if (omit_set.size() == distance_from_own.size()) {
    m_no_waypoints_available_to_sample = true;
    std::cout <<  " * End of search and no points were found." << std::endl;
    
  } else {
    
    m_no_waypoints_available_to_sample = false;
    // update waypoint behavior(s)
    spec = it_idx->get_spec_xy(',');

    std::cout <<  " * End of search updating travel point to be:" << spec << std::endl;
    m_dist_to_closest_sample = dist_to_closest;
    m_time_of_last_sample_available = MOOSTime();
  }

  
  Notify("SAMPLE_TRAVEL_UPDATE","point=" + spec); 
  Notify("SAMPLE_UPDATE","station_pt=" + spec);
    
 
  return;
}


//----------------------------------------------------------
// Procedure:  clearSamplePoints()
void BloomStormCtrl::clearSamplePoints()
{
  std::list<XYPoint>::iterator it_br;
  for (it_br = m_bloom_records.begin(); it_br != m_bloom_records.end(); ) {
    it_br->set_active(false);
    Notify("VIEW_POINT", it_br->get_spec());
    it_br = m_bloom_records.erase(it_br);
  }

}



//----------------------------------------------------------
// Procedure: calculateInputs(time)

bool BloomStormCtrl::calculateInputs(double time)
{

  // Calculate battery amount exhausted
  updateOdometry();
  double max_heron_range = 1 * 60 * 60 * 2;
  if (m_got_real_batt_voltage) {
    m_current_batt_voltage = m_m300_batt_voltage;
  } else {
    m_current_batt_voltage = (m_min_batt - m_max_batt) * (m_odometry / max_heron_range) + m_max_batt;
  }

  if (m_current_batt_voltage > m_max_batt)
    m_current_batt_voltage = m_max_batt;

  if (m_current_batt_voltage < m_min_batt)
    m_current_batt_voltage = m_min_batt; 

  double batt_state = (m_max_batt - m_current_batt_voltage)/ (m_max_batt - m_min_batt);

  double delta_t = MOOSTime() - m_time_last_got_batt;
  double alpha;
  
  // Low pass filter. 
  alpha = 1.0 - exp( -1.0 * delta_t / m_time_const_batt);
  m_battery_exhausted += alpha * ( batt_state - m_battery_exhausted); 

  if (!m_got_real_batt_voltage){
    m_time_last_got_batt = MOOSTime(); 
  }
  

  ////////////////////////
  // Explore input

  // Add the battery input
  double explore_input = m_batt_input_gain * m_battery_exhausted;

  // Add a term if there are no waypoints to sample
  // and no storm is detected
  if (m_no_waypoints_available_to_sample && !m_storm_detected)
    explore_input += m_no_sample_available_weight; 
  
  m_opinion_inputs["EXPLORE_INPUT"] = explore_input; 
  

  ///////////////////////
  // Exploit input
  
  double exploit_input = 0.0;

  // Only add input if there are waypoints to sample
  if (m_no_waypoints_available_to_sample) {
    exploit_input += 0.0; 
  } else {
    
    // If we are really close to a sample, add the max input value
    if ( m_dist_to_closest_sample < m_dist_to_next_sample_max_input_range) {
      exploit_input +=  m_dist_to_next_sample_max_input_value;
      
      // Else if we are within the min input range, add some proportion
      // of the input
    } else if ( m_dist_to_closest_sample < m_dist_to_next_sample_min_input_range) {
      
      double diff = ( m_dist_to_next_sample_min_input_range - m_dist_to_next_sample_max_input_range ); 
      double prop = ( m_dist_to_closest_sample - m_dist_to_next_sample_max_input_range) / diff;
      exploit_input += m_dist_to_next_sample_max_input_value * prop; 
    }

    // Check if we are currently sampling
    std::map<std::string, double>::iterator iter = m_map_sampling_agents.find(m_vname); 
    if (iter != m_map_sampling_agents.end()) {
      exploit_input += m_currently_sampling_weight; 
    }
  }
  
  m_opinion_inputs["EXPLOIT_INPUT"] = exploit_input; 

  ////////////////////////
  // Migrate input

  double migrate_input = 0.0;
  
  if (m_storm_detected) {
      migrate_input += m_storm_detected_input_val; 
  }


  
  if (!m_migration_done){

    // did all my neighbors make it?
    std::map<std::string, NodeRecord>::iterator it_nr;
    bool all_neighbors_migrated = true; 
    for (it_nr = m_map_node_records.begin(); it_nr != m_map_node_records.end(); it_nr++ ) {
      // if it is not in the contacts list, and is not ownship, then skip it. 
      if (!m_contacts.count(it_nr->second.getName()) && (it_nr->second.getName() != m_vname) ) {
	std::cout << " migration calc, rejecting nr = " << it_nr->second.getName() << std::endl;
	continue; 
      }
      std::cout << " migration calc, checking nr = " << it_nr->second.getName() << std::endl;
      // m_next_region = true => current region was 2, the next region is 1;
      // m_next_region = flase => current region was 1, the next region is 2;
      if (m_next_region) {
	if (!m_region_2_poly.contains(it_nr->second.getX(), it_nr->second.getY())){
	  all_neighbors_migrated = false;
	  std::cout << " found this one was not contained in poly2= " << std::endl;
	  break;
	}	  
      } else {
	if (!m_region_1_poly.contains(it_nr->second.getX(), it_nr->second.getY())){
	  all_neighbors_migrated = false;
	  std::cout << " found this one was not contained in poly1= " << std::endl;
	  break;
	}
      }  
    }

    if (!all_neighbors_migrated){
      std::cout << " not all migrated, adding output.  " << std::endl;
      migrate_input += m_finish_migration_val;
    }
    
   }
  
  // Did we just complete a migration?
  double dt_migrate = MOOSTime() - m_time_of_last_migration_completion;

  if (( 2.0 < dt_migrate) && ( dt_migrate < m_time_since_last_migration_thresh ) && !m_storm_detected){
    migrate_input += m_time_since_last_migration_resistance_val * ( 1.0 - dt_migrate/m_time_since_last_migration_thresh);
  }

 
  // Are there no samples detected here?
  double dt_no_samples = MOOSTime() - m_time_of_last_sample_available;
  if ( dt_no_samples > m_time_since_no_samples_thresh) {
    migrate_input += (dt_no_samples - m_time_since_no_samples_thresh) * m_time_since_no_samples_encouragement_slope; 
  }

  m_opinion_inputs["MIGRATE_INPUT"] = migrate_input; 

  return(true); 
}


 //----------------------------------------------------------
// Procedure: publishInputs()

bool BloomStormCtrl::publishInputs()
{

  std::map<std::string, double>::iterator it;
  for (it = m_opinion_inputs.begin(); it!= m_opinion_inputs.end(); it++) {
    Notify(it->first, it->second); 
  }
  
  return(true); 
}

 
//---------------------------------------------------------------
// publishIgnoreLists()
//         Any and all
bool BloomStormCtrl::publishIgnoreLists()
{

  std::string val = "";
  std::set<std::string>::iterator it;
  for (it = m_contacts.begin(); it != m_contacts.end(); it++){
    std::cout << "Checking to ignore Contact = " << *it << std::endl;
    if (m_voronoi_active.count(*it)==0) {
      std::cout << " set to ignore" << std::endl;
      val += *it + ",";
    }
  }

  std::cout << " voronoi active set ="; 
  for (it = m_voronoi_active.begin(); it != m_voronoi_active.end(); it++) {
    std::cout << *it << ", ";

  }
  std::cout << std::endl;

  // remove the last comma if needed
  if (val.length()>0)
    val = val.substr(0, val.size()-1);

  Notify("PROX_SET_IGNORE_LIST", val);
  m_send_new_ignore_set = false;
  
  
  return(true);
}

void BloomStormCtrl::updateOdometry()
{
// update odometry
  
  bool same_length = m_nav_x_vec.size() == m_nav_y_vec.size(); 
  if ( (m_nav_x_vec.size() >= 2) && same_length) {
    //
    unsigned int i;
    for (i = 0; i<m_nav_x_vec.size()-1; i++) {
      double dx = m_nav_x_vec[i+1] - m_nav_x_vec[i];
      double dy = m_nav_y_vec[i+1] - m_nav_y_vec[i];
      m_odometry += sqrt( dx * dx + dy * dy);
    }
    
    double last_x = m_nav_x_vec[m_nav_x_vec.size()-1];
    double last_y = m_nav_y_vec[m_nav_y_vec.size()-1];
    m_nav_x_vec.clear();
    m_nav_y_vec.clear();
    m_nav_x_vec.push_back(last_x);
    m_nav_y_vec.push_back(last_y);

    Notify("N_ODOM_BLOOM_CTRL", m_odometry); 
  }
  return;
}

//---------------------------------------------------------
// Procedure: handleMailNodeReport()
//   Example: NAME=alpha,TYPE=KAYAK,UTC_TIME=1267294386.51,
//            X=29.66,Y=-23.49,LAT=43.825089, LON=-70.330030, 
//            SPD=2.00, HDG=119.06,YAW=119.05677,DEPTH=0.00,     
//            LENGTH=4.0,MODE=ENGAGED

bool BloomStormCtrl::handleMailNodeReport(const string& node_report_str)
{
  NodeRecord new_record = string2NodeRecord(node_report_str);

  if(!new_record.valid()) {
    reportRunWarning("ERROR: Unhandled node record");
    return(false);
  }

  // In case there is an outstanding RunWarning indicating the lack
  // of a node report for a given vehicle, retract it here. This is 
  // mostly a startup timing issue. Sometimes a sensor request is 
  // received before a node report. Only a problem if the node report
  // never comes. Once we get one, it's no longer a problem.
  string vname = new_record.getName();
  retractRunWarning("No NODE_REPORT received for " + vname);
  
  m_map_node_records[vname] = new_record;  

  return(true);
}


//---------------------------------------------------------
// Procedure: handleOptionMsg()

bool BloomStormCtrl::handleOptionMsg(const std::string& msg) {

  // update history
  m_last_option = m_curr_option; 
  m_curr_option = msg;

  // update sets
  if (m_curr_option == "exploit") {
    m_sample_active.insert(m_vname); 
  }

  // handle bloom detection
  if (m_curr_option != "explore"){
    m_new_bloom_detected = false;
  }

  // handle migration
  if (m_curr_option == "migrate"){
    clearSamplePoints();
    if  ( (MOOSTime() - m_time_of_last_migration_started) < m_time_since_last_migration_thresh)
      return(true);
    


    // if this is a new migration, then post the updated region info
    if (m_last_option != "migrate"){
      
      std::string spec;
      // m_next_region = true => current region is 2, the next region is 1;
      // m_next_region = flase => current region is 1, the next region is 2;
      if (m_next_region) {
	// region1
	spec = m_region_1_spec;
      } else {
	// region2
	spec = m_region_2_spec; 
      }
      
      Notify("PROX_UP_REGION", spec);
      Notify("UP_MOVE", "region=pts={" + spec + "},label=two"); 
      Notify("VECTOR_UPDATE", "op_region=pts={" + spec + "}");
      Notify("CURR_REGION","pts={" + spec + "}"); 
      Notify("OP_REGION_RESET", "true"); 
      
      // cycle the flag for the next migration.
      m_next_region = !m_next_region; 
      
    }

  }
  
  return(true); 
}

bool BloomStormCtrl::handleBloomMsg(const std::string& msg)
{
  bool handled = false; 
  // check if this is a new bloom
  // i.e. did we just go from true to false?
  bool this_msg = false;
  handled = setBooleanOnString(this_msg, msg);
  if (!handled)
    return(false); 

  bool cond1 = (m_bloom_detected ==false);
  bool cond2 = (this_msg ==true);
  bool cond3 = (m_curr_option == "explore");
  bool in_region = false;

  // m_next_region = true => current region is 2, the next region is 1;
  // m_next_region = flase => current region is 1, the next region is 2;
  if (m_next_region) {
    if (m_region_2_poly.contains(m_nav_x, m_nav_y)){
      in_region = true; 
    }	  
  } else {
    if (m_region_1_poly.contains(m_nav_x, m_nav_y)){
      in_region = true; 
    }
  } 
  
  if (cond1 && cond2 && cond3 && in_region){
    // this is a new bloom
    m_new_bloom_detected = true;
  }
  m_bloom_detected = this_msg;

  return(true);
}
