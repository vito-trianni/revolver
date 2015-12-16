#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_

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
 * @file <simulator/definitions.h>
 *
 * @author Eliseo Ferrante  - <eliseo.ferrante@bio.kuleuven.be>
 * @author Vito Trianni - <vito.trianni@istc.cnr.it>
 */

#include <argos_utility/logging/argos_log.h>

#include <utility/vector.h>
#include <utility/control_parameters.h>

using namespace std;
using namespace argos;

struct FoundingTeam {
  CControlParameters t_motherParams;
  vector<CControlParameters> t_vec_fathersParams;
};


#endif
