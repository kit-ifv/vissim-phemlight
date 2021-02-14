/********************************************************************************/
// Vissim PHEMlight Handler
// Copyright (C) 2021 Karlsruhe Institut of Technology (KIT), https://ifv.kit.edu
// PHEMlight module
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// SPDX-License-Identifier: EPL-2.0
/********************************************************************************/
/// @file    PHEMlightHandler.cpp
/// @author  Sebastian Buck
/// @author  Oliver Neumann
/// @date    2021/02/14
///
//
/****************************************************************************/

#include "PHEMlightHandler.h"

#define DEBUG_PHEM_LIGHT 0

#if DEBUG_PHEM_LIGHT > 0

std::ofstream debug_phem_light("phemlight.txt");

#endif

#define PROFILE_PHEM_LIGHT 0

#if PROFILE_PHEM_LIGHT > 0

#include <iostream>
#include <fstream>

#include <chrono>
using default_time = std::chrono::nanoseconds;

std::ofstream profile_phem("profile_phem.txt");

#endif

phem_light_handler::phem_light_handler()
{
  // Initalise cache for faster access
  cached_emission = NULL;
  cached_emission_id = -1;
  cached_vehicle = NULL;
  cached_vehicle_id = -1;

  // Initialise PHEMlight helper and cep class and
  helper_init = false;
}

phem_light_handler::~phem_light_handler()
{
  cached_emission = NULL;
  cached_emission_id = -1;
  cached_vehicle = NULL;
  cached_vehicle_id = -1;

  for (std::map<long, vehicle *>::iterator iterator_vehicles = vehicles.begin(); iterator_vehicles != vehicles.end(); iterator_vehicles++)
  {
    delete iterator_vehicles->second;
  }

  for (std::map<long, emission *>::iterator iterator_emissions = emissions.begin(); iterator_emissions != emissions.end(); iterator_emissions++)
  {
    delete iterator_emissions->second;
  }

  for (std::map<long, PHEMlightdll::Helpers *>::iterator iterator_helpers = helpers.begin(); iterator_helpers != helpers.end(); iterator_helpers++)
  {
    delete iterator_helpers->second;
  }

  for (std::map<long, PHEMlightdll::CEPHandler *>::iterator iterator_cep_handlers = cep_handlers.begin(); iterator_cep_handlers != cep_handlers.end(); iterator_cep_handlers++)
  {
    delete iterator_cep_handlers->second;
  }
}

bool phem_light_handler::read_config()
{
  // define config file input stream
  ifstream config("Vissim_PHEMlight.cfg");
  string line;
  string base_path = "";
  if (config.is_open())
  {
    // if file could be opened
    while (getline(config, line))
    {
      // read line
      if (line.length() > 0 && line[0] != '#')
      {
        // skip lines with "#" at the beginning
        if (line.find("PATH") != string::npos)
        {
          // line contains "PATH" so base path should be defined
          base_path = line.substr(line.find("=") + 1);

          // skip empty spaces
          base_path.erase(0, base_path.find_first_not_of(' '));
          base_path.erase(base_path.find_last_not_of(' ') + 1);

          if (base_path.back() != '\\')
          {
            // check whether the path ends with "\" if not, append "\" to string
            base_path += "\\";
          }
        }
        else
        {
          // parse config line
          vector<string> cells;
          istringstream linestream(line);
          string s;
          while (getline(linestream, s, ';'))
          {
            // split line by ";"
            cells.push_back(s);
          }

          long vissim_id = -1;
          if (cells[0].compare("DEFAULT") != 0)
          {
            // if vehicle id is not default set id
            vissim_id = stoi(cells[0]);
          }
          string vehicle_type = cells[1];
          string power_type = cells[2];
          string eu_class = cells[3];

          // fleet currently not supported
          // helper.setclass nor working
          PHEMlightdll::Helpers *helper = new PHEMlightdll::Helpers();
          PHEMlightdll::CEPHandler *cep_handler = new PHEMlightdll::CEPHandler();

          bool valid = false;
          if (eu_class.substr(0, 2).compare("EU") == 0)
          {
            // if eu class starts with "EU" helper class must be initialized with _eu_class
            valid = helper->setclass(vehicle_type + "_" + power_type + "_" + eu_class);
          }
          else
          {
            // otherwise without _eu_class
            valid = helper->setclass(vehicle_type + "_" + power_type);
          }

          if (!valid)
          {
// return false if parsing failed
#if DEBUG_PHEM_LIGHT >= 1
            debug_phem_light << "<ERROR> " << helper->getErrMsg() << std::endl;
#endif
            return false;
          }
          // otherwise set helper class
          helper->setvClass(vehicle_type);
          helper->settClass(power_type);
          helper->seteClass(eu_class);
          helper->setCommentPrefix("c");

          std::vector<std::string> vec = std::vector<std::string>();
          vec.push_back(base_path);
          if (!cep_handler->GetCEP(vec, helper))
          {
// return false if get cep failed
#if DEBUG_PHEM_LIGHT >= 1
            debug_phem_light << "<ERROR> While getting cep." << std::endl;
            debug_phem_light << base_path << std::endl;
#endif
            return false;
          }
          if (vissim_id == -1)
          {
            // if vehicle id is -1 the default helper and cep_handler will be set
            default_helper = helper;
            default_cep_handler = cep_handler;
          }
          else
          {
            // otherwise helper and cep_handler will be added to PHEMlightHandler
            create_phemlight_helper(vissim_id, helper);
            create_phemlight_cep_handlers(vissim_id, cep_handler);
          }
        }
      }
    }
    config.close();
  }
  else
  {
// error while trying to read config file
#if DEBUG_PHEM_LIGHT >= 1
    debug_phem_light << "<ERROR> Unable to find or open config.txt in project directory." << std::endl;
#endif
    return false;
  }
  return true;
}

