/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * @file <simulator/main.cpp>
 *
 * @brief This file provides the standard main() function to run the AHSS
 * simulator.
 *
 * In this file the standard main() function is defined. It provides the
 * basic functionalities to run the AHSS simulator: parses the command line,
 * loads the experiment, runs the simulation and disposes all the data.
 * 
 * @author Eliseo Ferrante  - <eliseo.ferrante@bio.kuleuven.be>
 * @author Vito Trianni     - <vito.trianni@istc.cnr.it>
 */
#include <argos_utility/configuration/command_line_arg_parser.h>

#include <utility/objectives.h>
#include <utility/evaluation_config.h>
#include <utility/control_parameters.h>

#include <iostream>
#include <unistd.h>

// #include "../../src/revolver_general.h"
#include "definitions.h"
#include "simulator.h"

using namespace argos;
using namespace std;


/**
 * @brief The standard main() function to run the simplified simulation.
 *
 * This main() function provides tha basic functionalities to run the simplified
 * simulator: parses the command line, loads the experiment, runs the
 * simulation and disposes all the data.
 *
 * @param argc the number of command line arguments given at the shell.
 * @param argv the actual command line arguments.
 *
 * @return 0 if everything OK; -1 in case of errors.
 *
 */

const string CONFIGURATION_STANDALONE_GENOTYPE          = "standalone_genotype";
const string CONFIGURATION_RECOMBINATION_FACTOR         = "recombination_factor";

////////////////////////////////////////////////////////////////////////////////
// Evaluation Configuration - This could be added to the XML
////////////////////////////////////////////////////////////////////////////////
UInt32  un_genotype_length = 2;
UInt32  un_num_teams = 1;
UInt32  un_team_size = 2; // This will be overwritten once the configuratio file is read (using the standalone founding team size)
UInt32  un_num_samples = 1;
UInt32  un_num_objectives = 1;

Real f_recombination_factor;

string s_standalone_genotype_string;
CVector<Real> c_standalone_genotype;

string s_model_type = "";

CRange<Real> c_genotype_value_range = CRange<Real>(0.0,100.0);

////////////////////////////////////////////////////////////////////////////////
// Create an instance of the simulator
////////////////////////////////////////////////////////////////////////////////
CSimulator cSimulator;

void LoadExperimentStandalone(string s_experiment_filename){
    ticpp::Document tConfiguration;
    tConfiguration.LoadFile(s_experiment_filename);
    TConfigurationNode tConfigurationRoot = *tConfiguration.FirstChildElement();
    TConfigurationNode t_simulator_configuration = GetNode(tConfigurationRoot, "standalone");
    
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_STANDALONE_GENOTYPE, s_standalone_genotype_string);
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_RECOMBINATION_FACTOR, f_recombination_factor);
    
    s_model_type = cSimulator.GetModelType();
    if(s_model_type.compare("two_thresholds") == 0){
       un_genotype_length = 2;
    }
    else if(s_model_type.compare("single_threshold") == 0){
       un_genotype_length = 1;
    }
    else{
       LOGERR << "[ERROR] Non existing or not yet implemented model. Exiting " << std::endl;
       exit(-1);
    }
    
    istringstream standaloneGenotypeStream(s_standalone_genotype_string);
    standaloneGenotypeStream >> c_standalone_genotype;
    
    un_team_size = c_standalone_genotype.GetSize() / un_genotype_length;
    
}

/****************************************/
/****************************************/

