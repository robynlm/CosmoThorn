/*  File produced by Kranc */

#include "cctk.h"
#include "cctk_Arguments.h"
#include "cctk_Parameters.h"
#include "cctk_Faces.h"
#include "util_Table.h"
#include "Symmetry.h"
#include "Kranc.hh"


/* the boundary treatment is split into 3 steps:    */
/* 1. excision                                      */
/* 2. symmetries                                    */
/* 3. "other" boundary conditions, e.g. radiative */

/* to simplify scheduling and testing, the 3 steps  */
/* are currently applied in separate functions      */


extern "C" void CosmoLapse_CheckBoundaries(CCTK_ARGUMENTS)
{
#ifdef DECLARE_CCTK_ARGUMENTS_CosmoLapse_CheckBoundaries
  DECLARE_CCTK_ARGUMENTS_CHECKED(CosmoLapse_CheckBoundaries);
#else
  DECLARE_CCTK_ARGUMENTS;
#endif
  DECLARE_CCTK_PARAMETERS;
  
  return;
}

extern "C" void CosmoLapse_SelectBoundConds(CCTK_ARGUMENTS)
{
#ifdef DECLARE_CCTK_ARGUMENTS_CosmoLapse_SelectBoundConds
  DECLARE_CCTK_ARGUMENTS_CHECKED(CosmoLapse_SelectBoundConds);
#else
  DECLARE_CCTK_ARGUMENTS;
#endif
  DECLARE_CCTK_PARAMETERS;
  
  CCTK_INT ierr CCTK_ATTRIBUTE_UNUSED = 0;
  
  if (CCTK_EQUALS(propertime_bound, "none"  ) ||
      CCTK_EQUALS(propertime_bound, "static") ||
      CCTK_EQUALS(propertime_bound, "flat"  ) ||
      CCTK_EQUALS(propertime_bound, "zero"  ) )
  {
    ierr = KrancBdy_SelectGroupForBC(cctkGH, CCTK_ALL_FACES, 1, -1,
                      "CosmoLapse::propertime", propertime_bound);
    if (ierr < 0)
       CCTK_ERROR("Failed to register propertime_bound BC for CosmoLapse::propertime!");
  }
  
  if (CCTK_EQUALS(tau_bound, "none"  ) ||
      CCTK_EQUALS(tau_bound, "static") ||
      CCTK_EQUALS(tau_bound, "flat"  ) ||
      CCTK_EQUALS(tau_bound, "zero"  ) )
  {
    ierr = KrancBdy_SelectVarForBC(cctkGH, CCTK_ALL_FACES, 1, -1,
                      "CosmoLapse::tau", tau_bound);
    if (ierr < 0)
       CCTK_ERROR("Failed to register tau_bound BC for CosmoLapse::tau!");
  }
  
  if (CCTK_EQUALS(propertime_bound, "radiative"))
  {
   /* select radiation boundary condition */
    static CCTK_INT handle_propertime_bound CCTK_ATTRIBUTE_UNUSED = -1;
    if (handle_propertime_bound < 0) handle_propertime_bound = Util_TableCreate(UTIL_TABLE_FLAGS_CASE_INSENSITIVE);
    if (handle_propertime_bound < 0) CCTK_ERROR("could not create table!");
    if (Util_TableSetReal(handle_propertime_bound , propertime_bound_limit, "LIMIT") < 0)
       CCTK_ERROR("could not set LIMIT value in table!");
    if (Util_TableSetReal(handle_propertime_bound ,propertime_bound_speed, "SPEED") < 0)
       CCTK_ERROR("could not set SPEED value in table!");
  
    ierr = KrancBdy_SelectGroupForBC(cctkGH, CCTK_ALL_FACES, 1, handle_propertime_bound, 
                      "CosmoLapse::propertime", "Radiation");
  
    if (ierr < 0)
       CCTK_ERROR("Failed to register Radiation BC for CosmoLapse::propertime!");
  
  }
  
  if (CCTK_EQUALS(tau_bound, "radiative"))
  {
   /* select radiation boundary condition */
    static CCTK_INT handle_tau_bound CCTK_ATTRIBUTE_UNUSED = -1;
    if (handle_tau_bound < 0) handle_tau_bound = Util_TableCreate(UTIL_TABLE_FLAGS_CASE_INSENSITIVE);
    if (handle_tau_bound < 0) CCTK_ERROR("could not create table!");
    if (Util_TableSetReal(handle_tau_bound , tau_bound_limit, "LIMIT") < 0)
       CCTK_ERROR("could not set LIMIT value in table!");
    if (Util_TableSetReal(handle_tau_bound ,tau_bound_speed, "SPEED") < 0)
        CCTK_ERROR("could not set SPEED value in table!");
  
    ierr = KrancBdy_SelectVarForBC(cctkGH, CCTK_ALL_FACES, 1, handle_tau_bound, 
                      "CosmoLapse::tau", "Radiation");
  
    if (ierr < 0)
       CCTK_ERROR("Failed to register Radiation BC for CosmoLapse::tau!");
  
  }
  
  if (CCTK_EQUALS(propertime_bound, "scalar"))
  {
   /* select scalar boundary condition */
    static CCTK_INT handle_propertime_bound CCTK_ATTRIBUTE_UNUSED = -1;
    if (handle_propertime_bound < 0) handle_propertime_bound = Util_TableCreate(UTIL_TABLE_FLAGS_CASE_INSENSITIVE);
    if (handle_propertime_bound < 0) CCTK_ERROR("could not create table!");
    if (Util_TableSetReal(handle_propertime_bound ,propertime_bound_scalar, "SCALAR") < 0)
        CCTK_ERROR("could not set SCALAR value in table!");
  
    ierr = KrancBdy_SelectGroupForBC(cctkGH, CCTK_ALL_FACES, 1, handle_propertime_bound, 
                      "CosmoLapse::propertime", "scalar");
  
    if (ierr < 0)
       CCTK_ERROR("Failed to register Scalar BC for CosmoLapse::propertime!");
  
  }
  
  if (CCTK_EQUALS(tau_bound, "scalar"))
  {
   /* select scalar boundary condition */
    static CCTK_INT handle_tau_bound CCTK_ATTRIBUTE_UNUSED = -1;
    if (handle_tau_bound < 0) handle_tau_bound = Util_TableCreate(UTIL_TABLE_FLAGS_CASE_INSENSITIVE);
    if (handle_tau_bound < 0) CCTK_ERROR("could not create table!");
    if (Util_TableSetReal(handle_tau_bound ,tau_bound_scalar, "SCALAR") < 0)
      CCTK_ERROR("could not set SCALAR value in table!");
  
    ierr = KrancBdy_SelectVarForBC(cctkGH, CCTK_ALL_FACES, 1, handle_tau_bound, 
                      "CosmoLapse::tau", "scalar");
  
    if (ierr < 0)
       CCTK_ERROR("Error in registering Scalar BC for CosmoLapse::tau!");
  
  }
  return;
}



/* template for entries in parameter file:
#$bound$#CosmoLapse::propertime_bound       = "skip"
#$bound$#CosmoLapse::propertime_bound_speed = 1.0
#$bound$#CosmoLapse::propertime_bound_limit = 0.0
#$bound$#CosmoLapse::propertime_bound_scalar = 0.0

#$bound$#CosmoLapse::tau_bound       = "skip"
#$bound$#CosmoLapse::tau_bound_speed = 1.0
#$bound$#CosmoLapse::tau_bound_limit = 0.0
#$bound$#CosmoLapse::tau_bound_scalar = 0.0

*/

