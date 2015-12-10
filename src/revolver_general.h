/* -*- Mode:C++ -*- */

#ifndef REVOLVER_GENERAL_H
#define REVOLVER_GENERAL_H

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
 * @file <revolver_general.h>
 *
 * @author Vito Trianni - <vito.trianni@istc.cnr.it>
 */

//#include <argos3/core/config.h>

#ifdef ARGOS_USE_DOUBLE
#define MPI_ARGOSREAL MPI_DOUBLE
#else
#define MPI_ARGOSREAL MPI_FLOAT
#endif




#endif
