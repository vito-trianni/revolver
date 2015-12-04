#include "evaluation_config.h"

const string EVALUATION_CONFIG_FILE         = "file";
const string EVALUATION_POPULATION_NODE     = "population";
const string EVALUATION_TEAM                = "team";

/****************************************/
/****************************************/

CEvaluationConfig::CEvaluationConfig() :
   m_unNumTeams( 0 ),
   m_unTeamSize( 0 ),
   m_unIndividualIndex( 0 )
{}


/****************************************/
/****************************************/

CEvaluationConfig::CEvaluationConfig( UInt32 un_num_teams, UInt32 un_team_size ) :
   m_unNumTeams( un_num_teams ),
   m_unTeamSize( un_team_size ),
   m_unIndividualIndex( 0 )
{
   // reserve memory for the team specificiation
   m_vecTeams.resize( m_unNumTeams );
}


/****************************************/
/****************************************/

CEvaluationConfig::~CEvaluationConfig() {
}


/****************************************/
/****************************************/

void CEvaluationConfig::Init( TConfigurationNode& t_tree ) {
   // Initialisation from the xml file allows to specify only a single
   // team. The team composition can be provided in two ways, either
   // specifying the population file and the team composition, or by
   // providing a list of the control parametrers to be used
   TTeam team;

   // Check if a population file is provided
   if( NodeExists(t_tree, EVALUATION_POPULATION_NODE ) ) {
      TConfigurationNode& tPopulationNode = GetNode(t_tree, EVALUATION_POPULATION_NODE);

      string s_population_file;
      GetNodeAttribute(tPopulationNode, EVALUATION_CONFIG_FILE, s_population_file);

      LOG << "[INFO] Loading population from file " << s_population_file << endl;
	 
      LoadControlParameters( s_population_file );

      GetNodeAttribute(tPopulationNode, EVALUATION_TEAM, team);
   }
   else {
      // Get the base node for the list of parameter files, and load the list
      TConfigurationNodeIterator itParameterFiles;
      UInt32 un_controller_num = 0;
      for( itParameterFiles = itParameterFiles.begin(&t_tree); itParameterFiles != itParameterFiles.end(); ++itParameterFiles ) {
	 string s_parameter_file;
	 GetNodeAttribute(*itParameterFiles, EVALUATION_CONFIG_FILE, s_parameter_file);
	 LOG << "[INFO] Loading control parameters from file " << s_parameter_file << endl;
	 LoadControlParameters( un_controller_num, s_parameter_file );
	 team.Insert(un_controller_num);
	 un_controller_num += 1;
      }
   }
   
   m_unIndividualIndex = team[0];
   m_unNumTeams = 1;
   m_unTeamSize = team.GetSize();
   m_vecTeams.clear();
   m_vecTeams.push_back( team );
}


/****************************************/
/****************************************/

void CEvaluationConfig::InsertControlParameters( const UInt32 un_index, CControlParameters c_control_parameters ){ 
   if( m_mapControlParameters.find(un_index) == m_mapControlParameters.end() ) {
      m_mapControlParameters.insert(make_pair(un_index, c_control_parameters));
   }
}

/****************************************/
/****************************************/

void CEvaluationConfig::LoadControlParameters( const string& s_population_file ) {
   ifstream in(s_population_file.c_str(), ios::in );
   if( !in ) {
      THROW_ARGOSEXCEPTION("Cannot open population file '" << s_population_file << "' for reading");
   }
   
   UInt32 un_index = 0;
   CControlParameters c_control_parameters;
   string s_line;
   while(std::getline(in, s_line, '\n')) {
      std::istringstream iss(s_line);
      iss >> c_control_parameters;
      InsertControlParameters( un_index++, c_control_parameters );
   }
}

/****************************************/
/****************************************/

void CEvaluationConfig::LoadControlParameters( const UInt32 un_index, const string& s_parameters_file ) {
   ifstream in(s_parameters_file.c_str(), ios::in );
   if( !in ) {
      THROW_ARGOSEXCEPTION("Cannot open population file '" << s_parameters_file << "' for reading");
   }
   
   try {
      CControlParameters c_control_parameters;
      if( in >> c_control_parameters ) {
	 InsertControlParameters( un_index, c_control_parameters );
      }
   }
   catch(CARGoSException& ex) {
      LOGERR << "[REVOLVER] could not load parameters from file " << s_parameters_file << endl;
   }
}

/****************************************/
/****************************************/

void CEvaluationConfig::InsertTeam( const UInt32 un_team_index, TTeam vec_team ) {
   m_vecTeams[un_team_index] = vec_team;
}


/****************************************/
/****************************************/

void CEvaluationConfig::InsertTeamMember( const UInt32 un_team_index, const UInt32 un_team_member ) {
   m_vecTeams[un_team_index].Insert( un_team_member );
}


/****************************************/
/****************************************/

void CEvaluationConfig::SetTeams( const UInt32 un_num_values, const UInt32* pun_teams ) {
   if( un_num_values != m_unNumTeams*m_unTeamSize ) {
      THROW_ARGOSEXCEPTION("[REVOLVER] team configuration sizes do no correspond");
   }
   
   for( UInt32 i = 0; i < m_unNumTeams; i++ ) {
      for( UInt32 j = 0; j < m_unTeamSize; j++ ) {
	 m_vecTeams[i].Insert( pun_teams[i*m_unTeamSize + j] );
      }
   }
}


/****************************************/
/****************************************/

ostream& operator <<( ostream& os, const CEvaluationConfig& c_ec ) {
   for( UInt32 i = 0; i < c_ec.m_vecSampleSeeds.GetSize(); i++ ) {
      // write down the sample seed
      os << c_ec.m_vecSampleSeeds[i] << " ";

      // get the team and dump it
      UInt32 un_team_index = i % c_ec.m_unNumTeams;
      os << c_ec.m_vecTeams[un_team_index] << " ";

      // get the results and dump them
      os << c_ec.m_vecResults[i] << endl;
   }
   return os;
}