bool phem_light_handler::create_phemlight_helper(long id, PHEMlightdll::Helpers *helper)
{
#if PROFILE_PHEM_LIGHT > 0
  auto start = std::chrono::high_resolution_clock::now();
#endif

  // assume phemlight helper id not existing
  if (helpers.find(id) == helpers.end())
  {
    // no matching id found -> can insert helper
    // insert helper to map
    helpers.insert(std::pair<long, PHEMlightdll::Helpers *>(id, helper));

#if PROFILE_PHEM_LIGHT > 0
    auto end = std::chrono::high_resolution_clock::now();
    profile_phem << "PHEM_CREATE_HELPER;" << std::chrono::duration_cast<default_time>(end - start).count() << std::endl;
#endif

    return true;
  }
  else
  {
// found existing helper for id -> can't insert helper
#if DEBUG_PHEM_LIGHT >= 1
    debug_phem_light << "<ERROR> Vehicle with id " << id << " already exists in create_vehicle." << std::endl;
#endif
    return false;
  }
}

PHEMlightdll::Helpers *phem_light_handler::get_phemlight_helper(long id)
{
#if PROFILE_PHEM_LIGHT > 0
  auto start = std::chrono::high_resolution_clock::now();
#endif

  // assume id is in helper
  std::map<long, PHEMlightdll::Helpers *>::iterator element = helpers.find(id);
  PHEMlightdll::Helpers *helper;
  if (element == helpers.end())
  {
    // no helper for given id found -> return default helper
    helper = default_helper;
  }
  else
  {
    helper = element->second;
  }

#if PROFILE_PHEM_LIGHT > 0
  auto end = std::chrono::high_resolution_clock::now();
  profile_phem << "PHEM_GET_HELPER;" << std::chrono::duration_cast<default_time>(end - start).count() << std::endl;
#endif

  // return helper
  return helper;
}

bool phem_light_handler::create_phemlight_cep_handlers(long id, PHEMlightdll::CEPHandler *cep_handler)
{
#if PROFILE_PHEM_LIGHT > 0
  auto start = std::chrono::high_resolution_clock::now();
#endif

  // assume phemlight handler id not existing
  if (cep_handlers.find(id) == cep_handlers.end())
  {
    // no matching id found -> can insert handler
    // insert handler to map
    cep_handlers.insert(std::pair<long, PHEMlightdll::CEPHandler *>(id, cep_handler));

#if PROFILE_PHEM_LIGHT > 0
    auto end = std::chrono::high_resolution_clock::now();
    profile_phem << "PHEM_CREATE_CEP_HANDLER;" << std::chrono::duration_cast<default_time>(end - start).count() << std::endl;
#endif

    return true;
  }
  else
  {
// found existing handler for id -> can't insert handler
#if DEBUG_PHEM_LIGHT >= 1
    debug_phem_light << "<ERROR> Vehicle with id " << id << " already exists in create_vehicle." << std::endl;
#endif
    return false;
  }
}

