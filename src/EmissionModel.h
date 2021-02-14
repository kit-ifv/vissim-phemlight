/********************************************************************************/
// Vissim PHEMlight Handler
// Copyright (C) 2021 Karlsruhe Institut of Technology (KIT), https://ifv.kit.edu
// PHEMlight module
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// SPDX-License-Identifier: EPL-2.0
/********************************************************************************/
/// @file    EmissionModel.h
/// @author  Sebastian Buck
/// @author  Oliver Neumann
/// @date    2021/02/14
///
/// Based on an example provided by Lukas Kautzsch
//
/****************************************************************************/

#ifndef __EMISSIONMODEL_H
#define __EMISSIONMODEL_H

#ifndef _CONSOLE
#include <windows.h>
#endif

/*==========================================================================*/

/* Global defines for use in cpp file. For example buffering */
/* parameters or handling vehicle objects in map.            */

/* general data buffer: */
double buffer_timestep = -1;

/* current vehicle data buffer: */
long buffer_veh_id = -1;
long buffer_veh_type = -1;
double buffer_veh_velocity = -1;
double buffer_veh_acceleration = -1;
double buffer_veh_weight = -1;

/* link buffer: */
double buffer_slope = -1;

/*==========================================================================*/

/* In the creation of EmissionModel.DLL all files must be compiled */
/* with the preprocessor definition EMISSIONMODEL_EXPORTS.         */
/* Programs that use EmissionModel.DLL must not be compiled        */
/* with that preprocessor definition.                              */

#ifdef EMISSIONMODEL_EXPORTS
#define EMISSIONMODEL_API extern "C" __declspec(dllexport)
#else
#define EMISSIONMODEL_API extern "C" __declspec(dllimport)
#endif

/*==========================================================================*/

/* general data: */
#define EMISSION_DATA_TIMESTEP 101
/* double: simulation time step length [s] */
#define EMISSION_DATA_TIME 102
/* double: current simulation time [s] */
#define EMISSION_DATA_TIME_OF_DAY 103
/* string: current simulation time [HH:MM:SS.CC, 24-hour format] */
#define EMISSION_DATA_EMI_LEVEL 104
/* long: EMI level in Vissim license */

/* current vehicle data: */
#define EMISSION_DATA_VEH_ID 201
/* long:   vehicle number */
#define EMISSION_DATA_VEH_TYPE 202
/* long:   vehicle type number (user defined) */
#define EMISSION_DATA_VEH_VELOCITY 203
/* double: current speed [m/s] */
#define EMISSION_DATA_VEH_ACCELERATION 204
/* double: current acceleration [m/s�] */
#define EMISSION_DATA_VEH_WEIGHT 205
/* double: vehicle weight [kg] */

/* link data: */
#define EMISSION_DATA_SLOPE 301
/* double: link slope [radians] */
#define EMISSION_DATA_LINKTYPE 302
/* double: link behavior type number */