CEvaluationConfig GenerateFoundingTeam(){

   CEvaluationConfig cSingleTeamEC( 1, un_team_size );
   cSingleTeamEC.SetRecombinationFactor(f_recombination_factor);
   cSingleTeamEC.SetIndividualIndex(0); 
   
   UInt32 m_punEvaluationSeeds[un_num_samples];
   
   for( UInt32 i = 0; i < un_num_samples; ++i ) {
      m_punEvaluationSeeds[i] = cSimulator.GetRNG()->Uniform(CRange<UInt32>(0,INT_MAX));
   }
   
   cSingleTeamEC.SetSampleSeeds(CVector<UInt32>(un_num_samples,m_punEvaluationSeeds));
   
   UInt32 uGenotypeCounter = 0;
      
   // build a fake team
   TTeam team;
   
   for(UInt32 j = 0; j < un_team_size; ++j){
      
      Real pf_control_parameters[un_genotype_length];
      for(UInt32 k = 0 ; k < un_genotype_length ; ++k){
         pf_control_parameters[k] = c_standalone_genotype[uGenotypeCounter];
         LOG << "Setting genotype of founding team to " << pf_control_parameters[k] << std::endl;
         uGenotypeCounter++;
      }
      CGenotype cTeamMemberGenotype(un_genotype_length,pf_control_parameters,pf_control_parameters,c_genotype_value_range);
      cTeamMemberGenotype.SetHaploid();
      cTeamMemberGenotype.SetID(j); 
      cTeamMemberGenotype.SetRNG(cSimulator.GetRNG());
      
      cSingleTeamEC.InsertControlParameters(j,cTeamMemberGenotype);
      
      // insert a fake team member
      team.Insert(j);
      
   }
   
   cSingleTeamEC.InsertTeam(0, team);
   
   return cSingleTeamEC;
}

/****************************************/
/****************************************/


int main(int argc, char** argv) {

   // ////////////////////////////////////////////////////////////////////////////////
   // // parse the command line
   // ////////////////////////////////////////////////////////////////////////////////

   // fitness base file
   string s_working_dir = "";
   string invalid_xml_configuration_filename = "";
      
   // // define the class for parsing the command line
   CCommandLineArgParser cCommandLineArgs;
   
   // // argument -f - fitness file
   cCommandLineArgs.AddArgument<std::string>('d',
					     "--directory",
					     "Specify the working directory",
					     s_working_dir);
					     
   // // argument -c - experiment configuration file
   cCommandLineArgs.AddArgument<std::string>('c',
					     "--config",
					     "Specify the xml configuration file",
					     invalid_xml_configuration_filename);
   
   
   // ////////////////////////////////////////////////////////////////////////////////
   // // Build and initialise the simulator according to the command line arguments
   // ////////////////////////////////////////////////////////////////////////////////
   try {
      
      /* Parse command line */
      cCommandLineArgs.Parse(argc, argv);

      /* Change working directory */
      if( s_working_dir != "" ) {
	      chdir( s_working_dir.c_str() );
	   }
      //cSimulator.SetMonomorphicGenotype(f_monomorphic_genotype);
      cSimulator.SetExperimentFileName(invalid_xml_configuration_filename);
      cSimulator.LoadExperiment();
      LoadExperimentStandalone(invalid_xml_configuration_filename);
   }
   catch(std::exception& ex) {
      /* A fatal error occurred: dispose of data, print error and exit */
      cSimulator.Destroy();
      LOGERR << "[STANDALONE] " << ex.what() << endl;
      LOG.Flush();
      LOGERR.Flush();
      return -1;
   }


   // ////////////////////////////////////////////////////////////////////////////////
   // // Start individual evaluation
   // ////////////////////////////////////////////////////////////////////////////////

   CEvaluationConfig evaluation_config = GenerateFoundingTeam();
   
   //LOGERR << "Generated founding team " << std::endl;
   
   // start evaluation
   for( UInt32 i = 0; i < un_num_samples; ++i ) {
      // set the random seed in the simulator
   	cSimulator.SetRandomSeed(evaluation_config.GetSampleSeed(i));
 
      // set the trial number
      cSimulator.SetTrialNumber( i );
      
      // resetting the experiment
      cSimulator.Reset();
      
      //LOGERR << "Setting control parameters " << std::endl;
      
      // set the controller parameters
      cSimulator.SetControlParameters(&evaluation_config);
      
      //LOGERR << "Running simulation " << std::endl;
      
      // run the simulation
      cSimulator.Execute();
      
      // retrieve the fitness values
      CObjectives cPerformance = cSimulator.ComputePerformanceInExperiment();
      
      // TODO: Do something with the fitness
   }
 
   cSimulator.Destroy();

   // // everything's ok, exit
   return 0;
}