PHEMlightdll::CEPHandler *phem_light_handler::get_phemlight_cep_handlers(long id)
{
#if PROFILE_PHEM_LIGHT > 0
  auto start = std::chrono::high_resolution_clock::now();
#endif

  // assume id is in handler
  std::map<long, PHEMlightdll::CEPHandler *>::iterator element = cep_handlers.find(id);
  PHEMlightdll::CEPHandler *cep_handler;
  if (element == cep_handlers.end())
  {
    // no handler for given id found -> return default cep_handler
    cep_handler = default_cep_handler;
  }
  else
  {
    cep_handler = element->second;
  }

#if PROFILE_PHEM_LIGHT > 0
  auto end = std::chrono::high_resolution_clock::now();
  profile_phem << "PHEM_GET_CEP_HANDLER;" << std::chrono::duration_cast<default_time>(end - start).count() << std::endl;
#endif

  // return handler
  return cep_handler;
}

bool phem_light_handler::create_vehicle(long id, long type)
{
#if PROFILE_PHEM_LIGHT > 0
  auto start = std::chrono::high_resolution_clock::now();
#endif

  // assume vehicle id not existing
  if (vehicles.find(id) == vehicles.end())
  {
    // no matching id found -> can insert vehicle
    vehicle *new_vehicle = new vehicle(type);

    // set cache
    cached_vehicle = new_vehicle;
    cached_vehicle_id = id;

    // insert vehicle to map
    vehicles.insert(std::pair<long, vehicle *>(id, new_vehicle));

#if PROFILE_PHEM_LIGHT > 0
    auto end = std::chrono::high_resolution_clock::now();
    profile_phem << "PHEM_CREATE_VEHICLE;" << std::chrono::duration_cast<default_time>(end - start).count() << std::endl;
#endif

    return true;
  }
  else
  {
// found existing vehicle id -> can't insert vehicle
#if DEBUG_PHEM_LIGHT >= 1
    debug_phem_light << "<ERROR> Vehicle with id " << id << " already exists in create_vehicle." << std::endl;
#endif
    return false;
  }
}

bool phem_light_handler::destroy_vehicle(long id)
{
#if PROFILE_PHEM_LIGHT > 0
  auto start = std::chrono::high_resolution_clock::now();
#endif

  // assume vehicle id existing
  // clear cache
  if (cached_emission_id == id)
  {
    cached_emission_id = -1;
    cached_emission = NULL;
  }
  if (cached_vehicle_id == id)
  {
    cached_vehicle_id = -1;
    cached_vehicle = NULL;
  }

  // check for delete vehicle
  std::map<long, vehicle *>::iterator vehicles_element = vehicles.find(id);
  if (vehicles_element == vehicles.end())
  {
// no matching id found -> can't remove vehicle from map
#if DEBUG_PHEM_LIGHT >= 1
    debug_phem_light << "<ERROR> No vehicle for id " << id << " found in destroy_vehicle." << std::endl;
#endif
    return false;
  }

  // check for delete emission
  std::map<long, emission *>::iterator emission_element = emissions.find(id);
  if (emission_element == emissions.end())
  {
// no matching id found -> can't remove vehicle from map
#if DEBUG_PHEM_LIGHT >= 1
    debug_phem_light << "<ERROR> No emission for id " << id << " found in destroy_vehicle." << std::endl;
#endif
    return false;
  }

  // found existing vehicle id -> can remove vehicle from map
  delete vehicles_element->second;
  vehicles.erase(id);

  // found existing vehicle id -> can remove vehicle from map
  delete emission_element->second;
  emissions.erase(id);

#if PROFILE_PHEM_LIGHT > 0
  auto end = std::chrono::high_resolution_clock::now();
  profile_phem << "PHEM_DESTROY_VEHICLE;" << std::chrono::duration_cast<default_time>(end - start).count() << std::endl;
#endif

  return true;
}

