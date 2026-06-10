/*  File produced by Kranc */

#define KRANC_C

#include <algorithm>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cctk.h"
#include "cctk_Arguments.h"
#include "cctk_Parameters.h"
#include "Kranc.hh"
#include "Differencing.h"
#include "loopcontrol.h"

namespace CosmoLapse {

extern "C" void CosmoLapse_dtLapsePostStep_SelectBCs(CCTK_ARGUMENTS)
{
  #ifdef DECLARE_CCTK_ARGUMENTS_CosmoLapse_dtLapsePostStep_SelectBCs
  DECLARE_CCTK_ARGUMENTS_CHECKED(CosmoLapse_dtLapsePostStep_SelectBCs);
  #else
  DECLARE_CCTK_ARGUMENTS;
  #endif
  DECLARE_CCTK_PARAMETERS;
  
  if (cctk_iteration % CosmoLapse_dtLapsePostStep_calc_every != CosmoLapse_dtLapsePostStep_calc_offset)
    return;
  CCTK_INT ierr CCTK_ATTRIBUTE_UNUSED = 0;
  ierr = KrancBdy_SelectGroupForBC(cctkGH, CCTK_ALL_FACES, GetBoundaryWidth(cctkGH), -1 /* no table */, "ADMBase::dtlapse","flat");
  if (ierr < 0)
    CCTK_WARN(CCTK_WARN_ALERT, "Failed to register flat BC for ADMBase::dtlapse.");
  ierr = KrancBdy_SelectGroupForBC(cctkGH, CCTK_ALL_FACES, GetBoundaryWidth(cctkGH), -1 /* no table */, "CosmoLapse::Kthreshold","flat");
  if (ierr < 0)
    CCTK_WARN(CCTK_WARN_ALERT, "Failed to register flat BC for CosmoLapse::Kthreshold.");
  ierr = KrancBdy_SelectGroupForBC(cctkGH, CCTK_ALL_FACES, GetBoundaryWidth(cctkGH), -1 /* no table */, "ML_BSSN::ML_lapserhs","flat");
  if (ierr < 0)
    CCTK_WARN(CCTK_WARN_ALERT, "Failed to register flat BC for ML_BSSN::ML_lapserhs.");
  return;
}

static void CosmoLapse_dtLapsePostStep_Body(const cGH* restrict const cctkGH, const int dir, const int face, const CCTK_REAL normal[3], const CCTK_REAL tangentA[3], const CCTK_REAL tangentB[3], const int imin[3], const int imax[3], const int n_subblock_gfs, CCTK_REAL* restrict const subblock_gfs[])
{
  DECLARE_CCTK_ARGUMENTS;
  DECLARE_CCTK_PARAMETERS;
  
  /* Include user-supplied include files */
  /* Initialise finite differencing variables */
  const ptrdiff_t di CCTK_ATTRIBUTE_UNUSED = 1;
  const ptrdiff_t dj CCTK_ATTRIBUTE_UNUSED = 
    CCTK_GFINDEX3D(cctkGH,0,1,0) - CCTK_GFINDEX3D(cctkGH,0,0,0);
  const ptrdiff_t dk CCTK_ATTRIBUTE_UNUSED = 
    CCTK_GFINDEX3D(cctkGH,0,0,1) - CCTK_GFINDEX3D(cctkGH,0,0,0);
  const ptrdiff_t cdi CCTK_ATTRIBUTE_UNUSED = sizeof(CCTK_REAL) * di;
  const ptrdiff_t cdj CCTK_ATTRIBUTE_UNUSED = sizeof(CCTK_REAL) * dj;
  const ptrdiff_t cdk CCTK_ATTRIBUTE_UNUSED = sizeof(CCTK_REAL) * dk;
  const ptrdiff_t cctkLbnd1 CCTK_ATTRIBUTE_UNUSED = cctk_lbnd[0];
  const ptrdiff_t cctkLbnd2 CCTK_ATTRIBUTE_UNUSED = cctk_lbnd[1];
  const ptrdiff_t cctkLbnd3 CCTK_ATTRIBUTE_UNUSED = cctk_lbnd[2];
  const CCTK_REAL t CCTK_ATTRIBUTE_UNUSED = cctk_time;
  const CCTK_REAL cctkOriginSpace1 CCTK_ATTRIBUTE_UNUSED = 
    CCTK_ORIGIN_SPACE(0);
  const CCTK_REAL cctkOriginSpace2 CCTK_ATTRIBUTE_UNUSED = 
    CCTK_ORIGIN_SPACE(1);
  const CCTK_REAL cctkOriginSpace3 CCTK_ATTRIBUTE_UNUSED = 
    CCTK_ORIGIN_SPACE(2);
  const CCTK_REAL dt CCTK_ATTRIBUTE_UNUSED = CCTK_DELTA_TIME;
  const CCTK_REAL dx CCTK_ATTRIBUTE_UNUSED = CCTK_DELTA_SPACE(0);
  const CCTK_REAL dy CCTK_ATTRIBUTE_UNUSED = CCTK_DELTA_SPACE(1);
  const CCTK_REAL dz CCTK_ATTRIBUTE_UNUSED = CCTK_DELTA_SPACE(2);
  const CCTK_REAL dxi CCTK_ATTRIBUTE_UNUSED = pow(dx,-1);
  const CCTK_REAL dyi CCTK_ATTRIBUTE_UNUSED = pow(dy,-1);
  const CCTK_REAL dzi CCTK_ATTRIBUTE_UNUSED = pow(dz,-1);
  const CCTK_REAL khalf CCTK_ATTRIBUTE_UNUSED = 0.5;
  const CCTK_REAL kthird CCTK_ATTRIBUTE_UNUSED = 
    0.333333333333333333333333333333;
  const CCTK_REAL ktwothird CCTK_ATTRIBUTE_UNUSED = 
    0.666666666666666666666666666667;
  const CCTK_REAL kfourthird CCTK_ATTRIBUTE_UNUSED = 
    1.33333333333333333333333333333;
  const CCTK_REAL hdxi CCTK_ATTRIBUTE_UNUSED = 0.5*dxi;
  const CCTK_REAL hdyi CCTK_ATTRIBUTE_UNUSED = 0.5*dyi;
  const CCTK_REAL hdzi CCTK_ATTRIBUTE_UNUSED = 0.5*dzi;
  /* Initialize predefined quantities */
  const CCTK_REAL p1o120dx CCTK_ATTRIBUTE_UNUSED = 0.00833333333333333333333333333333*pow(dx,-1);
  const CCTK_REAL p1o120dy CCTK_ATTRIBUTE_UNUSED = 0.00833333333333333333333333333333*pow(dy,-1);
  const CCTK_REAL p1o120dz CCTK_ATTRIBUTE_UNUSED = 0.00833333333333333333333333333333*pow(dz,-1);
  const CCTK_REAL p1o12dx CCTK_ATTRIBUTE_UNUSED = 0.0833333333333333333333333333333*pow(dx,-1);
  const CCTK_REAL p1o12dy CCTK_ATTRIBUTE_UNUSED = 0.0833333333333333333333333333333*pow(dy,-1);
  const CCTK_REAL p1o12dz CCTK_ATTRIBUTE_UNUSED = 0.0833333333333333333333333333333*pow(dz,-1);
  const CCTK_REAL p1o144dxdy CCTK_ATTRIBUTE_UNUSED = 0.00694444444444444444444444444444*pow(dx,-1)*pow(dy,-1);
  const CCTK_REAL p1o144dxdz CCTK_ATTRIBUTE_UNUSED = 0.00694444444444444444444444444444*pow(dx,-1)*pow(dz,-1);
  const CCTK_REAL p1o144dydz CCTK_ATTRIBUTE_UNUSED = 0.00694444444444444444444444444444*pow(dy,-1)*pow(dz,-1);
  const CCTK_REAL p1o1680dx CCTK_ATTRIBUTE_UNUSED = 0.000595238095238095238095238095238*pow(dx,-1);
  const CCTK_REAL p1o1680dy CCTK_ATTRIBUTE_UNUSED = 0.000595238095238095238095238095238*pow(dy,-1);
  const CCTK_REAL p1o1680dz CCTK_ATTRIBUTE_UNUSED = 0.000595238095238095238095238095238*pow(dz,-1);
  const CCTK_REAL p1o180dx2 CCTK_ATTRIBUTE_UNUSED = 0.00555555555555555555555555555556*pow(dx,-2);
  const CCTK_REAL p1o180dy2 CCTK_ATTRIBUTE_UNUSED = 0.00555555555555555555555555555556*pow(dy,-2);
  const CCTK_REAL p1o180dz2 CCTK_ATTRIBUTE_UNUSED = 0.00555555555555555555555555555556*pow(dz,-2);
  const CCTK_REAL p1o24dx CCTK_ATTRIBUTE_UNUSED = 0.0416666666666666666666666666667*pow(dx,-1);
  const CCTK_REAL p1o24dy CCTK_ATTRIBUTE_UNUSED = 0.0416666666666666666666666666667*pow(dy,-1);
  const CCTK_REAL p1o24dz CCTK_ATTRIBUTE_UNUSED = 0.0416666666666666666666666666667*pow(dz,-1);
  const CCTK_REAL p1o2dx CCTK_ATTRIBUTE_UNUSED = 0.5*pow(dx,-1);
  const CCTK_REAL p1o2dy CCTK_ATTRIBUTE_UNUSED = 0.5*pow(dy,-1);
  const CCTK_REAL p1o2dz CCTK_ATTRIBUTE_UNUSED = 0.5*pow(dz,-1);
  const CCTK_REAL p1o3600dxdy CCTK_ATTRIBUTE_UNUSED = 0.000277777777777777777777777777778*pow(dx,-1)*pow(dy,-1);
  const CCTK_REAL p1o3600dxdz CCTK_ATTRIBUTE_UNUSED = 0.000277777777777777777777777777778*pow(dx,-1)*pow(dz,-1);
  const CCTK_REAL p1o3600dydz CCTK_ATTRIBUTE_UNUSED = 0.000277777777777777777777777777778*pow(dy,-1)*pow(dz,-1);
  const CCTK_REAL p1o4dx CCTK_ATTRIBUTE_UNUSED = 0.25*pow(dx,-1);
  const CCTK_REAL p1o4dxdy CCTK_ATTRIBUTE_UNUSED = 0.25*pow(dx,-1)*pow(dy,-1);
  const CCTK_REAL p1o4dxdz CCTK_ATTRIBUTE_UNUSED = 0.25*pow(dx,-1)*pow(dz,-1);
  const CCTK_REAL p1o4dy CCTK_ATTRIBUTE_UNUSED = 0.25*pow(dy,-1);
  const CCTK_REAL p1o4dydz CCTK_ATTRIBUTE_UNUSED = 0.25*pow(dy,-1)*pow(dz,-1);
  const CCTK_REAL p1o4dz CCTK_ATTRIBUTE_UNUSED = 0.25*pow(dz,-1);
  const CCTK_REAL p1o5040dx2 CCTK_ATTRIBUTE_UNUSED = 0.000198412698412698412698412698413*pow(dx,-2);
  const CCTK_REAL p1o5040dy2 CCTK_ATTRIBUTE_UNUSED = 0.000198412698412698412698412698413*pow(dy,-2);
  const CCTK_REAL p1o5040dz2 CCTK_ATTRIBUTE_UNUSED = 0.000198412698412698412698412698413*pow(dz,-2);
  const CCTK_REAL p1o560dx CCTK_ATTRIBUTE_UNUSED = 0.00178571428571428571428571428571*pow(dx,-1);
  const CCTK_REAL p1o560dy CCTK_ATTRIBUTE_UNUSED = 0.00178571428571428571428571428571*pow(dy,-1);
  const CCTK_REAL p1o560dz CCTK_ATTRIBUTE_UNUSED = 0.00178571428571428571428571428571*pow(dz,-1);
  const CCTK_REAL p1o60dx CCTK_ATTRIBUTE_UNUSED = 0.0166666666666666666666666666667*pow(dx,-1);
  const CCTK_REAL p1o60dy CCTK_ATTRIBUTE_UNUSED = 0.0166666666666666666666666666667*pow(dy,-1);
  const CCTK_REAL p1o60dz CCTK_ATTRIBUTE_UNUSED = 0.0166666666666666666666666666667*pow(dz,-1);
  const CCTK_REAL p1o705600dxdy CCTK_ATTRIBUTE_UNUSED = 1.41723356009070294784580498866e-6*pow(dx,-1)*pow(dy,-1);
  const CCTK_REAL p1o705600dxdz CCTK_ATTRIBUTE_UNUSED = 1.41723356009070294784580498866e-6*pow(dx,-1)*pow(dz,-1);
  const CCTK_REAL p1o705600dydz CCTK_ATTRIBUTE_UNUSED = 1.41723356009070294784580498866e-6*pow(dy,-1)*pow(dz,-1);
  const CCTK_REAL p1o840dx CCTK_ATTRIBUTE_UNUSED = 0.00119047619047619047619047619048*pow(dx,-1);
  const CCTK_REAL p1o840dy CCTK_ATTRIBUTE_UNUSED = 0.00119047619047619047619047619048*pow(dy,-1);
  const CCTK_REAL p1o840dz CCTK_ATTRIBUTE_UNUSED = 0.00119047619047619047619047619048*pow(dz,-1);
  const CCTK_REAL p1odx2 CCTK_ATTRIBUTE_UNUSED = pow(dx,-2);
  const CCTK_REAL p1ody2 CCTK_ATTRIBUTE_UNUSED = pow(dy,-2);
  const CCTK_REAL p1odz2 CCTK_ATTRIBUTE_UNUSED = pow(dz,-2);
  const CCTK_REAL pm1o120dx CCTK_ATTRIBUTE_UNUSED = -0.00833333333333333333333333333333*pow(dx,-1);
  const CCTK_REAL pm1o120dy CCTK_ATTRIBUTE_UNUSED = -0.00833333333333333333333333333333*pow(dy,-1);
  const CCTK_REAL pm1o120dz CCTK_ATTRIBUTE_UNUSED = -0.00833333333333333333333333333333*pow(dz,-1);
  const CCTK_REAL pm1o12dx2 CCTK_ATTRIBUTE_UNUSED = -0.0833333333333333333333333333333*pow(dx,-2);
  const CCTK_REAL pm1o12dy2 CCTK_ATTRIBUTE_UNUSED = -0.0833333333333333333333333333333*pow(dy,-2);
  const CCTK_REAL pm1o12dz2 CCTK_ATTRIBUTE_UNUSED = -0.0833333333333333333333333333333*pow(dz,-2);
  const CCTK_REAL pm1o4dx CCTK_ATTRIBUTE_UNUSED = -0.25*pow(dx,-1);
  const CCTK_REAL pm1o4dy CCTK_ATTRIBUTE_UNUSED = -0.25*pow(dy,-1);
  const CCTK_REAL pm1o4dz CCTK_ATTRIBUTE_UNUSED = -0.25*pow(dz,-1);
  /* Jacobian variable pointers */
  const bool usejacobian1 = (!CCTK_IsFunctionAliased("MultiPatch_GetMap") || MultiPatch_GetMap(cctkGH) != jacobian_identity_map)
                        && strlen(jacobian_group) > 0;
  const bool usejacobian = assume_use_jacobian>=0 ? assume_use_jacobian : usejacobian1;
  if (usejacobian && (strlen(jacobian_derivative_group) == 0))
  {
    CCTK_WARN(CCTK_WARN_ALERT, "GenericFD::jacobian_group and GenericFD::jacobian_derivative_group must both be set to valid group names");
  }
  
  const CCTK_REAL* restrict jacobian_ptrs[9];
  if (usejacobian) GroupDataPointers(cctkGH, jacobian_group,
                                                9, jacobian_ptrs);
  
  const CCTK_REAL* restrict const J11 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_ptrs[0] : 0;
  const CCTK_REAL* restrict const J12 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_ptrs[1] : 0;
  const CCTK_REAL* restrict const J13 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_ptrs[2] : 0;
  const CCTK_REAL* restrict const J21 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_ptrs[3] : 0;
  const CCTK_REAL* restrict const J22 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_ptrs[4] : 0;
  const CCTK_REAL* restrict const J23 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_ptrs[5] : 0;
  const CCTK_REAL* restrict const J31 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_ptrs[6] : 0;
  const CCTK_REAL* restrict const J32 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_ptrs[7] : 0;
  const CCTK_REAL* restrict const J33 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_ptrs[8] : 0;
  
  const CCTK_REAL* restrict jacobian_determinant_ptrs[1] CCTK_ATTRIBUTE_UNUSED;
  if (usejacobian && strlen(jacobian_determinant_group) > 0) GroupDataPointers(cctkGH, jacobian_determinant_group,
                                                1, jacobian_determinant_ptrs);
  
  const CCTK_REAL* restrict const detJ CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_determinant_ptrs[0] : 0;
  
  const CCTK_REAL* restrict jacobian_inverse_ptrs[9] CCTK_ATTRIBUTE_UNUSED;
  if (usejacobian && strlen(jacobian_inverse_group) > 0) GroupDataPointers(cctkGH, jacobian_inverse_group,
                                                9, jacobian_inverse_ptrs);
  
  const CCTK_REAL* restrict const iJ11 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_inverse_ptrs[0] : 0;
  const CCTK_REAL* restrict const iJ12 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_inverse_ptrs[1] : 0;
  const CCTK_REAL* restrict const iJ13 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_inverse_ptrs[2] : 0;
  const CCTK_REAL* restrict const iJ21 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_inverse_ptrs[3] : 0;
  const CCTK_REAL* restrict const iJ22 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_inverse_ptrs[4] : 0;
  const CCTK_REAL* restrict const iJ23 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_inverse_ptrs[5] : 0;
  const CCTK_REAL* restrict const iJ31 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_inverse_ptrs[6] : 0;
  const CCTK_REAL* restrict const iJ32 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_inverse_ptrs[7] : 0;
  const CCTK_REAL* restrict const iJ33 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_inverse_ptrs[8] : 0;
  
  const CCTK_REAL* restrict jacobian_derivative_ptrs[18] CCTK_ATTRIBUTE_UNUSED;
  if (usejacobian) GroupDataPointers(cctkGH, jacobian_derivative_group,
                                      18, jacobian_derivative_ptrs);
  
  const CCTK_REAL* restrict const dJ111 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_derivative_ptrs[0] : 0;
  const CCTK_REAL* restrict const dJ112 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_derivative_ptrs[1] : 0;
  const CCTK_REAL* restrict const dJ113 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_derivative_ptrs[2] : 0;
  const CCTK_REAL* restrict const dJ122 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_derivative_ptrs[3] : 0;
  const CCTK_REAL* restrict const dJ123 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_derivative_ptrs[4] : 0;
  const CCTK_REAL* restrict const dJ133 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_derivative_ptrs[5] : 0;
  const CCTK_REAL* restrict const dJ211 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_derivative_ptrs[6] : 0;
  const CCTK_REAL* restrict const dJ212 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_derivative_ptrs[7] : 0;
  const CCTK_REAL* restrict const dJ213 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_derivative_ptrs[8] : 0;
  const CCTK_REAL* restrict const dJ222 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_derivative_ptrs[9] : 0;
  const CCTK_REAL* restrict const dJ223 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_derivative_ptrs[10] : 0;
  const CCTK_REAL* restrict const dJ233 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_derivative_ptrs[11] : 0;
  const CCTK_REAL* restrict const dJ311 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_derivative_ptrs[12] : 0;
  const CCTK_REAL* restrict const dJ312 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_derivative_ptrs[13] : 0;
  const CCTK_REAL* restrict const dJ313 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_derivative_ptrs[14] : 0;
  const CCTK_REAL* restrict const dJ322 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_derivative_ptrs[15] : 0;
  const CCTK_REAL* restrict const dJ323 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_derivative_ptrs[16] : 0;
  const CCTK_REAL* restrict const dJ333 CCTK_ATTRIBUTE_UNUSED = usejacobian ? jacobian_derivative_ptrs[17] : 0;
  /* Assign local copies of arrays functions */
  
  
  /* Calculate temporaries and arrays functions */
  /* Copy local copies back to grid functions */
  /* Loop over the grid points */
  const int imin0=imin[0];
  const int imin1=imin[1];
  const int imin2=imin[2];
  const int imax0=imax[0];
  const int imax1=imax[1];
  const int imax2=imax[2];
  #pragma omp parallel
  CCTK_LOOP3(CosmoLapse_dtLapsePostStep,
    i,j,k, imin0,imin1,imin2, imax0,imax1,imax2,
    cctk_ash[0],cctk_ash[1],cctk_ash[2])
  {
    const ptrdiff_t index CCTK_ATTRIBUTE_UNUSED = di*i + dj*j + dk*k;
    /* Assign local copies of grid functions */
    
    CCTK_REAL alpL CCTK_ATTRIBUTE_UNUSED = alp[index];
    CCTK_REAL betaxL CCTK_ATTRIBUTE_UNUSED = betax[index];
    CCTK_REAL betayL CCTK_ATTRIBUTE_UNUSED = betay[index];
    CCTK_REAL betazL CCTK_ATTRIBUTE_UNUSED = betaz[index];
    CCTK_REAL epsL CCTK_ATTRIBUTE_UNUSED = eps[index];
    CCTK_REAL KtransitionL CCTK_ATTRIBUTE_UNUSED = Ktransition[index];
    CCTK_REAL pressL CCTK_ATTRIBUTE_UNUSED = press[index];
    CCTK_REAL rhoL CCTK_ATTRIBUTE_UNUSED = rho[index];
    CCTK_REAL tauL CCTK_ATTRIBUTE_UNUSED = tau[index];
    CCTK_REAL trKL CCTK_ATTRIBUTE_UNUSED = trK[index];
    
    
    CCTK_REAL J11L, J12L, J13L, J21L, J22L, J23L, J31L, J32L, J33L CCTK_ATTRIBUTE_UNUSED ;
    
    if (usejacobian)
    {
      J11L = J11[index];
      J12L = J12[index];
      J13L = J13[index];
      J21L = J21[index];
      J22L = J22[index];
      J23L = J23[index];
      J31L = J31[index];
      J32L = J32[index];
      J33L = J33[index];
    }
    /* Include user supplied include files */
    /* Precompute derivatives */
    CCTK_REAL PDstandardNth1alp CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth2alp CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth3alp CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti1alp CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti2alp CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti3alp CCTK_ATTRIBUTE_UNUSED;
    
    switch (fdOrder)
    {
      case 2:
      {
        PDstandardNth1alp = PDstandardNthfdOrder21(&alp[index]);
        PDstandardNth2alp = PDstandardNthfdOrder22(&alp[index]);
        PDstandardNth3alp = PDstandardNthfdOrder23(&alp[index]);
        PDupwindNthAnti1alp = PDupwindNthAntifdOrder21(&alp[index]);
        PDupwindNthAnti2alp = PDupwindNthAntifdOrder22(&alp[index]);
        PDupwindNthAnti3alp = PDupwindNthAntifdOrder23(&alp[index]);
        break;
      }
      
      case 4:
      {
        PDstandardNth1alp = PDstandardNthfdOrder41(&alp[index]);
        PDstandardNth2alp = PDstandardNthfdOrder42(&alp[index]);
        PDstandardNth3alp = PDstandardNthfdOrder43(&alp[index]);
        PDupwindNthAnti1alp = PDupwindNthAntifdOrder41(&alp[index]);
        PDupwindNthAnti2alp = PDupwindNthAntifdOrder42(&alp[index]);
        PDupwindNthAnti3alp = PDupwindNthAntifdOrder43(&alp[index]);
        break;
      }
      
      case 6:
      {
        PDstandardNth1alp = PDstandardNthfdOrder61(&alp[index]);
        PDstandardNth2alp = PDstandardNthfdOrder62(&alp[index]);
        PDstandardNth3alp = PDstandardNthfdOrder63(&alp[index]);
        PDupwindNthAnti1alp = PDupwindNthAntifdOrder61(&alp[index]);
        PDupwindNthAnti2alp = PDupwindNthAntifdOrder62(&alp[index]);
        PDupwindNthAnti3alp = PDupwindNthAntifdOrder63(&alp[index]);
        break;
      }
      
      case 8:
      {
        PDstandardNth1alp = PDstandardNthfdOrder81(&alp[index]);
        PDstandardNth2alp = PDstandardNthfdOrder82(&alp[index]);
        PDstandardNth3alp = PDstandardNthfdOrder83(&alp[index]);
        PDupwindNthAnti1alp = PDupwindNthAntifdOrder81(&alp[index]);
        PDupwindNthAnti2alp = PDupwindNthAntifdOrder82(&alp[index]);
        PDupwindNthAnti3alp = PDupwindNthAntifdOrder83(&alp[index]);
        break;
      }
      default:
        CCTK_BUILTIN_UNREACHABLE();
    }
    /* Calculate temporaries and grid functions */
    CCTK_REAL JacPDstandardNth1alp CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth2alp CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth3alp CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti1alp CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti2alp CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti3alp CCTK_ATTRIBUTE_UNUSED;
    
    if (usejacobian)
    {
      JacPDstandardNth1alp = J11L*PDstandardNth1alp + J21L*PDstandardNth2alp 
        + J31L*PDstandardNth3alp;
      
      JacPDstandardNth2alp = J12L*PDstandardNth1alp + J22L*PDstandardNth2alp 
        + J32L*PDstandardNth3alp;
      
      JacPDstandardNth3alp = J13L*PDstandardNth1alp + J23L*PDstandardNth2alp 
        + J33L*PDstandardNth3alp;
      
      JacPDupwindNthAnti1alp = J11L*PDupwindNthAnti1alp + 
        J21L*PDupwindNthAnti2alp + J31L*PDupwindNthAnti3alp;
      
      JacPDupwindNthAnti2alp = J12L*PDupwindNthAnti1alp + 
        J22L*PDupwindNthAnti2alp + J32L*PDupwindNthAnti3alp;
      
      JacPDupwindNthAnti3alp = J13L*PDupwindNthAnti1alp + 
        J23L*PDupwindNthAnti2alp + J33L*PDupwindNthAnti3alp;
    }
    else
    {
      JacPDstandardNth1alp = PDstandardNth1alp;
      
      JacPDstandardNth2alp = PDstandardNth2alp;
      
      JacPDstandardNth3alp = PDstandardNth3alp;
      
      JacPDupwindNthAnti1alp = PDupwindNthAnti1alp;
      
      JacPDupwindNthAnti2alp = PDupwindNthAnti2alp;
      
      JacPDupwindNthAnti3alp = PDupwindNthAnti3alp;
    }
    
    CCTK_REAL eosw CCTK_ATTRIBUTE_UNUSED = pressL*pow(rhoL + 
      epsL*rhoL,-1);
    
    CCTK_REAL Kth CCTK_ATTRIBUTE_UNUSED = -2.*pow(tauL + tauL*eosw,-1);
    
    CCTK_REAL fBM CCTK_ATTRIBUTE_UNUSED = fBMa + fBMb*pow(alpL,fBMc);
    
    CCTK_REAL KaTransitionVal CCTK_ATTRIBUTE_UNUSED = 
      IfThen(KaTransitionExp == 1,KtransitionL,KaTransition);
    
    CCTK_REAL Ka CCTK_ATTRIBUTE_UNUSED = IfThen(KaExpression == 
      2,trKL*exp(-alpL),IfThen(KaExpression == 1,IfThen(20 + trKL*KaSteepness 
      < KaSteepness*KaTransitionVal,KaTransitionVal,IfThen(trKL*KaSteepness > 
      20 + KaSteepness*KaTransitionVal,trKL,KaTransitionVal + log(1 + 
      exp(KaSteepness*(trKL - 
      KaTransitionVal)))*pow(KaSteepness,-1))),trKL));
    
    KtransitionL = -fmax(-KtransitionL,Ka*KaTransitionFac);
    
    CCTK_REAL Kb CCTK_ATTRIBUTE_UNUSED = IfThen(KbExpression == 
      2,-8.6832150546992119123561755275*pow((1 + 
      epsL)*rhoL,0.5),IfThen(KbExpression == 1,Kth,0.));
    
    CCTK_REAL partialtalpha CCTK_ATTRIBUTE_UNUSED = fBM*(-Ka + Kb) + 
      IfThen(alphaFullLieDeriv != 0,IfThen(upwind == 
      1,betaxL*JacPDupwindNthAnti1alp + betayL*JacPDupwindNthAnti2alp + 
      betazL*JacPDupwindNthAnti3alp,betaxL*JacPDstandardNth1alp + 
      betayL*JacPDstandardNth2alp + betazL*JacPDstandardNth3alp),0);
    
    CCTK_REAL dtalpL CCTK_ATTRIBUTE_UNUSED = partialtalpha;
    
    CCTK_REAL alpharhsL CCTK_ATTRIBUTE_UNUSED = partialtalpha;
    /* Copy local copies back to grid functions */
    alpharhs[index] = alpharhsL;
    dtalp[index] = dtalpL;
    Ktransition[index] = KtransitionL;
  }
  CCTK_ENDLOOP3(CosmoLapse_dtLapsePostStep);
}
extern "C" void CosmoLapse_dtLapsePostStep(CCTK_ARGUMENTS)
{
  #ifdef DECLARE_CCTK_ARGUMENTS_CosmoLapse_dtLapsePostStep
  DECLARE_CCTK_ARGUMENTS_CHECKED(CosmoLapse_dtLapsePostStep);
  #else
  DECLARE_CCTK_ARGUMENTS;
  #endif
  DECLARE_CCTK_PARAMETERS;
  
  if (verbose > 1)
  {
    CCTK_VInfo(CCTK_THORNSTRING,"Entering CosmoLapse_dtLapsePostStep_Body");
  }
  if (cctk_iteration % CosmoLapse_dtLapsePostStep_calc_every != CosmoLapse_dtLapsePostStep_calc_offset)
  {
    return;
  }
  
  const char* const groups[] = {
    "ADMBase::dtlapse",
    "ADMBase::lapse",
    "ADMBase::shift",
    "HydroBase::eps",
    "HydroBase::press",
    "HydroBase::rho",
    "CosmoLapse::Kthreshold",
    "ML_BSSN::ML_lapserhs",
    "ML_BSSN::ML_trace_curv",
    "CosmoLapse::propertime"};
  AssertGroupStorage(cctkGH, "CosmoLapse_dtLapsePostStep", 10, groups);
  
  switch (fdOrder)
  {
    case 2:
    {
      EnsureStencilFits(cctkGH, "CosmoLapse_dtLapsePostStep", 2, 2, 2);
      break;
    }
    
    case 4:
    {
      EnsureStencilFits(cctkGH, "CosmoLapse_dtLapsePostStep", 3, 3, 3);
      break;
    }
    
    case 6:
    {
      EnsureStencilFits(cctkGH, "CosmoLapse_dtLapsePostStep", 4, 4, 4);
      break;
    }
    
    case 8:
    {
      EnsureStencilFits(cctkGH, "CosmoLapse_dtLapsePostStep", 5, 5, 5);
      break;
    }
    default:
      CCTK_BUILTIN_UNREACHABLE();
  }
  
  LoopOverInterior(cctkGH, CosmoLapse_dtLapsePostStep_Body);
  if (verbose > 1)
  {
    CCTK_VInfo(CCTK_THORNSTRING,"Leaving CosmoLapse_dtLapsePostStep_Body");
  }
}

} // namespace CosmoLapse
