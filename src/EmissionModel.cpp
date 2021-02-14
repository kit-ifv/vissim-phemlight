/********************************************************************************/
// Vissim PHEMlight Handler
// Copyright (C) 2021 Karlsruhe Institut of Technology (KIT), https://ifv.kit.edu
// PHEMlight module
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// SPDX-License-Identifier: EPL-2.0
/********************************************************************************/
/// @file    EmissionModel.cpp
/// @author  Sebastian Buck
/// @author  Oliver Neumann
/// @date    2021/02/14
///
/// Based on an example provided by Lukas Kautzsch
//
/****************************************************************************/

#include "EmissionModel.h"
#include "PHEMlightHandler.h"

#define DEBUG_EMISSION_MODEL 0

#if DEBUG_EMISSION_MODEL > 0

#include <iostream>
#include <fstream>

std::ofstream debug_emission_model("emissionmodel.txt");

#endif

#define PROFILE_EMISSION_MODEL 0

#if PROFILE_EMISSION_MODEL > 0

#include <iostream>
#include <fstream>

#include <chrono>
using default_time = std::chrono::nanoseconds;

std::ofstream profile_em("profile_em.txt");

#endif

phem_light_handler phem = phem_light_handler();

/*==========================================================================*/

BOOL APIENTRY DllMain(HANDLE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved)
{
  switch (ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH:
    break;
  }
  return TRUE;
}

/*==========================================================================*/

EMISSIONMODEL_API int EmissionModelSetValue(long type,
                                            long index1,
                                            long index2,
                                            long long_value,
                                            double double_value,
                                            char *string_value)
{
  /* Sets the value of a data object of type <type>, selected by <index1> */
  /* and possibly <index2>, to <long_value>, <double_value> or            */
  /* <*string_value> (object and value selection depending on <type>).    */
  /* Return value is 1 on success, otherwise 0.                           */

#if DEBUG_EMISSION_MODEL >= 3
  debug_emission_model << "<SET> \t\t type: " << type
                       << " index1: " << index1
                       << " index2: " << index2
                       << " long_value: " << long_value
                       << " double_value: " << double_value;

  switch (type)
  {
  case EMISSION_DATA_TIMESTEP:
    debug_emission_model << ";\t\t // EMISSION_DATA_TIMESTEP = " << double_value << std::endl;
    break;
  case EMISSION_DATA_TIME:
    debug_emission_model << ";\t\t // EMISSION_DATA_TIME unused parameter" << std::endl;
    break;
  case EMISSION_DATA_TIME_OF_DAY:
    debug_emission_model << ";\t\t // EMISSION_DATA_TIME_OF_DAY unused parameter" << std::endl;
    break;
  case EMISSION_DATA_EMI_LEVEL:
    debug_emission_model << ";\t\t // EMISSION_DATA_EMI_LEVEL unused parameter" << std::endl;
    break;
  case EMISSION_DATA_VEH_ID:
    debug_emission_model << ";\t\t // EMISSION_DATA_VEH_ID = " << long_value << std::endl;
    break;
  case EMISSION_DATA_VEH_TYPE:
    debug_emission_model << ";\t\t // EMISSION_DATA_VEH_TYPE = " << long_value << std::endl;
    break;
  case EMISSION_DATA_VEH_VELOCITY:
    debug_emission_model << ";\t\t // EMISSION_DATA_VEH_VELOCITY = " << double_value << std::endl;
    break;
  case EMISSION_DATA_VEH_ACCELERATION:
    debug_emission_model << ";\t\t // EMISSION_DATA_VEH_ACCELERATION = " << double_value << std::endl;
    break;
  case EMISSION_DATA_VEH_WEIGHT:
    debug_emission_model << ";\t\t // EMISSION_DATA_VEH_WEIGHT = " << double_value << std::endl;
    break;
  case EMISSION_DATA_SLOPE:
    debug_emission_model << ";\t\t // EMISSION_DATA_SLOPE = " << double_value << std::endl;
    break;
  case EMISSION_DATA_LINKTYPE:
    debug_emission_model << ";\t\t // EMISSION_DATA_LINKTYPE unused parameter" << std::endl;
    break;
  default:
    debug_emission_model << std::endl
                         << "<ERROR> \t\t UNKOWN SET TYPE." << std::endl;
    break;
  }
#endif

#if PROFILE_EMISSION_MODEL > 0
  auto start = std::chrono::high_resolution_clock::now();
#endif

  vehicle *veh;
  switch (type)
  {
  case EMISSION_DATA_TIMESTEP:
    buffer_timestep = double_value;
    veh = phem.get_vehicle(buffer_veh_id);
    if (veh != NULL)
    {
      veh->timestep = buffer_timestep;
    }
    break;
  case EMISSION_DATA_TIME:
    // unused parameter
    break;
  case EMISSION_DATA_TIME_OF_DAY:
    // unused parameter
    break;
  case EMISSION_DATA_EMI_LEVEL:
    // unused parameter
    break;
  case EMISSION_DATA_VEH_ID:
    buffer_veh_id = long_value;
    break;
  case EMISSION_DATA_VEH_TYPE:
    buffer_veh_type = long_value;
    break;
  case EMISSION_DATA_VEH_VELOCITY:
    buffer_veh_velocity = double_value;
    veh = phem.get_vehicle(buffer_veh_id);
    if (veh != NULL)
    {
      veh->velocity = buffer_veh_velocity;
    }
    break;
  case EMISSION_DATA_VEH_ACCELERATION:
    buffer_veh_acceleration = double_value;
    veh = phem.get_vehicle(buffer_veh_id);
    if (veh != NULL)
    {
      veh->acceleration = buffer_veh_acceleration;
    }
    break;
  case EMISSION_DATA_VEH_WEIGHT:
    buffer_veh_weight = double_value;
    veh = phem.get_vehicle(buffer_veh_id);
    if (veh != NULL)
    {
      veh->weight = buffer_veh_weight;
    }
    break;
  case EMISSION_DATA_SLOPE:
    buffer_slope = double_value;
    veh = phem.get_vehicle(buffer_veh_id);
    if (veh != NULL)
    {
      veh->slope = buffer_slope;
    }
    break;
  case EMISSION_DATA_LINKTYPE:
    // unused parameter
    break;
  default:
    return 0;
  }
#if PROFILE_EMISSION_MODEL > 0
  auto end = std::chrono::high_resolution_clock::now();
  profile_em << "EM_SET;" << std::chrono::duration_cast<default_time>(end - start).count() << std::endl;
#endif
  return 1;
}