vehicle *phem_light_handler::get_vehicle(long id)
{
#if PROFILE_PHEM_LIGHT > 0
  auto start = std::chrono::high_resolution_clock::now();
#endif

  // assume id is in vehicles
  // check cached vehicle first
  vehicle *veh;
  if (cached_vehicle_id == id)
  {
    veh = cached_vehicle;
  }
  else
  {
    std::map<long, vehicle *>::iterator element = vehicles.find(id);
    if (element == vehicles.end())
    {
// no emission for given id found
#if DEBUG_PHEM_LIGHT >= 2
      debug_phem_light << "<ERROR> No vehicle for id " << id << " found in get_vehicle." << std::endl;
#endif
      return NULL;
    }
    else
    {
      // vehicle for given id exists
      // update cache
      cached_vehicle_id = id;
      cached_vehicle = element->second;

      veh = element->second;
    }
  }

#if PROFILE_PHEM_LIGHT > 0
  auto end = std::chrono::high_resolution_clock::now();
  profile_phem << "PHEM_GET_VEHICLE;" << std::chrono::duration_cast<default_time>(end - start).count() << std::endl;
#endif

  // return vehicle
  return veh;
}

emission *phem_light_handler::calculate_vehicle_emission(vehicle *veh)
{
#if PROFILE_PHEM_LIGHT > 0
  auto start = std::chrono::high_resolution_clock::now();
#endif

  PHEMlightdll::Helpers *helper = get_phemlight_helper(veh->type);
  PHEMlightdll::CEPHandler *cep_handler = get_phemlight_cep_handlers(veh->type);
  if (cep_handler->getCEPS().find(helper->getgClass()) != cep_handler->getCEPS().end())
  {
    // CEPS found
    PHEMlightdll::CEP *cep = cep_handler->getCEPS().find(helper->getgClass())->second;

    /***
    * Actually from PHEMlight cs code in starter.
    * Fleet currently not working because missing code in sumo cpp.
    ***/
    double time = veh->timestep;
    double gradient = veh->slope;

    // set speed and acceleration with limitations
    double velocity = 0;
    if (veh->velocity > 0)
    {
      velocity = veh->velocity;
    }
    double acceleration = veh->acceleration;
    if (velocity == 0)
    {
      acceleration = 0;
    }
    else if (acceleration > cep->GetMaxAccel(velocity, gradient))
    {
      acceleration = cep->GetMaxAccel(velocity, gradient);
    }

    // calculate the power
    double power = cep->CalcPower(velocity, acceleration, gradient);
    double energie = cep->CalcEngPower(power);

    emission *emis = new emission(0.0);
    // calculate result if BEV
    if (helper->gettClass() == PHEMlightdll::Constants::strBEV)
    {
      emis->fuel_consumption = cep->GetEmission("FC", power, velocity, helper) / 3600.0;
      emis->norm_drive = energie / cep->getDrivingPower();
      emis->norm_rated = energie / cep->getRatedPower();
      emis->co = 0;
      emis->co2 = 0;
      emis->hc = 0;
      emis->nox = 0;
      emis->pm = 0;
    }
    else
    {
      double decel_coast = cep->GetDecelCoast(velocity, acceleration, gradient);

      // calculate the result values (zero emissions by costing, idling emissions by v <= 0.5m/s�)
      if (acceleration >= decel_coast || velocity <= PHEMlightdll::Constants::ZERO_SPEED_ACCURACY)
      {
        double fuel_consumption = cep->GetEmission("FC", power, velocity, helper);
        emis->norm_drive = energie / cep->getDrivingPower();
        emis->norm_rated = energie / cep->getRatedPower();
        double co = cep->GetEmission("CO", power, velocity, helper);
        double hc = cep->GetEmission("HC", power, velocity, helper);
        double nox = cep->GetEmission("NOx", power, velocity, helper);
        double pm = cep->GetEmission("PM", power, velocity, helper);
        emis->co2 = cep->GetCO2Emission(fuel_consumption, co, hc, helper) / 3600.0;
        emis->fuel_consumption = fuel_consumption / 3600.0;
        emis->co = co / 3600.0;
        emis->hc = hc / 3600.0;
        emis->nox = nox / 3600.0;
        emis->pm = pm / 3600.0;
      }
      else
      {
        emis->fuel_consumption = 0;
        emis->norm_drive = energie / cep->getDrivingPower();
        emis->norm_rated = energie / cep->getRatedPower();
        emis->co = 0;
        emis->co2 = 0;
        emis->hc = 0;
        emis->nox = 0;
        emis->pm = 0;
      }
    }

#if DEBUG_PHEM_LIGHT >= 2
    debug_phem_light << "<MSG> Calculation Done. Calculated FC " << emis->fuel_consumption << " ND " << emis->norm_drive << " NDR " << emis->norm_rated
                     << " CO " << emis->co << " CO2 " << emis->co2 << " HC " << emis->hc << " NOx " << emis->nox << " PM " << emis->pm << std::endl;
#endif

#if PROFILE_PHEM_LIGHT > 0
    auto end = std::chrono::high_resolution_clock::now();
    profile_phem << "PHEM_CALC_EMISSION;" << std::chrono::duration_cast<default_time>(end - start).count() << std::endl;
#endif

    return emis;
  }
  else
  {
// no entry in CEPS found
#if DEBUG_PHEM_LIGHT >= 1
    debug_phem_light << "<Error> No CEPS found." << std::endl;
#endif
    return NULL;
  }
}

