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
 * @file <simulator/results_parser.cpp>
 *
 * @brief This file provides a standalone program to parse genotypes and other results
 *
 * In this file the standard main() function is defined. It provides the
 * basic functionalities to run the AHSS simulator: parses the command line,
 * loads the experiment, runs the simulation and disposes all the data.
 * 
 * @author Eliseo Ferrante  - <eliseo.ferrante@bio.kuleuven.be>
 * @author Vito Trianni     - <vito.trianni@istc.cnr.it>
 */

#include <utility/genotype.h>

#include <iostream>
#include <unistd.h>

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
 


int main(int argc, char** argv) {
   UInt32 uPopSize = 1000;
   UInt32 uFoundingTeamSize = 2;
   string filename = "/home/ubuntu/Dropbox/evobranching/genotype.log";
   
   std::ifstream in;
   in.open(filename.c_str(), ios::in);
   std::string line;
   
   LOGERR << "Starting parsing " << std::endl;
   
   for(UInt32 i = 0 ; i < uPopSize ; ++i){
      CGenotype cFoundingTeam[uFoundingTeamSize];
      std::getline(in, line);  // have line i
      stringstream iss(line);
      
      LOGERR << "Read line: " << line << std::endl << std::endl;
      
      for(UInt32 j = 0 ; j < uFoundingTeamSize ; ++j){
         LOGERR << "Ind " << i << " member " << j << std::endl;
         iss >> cFoundingTeam[j] ;
         //LOGERR << cFoundingTeam[j] << std::endl;
      }
   }

   // // everything's ok, exit
   return 0;
}


