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

const string CUSTOM_FREQ_MUT_A                  = "frequency_mutant_A";
const string CUSTOM_FREQ_MUT_B                  = "frequency_mutant_B";

const string CUSTOM_RES_A_GENE                  = "resident_A_gene";
const string CUSTOM_RES_B_GENE                  = "resident_B_gene";
const string CUSTOM_MUT_A_GENE                  = "mutant_A_gene";
const string CUSTOM_MUT_B_GENE                  = "mutant_B_gene";



UInt32  un_num_samples = 1;

Real f_frequency_mutant_A = 0.0;
Real f_frequency_mutant_B = 0.0;
Real f_resident_A_gene    = 0.0;
Real f_resident_B_gene    = 0.0;
Real f_mutant_A_gene      = 0.0;
Real f_mutant_B_gene      = 0.0;


void LoadExperimentCustomColony(string s_experiment_filename){
    ticpp::Document tConfiguration;
    tConfiguration.LoadFile(s_experiment_filename);
    TConfigurationNode tConfigurationRoot = *tConfiguration.FirstChildElement();
    TConfigurationNode t_simulator_configuration = GetNode(tConfigurationRoot, "custom-colony");
    
    GetNodeAttribute(t_simulator_configuration, CUSTOM_FREQ_MUT_A, f_frequency_mutant_A );
    GetNodeAttribute(t_simulator_configuration, CUSTOM_FREQ_MUT_B, f_frequency_mutant_B );
    GetNodeAttribute(t_simulator_configuration, CUSTOM_RES_A_GENE, f_resident_A_gene );
    GetNodeAttribute(t_simulator_configuration, CUSTOM_RES_B_GENE, f_resident_B_gene );
    GetNodeAttribute(t_simulator_configuration, CUSTOM_MUT_A_GENE, f_mutant_A_gene );
    GetNodeAttribute(t_simulator_configuration, CUSTOM_MUT_B_GENE, f_mutant_B_gene );
    
}

int main(int argc, char** argv) {

   ////////////////////////////////////////////////////////////////////////////////
   // Create an instance of the simulator
   ////////////////////////////////////////////////////////////////////////////////
   CSimulator cSimulator;
   
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
      LoadExperimentCustomColony(invalid_xml_configuration_filename);
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

   // start evaluation
   for( UInt32 i = 0; i < un_num_samples; ++i ) {
      // set the random seed in the simulator using the internal function
   	cSimulator.SetRandomSeed();
 
      // set the trial number
      cSimulator.SetTrialNumber( i );
      
      // resetting the experiment
      cSimulator.Reset();
      
      // set the controller parameters
      
      Real f_frequency_monomorphic = (1.0 - f_frequency_mutant_A - f_frequency_mutant_B) / 2.0;
      
      UInt32 u_num_agents_mutant_A = (UInt32)Round(f_frequency_mutant_A * (Real)cSimulator.GetColonySize());
      UInt32 u_num_agents_mutant_B = (UInt32)Round(f_frequency_mutant_B * (Real)cSimulator.GetColonySize());
      UInt32 u_num_monomorphic = (UInt32)Round(f_frequency_monomorphic * (Real)cSimulator.GetColonySize());
      UInt32 u_count_mutant_A = 0;
      UInt32 u_count_mutant_B = 0;
      
      UInt32 u_count_agents = 0;
      
      for ( UInt32 i = 0; i < u_num_agents_mutant_A; ++i){
         CSimulator::Agent cNewAgent = {};
         cNewAgent.m_fThresholdTaskA = f_mutant_A_gene;
         cNewAgent.m_fThresholdTaskB = f_resident_B_gene;
         cNewAgent.m_unCurrentTask = 0; // IDLE
         cNewAgent.m_unSwitchingTimestep = 0;
         cNewAgent.m_unNonSwitchingTaskCounter = 0;
         cNewAgent.m_unTotalActionsPerAgent = 0;
         cSimulator.AddAgentToColony(cNewAgent);
         u_count_agents++;
         LOG << "Adding agent with A " << cNewAgent.m_fThresholdTaskA << " B " << cNewAgent.m_fThresholdTaskB << std::endl;
      }
      
      for ( UInt32 i = 0; i < u_num_agents_mutant_B; ++i){
         CSimulator::Agent cNewAgent = {};
         cNewAgent.m_fThresholdTaskA = f_resident_A_gene;
         cNewAgent.m_fThresholdTaskB = f_mutant_B_gene;
         cNewAgent.m_unCurrentTask = 0; // IDLE
         cNewAgent.m_unSwitchingTimestep = 0;
         cNewAgent.m_unNonSwitchingTaskCounter = 0;
         cNewAgent.m_unTotalActionsPerAgent = 0;
         cSimulator.AddAgentToColony(cNewAgent);
         LOG << "Adding agent with A " << cNewAgent.m_fThresholdTaskA << " B " << cNewAgent.m_fThresholdTaskB << std::endl;
         u_count_agents++;
      }
      
      for ( UInt32 i = 0; i < u_num_monomorphic; ++i){
         CSimulator::Agent cNewAgent = {};
         cNewAgent.m_fThresholdTaskA = f_resident_A_gene;
         cNewAgent.m_fThresholdTaskB = f_resident_B_gene;
         cNewAgent.m_unCurrentTask = 0; // IDLE
         cNewAgent.m_unSwitchingTimestep = 0;
         cNewAgent.m_unNonSwitchingTaskCounter = 0;
         cNewAgent.m_unTotalActionsPerAgent = 0;
         cSimulator.AddAgentToColony(cNewAgent);
         LOG << "Adding agent with A " << cNewAgent.m_fThresholdTaskA << " B " << cNewAgent.m_fThresholdTaskB << std::endl;
         u_count_agents++;
      }
      
      for ( UInt32 i = 0; i < u_num_monomorphic; ++i){
         CSimulator::Agent cNewAgent = {};
         cNewAgent.m_fThresholdTaskA = f_mutant_A_gene;
         cNewAgent.m_fThresholdTaskB = f_mutant_B_gene;
         cNewAgent.m_unCurrentTask = 0; // IDLE
         cNewAgent.m_unSwitchingTimestep = 0;
         cNewAgent.m_unNonSwitchingTaskCounter = 0;
         cNewAgent.m_unTotalActionsPerAgent = 0;
         cSimulator.AddAgentToColony(cNewAgent);
         LOG << "Adding agent with A " << cNewAgent.m_fThresholdTaskA << " B " << cNewAgent.m_fThresholdTaskB << std::endl;
         u_count_agents++;
      }
      
      //LOG << "Added " << u_count_agents << " agents in total." << std::endl;

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


