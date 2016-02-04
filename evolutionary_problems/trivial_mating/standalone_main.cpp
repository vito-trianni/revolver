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

Real f_monomorphic_genotype = 10.0;

UInt32 vun_complete_genotype[4] = {f_monomorphic_genotype,    // Mother A threshold
                                  f_monomorphic_genotype,    // Mother B threshold
                                  f_monomorphic_genotype,    // Father A threshold
                                  f_monomorphic_genotype } ; // Father B threshold
                                   
// UInt32 vun_complete_genotype[4] = {0.0,     // Mother A threshold
//                                   11.0,    // Mother B threshold
//                                   0.0,     // Father A threshold
//                                   22.0 } ; // Father B threshold


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
   UInt32 u_r_seed = 0;
      
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
   
   // // argument -s - random seed
   cCommandLineArgs.AddArgument<UInt32>('s',
					     "--seed",
					     "Specify the random seed",
					     u_r_seed);
   
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
      cSimulator.SetRandomSeed(u_r_seed);
      cSimulator.SetMonomorphicGenotype(f_monomorphic_genotype);
      cSimulator.SetExperimentFileName(invalid_xml_configuration_filename);
      cSimulator.LoadExperiment();
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

   CEvaluationConfig evaluation_config = cSimulator.GenerateFoundingTeam(un_team_size, un_genotype_length, m_cGenotypeValueRange, f_recombination_factor, un_num_samples);
   
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
      CObjectives cPerformance = cSimulator.ComputePerformanceInExperiment();
      
      // TODO: Do something with the fitness
   }
 
   cSimulator.Destroy();

   // // everything's ok, exit
   return 0;
}