/* emission data: */
/* must be provided by the emission model after EMISSION_COMMAND_CALCULATE */
/* for GetValue()                                                          */
#define EMISSION_DATA_BENZ 801
/* double: benzene emissions [g/s] in the current time step */
#define EMISSION_DATA_CO 802
/* double: carbon monoxide emissions [g/s] in the current time step */
#define EMISSION_DATA_CO2 803
/* double: carbon dioxide emissions [g/s] in the current time step */
#define EMISSION_DATA_HC 804
/* double: hydrocarbon emissions [g/s] in the current time step */
#define EMISSION_DATA_FUEL 805
/* double: fuel consumption [g/s] in the current time step */
#define EMISSION_DATA_NMOG 806
/* double: nonmethane organic gas emissions [g/s] in the current time step */
#define EMISSION_DATA_NMHC 807
/* double: nonmethane hydrocarbon emissions [g/s] in the current time step */
#define EMISSION_DATA_NOX 808
/* double: nitrogen oxide emissions [g/s] in the current time step */
#define EMISSION_DATA_PART 809
/* double: particulate emissions [g/s] in the current time step */
#define EMISSION_DATA_SOOT 810
/* double: soot emissions [g/s] in the current time step */
#define EMISSION_DATA_SO2 811
/* double: sulfur dioxide emissions [g/s] in the current time step */
#define EMISSION_DATA_EVAP 812
/* double: evaporation HC emissions [g/s] in the current time step */
#define EMISSION_DATA_224TMP 813
/* double: 2,2,4-trimethylpentane emissions [g/s] in the current time step */
#define EMISSION_DATA_CH4 814
/* double: methane emissions [g/s] in the current time step */
#define EMISSION_DATA_ELEM_C 815
/* double: elemental carbon emissions [g/s] in the current time step */
#define EMISSION_DATA_ETHYLBENZ 816
/* double: ethyl benzene emissions [g/s] in the current time step */
#define EMISSION_DATA_MTBE 817
/* double: methyl tert-butyl ether emissions [g/s] in the current time step */
#define EMISSION_DATA_N2O 818
/* double: dinitrogene monoxide emissions [g/s] in the current time step */
#define EMISSION_DATA_NH3 819
/* double: ammonia emissions [g/s] in the current time step */
#define EMISSION_DATA_NO3 820
/* double: nitrate emissions [g/s] in the current time step */
#define EMISSION_DATA_PM10BRA 821
/* double: PM10 brakewear emissions [g/s] in the current time step */
#define EMISSION_DATA_PM10TIR 822
/* double: PM10 tirewear emissions [g/s] in the current time step */
#define EMISSION_DATA_PM10TOT 823
/* double: PM10 total emissions [g/s] in the current time step */
#define EMISSION_DATA_PM25BRA 824
/* double: PM2.5 brakewear emissions [g/s] in the current time step */
#define EMISSION_DATA_PM25TIR 825
/* double: PM2.5 tirewear emissions [g/s] in the current time step */
#define EMISSION_DATA_PM25TOT 826
/* double: PM2.5 total emissions [g/s] in the current time step */
#define EMISSION_DATA_SULF_PART 827
/* double: sulfate particulate emissions [g/s] in the current time step */
#define EMISSION_DATA_TOG 828
/* double: total organic gases emissions [g/s] in the current time step */
#define EMISSION_DATA_TOLUENE 829
/* double: toluene emissions [g/s] in the current time step */
#define EMISSION_DATA_VOC 830
/* double: volatile organic compounds emissions [g/s] in the current time step */
#define EMISSION_DATA_XYLENE 831
/* double: xylene emissions [g/s] in the current time step */
#define EMISSION_DATA_13BUT 832
/* double: 1,3-butadiene emissions [g/s] in the current time step */
#define EMISSION_DATA_FORMALD 833
/* double: formaldheyde emissions [g/s] in the current time step */
#define EMISSION_DATA_ACETALD 834
/* double: acetaldehyde emissions [g/s] in the current time step */
#define EMISSION_DATA_HEXANE 835
/* double: hexane emissions [g/s] in the current time step */
#define EMISSION_DATA_NAPHT_GAS 836
/* double: naphtalene gas emissions [g/s] in the current time step */

/*--------------------------------------------------------------------------*/

EMISSIONMODEL_API int EmissionModelSetValue(long type,
                                            long index1,
                                            long index2,
                                            long long_value,
                                            double double_value,
                                            char *string_value);

/* Sets the value of a data object of type <type>, selected by <index1> */
/* and possibly <index2>, to <long_value>, <double_value> or            */
/* <*string_value> (object and value selection depending on <type>).    */
/* Return value is 1 on success, otherwise 0.                           */

/*--------------------------------------------------------------------------*/

EMISSIONMODEL_API int EmissionModelGetValue(long type,
                                            long index1,
                                            long index2,
                                            long *long_value,
                                            double *double_value,
                                            char **string_value);

/* Gets the value of a data object of type <type>, selected by <index1> */
/* and possibly <index2>, and writes that value to <*double_value>,     */
/* <*float_value> or <**string_value> (object and value selection       */
/* depending on <type>).                                                */
/* Return value is 1 on success, otherwise 0.                           */

/*==========================================================================*/

#define EMISSION_COMMAND_INIT 0
/* called from VISSIM once at the start of a simulation run */
/* values set before: EMISSION_DATA_TIMESTEP */
/*                    EMISSION_DATA_TIME     */

#define EMISSION_COMMAND_CREATE_VEHICLE 1
/* called from VISSIM once per vehicle entering the network */
/* values set before: EMISSION_DATA_VEH_ID               */
/*                    EMISSION_DATA_VEH_TYPE             */

#define EMISSION_COMMAND_KILL_VEHICLE 2
/* called from VISSIM once per vehicle leaving the network */
/* value set before: EMISSION_DATA_VEH_ID */

#define EMISSION_COMMAND_CALCULATE_VEHICLE 3
/* called from VISSIM once per time step during a simulation run */
/* values set before: all values                                 */

/*--------------------------------------------------------------------------*/

EMISSIONMODEL_API int EmissionModelExecuteCommand(long number);

/* Executes the command <number> if that is available in the emission */
/* module. Return value is 1 on success, otherwise 0.                 */

/*==========================================================================*/

#endif /* __EMISSIONMODEL_H */