/*--------------------------------------------------------------------------*/

EMISSIONMODEL_API int EmissionModelGetValue(long type,
                                            long index1,
                                            long index2,
                                            long *long_value,
                                            double *double_value,
                                            char **string_value)
{
  /* Gets the value of a data object of type <type>, selected by <index1> */
  /* and possibly <index2>, and writes that value to <*double_value>,     */
  /* <*float_value> or <**string_value> (object and value selection       */
  /* depending on <type>).                                                */
  /* Return value is 1 on success, otherwise 0.                           */

#if DEBUG_EMISSION_MODEL >= 3
  debug_emission_model << "<GET> \t\t type: " << type
                       << " index1: " << index1
                       << " index2: " << index2;

  switch (type)
  {
  case EMISSION_DATA_BENZ:
    debug_emission_model << ";\t\t // Getting EMISSION_DATA_BENZ" << std::endl;
    break;
  case EMISSION_DATA_CO:
    debug_emission_model << ";\t\t // Getting EMISSION_DATA_CO" << std::endl;
    break;
  case EMISSION_DATA_CO2:
    debug_emission_model << ";\t\t // Getting EMISSION_DATA_CO2" << std::endl;
    break;
  case EMISSION_DATA_HC:
    debug_emission_model << ";\t\t // Getting EMISSION_DATA_HC" << std::endl;
    break;
  case EMISSION_DATA_FUEL:
    debug_emission_model << ";\t\t // Getting EMISSION_DATA_FUEL" << std::endl;
    break;
  case EMISSION_DATA_NMOG:
    debug_emission_model << ";\t\t // Getting EMISSION_DATA_NMOG" << std::endl;
    break;
  case EMISSION_DATA_NMHC:
    debug_emission_model << ";\t\t // Getting EMISSION_DATA_NMHC" << std::endl;
    break;
  case EMISSION_DATA_NOX:
    debug_emission_model << ";\t\t // Getting EMISSION_DATA_NOX" << std::endl;
    break;
  case EMISSION_DATA_PART:
    debug_emission_model << ";\t\t // Getting EMISSION_DATA_PART" << std::endl;
    break;
  case EMISSION_DATA_SOOT:
    debug_emission_model << ";\t\t // Getting EMISSION_DATA_SOOT" << std::endl;
    break;
  case EMISSION_DATA_SO2:
    debug_emission_model << ";\t\t // Getting EMISSION_DATA_SO2" << std::endl;
    break;
  case EMISSION_DATA_EVAP:
    debug_emission_model << ";\t\t // Getting EMISSION_DATA_EVAP" << std::endl;
    break;
  default:
    debug_emission_model << std::endl
                         << "<ERROR> Asking for unkown emission type " << type << "." << std::endl;
    break;
  }
#endif

#if PROFILE_EMISSION_MODEL > 0
  auto start = std::chrono::high_resolution_clock::now();
#endif

  emission *emi = NULL;
  int i = 1;
  switch (type)
  {
  case EMISSION_DATA_CO:
    *double_value = 0.0;
    emi = phem.get_vehicle_emission(buffer_veh_id);
    if (emi != NULL)
    {
      *double_value = emi->co;
    }
#if DEBUG_EMISSION_MODEL >= 1
    else
    {
      debug_emission_model << std::endl
                           << "<ERROR> phem.get_vehicle_emission( id ) found no emission!" << type << "." << std::endl;
    }
#endif
    break;
  case EMISSION_DATA_CO2:
    *double_value = 0.0;
    emi = phem.get_vehicle_emission(buffer_veh_id);
    if (emi != NULL)
    {
      *double_value = emi->co2;
    }
#if DEBUG_EMISSION_MODEL >= 1
    else
    {
      debug_emission_model << std::endl
                           << "<ERROR> phem.get_vehicle_emission( id ) found no emission!" << type << "." << std::endl;
    }
#endif
    break;
  case EMISSION_DATA_HC:
    *double_value = 0.0;
    emi = phem.get_vehicle_emission(buffer_veh_id);
    if (emi != NULL)
    {
      *double_value = emi->hc;
    }
#if DEBUG_EMISSION_MODEL >= 1
    else
    {
      debug_emission_model << std::endl
                           << "<ERROR> phem.get_vehicle_emission( id ) found no emission!" << type << "." << std::endl;
    }
#endif
    break;
  case EMISSION_DATA_FUEL:
    *double_value = 0.0;
    emi = phem.get_vehicle_emission(buffer_veh_id);
    if (emi != NULL)
    {
      *double_value = emi->fuel_consumption;
    }
#if DEBUG_EMISSION_MODEL >= 1
    else
    {
      debug_emission_model << std::endl
                           << "<ERROR> phem.get_vehicle_emission( id ) found no emission!" << type << "." << std::endl;
    }
#endif
    break;
  case EMISSION_DATA_NOX:
    *double_value = 0.0;
    emi = phem.get_vehicle_emission(buffer_veh_id);
    if (emi != NULL)
    {
      *double_value = emi->nox;
    }
#if DEBUG_EMISSION_MODEL >= 1
    else
    {
      debug_emission_model << std::endl
                           << "<ERROR> phem.get_vehicle_emission( id ) found no emission!" << type << "." << std::endl;
    }
#endif
    break;
  case EMISSION_DATA_PART:
    *double_value = 0.0;
    emi = phem.get_vehicle_emission(buffer_veh_id);
    if (emi != NULL)
    {
      *double_value = emi->pm;
    }
#if DEBUG_EMISSION_MODEL >= 1
    else
    {
      debug_emission_model << std::endl
                           << "<ERROR> phem.get_vehicle_emission( id ) found no emission!" << type << "." << std::endl;
    }
#endif
    break;
  case EMISSION_DATA_PM10TOT:
    *double_value = 0.0;
    emi = phem.get_vehicle_emission(buffer_veh_id);
    if (emi != NULL)
    {
      *double_value = emi->pm;
    }
#if DEBUG_EMISSION_MODEL >= 1
    else
    {
      debug_emission_model << std::endl
                           << "<ERROR> phem.get_vehicle_emission( id ) found no emission!" << type << "." << std::endl;
    }
#endif
    break;
  case EMISSION_DATA_PM25TOT:
    *double_value = 0.0;
    emi = phem.get_vehicle_emission(buffer_veh_id);
    if (emi != NULL)
    {
      *double_value = emi->pm;
    }
#if DEBUG_EMISSION_MODEL >= 1
    else
    {
      debug_emission_model << std::endl
                           << "<ERROR> phem.get_vehicle_emission( id ) found no emission!" << type << "." << std::endl;
    }
#endif
    break;
  default:
    *double_value = 0;
    break;
  }
#if PROFILE_EMISSION_MODEL > 0
  auto end = std::chrono::high_resolution_clock::now();
  profile_em << "EM_GET;" << std::chrono::duration_cast<default_time>(end - start).count() << std::endl;
#endif
  return 1;
}

