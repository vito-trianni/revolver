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
 

////////////////////////////////////////////////////////////////////////////////
// Evaluation Configuration - This could be added to the XML
////////////////////////////////////////////////////////////////////////////////
UInt32  un_genotype_length = 2;
UInt32  un_num_teams = 1;
UInt32  un_team_size = 2;
UInt32  un_num_samples = 1;
UInt32  un_num_objectives = 1;
Real f_recombination_factor = 0.5; 

CRange<Real> m_cGenotypeValueRange = CRange<Real>(0.0,100.0);

UInt32 vun_complete_genotype[4] = {10.0,10.0,10.0,10.0 } ;


CEvaluationConfig generateFoundingTeam(CSimulator sim){

   CEvaluationConfig cSingleTeamEC( 1, un_team_size );
   cSingleTeamEC.SetRecombinationFactor(f_recombination_factor);
   cSingleTeamEC.SetIndividualIndex(0); 
   
   UInt32 m_punEvaluationSeeds[un_num_samples];
   
   for( UInt32 i = 0; i < un_num_samples; ++i ) {
      m_punEvaluationSeeds[i] = sim.GetRNG()->Uniform(CRange<UInt32>(0,INT_MAX));
   }
   
   cSingleTeamEC.SetSampleSeeds(CVector<UInt32>(un_num_samples,m_punEvaluationSeeds));
   
   UInt32 uGenotypeCounter = 0;
      
   // build a fake team
   TTeam team;
   
   for(UInt32 j = 0; j < un_team_size; ++j){
      
      Real pf_control_parameters[un_genotype_length];
      for(UInt32 k = 0 ; k < un_genotype_length ; ++k){
         pf_control_parameters[k] = vun_complete_genotype[uGenotypeCounter];
      }
      CGenotype cTeamMemberGenotype(un_genotype_length,pf_control_parameters,m_cGenotypeValueRange);
      cTeamMemberGenotype.SetID(j); 
      cTeamMemberGenotype.SetRNG(sim.GetRNG());
      
      cSingleTeamEC.InsertControlParameters(j,cTeamMemberGenotype);
      
      // insert a fake team member
      team.Insert(j);
      uGenotypeCounter++;
   }
   
   //evaluation_config.SetRecombinationFactor(f_recombination_factor);
   //evaluation_config.SetSampleSeeds(CVector<UInt32>(un_num_samples,pun_sample_seeds));
   
   cSingleTeamEC.InsertTeam(0, team);
   
   
   
   return cSingleTeamEC;
}
 
int main(int argc, char** argv) {

   ////////////////////////////////////////////////////////////////////////////////
   // Create an instance of the simulator
   ////////////////////////////////////////////////////////////////////////////////
   CSimulator cSimulator;
   
   ////////////////////////////////////////////////////////////////////////////////
   // parse the command line
   ////////////////////////////////////////////////////////////////////////////////

   // fitness base file
   string s_working_dir = "";
   string invalid_xml_configuration_filename = "";
      
   // define the class for parsing the command line
   CCommandLineArgParser cCommandLineArgs;
   
   // argument -f - fitness file
   cCommandLineArgs.AddArgument<std::string>('d',
					     "--directory",
					     "Specify the working directory",
					     s_working_dir);
					     
   // argument -c - experiment configuration file
   cCommandLineArgs.AddArgument<std::string>('c',
					     "--config",
					     "Specify the xml configuration file",
					     invalid_xml_configuration_filename);
   
   ////////////////////////////////////////////////////////////////////////////////
   // Build and initialise the simulator according to the command line arguments
   ////////////////////////////////////////////////////////////////////////////////
   try {
      
      /* Parse command line */
      cCommandLineArgs.Parse(argc, argv);

      /* Change working directory */
      if( s_working_dir != "" ) {
	      chdir( s_working_dir.c_str() );
	      // LOG << "[INVALID] working directory is: " << get_current_dir_name() << endl;
      }
      
      cSimulator.SetExperimentFileName(invalid_xml_configuration_filename);
      cSimulator.LoadExperiment();
   }
   catch(std::exception& ex) {
      /* A fatal error occurred: dispose of data, print error and exit */
      cSimulator.Destroy();
      LOGERR << "[INVALID] " << ex.what() << endl;
      LOG.Flush();
      LOGERR.Flush();
      return -1;
   }


   ////////////////////////////////////////////////////////////////////////////////
   // Start individual evaluation
   ////////////////////////////////////////////////////////////////////////////////

      
   CEvaluationConfig evaluation_config = generateFoundingTeam(cSimulator);
   
   // start evaluation
   for( UInt32 i = 0; i < un_num_samples; ++i ) {
      // set the random seed in the simulator
   	cSimulator.SetRandomSeed(evaluation_config.GetSampleSeed(i));
 
      // set the trial number
      cSimulator.SetTrialNumber( i );
      
      // resetting the experiment
      cSimulator.Reset();
      
      // set the controller parameters
      cSimulator.SetControlParameters(&evaluation_config);
      
      // run the simulation
      cSimulator.Execute();
      
      // retrieve the fitness values
      Real fitness = cSimulator.ComputePerformanceInExperiment();
      
      // Do something with the fitness
   }
 

   // everything's ok, exit
   return 0;
}

