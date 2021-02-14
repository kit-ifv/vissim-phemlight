/********************************************************************************/
// Vissim PHEMlight Handler
// Copyright (C) 2021 Karlsruhe Institut of Technology (KIT), https://ifv.kit.edu
// PHEMlight module
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// SPDX-License-Identifier: EPL-2.0
/********************************************************************************/
/// @file    PHEMlightHandler.h
/// @author  Sebastian Buck
/// @author  Oliver Neumann
/// @date    2021/02/14
///
//
/****************************************************************************/

#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include "PHEMlight/CEP.h"
#include "PHEMlight/CEPHandler.h"
#include "PHEMlight/Constants.h"
#include "PHEMlight/Helpers.h"

using namespace std;

struct vehicle
{
  long type;
  double acceleration;
  double velocity;
  double slope;
  double weight;
  double timestep;

  vehicle() : type(-1) {}

  vehicle(long p_type) : type(p_type) {}

  vehicle(long p_type, double p_acceleration, double p_velocity, double p_slope, double p_weight, double p_timestep)
  {
    type = p_type;
    acceleration = p_acceleration;
    velocity = p_velocity;
    slope = p_slope;
    weight = p_weight;
    timestep = p_timestep;
  }
};

struct emission
{
  double fuel_consumption; // [g/s] / [kWh/s for BEV]
  double norm_drive;       //
  double norm_rated;       //
  double co;               // [g/s]
  double co2;              // [g/s]
  double hc;               // [g/s]
  double nox;              // [g/s]
  double pm;               // [g/s]

  emission(double default_value)
  {
    fuel_consumption = default_value;
    norm_drive = default_value;
    norm_rated = default_value;
    co = default_value;
    co2 = default_value;
    hc = default_value;
    nox = default_value;
    pm = default_value;
  }

  emission()
  {
    fuel_consumption = 0.1;
    norm_drive = 0.2;
    norm_rated = 0.3;
    co = 0.4;
    co2 = 0.5;
    hc = 0.6;
    nox = 0.7;
    pm = 0.8;
  }

  emission(double p_fuel_consumption, double p_norm_drive, double p_norm_rated, double p_co, double p_co2, double p_hc, double p_nox, double p_pm)
  {
    fuel_consumption = p_fuel_consumption;
    norm_drive = p_norm_drive;
    norm_rated = p_norm_rated;
    co = p_co;
    co2 = p_co2;
    hc = p_hc;
    nox = p_nox;
    pm = p_pm;
  }
};

class phem_light_handler
{

private:
  std::map<long, vehicle *> vehicles;
  std::map<long, emission *> emissions;

  // emission and vehicle cache
  emission *cached_emission;
  long cached_emission_id;
  vehicle *cached_vehicle;
  long cached_vehicle_id;

  bool helper_init;
  PHEMlightdll::Helpers *default_helper;
  PHEMlightdll::CEPHandler *default_cep_handler;
  std::map<long, PHEMlightdll::Helpers *> helpers;
  std::map<long, PHEMlightdll::CEPHandler *> cep_handlers;

  bool create_phemlight_helper(long id, PHEMlightdll::Helpers *helper);
  PHEMlightdll::Helpers *get_phemlight_helper(long id);
  bool create_phemlight_cep_handlers(long id, PHEMlightdll::CEPHandler *cep_handler);
  PHEMlightdll::CEPHandler *get_phemlight_cep_handlers(long id);

  bool read_config();

  emission *calculate_vehicle_emission(vehicle *veh);

public:
  phem_light_handler();
  ~phem_light_handler();

  bool create_vehicle(long id, long type);
  bool destroy_vehicle(long id);
  vehicle *get_vehicle(long id);
  bool calculate_vehicle_emission(long id);
  emission *get_vehicle_emission(long id);
};
