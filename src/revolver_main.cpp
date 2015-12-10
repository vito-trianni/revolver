#include <argos_utility/configuration/command_line_arg_parser.h>
//#include <argos3/core/utility/plugins/dynamic_loading.h>

#include "evolution.h"

using namespace argos;
using namespace std;

const unsigned int SIMULATORLOGGER_NOVERBOSE = 0;
const unsigned int SIMULATORLOGGER_VERBOSE = 1;

const string CONFIGURATION_EVOLUTION_TYPE            = "type";
const string CONFIGURATION_EVOLUTION_TYPE_MULAMBDA   = "mulambda-evolution";
const string CONFIGURATION_EVOLUTION_TYPE_MOMULAMBDA = "mo-mulambda-evolution";
const string CONFIGURATION_EVOLUTION_TYPE_SMULAMBDA  = "single-mulambda-evolution";

////////////////////////////////////////////////////////////////////////////////
// Main function
////////////////////////////////////////////////////////////////////////////////

int main( int argc, char** argv ) {
   // Command-line args  
   string 	 conf_filename = "";
   string 	 working_dir = "";
   long int	 seed = 0;
   bool          bPrintUsage = false;
   bool          bVerbose = false;
   bool          bPosteval = false;


   // define the class for parsing the command line
   CCommandLineArgParser cCommandLineArgs;

   // argument -h - print usage
   cCommandLineArgs.AddFlag('h',
			    "--help",
			    "Prints the command line arguments and exits",
			    bPrintUsage);
  
   // argument -v - verbose output
   cCommandLineArgs.AddFlag('v',
			    "--verbose",
			    "Verbose output of the simulator",
			    bVerbose);
  
   // argument -c - configuration file
   cCommandLineArgs.AddArgument<std::string>('c',
					     "--config",
					     "Specify the xml configuration file",
					     conf_filename);
  
   // argument -d - working directory
   cCommandLineArgs.AddArgument<std::string>('d',
					     "--dir",
					     "Specify the working directory",
					     working_dir);
  
   // argument -s - seed
   cCommandLineArgs.AddArgument<long int>('s',
					  "--seed",
					  "Specify the seed of the random number generator",
					  seed);

   // argument -p - evolution/post-evaluation flag
   cCommandLineArgs.AddFlag('p',
			    "--post-evaluation",
			    "Specify the flag for evolution or post-evaluation",
			    bPosteval);
   

   // parse the command line
   try {
      cCommandLineArgs.Parse(argc, argv);
   }
   catch(CARGoSException& ex) {
      LOGERR << "Error: " << ex.what() << std::endl;
      cCommandLineArgs.PrintUsage( LOGERR );
   }


   // check print usage flag
   if( bPrintUsage ) {
      cCommandLineArgs.PrintUsage( LOGERR );
      return 0;
   }


   // open the configuration file
   if( conf_filename == "" ) {
      LOGERR  << "[REVOLVER] Missing required configuration file" << endl;
      LOGERR  << "[REVOLVER] Use -h for help" << endl;
      return -1;
   }
   else {
      LOG << "[REVOLVER] configuration file is '" << conf_filename << "'" << endl;
   }

   // evolution seed
   if( seed == 0 ) {
      seed = time( NULL );
   }


   ticpp::Document tConfiguration;
   tConfiguration.LoadFile(conf_filename);
   TConfigurationNode tConfigurationRoot = *tConfiguration.FirstChildElement();
   TConfigurationNode t_evolution_configuration = GetNode(tConfigurationRoot, "evolution");
  
   ////////////////////////////////////////////////////////////////////////////////
   // evolution type
   ////////////////////////////////////////////////////////////////////////////////
   string s_evolution_type;
   GetNodeAttribute( t_evolution_configuration, "type", s_evolution_type );
   CEvolution* pc_evolution = CFactory<CEvolution>::New(s_evolution_type);

   pc_evolution->SetVerbose( bVerbose );
   pc_evolution->SetEvolutionFlag( !bPosteval );

   // initialise evolution seed - xml overwrites commandline
   pc_evolution->SetRandomSeed( seed );
   
   // set working dir for saving logs
   if( working_dir != "" ) {
      pc_evolution->SetWorkingDir( working_dir );
   }

   // initialise the evolution
   pc_evolution->Init( t_evolution_configuration );

   ////////////////////////////////////////////////////////////////////////////////
   // start evolution or post-evaluation
   ////////////////////////////////////////////////////////////////////////////////
   if( bPosteval ) {
      pc_evolution->PostEvaluation();
   }
   else {
      pc_evolution->Evolve();
   }

   //CDynamicLoading::UnloadAllLibraries();
   CFactory<CEvolution>::Destroy();

   delete pc_evolution;

   return 0;
}

