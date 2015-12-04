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
 * @author Vito Trianni - <vito.trianni@istc.cnr.it>
 */

#include <argos3/core/utility/plugins/dynamic_loading.h>
#include <argos3/core/simulator/argos_command_line_arg_parser.h>
#include <argos3/core/simulator/simulator.h>
#include <loop_functions/evolutionary_lf/evolutionary_lf.h>
#include <utility/objectives.h>
#include <utility/evaluation_config.h>
#include <utility/control_parameters.h>

#include <iostream>
#include <unistd.h>

#include <mpi.h>

#include "revolver_general.h"




using namespace argos;
using namespace std;


/**
 * @brief The standard main() function to run the AHSS simulator.
 *
 * This main() function provides tha basic functionalities to run the AHSS
 * simulator: parses the command line, loads the experiment, runs the
 * simulation and disposes all the data.
 *
 * @param argc the number of command line arguments given at the shell.
 * @param argv the actual command line arguments.
 *
 * @return 0 if everything OK; -1 in case of errors.
 *
 */
int main(int argc, char** argv) {

   ////////////////////////////////////////////////////////////////////////////////
   // get an instance for the simulator
   ////////////////////////////////////////////////////////////////////////////////
   CSimulator& cSimulator = CSimulator::GetInstance();

   ////////////////////////////////////////////////////////////////////////////////
   // parse the command line
   ////////////////////////////////////////////////////////////////////////////////

   // fitness base file
   string s_working_dir = "";
      
   // define the class for parsing the command line
   CARGoSCommandLineArgParser cCommandLineArgs;
   
   // argument -f - fitness file
   cCommandLineArgs.AddArgument<std::string>('d',
					     "--directory",
					     "Specify the working directory",
					     s_working_dir);
   
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

      CDynamicLoading::LoadAllLibraries();
      cSimulator.SetExperimentFileName(cCommandLineArgs.GetExperimentConfigFile());
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

   CEvolutionaryLoopFunctions* pcEvoLoopFunctions = &dynamic_cast<CEvolutionaryLoopFunctions&>(cSimulator.GetLoopFunctions());

   ////////////////////////////////////////////////////////////////////////////////
   // Initialise parallel environment
   ////////////////////////////////////////////////////////////////////////////////
   MPI::Intercomm parent_comm;
   try{
      MPI::Init();       
   }
   catch(CARGoSException& ex) {
      THROW_ARGOSEXCEPTION_NESTED("Initialization error of MPI parallel environment", ex);
      SInt32 errorcode;
      MPI::COMM_WORLD.Abort(errorcode);
   }
   parent_comm = MPI::Comm::Get_parent();
   

   ////////////////////////////////////////////////////////////////////////////////
   // Evaluation Configuration
   ////////////////////////////////////////////////////////////////////////////////
   UInt32  un_genotype_length = 0;
   UInt32  un_num_teams = 0;
   UInt32  un_team_size = 0;
   UInt32  un_num_samples = 0;
   UInt32  un_num_objectives = 0;
   parent_comm.Recv( &un_genotype_length, 1, MPI_INT, 0, 1);
   parent_comm.Recv( &un_num_teams, 1, MPI_INT, 0, 1);
   parent_comm.Recv( &un_team_size, 1, MPI_INT, 0, 1);
   parent_comm.Recv( &un_num_samples, 1, MPI_INT, 0, 1);
   parent_comm.Recv( &un_num_objectives, 1, MPI_INT, 0, 1);

   UInt32 pun_teams[un_num_teams*un_team_size];
   UInt32 pun_sample_seeds[un_num_samples];
   Real pf_genotype[un_genotype_length];
   Real pf_results[un_num_objectives*un_num_samples + un_team_size];

   ////////////////////////////////////////////////////////////////////////////////
   // Start individual evaluation
   ////////////////////////////////////////////////////////////////////////////////
   while( true ) {
      SInt32  n_individual_number = -1;
      UInt32  un_num_genotypes = 0;

      // blocking calls - waiting for the parent to send information
      // receiving random seed and individual number
      CEvaluationConfig evaluation_config( un_num_teams, un_team_size );
      parent_comm.Recv( &n_individual_number, 1, MPI_INT, 0, 1);

      // check wether parent sent a stop signal (ind < 0)
      if( n_individual_number < 0 ) break;
      
      // receiving team composition
      parent_comm.Recv( pun_teams, un_num_teams*un_team_size, MPI_INT, 0, 1);
      evaluation_config.SetTeams(un_num_teams*un_team_size,pun_teams);

      // receiving control parameters
      parent_comm.Recv( &un_num_genotypes, 1, MPI_INT, 0, 1);
      for( UInt32 i = 0; i < un_num_genotypes; i++ ) {
	 UInt32 un_index;
	 parent_comm.Recv( &un_index, 1, MPI_INT, 0, 1);
	 parent_comm.Recv( pf_genotype, un_genotype_length, MPI_ARGOSREAL, 0, 1);
	 parent_comm.Recv( pun_sample_seeds, un_num_samples, MPI_INT, 0, 1);
	 evaluation_config.InsertControlParameters( un_index, CControlParameters(un_genotype_length,pf_genotype) );
      }

      // set the evaluation seed
      evaluation_config.SetSampleSeeds(CVector<UInt32>(un_num_samples,pun_sample_seeds));

      // vector for storing all the evaluation results
      TVecObjectives vec_results;
      
      // start evaluation
      for( UInt32 i = 0; i < un_num_samples; ++i ) {
	 // set the random seed in the simulator
	 cSimulator.SetRandomSeed(evaluation_config.GetSampleSeed(i));
	 
	 // set the trial number
	 pcEvoLoopFunctions->SetTrialNumber( i );

	 // resetting the experiment
	 cSimulator.Reset();
	 
	 // set the controller parameters
	 pcEvoLoopFunctions->ConfigureEvaluation( &evaluation_config );
	 pcEvoLoopFunctions->SetEvolution();
	 
	 // run the simulation
	 cSimulator.Execute();
	 
	 // retrieve the fitness values
	 CObjectives objs = pcEvoLoopFunctions->ComputePerformanceInExperiment();
	 vec_results.push_back( objs );

	 for( UInt32 j = 0; j < un_num_objectives; j++ ) {
	    pf_results[i*un_num_objectives + j] = objs[j];
	 }
      }

//       for( UInt32 i = 0; i < un_num_teams; ++i ) {
// 	 pf_results[un_num_objectives*un_num_samples + i] = evaluation_config.GetTeam()[i];
//       }

      // send the computed fitness to the parent process
      parent_comm.Send(pf_results, un_num_objectives*un_num_samples, MPI_ARGOSREAL, 0, 1);
   }
  
   // no more individuls to evaluate - destroy the simulator
   cSimulator.Destroy();

   // everything's ok, exit
   MPI_Finalize(); 
   return 0;
}
