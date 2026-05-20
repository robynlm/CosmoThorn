/*  File produced by Kranc */

#include "cctk.h"
#include "cctk_Arguments.h"
#include "cctk_Parameters.h"

extern "C" void CosmoLapse_CTD_RegisterVars(CCTK_ARGUMENTS)
{
  #ifdef DECLARE_CCTK_ARGUMENTS_CosmoLapse_CTD_RegisterVars
  DECLARE_CCTK_ARGUMENTS_CHECKED(CosmoLapse_CTD_RegisterVars);
  #else
  DECLARE_CCTK_ARGUMENTS;
  #endif
  DECLARE_CCTK_PARAMETERS;
  
  CCTK_INT ierr CCTK_ATTRIBUTE_UNUSED = 0;
  /* Register all the evolved grid functions with MoL */
  ierr += MoLRegisterEvolved(CCTK_VarIndex("CosmoLapse_CTD::Ktransition"),  CCTK_VarIndex("CosmoLapse_CTD::Ktransitionrhs"));
  ierr += MoLRegisterEvolved(CCTK_VarIndex("CosmoLapse_CTD::tau"),  CCTK_VarIndex("CosmoLapse_CTD::taurhs"));
  /* Register all the evolved Array functions with MoL */
  return;
}