/*==========================================================================*/

EMISSIONMODEL_API int EmissionModelExecuteCommand(long number)
{
  /* Executes the command <number> if that is available in the emission */
  /* module. Return value is 1 on success, otherwise 0.                 */

#if DEBUG_EMISSION_MODEL >= 2
  switch (number)
  {
  case EMISSION_COMMAND_INIT:
    debug_emission_model << "<COMMAND> \t INIT" << std::endl;
    break;
  case EMISSION_COMMAND_CREATE_VEHICLE:
    debug_emission_model << "<COMMAND> \t Create Vehicle with ID " << buffer_veh_id << " and Type " << buffer_veh_type << "." << std::endl;
    break;
  case EMISSION_COMMAND_KILL_VEHICLE:
    debug_emission_model << "<COMMAND> \t Kill Vehicle with ID " << buffer_veh_id << "." << std::endl;
    break;
  case EMISSION_COMMAND_CALCULATE_VEHICLE:
    debug_emission_model << "<COMMAND> \t Calculate Emission" << std::endl;
    break;
  default:
    break;
  }
#endif

#if PROFILE_EMISSION_MODEL > 0
  auto start = std::chrono::high_resolution_clock::now();
#endif

  bool all_right = false;
  switch (number)
  {
  case EMISSION_COMMAND_INIT:
    // seems like never called
    all_right = true;
    break;
  case EMISSION_COMMAND_CREATE_VEHICLE:
    // create vehicle with given id and type from buffer
    all_right = phem.create_vehicle(buffer_veh_id, buffer_veh_type);
#if DEBUG_EMISSION_MODEL >= 2
    if (!all_right)
    {
      debug_emission_model << "<ERROR> \t\t Couldn't create vehicle with id " << buffer_veh_id << " and type " << buffer_veh_type << "." << std::endl;
    }
#endif
    break;
  case EMISSION_COMMAND_KILL_VEHICLE:
    // remove vehicle with given id
    all_right = phem.destroy_vehicle(buffer_veh_id);
#if DEBUG_EMISSION_MODEL >= 2
    if (!all_right)
    {
      debug_emission_model << "<ERROR> \t\t Couldn't destroy vehicle with id " << buffer_veh_id << "." << std::endl;
    }
#endif
    break;
  case EMISSION_COMMAND_CALCULATE_VEHICLE:
    /* ### call emission calculation here */
    all_right = phem.calculate_vehicle_emission(buffer_veh_id);
#if DEBUG_EMISSION_MODEL >= 2
    if (!all_right)
    {
      debug_emission_model << "<ERROR> \t\t Couldn't calculate vehicle emission with id " << buffer_veh_id << "." << std::endl;
    }
#endif
    break;
  default:
#if DEBUG_EMISSION_MODEL >= 1
    debug_emission_model << "<ERROR> \t\t COMMAND " << number << " UNKOWN." << std::endl;
#endif
    break;
  }

#if PROFILE_EMISSION_MODEL > 0
  auto end = std::chrono::high_resolution_clock::now();
  profile_em << "EM_EXEC;" << std::chrono::duration_cast<default_time>(end - start).count() << std::endl;
#endif

  if (all_right)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