bool phem_light_handler::calculate_vehicle_emission(long id)
{
#if PROFILE_PHEM_LIGHT > 0
  auto start = std::chrono::high_resolution_clock::now();
#endif

  if (helper_init == false)
  {
    read_config();
    helper_init = true;
  }
  // free emission object if necessary and set new one
  // check cached vehicle first
  vehicle *veh = NULL;
  if (cached_vehicle_id == id)
  {
    veh = cached_vehicle;
  }
  else
  {
    std::map<long, vehicle *>::iterator element = vehicles.find(id);
    if (element == vehicles.end())
    {
// no vehicle for given id found
#if DEBUG_PHEM_LIGHT >= 1
      debug_phem_light << "<ERROR> No vehicle for id " << id << " found in calculate_vehicle_emission." << std::endl;
#endif
      return false;
    }
    else
    {
      veh = element->second;
      cached_vehicle_id = id;
      cached_vehicle = veh;
    }
  }

  // calculate emission
  emission *emis = calculate_vehicle_emission(veh);

  if (emis == NULL)
  {
    return false;
  }

  // update cache
  cached_emission = emis;
  cached_emission_id = id;

  // insert or replace emission
  if (emissions.find(id) == emissions.end())
  {
    // no emission for given id found -> only insert emission for id
    emissions.insert(std::pair<long, emission *>(id, emis));
  }
  else
  {
    // emission for given id exists -> free old emission and set new one
    delete emissions.find(id)->second;
    emissions.erase(id);
    emissions.insert(std::pair<long, emission *>(id, emis));
  }

#if PROFILE_PHEM_LIGHT > 0
  auto end = std::chrono::high_resolution_clock::now();
  profile_phem << "PHEM_CALC_EMISSION_PUB;" << std::chrono::duration_cast<default_time>(end - start).count() << std::endl;
#endif

  return true;
}

emission *phem_light_handler::get_vehicle_emission(long id)
{
#if PROFILE_PHEM_LIGHT > 0
  auto start = std::chrono::high_resolution_clock::now();
#endif

  // assume id is in emissions
  // check cached emission first
  emission *emis;
  if (cached_emission_id == id)
  {
    emis = cached_emission;
  }
  else
  {
    std::map<long, emission *>::iterator element = emissions.find(id);
    if (element == emissions.end())
    {
      // no emission for given id found
#if DEBUG_PHEM_LIGHT >= 1
      debug_phem_light << "<ERROR> Vehicle id for get request not found." << std::endl;
#endif
      return NULL;
    }
    else
    {
#if DEBUG_PHEM_LIGHT >= 3
      debug_phem_light << "<GET> Returning emission for vehicle " << id << "." << std::endl;
#endif
      // emission for given id exists
      // update cache
      this->cached_emission_id = id;
      this->cached_emission = element->second;

      emis = element->second;
    }
  }

#if PROFILE_PHEM_LIGHT > 0
  auto end = std::chrono::high_resolution_clock::now();
  profile_phem << "PHEM_GET_VEHICLE_EMISSION;" << std::chrono::duration_cast<default_time>(end - start).count() << std::endl;
#endif

  // return emission
  return emis;
}
