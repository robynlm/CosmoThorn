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

namespace CosmoLapse_CTD {

extern "C" void CosmoLapse_CTD_dtShiftPostRHS_SelectBCs(CCTK_ARGUMENTS)
{
  #ifdef DECLARE_CCTK_ARGUMENTS_CosmoLapse_CTD_dtShiftPostRHS_SelectBCs
  DECLARE_CCTK_ARGUMENTS_CHECKED(CosmoLapse_CTD_dtShiftPostRHS_SelectBCs);
  #else
  DECLARE_CCTK_ARGUMENTS;
  #endif
  DECLARE_CCTK_PARAMETERS;
  
  if (cctk_iteration % CosmoLapse_CTD_dtShiftPostRHS_calc_every != CosmoLapse_CTD_dtShiftPostRHS_calc_offset)
    return;
  CCTK_INT ierr CCTK_ATTRIBUTE_UNUSED = 0;
  ierr = KrancBdy_SelectGroupForBC(cctkGH, CCTK_ALL_FACES, GetBoundaryWidth(cctkGH), -1 /* no table */, "ADMBase::dtshift","flat");
  if (ierr < 0)
    CCTK_WARN(CCTK_WARN_ALERT, "Failed to register flat BC for ADMBase::dtshift.");
  ierr = KrancBdy_SelectGroupForBC(cctkGH, CCTK_ALL_FACES, GetBoundaryWidth(cctkGH), -1 /* no table */, "ML_BSSN::ML_dtshiftrhs","flat");
  if (ierr < 0)
    CCTK_WARN(CCTK_WARN_ALERT, "Failed to register flat BC for ML_BSSN::ML_dtshiftrhs.");
  ierr = KrancBdy_SelectGroupForBC(cctkGH, CCTK_ALL_FACES, GetBoundaryWidth(cctkGH), -1 /* no table */, "ML_BSSN::ML_shiftrhs","flat");
  if (ierr < 0)
    CCTK_WARN(CCTK_WARN_ALERT, "Failed to register flat BC for ML_BSSN::ML_shiftrhs.");
  return;
}

static void CosmoLapse_CTD_dtShiftPostRHS_Body(const cGH* restrict const cctkGH, const int dir, const int face, const CCTK_REAL normal[3], const CCTK_REAL tangentA[3], const CCTK_REAL tangentB[3], const int imin[3], const int imax[3], const int n_subblock_gfs, CCTK_REAL* restrict const subblock_gfs[])
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
  CCTK_LOOP3(CosmoLapse_CTD_dtShiftPostRHS,
    i,j,k, imin0,imin1,imin2, imax0,imax1,imax2,
    cctk_ash[0],cctk_ash[1],cctk_ash[2])
  {
    const ptrdiff_t index CCTK_ATTRIBUTE_UNUSED = di*i + dj*j + dk*k;
    /* Assign local copies of grid functions */
    
    CCTK_REAL alpL CCTK_ATTRIBUTE_UNUSED = alp[index];
    CCTK_REAL B1L CCTK_ATTRIBUTE_UNUSED = B1[index];
    CCTK_REAL B2L CCTK_ATTRIBUTE_UNUSED = B2[index];
    CCTK_REAL B3L CCTK_ATTRIBUTE_UNUSED = B3[index];
    CCTK_REAL betaxL CCTK_ATTRIBUTE_UNUSED = betax[index];
    CCTK_REAL betayL CCTK_ATTRIBUTE_UNUSED = betay[index];
    CCTK_REAL betazL CCTK_ATTRIBUTE_UNUSED = betaz[index];
    CCTK_REAL gxxL CCTK_ATTRIBUTE_UNUSED = gxx[index];
    CCTK_REAL gxyL CCTK_ATTRIBUTE_UNUSED = gxy[index];
    CCTK_REAL gxzL CCTK_ATTRIBUTE_UNUSED = gxz[index];
    CCTK_REAL gyyL CCTK_ATTRIBUTE_UNUSED = gyy[index];
    CCTK_REAL gyzL CCTK_ATTRIBUTE_UNUSED = gyz[index];
    CCTK_REAL gzzL CCTK_ATTRIBUTE_UNUSED = gzz[index];
    CCTK_REAL Xt1L CCTK_ATTRIBUTE_UNUSED = Xt1[index];
    CCTK_REAL Xt1rhsL CCTK_ATTRIBUTE_UNUSED = Xt1rhs[index];
    CCTK_REAL Xt2L CCTK_ATTRIBUTE_UNUSED = Xt2[index];
    CCTK_REAL Xt2rhsL CCTK_ATTRIBUTE_UNUSED = Xt2rhs[index];
    CCTK_REAL Xt3L CCTK_ATTRIBUTE_UNUSED = Xt3[index];
    CCTK_REAL Xt3rhsL CCTK_ATTRIBUTE_UNUSED = Xt3rhs[index];
    
    
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
    CCTK_REAL PDstandardNth1B1 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth2B1 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth3B1 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti1B1 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti2B1 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti3B1 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth1B2 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth2B2 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth3B2 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti1B2 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti2B2 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti3B2 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth1B3 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth2B3 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth3B3 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti1B3 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti2B3 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti3B3 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth1betax CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth2betax CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth3betax CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti1betax CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti2betax CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti3betax CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth1betay CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth2betay CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth3betay CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti1betay CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti2betay CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti3betay CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth1betaz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth2betaz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth3betaz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti1betaz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti2betaz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti3betaz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth1gxx CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth2gxx CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth3gxx CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth1gxy CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth2gxy CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth3gxy CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth1gxz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth2gxz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth3gxz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth1gyy CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth2gyy CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth3gyy CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth1gyz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth2gyz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth3gyz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth1gzz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth2gzz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth3gzz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth1Xt1 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth2Xt1 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth3Xt1 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti1Xt1 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti2Xt1 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti3Xt1 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth1Xt2 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth2Xt2 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth3Xt2 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti1Xt2 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti2Xt2 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti3Xt2 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth1Xt3 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth2Xt3 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDstandardNth3Xt3 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti1Xt3 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti2Xt3 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL PDupwindNthAnti3Xt3 CCTK_ATTRIBUTE_UNUSED;
    
    switch (fdOrder)
    {
      case 2:
      {
        PDstandardNth1B1 = PDstandardNthfdOrder21(&B1[index]);
        PDstandardNth2B1 = PDstandardNthfdOrder22(&B1[index]);
        PDstandardNth3B1 = PDstandardNthfdOrder23(&B1[index]);
        PDupwindNthAnti1B1 = PDupwindNthAntifdOrder21(&B1[index]);
        PDupwindNthAnti2B1 = PDupwindNthAntifdOrder22(&B1[index]);
        PDupwindNthAnti3B1 = PDupwindNthAntifdOrder23(&B1[index]);
        PDstandardNth1B2 = PDstandardNthfdOrder21(&B2[index]);
        PDstandardNth2B2 = PDstandardNthfdOrder22(&B2[index]);
        PDstandardNth3B2 = PDstandardNthfdOrder23(&B2[index]);
        PDupwindNthAnti1B2 = PDupwindNthAntifdOrder21(&B2[index]);
        PDupwindNthAnti2B2 = PDupwindNthAntifdOrder22(&B2[index]);
        PDupwindNthAnti3B2 = PDupwindNthAntifdOrder23(&B2[index]);
        PDstandardNth1B3 = PDstandardNthfdOrder21(&B3[index]);
        PDstandardNth2B3 = PDstandardNthfdOrder22(&B3[index]);
        PDstandardNth3B3 = PDstandardNthfdOrder23(&B3[index]);
        PDupwindNthAnti1B3 = PDupwindNthAntifdOrder21(&B3[index]);
        PDupwindNthAnti2B3 = PDupwindNthAntifdOrder22(&B3[index]);
        PDupwindNthAnti3B3 = PDupwindNthAntifdOrder23(&B3[index]);
        PDstandardNth1betax = PDstandardNthfdOrder21(&betax[index]);
        PDstandardNth2betax = PDstandardNthfdOrder22(&betax[index]);
        PDstandardNth3betax = PDstandardNthfdOrder23(&betax[index]);
        PDupwindNthAnti1betax = PDupwindNthAntifdOrder21(&betax[index]);
        PDupwindNthAnti2betax = PDupwindNthAntifdOrder22(&betax[index]);
        PDupwindNthAnti3betax = PDupwindNthAntifdOrder23(&betax[index]);
        PDstandardNth1betay = PDstandardNthfdOrder21(&betay[index]);
        PDstandardNth2betay = PDstandardNthfdOrder22(&betay[index]);
        PDstandardNth3betay = PDstandardNthfdOrder23(&betay[index]);
        PDupwindNthAnti1betay = PDupwindNthAntifdOrder21(&betay[index]);
        PDupwindNthAnti2betay = PDupwindNthAntifdOrder22(&betay[index]);
        PDupwindNthAnti3betay = PDupwindNthAntifdOrder23(&betay[index]);
        PDstandardNth1betaz = PDstandardNthfdOrder21(&betaz[index]);
        PDstandardNth2betaz = PDstandardNthfdOrder22(&betaz[index]);
        PDstandardNth3betaz = PDstandardNthfdOrder23(&betaz[index]);
        PDupwindNthAnti1betaz = PDupwindNthAntifdOrder21(&betaz[index]);
        PDupwindNthAnti2betaz = PDupwindNthAntifdOrder22(&betaz[index]);
        PDupwindNthAnti3betaz = PDupwindNthAntifdOrder23(&betaz[index]);
        PDstandardNth1gxx = PDstandardNthfdOrder21(&gxx[index]);
        PDstandardNth2gxx = PDstandardNthfdOrder22(&gxx[index]);
        PDstandardNth3gxx = PDstandardNthfdOrder23(&gxx[index]);
        PDstandardNth1gxy = PDstandardNthfdOrder21(&gxy[index]);
        PDstandardNth2gxy = PDstandardNthfdOrder22(&gxy[index]);
        PDstandardNth3gxy = PDstandardNthfdOrder23(&gxy[index]);
        PDstandardNth1gxz = PDstandardNthfdOrder21(&gxz[index]);
        PDstandardNth2gxz = PDstandardNthfdOrder22(&gxz[index]);
        PDstandardNth3gxz = PDstandardNthfdOrder23(&gxz[index]);
        PDstandardNth1gyy = PDstandardNthfdOrder21(&gyy[index]);
        PDstandardNth2gyy = PDstandardNthfdOrder22(&gyy[index]);
        PDstandardNth3gyy = PDstandardNthfdOrder23(&gyy[index]);
        PDstandardNth1gyz = PDstandardNthfdOrder21(&gyz[index]);
        PDstandardNth2gyz = PDstandardNthfdOrder22(&gyz[index]);
        PDstandardNth3gyz = PDstandardNthfdOrder23(&gyz[index]);
        PDstandardNth1gzz = PDstandardNthfdOrder21(&gzz[index]);
        PDstandardNth2gzz = PDstandardNthfdOrder22(&gzz[index]);
        PDstandardNth3gzz = PDstandardNthfdOrder23(&gzz[index]);
        PDstandardNth1Xt1 = PDstandardNthfdOrder21(&Xt1[index]);
        PDstandardNth2Xt1 = PDstandardNthfdOrder22(&Xt1[index]);
        PDstandardNth3Xt1 = PDstandardNthfdOrder23(&Xt1[index]);
        PDupwindNthAnti1Xt1 = PDupwindNthAntifdOrder21(&Xt1[index]);
        PDupwindNthAnti2Xt1 = PDupwindNthAntifdOrder22(&Xt1[index]);
        PDupwindNthAnti3Xt1 = PDupwindNthAntifdOrder23(&Xt1[index]);
        PDstandardNth1Xt2 = PDstandardNthfdOrder21(&Xt2[index]);
        PDstandardNth2Xt2 = PDstandardNthfdOrder22(&Xt2[index]);
        PDstandardNth3Xt2 = PDstandardNthfdOrder23(&Xt2[index]);
        PDupwindNthAnti1Xt2 = PDupwindNthAntifdOrder21(&Xt2[index]);
        PDupwindNthAnti2Xt2 = PDupwindNthAntifdOrder22(&Xt2[index]);
        PDupwindNthAnti3Xt2 = PDupwindNthAntifdOrder23(&Xt2[index]);
        PDstandardNth1Xt3 = PDstandardNthfdOrder21(&Xt3[index]);
        PDstandardNth2Xt3 = PDstandardNthfdOrder22(&Xt3[index]);
        PDstandardNth3Xt3 = PDstandardNthfdOrder23(&Xt3[index]);
        PDupwindNthAnti1Xt3 = PDupwindNthAntifdOrder21(&Xt3[index]);
        PDupwindNthAnti2Xt3 = PDupwindNthAntifdOrder22(&Xt3[index]);
        PDupwindNthAnti3Xt3 = PDupwindNthAntifdOrder23(&Xt3[index]);
        break;
      }
      
      case 4:
      {
        PDstandardNth1B1 = PDstandardNthfdOrder41(&B1[index]);
        PDstandardNth2B1 = PDstandardNthfdOrder42(&B1[index]);
        PDstandardNth3B1 = PDstandardNthfdOrder43(&B1[index]);
        PDupwindNthAnti1B1 = PDupwindNthAntifdOrder41(&B1[index]);
        PDupwindNthAnti2B1 = PDupwindNthAntifdOrder42(&B1[index]);
        PDupwindNthAnti3B1 = PDupwindNthAntifdOrder43(&B1[index]);
        PDstandardNth1B2 = PDstandardNthfdOrder41(&B2[index]);
        PDstandardNth2B2 = PDstandardNthfdOrder42(&B2[index]);
        PDstandardNth3B2 = PDstandardNthfdOrder43(&B2[index]);
        PDupwindNthAnti1B2 = PDupwindNthAntifdOrder41(&B2[index]);
        PDupwindNthAnti2B2 = PDupwindNthAntifdOrder42(&B2[index]);
        PDupwindNthAnti3B2 = PDupwindNthAntifdOrder43(&B2[index]);
        PDstandardNth1B3 = PDstandardNthfdOrder41(&B3[index]);
        PDstandardNth2B3 = PDstandardNthfdOrder42(&B3[index]);
        PDstandardNth3B3 = PDstandardNthfdOrder43(&B3[index]);
        PDupwindNthAnti1B3 = PDupwindNthAntifdOrder41(&B3[index]);
        PDupwindNthAnti2B3 = PDupwindNthAntifdOrder42(&B3[index]);
        PDupwindNthAnti3B3 = PDupwindNthAntifdOrder43(&B3[index]);
        PDstandardNth1betax = PDstandardNthfdOrder41(&betax[index]);
        PDstandardNth2betax = PDstandardNthfdOrder42(&betax[index]);
        PDstandardNth3betax = PDstandardNthfdOrder43(&betax[index]);
        PDupwindNthAnti1betax = PDupwindNthAntifdOrder41(&betax[index]);
        PDupwindNthAnti2betax = PDupwindNthAntifdOrder42(&betax[index]);
        PDupwindNthAnti3betax = PDupwindNthAntifdOrder43(&betax[index]);
        PDstandardNth1betay = PDstandardNthfdOrder41(&betay[index]);
        PDstandardNth2betay = PDstandardNthfdOrder42(&betay[index]);
        PDstandardNth3betay = PDstandardNthfdOrder43(&betay[index]);
        PDupwindNthAnti1betay = PDupwindNthAntifdOrder41(&betay[index]);
        PDupwindNthAnti2betay = PDupwindNthAntifdOrder42(&betay[index]);
        PDupwindNthAnti3betay = PDupwindNthAntifdOrder43(&betay[index]);
        PDstandardNth1betaz = PDstandardNthfdOrder41(&betaz[index]);
        PDstandardNth2betaz = PDstandardNthfdOrder42(&betaz[index]);
        PDstandardNth3betaz = PDstandardNthfdOrder43(&betaz[index]);
        PDupwindNthAnti1betaz = PDupwindNthAntifdOrder41(&betaz[index]);
        PDupwindNthAnti2betaz = PDupwindNthAntifdOrder42(&betaz[index]);
        PDupwindNthAnti3betaz = PDupwindNthAntifdOrder43(&betaz[index]);
        PDstandardNth1gxx = PDstandardNthfdOrder41(&gxx[index]);
        PDstandardNth2gxx = PDstandardNthfdOrder42(&gxx[index]);
        PDstandardNth3gxx = PDstandardNthfdOrder43(&gxx[index]);
        PDstandardNth1gxy = PDstandardNthfdOrder41(&gxy[index]);
        PDstandardNth2gxy = PDstandardNthfdOrder42(&gxy[index]);
        PDstandardNth3gxy = PDstandardNthfdOrder43(&gxy[index]);
        PDstandardNth1gxz = PDstandardNthfdOrder41(&gxz[index]);
        PDstandardNth2gxz = PDstandardNthfdOrder42(&gxz[index]);
        PDstandardNth3gxz = PDstandardNthfdOrder43(&gxz[index]);
        PDstandardNth1gyy = PDstandardNthfdOrder41(&gyy[index]);
        PDstandardNth2gyy = PDstandardNthfdOrder42(&gyy[index]);
        PDstandardNth3gyy = PDstandardNthfdOrder43(&gyy[index]);
        PDstandardNth1gyz = PDstandardNthfdOrder41(&gyz[index]);
        PDstandardNth2gyz = PDstandardNthfdOrder42(&gyz[index]);
        PDstandardNth3gyz = PDstandardNthfdOrder43(&gyz[index]);
        PDstandardNth1gzz = PDstandardNthfdOrder41(&gzz[index]);
        PDstandardNth2gzz = PDstandardNthfdOrder42(&gzz[index]);
        PDstandardNth3gzz = PDstandardNthfdOrder43(&gzz[index]);
        PDstandardNth1Xt1 = PDstandardNthfdOrder41(&Xt1[index]);
        PDstandardNth2Xt1 = PDstandardNthfdOrder42(&Xt1[index]);
        PDstandardNth3Xt1 = PDstandardNthfdOrder43(&Xt1[index]);
        PDupwindNthAnti1Xt1 = PDupwindNthAntifdOrder41(&Xt1[index]);
        PDupwindNthAnti2Xt1 = PDupwindNthAntifdOrder42(&Xt1[index]);
        PDupwindNthAnti3Xt1 = PDupwindNthAntifdOrder43(&Xt1[index]);
        PDstandardNth1Xt2 = PDstandardNthfdOrder41(&Xt2[index]);
        PDstandardNth2Xt2 = PDstandardNthfdOrder42(&Xt2[index]);
        PDstandardNth3Xt2 = PDstandardNthfdOrder43(&Xt2[index]);
        PDupwindNthAnti1Xt2 = PDupwindNthAntifdOrder41(&Xt2[index]);
        PDupwindNthAnti2Xt2 = PDupwindNthAntifdOrder42(&Xt2[index]);
        PDupwindNthAnti3Xt2 = PDupwindNthAntifdOrder43(&Xt2[index]);
        PDstandardNth1Xt3 = PDstandardNthfdOrder41(&Xt3[index]);
        PDstandardNth2Xt3 = PDstandardNthfdOrder42(&Xt3[index]);
        PDstandardNth3Xt3 = PDstandardNthfdOrder43(&Xt3[index]);
        PDupwindNthAnti1Xt3 = PDupwindNthAntifdOrder41(&Xt3[index]);
        PDupwindNthAnti2Xt3 = PDupwindNthAntifdOrder42(&Xt3[index]);
        PDupwindNthAnti3Xt3 = PDupwindNthAntifdOrder43(&Xt3[index]);
        break;
      }
      
      case 6:
      {
        PDstandardNth1B1 = PDstandardNthfdOrder61(&B1[index]);
        PDstandardNth2B1 = PDstandardNthfdOrder62(&B1[index]);
        PDstandardNth3B1 = PDstandardNthfdOrder63(&B1[index]);
        PDupwindNthAnti1B1 = PDupwindNthAntifdOrder61(&B1[index]);
        PDupwindNthAnti2B1 = PDupwindNthAntifdOrder62(&B1[index]);
        PDupwindNthAnti3B1 = PDupwindNthAntifdOrder63(&B1[index]);
        PDstandardNth1B2 = PDstandardNthfdOrder61(&B2[index]);
        PDstandardNth2B2 = PDstandardNthfdOrder62(&B2[index]);
        PDstandardNth3B2 = PDstandardNthfdOrder63(&B2[index]);
        PDupwindNthAnti1B2 = PDupwindNthAntifdOrder61(&B2[index]);
        PDupwindNthAnti2B2 = PDupwindNthAntifdOrder62(&B2[index]);
        PDupwindNthAnti3B2 = PDupwindNthAntifdOrder63(&B2[index]);
        PDstandardNth1B3 = PDstandardNthfdOrder61(&B3[index]);
        PDstandardNth2B3 = PDstandardNthfdOrder62(&B3[index]);
        PDstandardNth3B3 = PDstandardNthfdOrder63(&B3[index]);
        PDupwindNthAnti1B3 = PDupwindNthAntifdOrder61(&B3[index]);
        PDupwindNthAnti2B3 = PDupwindNthAntifdOrder62(&B3[index]);
        PDupwindNthAnti3B3 = PDupwindNthAntifdOrder63(&B3[index]);
        PDstandardNth1betax = PDstandardNthfdOrder61(&betax[index]);
        PDstandardNth2betax = PDstandardNthfdOrder62(&betax[index]);
        PDstandardNth3betax = PDstandardNthfdOrder63(&betax[index]);
        PDupwindNthAnti1betax = PDupwindNthAntifdOrder61(&betax[index]);
        PDupwindNthAnti2betax = PDupwindNthAntifdOrder62(&betax[index]);
        PDupwindNthAnti3betax = PDupwindNthAntifdOrder63(&betax[index]);
        PDstandardNth1betay = PDstandardNthfdOrder61(&betay[index]);
        PDstandardNth2betay = PDstandardNthfdOrder62(&betay[index]);
        PDstandardNth3betay = PDstandardNthfdOrder63(&betay[index]);
        PDupwindNthAnti1betay = PDupwindNthAntifdOrder61(&betay[index]);
        PDupwindNthAnti2betay = PDupwindNthAntifdOrder62(&betay[index]);
        PDupwindNthAnti3betay = PDupwindNthAntifdOrder63(&betay[index]);
        PDstandardNth1betaz = PDstandardNthfdOrder61(&betaz[index]);
        PDstandardNth2betaz = PDstandardNthfdOrder62(&betaz[index]);
        PDstandardNth3betaz = PDstandardNthfdOrder63(&betaz[index]);
        PDupwindNthAnti1betaz = PDupwindNthAntifdOrder61(&betaz[index]);
        PDupwindNthAnti2betaz = PDupwindNthAntifdOrder62(&betaz[index]);
        PDupwindNthAnti3betaz = PDupwindNthAntifdOrder63(&betaz[index]);
        PDstandardNth1gxx = PDstandardNthfdOrder61(&gxx[index]);
        PDstandardNth2gxx = PDstandardNthfdOrder62(&gxx[index]);
        PDstandardNth3gxx = PDstandardNthfdOrder63(&gxx[index]);
        PDstandardNth1gxy = PDstandardNthfdOrder61(&gxy[index]);
        PDstandardNth2gxy = PDstandardNthfdOrder62(&gxy[index]);
        PDstandardNth3gxy = PDstandardNthfdOrder63(&gxy[index]);
        PDstandardNth1gxz = PDstandardNthfdOrder61(&gxz[index]);
        PDstandardNth2gxz = PDstandardNthfdOrder62(&gxz[index]);
        PDstandardNth3gxz = PDstandardNthfdOrder63(&gxz[index]);
        PDstandardNth1gyy = PDstandardNthfdOrder61(&gyy[index]);
        PDstandardNth2gyy = PDstandardNthfdOrder62(&gyy[index]);
        PDstandardNth3gyy = PDstandardNthfdOrder63(&gyy[index]);
        PDstandardNth1gyz = PDstandardNthfdOrder61(&gyz[index]);
        PDstandardNth2gyz = PDstandardNthfdOrder62(&gyz[index]);
        PDstandardNth3gyz = PDstandardNthfdOrder63(&gyz[index]);
        PDstandardNth1gzz = PDstandardNthfdOrder61(&gzz[index]);
        PDstandardNth2gzz = PDstandardNthfdOrder62(&gzz[index]);
        PDstandardNth3gzz = PDstandardNthfdOrder63(&gzz[index]);
        PDstandardNth1Xt1 = PDstandardNthfdOrder61(&Xt1[index]);
        PDstandardNth2Xt1 = PDstandardNthfdOrder62(&Xt1[index]);
        PDstandardNth3Xt1 = PDstandardNthfdOrder63(&Xt1[index]);
        PDupwindNthAnti1Xt1 = PDupwindNthAntifdOrder61(&Xt1[index]);
        PDupwindNthAnti2Xt1 = PDupwindNthAntifdOrder62(&Xt1[index]);
        PDupwindNthAnti3Xt1 = PDupwindNthAntifdOrder63(&Xt1[index]);
        PDstandardNth1Xt2 = PDstandardNthfdOrder61(&Xt2[index]);
        PDstandardNth2Xt2 = PDstandardNthfdOrder62(&Xt2[index]);
        PDstandardNth3Xt2 = PDstandardNthfdOrder63(&Xt2[index]);
        PDupwindNthAnti1Xt2 = PDupwindNthAntifdOrder61(&Xt2[index]);
        PDupwindNthAnti2Xt2 = PDupwindNthAntifdOrder62(&Xt2[index]);
        PDupwindNthAnti3Xt2 = PDupwindNthAntifdOrder63(&Xt2[index]);
        PDstandardNth1Xt3 = PDstandardNthfdOrder61(&Xt3[index]);
        PDstandardNth2Xt3 = PDstandardNthfdOrder62(&Xt3[index]);
        PDstandardNth3Xt3 = PDstandardNthfdOrder63(&Xt3[index]);
        PDupwindNthAnti1Xt3 = PDupwindNthAntifdOrder61(&Xt3[index]);
        PDupwindNthAnti2Xt3 = PDupwindNthAntifdOrder62(&Xt3[index]);
        PDupwindNthAnti3Xt3 = PDupwindNthAntifdOrder63(&Xt3[index]);
        break;
      }
      
      case 8:
      {
        PDstandardNth1B1 = PDstandardNthfdOrder81(&B1[index]);
        PDstandardNth2B1 = PDstandardNthfdOrder82(&B1[index]);
        PDstandardNth3B1 = PDstandardNthfdOrder83(&B1[index]);
        PDupwindNthAnti1B1 = PDupwindNthAntifdOrder81(&B1[index]);
        PDupwindNthAnti2B1 = PDupwindNthAntifdOrder82(&B1[index]);
        PDupwindNthAnti3B1 = PDupwindNthAntifdOrder83(&B1[index]);
        PDstandardNth1B2 = PDstandardNthfdOrder81(&B2[index]);
        PDstandardNth2B2 = PDstandardNthfdOrder82(&B2[index]);
        PDstandardNth3B2 = PDstandardNthfdOrder83(&B2[index]);
        PDupwindNthAnti1B2 = PDupwindNthAntifdOrder81(&B2[index]);
        PDupwindNthAnti2B2 = PDupwindNthAntifdOrder82(&B2[index]);
        PDupwindNthAnti3B2 = PDupwindNthAntifdOrder83(&B2[index]);
        PDstandardNth1B3 = PDstandardNthfdOrder81(&B3[index]);
        PDstandardNth2B3 = PDstandardNthfdOrder82(&B3[index]);
        PDstandardNth3B3 = PDstandardNthfdOrder83(&B3[index]);
        PDupwindNthAnti1B3 = PDupwindNthAntifdOrder81(&B3[index]);
        PDupwindNthAnti2B3 = PDupwindNthAntifdOrder82(&B3[index]);
        PDupwindNthAnti3B3 = PDupwindNthAntifdOrder83(&B3[index]);
        PDstandardNth1betax = PDstandardNthfdOrder81(&betax[index]);
        PDstandardNth2betax = PDstandardNthfdOrder82(&betax[index]);
        PDstandardNth3betax = PDstandardNthfdOrder83(&betax[index]);
        PDupwindNthAnti1betax = PDupwindNthAntifdOrder81(&betax[index]);
        PDupwindNthAnti2betax = PDupwindNthAntifdOrder82(&betax[index]);
        PDupwindNthAnti3betax = PDupwindNthAntifdOrder83(&betax[index]);
        PDstandardNth1betay = PDstandardNthfdOrder81(&betay[index]);
        PDstandardNth2betay = PDstandardNthfdOrder82(&betay[index]);
        PDstandardNth3betay = PDstandardNthfdOrder83(&betay[index]);
        PDupwindNthAnti1betay = PDupwindNthAntifdOrder81(&betay[index]);
        PDupwindNthAnti2betay = PDupwindNthAntifdOrder82(&betay[index]);
        PDupwindNthAnti3betay = PDupwindNthAntifdOrder83(&betay[index]);
        PDstandardNth1betaz = PDstandardNthfdOrder81(&betaz[index]);
        PDstandardNth2betaz = PDstandardNthfdOrder82(&betaz[index]);
        PDstandardNth3betaz = PDstandardNthfdOrder83(&betaz[index]);
        PDupwindNthAnti1betaz = PDupwindNthAntifdOrder81(&betaz[index]);
        PDupwindNthAnti2betaz = PDupwindNthAntifdOrder82(&betaz[index]);
        PDupwindNthAnti3betaz = PDupwindNthAntifdOrder83(&betaz[index]);
        PDstandardNth1gxx = PDstandardNthfdOrder81(&gxx[index]);
        PDstandardNth2gxx = PDstandardNthfdOrder82(&gxx[index]);
        PDstandardNth3gxx = PDstandardNthfdOrder83(&gxx[index]);
        PDstandardNth1gxy = PDstandardNthfdOrder81(&gxy[index]);
        PDstandardNth2gxy = PDstandardNthfdOrder82(&gxy[index]);
        PDstandardNth3gxy = PDstandardNthfdOrder83(&gxy[index]);
        PDstandardNth1gxz = PDstandardNthfdOrder81(&gxz[index]);
        PDstandardNth2gxz = PDstandardNthfdOrder82(&gxz[index]);
        PDstandardNth3gxz = PDstandardNthfdOrder83(&gxz[index]);
        PDstandardNth1gyy = PDstandardNthfdOrder81(&gyy[index]);
        PDstandardNth2gyy = PDstandardNthfdOrder82(&gyy[index]);
        PDstandardNth3gyy = PDstandardNthfdOrder83(&gyy[index]);
        PDstandardNth1gyz = PDstandardNthfdOrder81(&gyz[index]);
        PDstandardNth2gyz = PDstandardNthfdOrder82(&gyz[index]);
        PDstandardNth3gyz = PDstandardNthfdOrder83(&gyz[index]);
        PDstandardNth1gzz = PDstandardNthfdOrder81(&gzz[index]);
        PDstandardNth2gzz = PDstandardNthfdOrder82(&gzz[index]);
        PDstandardNth3gzz = PDstandardNthfdOrder83(&gzz[index]);
        PDstandardNth1Xt1 = PDstandardNthfdOrder81(&Xt1[index]);
        PDstandardNth2Xt1 = PDstandardNthfdOrder82(&Xt1[index]);
        PDstandardNth3Xt1 = PDstandardNthfdOrder83(&Xt1[index]);
        PDupwindNthAnti1Xt1 = PDupwindNthAntifdOrder81(&Xt1[index]);
        PDupwindNthAnti2Xt1 = PDupwindNthAntifdOrder82(&Xt1[index]);
        PDupwindNthAnti3Xt1 = PDupwindNthAntifdOrder83(&Xt1[index]);
        PDstandardNth1Xt2 = PDstandardNthfdOrder81(&Xt2[index]);
        PDstandardNth2Xt2 = PDstandardNthfdOrder82(&Xt2[index]);
        PDstandardNth3Xt2 = PDstandardNthfdOrder83(&Xt2[index]);
        PDupwindNthAnti1Xt2 = PDupwindNthAntifdOrder81(&Xt2[index]);
        PDupwindNthAnti2Xt2 = PDupwindNthAntifdOrder82(&Xt2[index]);
        PDupwindNthAnti3Xt2 = PDupwindNthAntifdOrder83(&Xt2[index]);
        PDstandardNth1Xt3 = PDstandardNthfdOrder81(&Xt3[index]);
        PDstandardNth2Xt3 = PDstandardNthfdOrder82(&Xt3[index]);
        PDstandardNth3Xt3 = PDstandardNthfdOrder83(&Xt3[index]);
        PDupwindNthAnti1Xt3 = PDupwindNthAntifdOrder81(&Xt3[index]);
        PDupwindNthAnti2Xt3 = PDupwindNthAntifdOrder82(&Xt3[index]);
        PDupwindNthAnti3Xt3 = PDupwindNthAntifdOrder83(&Xt3[index]);
        break;
      }
      default:
        CCTK_BUILTIN_UNREACHABLE();
    }
    /* Calculate temporaries and grid functions */
    CCTK_REAL JacPDstandardNth1B1 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth1B2 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth1B3 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth1betax CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth1betay CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth1betaz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth1gxx CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth1gxy CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth1gxz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth1gyy CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth1gyz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth1gzz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth1Xt1 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth1Xt2 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth1Xt3 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth2B1 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth2B2 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth2B3 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth2betax CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth2betay CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth2betaz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth2gxx CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth2gxy CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth2gxz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth2gyy CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth2gyz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth2gzz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth2Xt1 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth2Xt2 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth2Xt3 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth3B1 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth3B2 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth3B3 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth3betax CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth3betay CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth3betaz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth3gxx CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth3gxy CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth3gxz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth3gyy CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth3gyz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth3gzz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth3Xt1 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth3Xt2 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDstandardNth3Xt3 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti1B1 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti1B2 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti1B3 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti1betax CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti1betay CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti1betaz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti1Xt1 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti1Xt2 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti1Xt3 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti2B1 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti2B2 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti2B3 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti2betax CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti2betay CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti2betaz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti2Xt1 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti2Xt2 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti2Xt3 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti3B1 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti3B2 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti3B3 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti3betax CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti3betay CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti3betaz CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti3Xt1 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti3Xt2 CCTK_ATTRIBUTE_UNUSED;
    CCTK_REAL JacPDupwindNthAnti3Xt3 CCTK_ATTRIBUTE_UNUSED;
    
    if (usejacobian)
    {
      JacPDstandardNth1B1 = J11L*PDstandardNth1B1 + J21L*PDstandardNth2B1 + 
        J31L*PDstandardNth3B1;
      
      JacPDstandardNth1B2 = J11L*PDstandardNth1B2 + J21L*PDstandardNth2B2 + 
        J31L*PDstandardNth3B2;
      
      JacPDstandardNth1B3 = J11L*PDstandardNth1B3 + J21L*PDstandardNth2B3 + 
        J31L*PDstandardNth3B3;
      
      JacPDstandardNth1betax = J11L*PDstandardNth1betax + 
        J21L*PDstandardNth2betax + J31L*PDstandardNth3betax;
      
      JacPDstandardNth1betay = J11L*PDstandardNth1betay + 
        J21L*PDstandardNth2betay + J31L*PDstandardNth3betay;
      
      JacPDstandardNth1betaz = J11L*PDstandardNth1betaz + 
        J21L*PDstandardNth2betaz + J31L*PDstandardNth3betaz;
      
      JacPDstandardNth1gxx = J11L*PDstandardNth1gxx + J21L*PDstandardNth2gxx 
        + J31L*PDstandardNth3gxx;
      
      JacPDstandardNth1gxy = J11L*PDstandardNth1gxy + J21L*PDstandardNth2gxy 
        + J31L*PDstandardNth3gxy;
      
      JacPDstandardNth1gxz = J11L*PDstandardNth1gxz + J21L*PDstandardNth2gxz 
        + J31L*PDstandardNth3gxz;
      
      JacPDstandardNth1gyy = J11L*PDstandardNth1gyy + J21L*PDstandardNth2gyy 
        + J31L*PDstandardNth3gyy;
      
      JacPDstandardNth1gyz = J11L*PDstandardNth1gyz + J21L*PDstandardNth2gyz 
        + J31L*PDstandardNth3gyz;
      
      JacPDstandardNth1gzz = J11L*PDstandardNth1gzz + J21L*PDstandardNth2gzz 
        + J31L*PDstandardNth3gzz;
      
      JacPDstandardNth1Xt1 = J11L*PDstandardNth1Xt1 + J21L*PDstandardNth2Xt1 
        + J31L*PDstandardNth3Xt1;
      
      JacPDstandardNth1Xt2 = J11L*PDstandardNth1Xt2 + J21L*PDstandardNth2Xt2 
        + J31L*PDstandardNth3Xt2;
      
      JacPDstandardNth1Xt3 = J11L*PDstandardNth1Xt3 + J21L*PDstandardNth2Xt3 
        + J31L*PDstandardNth3Xt3;
      
      JacPDstandardNth2B1 = J12L*PDstandardNth1B1 + J22L*PDstandardNth2B1 + 
        J32L*PDstandardNth3B1;
      
      JacPDstandardNth2B2 = J12L*PDstandardNth1B2 + J22L*PDstandardNth2B2 + 
        J32L*PDstandardNth3B2;
      
      JacPDstandardNth2B3 = J12L*PDstandardNth1B3 + J22L*PDstandardNth2B3 + 
        J32L*PDstandardNth3B3;
      
      JacPDstandardNth2betax = J12L*PDstandardNth1betax + 
        J22L*PDstandardNth2betax + J32L*PDstandardNth3betax;
      
      JacPDstandardNth2betay = J12L*PDstandardNth1betay + 
        J22L*PDstandardNth2betay + J32L*PDstandardNth3betay;
      
      JacPDstandardNth2betaz = J12L*PDstandardNth1betaz + 
        J22L*PDstandardNth2betaz + J32L*PDstandardNth3betaz;
      
      JacPDstandardNth2gxx = J12L*PDstandardNth1gxx + J22L*PDstandardNth2gxx 
        + J32L*PDstandardNth3gxx;
      
      JacPDstandardNth2gxy = J12L*PDstandardNth1gxy + J22L*PDstandardNth2gxy 
        + J32L*PDstandardNth3gxy;
      
      JacPDstandardNth2gxz = J12L*PDstandardNth1gxz + J22L*PDstandardNth2gxz 
        + J32L*PDstandardNth3gxz;
      
      JacPDstandardNth2gyy = J12L*PDstandardNth1gyy + J22L*PDstandardNth2gyy 
        + J32L*PDstandardNth3gyy;
      
      JacPDstandardNth2gyz = J12L*PDstandardNth1gyz + J22L*PDstandardNth2gyz 
        + J32L*PDstandardNth3gyz;
      
      JacPDstandardNth2gzz = J12L*PDstandardNth1gzz + J22L*PDstandardNth2gzz 
        + J32L*PDstandardNth3gzz;
      
      JacPDstandardNth2Xt1 = J12L*PDstandardNth1Xt1 + J22L*PDstandardNth2Xt1 
        + J32L*PDstandardNth3Xt1;
      
      JacPDstandardNth2Xt2 = J12L*PDstandardNth1Xt2 + J22L*PDstandardNth2Xt2 
        + J32L*PDstandardNth3Xt2;
      
      JacPDstandardNth2Xt3 = J12L*PDstandardNth1Xt3 + J22L*PDstandardNth2Xt3 
        + J32L*PDstandardNth3Xt3;
      
      JacPDstandardNth3B1 = J13L*PDstandardNth1B1 + J23L*PDstandardNth2B1 + 
        J33L*PDstandardNth3B1;
      
      JacPDstandardNth3B2 = J13L*PDstandardNth1B2 + J23L*PDstandardNth2B2 + 
        J33L*PDstandardNth3B2;
      
      JacPDstandardNth3B3 = J13L*PDstandardNth1B3 + J23L*PDstandardNth2B3 + 
        J33L*PDstandardNth3B3;
      
      JacPDstandardNth3betax = J13L*PDstandardNth1betax + 
        J23L*PDstandardNth2betax + J33L*PDstandardNth3betax;
      
      JacPDstandardNth3betay = J13L*PDstandardNth1betay + 
        J23L*PDstandardNth2betay + J33L*PDstandardNth3betay;
      
      JacPDstandardNth3betaz = J13L*PDstandardNth1betaz + 
        J23L*PDstandardNth2betaz + J33L*PDstandardNth3betaz;
      
      JacPDstandardNth3gxx = J13L*PDstandardNth1gxx + J23L*PDstandardNth2gxx 
        + J33L*PDstandardNth3gxx;
      
      JacPDstandardNth3gxy = J13L*PDstandardNth1gxy + J23L*PDstandardNth2gxy 
        + J33L*PDstandardNth3gxy;
      
      JacPDstandardNth3gxz = J13L*PDstandardNth1gxz + J23L*PDstandardNth2gxz 
        + J33L*PDstandardNth3gxz;
      
      JacPDstandardNth3gyy = J13L*PDstandardNth1gyy + J23L*PDstandardNth2gyy 
        + J33L*PDstandardNth3gyy;
      
      JacPDstandardNth3gyz = J13L*PDstandardNth1gyz + J23L*PDstandardNth2gyz 
        + J33L*PDstandardNth3gyz;
      
      JacPDstandardNth3gzz = J13L*PDstandardNth1gzz + J23L*PDstandardNth2gzz 
        + J33L*PDstandardNth3gzz;
      
      JacPDstandardNth3Xt1 = J13L*PDstandardNth1Xt1 + J23L*PDstandardNth2Xt1 
        + J33L*PDstandardNth3Xt1;
      
      JacPDstandardNth3Xt2 = J13L*PDstandardNth1Xt2 + J23L*PDstandardNth2Xt2 
        + J33L*PDstandardNth3Xt2;
      
      JacPDstandardNth3Xt3 = J13L*PDstandardNth1Xt3 + J23L*PDstandardNth2Xt3 
        + J33L*PDstandardNth3Xt3;
      
      JacPDupwindNthAnti1B1 = J11L*PDupwindNthAnti1B1 + 
        J21L*PDupwindNthAnti2B1 + J31L*PDupwindNthAnti3B1;
      
      JacPDupwindNthAnti1B2 = J11L*PDupwindNthAnti1B2 + 
        J21L*PDupwindNthAnti2B2 + J31L*PDupwindNthAnti3B2;
      
      JacPDupwindNthAnti1B3 = J11L*PDupwindNthAnti1B3 + 
        J21L*PDupwindNthAnti2B3 + J31L*PDupwindNthAnti3B3;
      
      JacPDupwindNthAnti1betax = J11L*PDupwindNthAnti1betax + 
        J21L*PDupwindNthAnti2betax + J31L*PDupwindNthAnti3betax;
      
      JacPDupwindNthAnti1betay = J11L*PDupwindNthAnti1betay + 
        J21L*PDupwindNthAnti2betay + J31L*PDupwindNthAnti3betay;
      
      JacPDupwindNthAnti1betaz = J11L*PDupwindNthAnti1betaz + 
        J21L*PDupwindNthAnti2betaz + J31L*PDupwindNthAnti3betaz;
      
      JacPDupwindNthAnti1Xt1 = J11L*PDupwindNthAnti1Xt1 + 
        J21L*PDupwindNthAnti2Xt1 + J31L*PDupwindNthAnti3Xt1;
      
      JacPDupwindNthAnti1Xt2 = J11L*PDupwindNthAnti1Xt2 + 
        J21L*PDupwindNthAnti2Xt2 + J31L*PDupwindNthAnti3Xt2;
      
      JacPDupwindNthAnti1Xt3 = J11L*PDupwindNthAnti1Xt3 + 
        J21L*PDupwindNthAnti2Xt3 + J31L*PDupwindNthAnti3Xt3;
      
      JacPDupwindNthAnti2B1 = J12L*PDupwindNthAnti1B1 + 
        J22L*PDupwindNthAnti2B1 + J32L*PDupwindNthAnti3B1;
      
      JacPDupwindNthAnti2B2 = J12L*PDupwindNthAnti1B2 + 
        J22L*PDupwindNthAnti2B2 + J32L*PDupwindNthAnti3B2;
      
      JacPDupwindNthAnti2B3 = J12L*PDupwindNthAnti1B3 + 
        J22L*PDupwindNthAnti2B3 + J32L*PDupwindNthAnti3B3;
      
      JacPDupwindNthAnti2betax = J12L*PDupwindNthAnti1betax + 
        J22L*PDupwindNthAnti2betax + J32L*PDupwindNthAnti3betax;
      
      JacPDupwindNthAnti2betay = J12L*PDupwindNthAnti1betay + 
        J22L*PDupwindNthAnti2betay + J32L*PDupwindNthAnti3betay;
      
      JacPDupwindNthAnti2betaz = J12L*PDupwindNthAnti1betaz + 
        J22L*PDupwindNthAnti2betaz + J32L*PDupwindNthAnti3betaz;
      
      JacPDupwindNthAnti2Xt1 = J12L*PDupwindNthAnti1Xt1 + 
        J22L*PDupwindNthAnti2Xt1 + J32L*PDupwindNthAnti3Xt1;
      
      JacPDupwindNthAnti2Xt2 = J12L*PDupwindNthAnti1Xt2 + 
        J22L*PDupwindNthAnti2Xt2 + J32L*PDupwindNthAnti3Xt2;
      
      JacPDupwindNthAnti2Xt3 = J12L*PDupwindNthAnti1Xt3 + 
        J22L*PDupwindNthAnti2Xt3 + J32L*PDupwindNthAnti3Xt3;
      
      JacPDupwindNthAnti3B1 = J13L*PDupwindNthAnti1B1 + 
        J23L*PDupwindNthAnti2B1 + J33L*PDupwindNthAnti3B1;
      
      JacPDupwindNthAnti3B2 = J13L*PDupwindNthAnti1B2 + 
        J23L*PDupwindNthAnti2B2 + J33L*PDupwindNthAnti3B2;
      
      JacPDupwindNthAnti3B3 = J13L*PDupwindNthAnti1B3 + 
        J23L*PDupwindNthAnti2B3 + J33L*PDupwindNthAnti3B3;
      
      JacPDupwindNthAnti3betax = J13L*PDupwindNthAnti1betax + 
        J23L*PDupwindNthAnti2betax + J33L*PDupwindNthAnti3betax;
      
      JacPDupwindNthAnti3betay = J13L*PDupwindNthAnti1betay + 
        J23L*PDupwindNthAnti2betay + J33L*PDupwindNthAnti3betay;
      
      JacPDupwindNthAnti3betaz = J13L*PDupwindNthAnti1betaz + 
        J23L*PDupwindNthAnti2betaz + J33L*PDupwindNthAnti3betaz;
      
      JacPDupwindNthAnti3Xt1 = J13L*PDupwindNthAnti1Xt1 + 
        J23L*PDupwindNthAnti2Xt1 + J33L*PDupwindNthAnti3Xt1;
      
      JacPDupwindNthAnti3Xt2 = J13L*PDupwindNthAnti1Xt2 + 
        J23L*PDupwindNthAnti2Xt2 + J33L*PDupwindNthAnti3Xt2;
      
      JacPDupwindNthAnti3Xt3 = J13L*PDupwindNthAnti1Xt3 + 
        J23L*PDupwindNthAnti2Xt3 + J33L*PDupwindNthAnti3Xt3;
    }
    else
    {
      JacPDstandardNth1B1 = PDstandardNth1B1;
      
      JacPDstandardNth1B2 = PDstandardNth1B2;
      
      JacPDstandardNth1B3 = PDstandardNth1B3;
      
      JacPDstandardNth1betax = PDstandardNth1betax;
      
      JacPDstandardNth1betay = PDstandardNth1betay;
      
      JacPDstandardNth1betaz = PDstandardNth1betaz;
      
      JacPDstandardNth1gxx = PDstandardNth1gxx;
      
      JacPDstandardNth1gxy = PDstandardNth1gxy;
      
      JacPDstandardNth1gxz = PDstandardNth1gxz;
      
      JacPDstandardNth1gyy = PDstandardNth1gyy;
      
      JacPDstandardNth1gyz = PDstandardNth1gyz;
      
      JacPDstandardNth1gzz = PDstandardNth1gzz;
      
      JacPDstandardNth1Xt1 = PDstandardNth1Xt1;
      
      JacPDstandardNth1Xt2 = PDstandardNth1Xt2;
      
      JacPDstandardNth1Xt3 = PDstandardNth1Xt3;
      
      JacPDstandardNth2B1 = PDstandardNth2B1;
      
      JacPDstandardNth2B2 = PDstandardNth2B2;
      
      JacPDstandardNth2B3 = PDstandardNth2B3;
      
      JacPDstandardNth2betax = PDstandardNth2betax;
      
      JacPDstandardNth2betay = PDstandardNth2betay;
      
      JacPDstandardNth2betaz = PDstandardNth2betaz;
      
      JacPDstandardNth2gxx = PDstandardNth2gxx;
      
      JacPDstandardNth2gxy = PDstandardNth2gxy;
      
      JacPDstandardNth2gxz = PDstandardNth2gxz;
      
      JacPDstandardNth2gyy = PDstandardNth2gyy;
      
      JacPDstandardNth2gyz = PDstandardNth2gyz;
      
      JacPDstandardNth2gzz = PDstandardNth2gzz;
      
      JacPDstandardNth2Xt1 = PDstandardNth2Xt1;
      
      JacPDstandardNth2Xt2 = PDstandardNth2Xt2;
      
      JacPDstandardNth2Xt3 = PDstandardNth2Xt3;
      
      JacPDstandardNth3B1 = PDstandardNth3B1;
      
      JacPDstandardNth3B2 = PDstandardNth3B2;
      
      JacPDstandardNth3B3 = PDstandardNth3B3;
      
      JacPDstandardNth3betax = PDstandardNth3betax;
      
      JacPDstandardNth3betay = PDstandardNth3betay;
      
      JacPDstandardNth3betaz = PDstandardNth3betaz;
      
      JacPDstandardNth3gxx = PDstandardNth3gxx;
      
      JacPDstandardNth3gxy = PDstandardNth3gxy;
      
      JacPDstandardNth3gxz = PDstandardNth3gxz;
      
      JacPDstandardNth3gyy = PDstandardNth3gyy;
      
      JacPDstandardNth3gyz = PDstandardNth3gyz;
      
      JacPDstandardNth3gzz = PDstandardNth3gzz;
      
      JacPDstandardNth3Xt1 = PDstandardNth3Xt1;
      
      JacPDstandardNth3Xt2 = PDstandardNth3Xt2;
      
      JacPDstandardNth3Xt3 = PDstandardNth3Xt3;
      
      JacPDupwindNthAnti1B1 = PDupwindNthAnti1B1;
      
      JacPDupwindNthAnti1B2 = PDupwindNthAnti1B2;
      
      JacPDupwindNthAnti1B3 = PDupwindNthAnti1B3;
      
      JacPDupwindNthAnti1betax = PDupwindNthAnti1betax;
      
      JacPDupwindNthAnti1betay = PDupwindNthAnti1betay;
      
      JacPDupwindNthAnti1betaz = PDupwindNthAnti1betaz;
      
      JacPDupwindNthAnti1Xt1 = PDupwindNthAnti1Xt1;
      
      JacPDupwindNthAnti1Xt2 = PDupwindNthAnti1Xt2;
      
      JacPDupwindNthAnti1Xt3 = PDupwindNthAnti1Xt3;
      
      JacPDupwindNthAnti2B1 = PDupwindNthAnti2B1;
      
      JacPDupwindNthAnti2B2 = PDupwindNthAnti2B2;
      
      JacPDupwindNthAnti2B3 = PDupwindNthAnti2B3;
      
      JacPDupwindNthAnti2betax = PDupwindNthAnti2betax;
      
      JacPDupwindNthAnti2betay = PDupwindNthAnti2betay;
      
      JacPDupwindNthAnti2betaz = PDupwindNthAnti2betaz;
      
      JacPDupwindNthAnti2Xt1 = PDupwindNthAnti2Xt1;
      
      JacPDupwindNthAnti2Xt2 = PDupwindNthAnti2Xt2;
      
      JacPDupwindNthAnti2Xt3 = PDupwindNthAnti2Xt3;
      
      JacPDupwindNthAnti3B1 = PDupwindNthAnti3B1;
      
      JacPDupwindNthAnti3B2 = PDupwindNthAnti3B2;
      
      JacPDupwindNthAnti3B3 = PDupwindNthAnti3B3;
      
      JacPDupwindNthAnti3betax = PDupwindNthAnti3betax;
      
      JacPDupwindNthAnti3betay = PDupwindNthAnti3betay;
      
      JacPDupwindNthAnti3betaz = PDupwindNthAnti3betaz;
      
      JacPDupwindNthAnti3Xt1 = PDupwindNthAnti3Xt1;
      
      JacPDupwindNthAnti3Xt2 = PDupwindNthAnti3Xt2;
      
      JacPDupwindNthAnti3Xt3 = PDupwindNthAnti3Xt3;
    }
    
    CCTK_REAL detg CCTK_ATTRIBUTE_UNUSED = 2*gxyL*gxzL*gyzL - 
      gzzL*pow(gxyL,2) + gyyL*(gxxL*gzzL - pow(gxzL,2)) - gxxL*pow(gyzL,2);
    
    CCTK_REAL psi CCTK_ATTRIBUTE_UNUSED = 
      pow(detg,0.0833333333333333333333333333333);
    
    CCTK_REAL psim2 CCTK_ATTRIBUTE_UNUSED = pow(psi,-2);
    
    CCTK_REAL dpsim21 CCTK_ATTRIBUTE_UNUSED = 
      0.166666666666666666666666666667*(gzzL*(2*gxyL*JacPDstandardNth1gxy - 
      gxxL*JacPDstandardNth1gyy) - 2*(gxyL*gxzL*JacPDstandardNth1gyz + 
      gyzL*(gxzL*JacPDstandardNth1gxy + gxyL*JacPDstandardNth1gxz - 
      gxxL*JacPDstandardNth1gyz)) - gyyL*(gzzL*JacPDstandardNth1gxx - 
      2*gxzL*JacPDstandardNth1gxz + gxxL*JacPDstandardNth1gzz) + 
      JacPDstandardNth1gzz*pow(gxyL,2) + JacPDstandardNth1gyy*pow(gxzL,2) + 
      JacPDstandardNth1gxx*pow(gyzL,2))*pow(detg,-1.16666666666666666666666666667);
    
    CCTK_REAL dpsim22 CCTK_ATTRIBUTE_UNUSED = 
      0.166666666666666666666666666667*(gzzL*(2*gxyL*JacPDstandardNth2gxy - 
      gxxL*JacPDstandardNth2gyy) - 2*(gxyL*gxzL*JacPDstandardNth2gyz + 
      gyzL*(gxzL*JacPDstandardNth2gxy + gxyL*JacPDstandardNth2gxz - 
      gxxL*JacPDstandardNth2gyz)) - gyyL*(gzzL*JacPDstandardNth2gxx - 
      2*gxzL*JacPDstandardNth2gxz + gxxL*JacPDstandardNth2gzz) + 
      JacPDstandardNth2gzz*pow(gxyL,2) + JacPDstandardNth2gyy*pow(gxzL,2) + 
      JacPDstandardNth2gxx*pow(gyzL,2))*pow(detg,-1.16666666666666666666666666667);
    
    CCTK_REAL dpsim23 CCTK_ATTRIBUTE_UNUSED = 
      0.166666666666666666666666666667*(gzzL*(2*gxyL*JacPDstandardNth3gxy - 
      gxxL*JacPDstandardNth3gyy) - 2*(gxyL*gxzL*JacPDstandardNth3gyz + 
      gyzL*(gxzL*JacPDstandardNth3gxy + gxyL*JacPDstandardNth3gxz - 
      gxxL*JacPDstandardNth3gyz)) - gyyL*(gzzL*JacPDstandardNth3gxx - 
      2*gxzL*JacPDstandardNth3gxz + gxxL*JacPDstandardNth3gzz) + 
      JacPDstandardNth3gzz*pow(gxyL,2) + JacPDstandardNth3gyy*pow(gxzL,2) + 
      JacPDstandardNth3gxx*pow(gyzL,2))*pow(detg,-1.16666666666666666666666666667);
    
    CCTK_REAL gucon11 CCTK_ATTRIBUTE_UNUSED = (gyyL*gzzL - 
      pow(gyzL,2))*pow(detg,-1)*pow(psi,4);
    
    CCTK_REAL gucon12 CCTK_ATTRIBUTE_UNUSED = (gxzL*gyzL - 
      gxyL*gzzL)*pow(detg,-1)*pow(psi,4);
    
    CCTK_REAL gucon13 CCTK_ATTRIBUTE_UNUSED = (-(gxzL*gyyL) + 
      gxyL*gyzL)*pow(detg,-1)*pow(psi,4);
    
    CCTK_REAL gucon21 CCTK_ATTRIBUTE_UNUSED = (gxzL*gyzL - 
      gxyL*gzzL)*pow(detg,-1)*pow(psi,4);
    
    CCTK_REAL gucon22 CCTK_ATTRIBUTE_UNUSED = (gxxL*gzzL - 
      pow(gxzL,2))*pow(detg,-1)*pow(psi,4);
    
    CCTK_REAL gucon23 CCTK_ATTRIBUTE_UNUSED = (gxyL*gxzL - 
      gxxL*gyzL)*pow(detg,-1)*pow(psi,4);
    
    CCTK_REAL gucon31 CCTK_ATTRIBUTE_UNUSED = (-(gxzL*gyyL) + 
      gxyL*gyzL)*pow(detg,-1)*pow(psi,4);
    
    CCTK_REAL gucon32 CCTK_ATTRIBUTE_UNUSED = (gxyL*gxzL - 
      gxxL*gyzL)*pow(detg,-1)*pow(psi,4);
    
    CCTK_REAL gucon33 CCTK_ATTRIBUTE_UNUSED = (gxxL*gyyL - 
      pow(gxyL,2))*pow(detg,-1)*pow(psi,4);
    
    CCTK_REAL eta CCTK_ATTRIBUTE_UNUSED = IfThen(varyingEta == 
      1,betaEta*pow(-1 + psim2,-2)*pow(dpsim21*(dpsim22*(gucon12 + gucon21) + 
      dpsim23*(gucon13 + gucon31)) + dpsim22*dpsim23*(gucon23 + gucon32) + 
      gucon11*pow(dpsim21,2) + gucon22*pow(dpsim22,2) + 
      gucon33*pow(dpsim23,2),0.5),betaEta);
    
    CCTK_REAL partialtbeta1 CCTK_ATTRIBUTE_UNUSED = B1L*betaXi1 + 
      IfThen(betaFullLieDeriv != 0,IfThen(upwind == 
      1,betaxL*JacPDupwindNthAnti1betax + betayL*JacPDupwindNthAnti2betax + 
      betazL*JacPDupwindNthAnti3betax,betaxL*JacPDstandardNth1betax + 
      betayL*JacPDstandardNth2betax + betazL*JacPDstandardNth3betax),0);
    
    CCTK_REAL partialtbeta2 CCTK_ATTRIBUTE_UNUSED = B2L*betaXi1 + 
      IfThen(betaFullLieDeriv != 0,IfThen(upwind == 
      1,betaxL*JacPDupwindNthAnti1betay + betayL*JacPDupwindNthAnti2betay + 
      betazL*JacPDupwindNthAnti3betay,betaxL*JacPDstandardNth1betay + 
      betayL*JacPDstandardNth2betay + betazL*JacPDstandardNth3betay),0);
    
    CCTK_REAL partialtbeta3 CCTK_ATTRIBUTE_UNUSED = B3L*betaXi1 + 
      IfThen(betaFullLieDeriv != 0,IfThen(upwind == 
      1,betaxL*JacPDupwindNthAnti1betaz + betayL*JacPDupwindNthAnti2betaz + 
      betazL*JacPDupwindNthAnti3betaz,betaxL*JacPDstandardNth1betaz + 
      betayL*JacPDstandardNth2betaz + betazL*JacPDstandardNth3betaz),0);
    
    CCTK_REAL partialtB1 CCTK_ATTRIBUTE_UNUSED = -(B1L*eta) + 
      IfThen(betaFullLieDeriv != 0,IfThen(upwind == 
      1,betaxL*JacPDupwindNthAnti1B1 + betayL*JacPDupwindNthAnti2B1 + 
      betazL*JacPDupwindNthAnti3B1,betaxL*JacPDstandardNth1B1 + 
      betayL*JacPDstandardNth2B1 + betazL*JacPDstandardNth3B1),0) + 
      betaXi2*(Xt1rhsL - IfThen(betaFullLieDeriv != 0,IfThen(upwind == 
      1,betaxL*JacPDupwindNthAnti1Xt1 + betayL*JacPDupwindNthAnti2Xt1 + 
      betazL*JacPDupwindNthAnti3Xt1,betaxL*JacPDstandardNth1Xt1 + 
      betayL*JacPDstandardNth2Xt1 + 
      betazL*JacPDstandardNth3Xt1),0))*pow(alpL,betaP);
    
    CCTK_REAL partialtB2 CCTK_ATTRIBUTE_UNUSED = -(B2L*eta) + 
      IfThen(betaFullLieDeriv != 0,IfThen(upwind == 
      1,betaxL*JacPDupwindNthAnti1B2 + betayL*JacPDupwindNthAnti2B2 + 
      betazL*JacPDupwindNthAnti3B2,betaxL*JacPDstandardNth1B2 + 
      betayL*JacPDstandardNth2B2 + betazL*JacPDstandardNth3B2),0) + 
      betaXi2*(Xt2rhsL - IfThen(betaFullLieDeriv != 0,IfThen(upwind == 
      1,betaxL*JacPDupwindNthAnti1Xt2 + betayL*JacPDupwindNthAnti2Xt2 + 
      betazL*JacPDupwindNthAnti3Xt2,betaxL*JacPDstandardNth1Xt2 + 
      betayL*JacPDstandardNth2Xt2 + 
      betazL*JacPDstandardNth3Xt2),0))*pow(alpL,betaP);
    
    CCTK_REAL partialtB3 CCTK_ATTRIBUTE_UNUSED = -(B3L*eta) + 
      IfThen(betaFullLieDeriv != 0,IfThen(upwind == 
      1,betaxL*JacPDupwindNthAnti1B3 + betayL*JacPDupwindNthAnti2B3 + 
      betazL*JacPDupwindNthAnti3B3,betaxL*JacPDstandardNth1B3 + 
      betayL*JacPDstandardNth2B3 + betazL*JacPDstandardNth3B3),0) + 
      betaXi2*(Xt3rhsL - IfThen(betaFullLieDeriv != 0,IfThen(upwind == 
      1,betaxL*JacPDupwindNthAnti1Xt3 + betayL*JacPDupwindNthAnti2Xt3 + 
      betazL*JacPDupwindNthAnti3Xt3,betaxL*JacPDstandardNth1Xt3 + 
      betayL*JacPDstandardNth2Xt3 + 
      betazL*JacPDstandardNth3Xt3),0))*pow(alpL,betaP);
    
    CCTK_REAL dtbetaxL CCTK_ATTRIBUTE_UNUSED = partialtbeta1;
    
    CCTK_REAL dtbetayL CCTK_ATTRIBUTE_UNUSED = partialtbeta2;
    
    CCTK_REAL dtbetazL CCTK_ATTRIBUTE_UNUSED = partialtbeta3;
    
    CCTK_REAL beta1rhsL CCTK_ATTRIBUTE_UNUSED = partialtbeta1;
    
    CCTK_REAL beta2rhsL CCTK_ATTRIBUTE_UNUSED = partialtbeta2;
    
    CCTK_REAL beta3rhsL CCTK_ATTRIBUTE_UNUSED = partialtbeta3;
    
    CCTK_REAL B1rhsL CCTK_ATTRIBUTE_UNUSED = partialtB1;
    
    CCTK_REAL B2rhsL CCTK_ATTRIBUTE_UNUSED = partialtB2;
    
    CCTK_REAL B3rhsL CCTK_ATTRIBUTE_UNUSED = partialtB3;
    /* Copy local copies back to grid functions */
    B1rhs[index] = B1rhsL;
    B2rhs[index] = B2rhsL;
    B3rhs[index] = B3rhsL;
    beta1rhs[index] = beta1rhsL;
    beta2rhs[index] = beta2rhsL;
    beta3rhs[index] = beta3rhsL;
    dtbetax[index] = dtbetaxL;
    dtbetay[index] = dtbetayL;
    dtbetaz[index] = dtbetazL;
  }
  CCTK_ENDLOOP3(CosmoLapse_CTD_dtShiftPostRHS);
}
extern "C" void CosmoLapse_CTD_dtShiftPostRHS(CCTK_ARGUMENTS)
{
  #ifdef DECLARE_CCTK_ARGUMENTS_CosmoLapse_CTD_dtShiftPostRHS
  DECLARE_CCTK_ARGUMENTS_CHECKED(CosmoLapse_CTD_dtShiftPostRHS);
  #else
  DECLARE_CCTK_ARGUMENTS;
  #endif
  DECLARE_CCTK_PARAMETERS;
  
  if (verbose > 1)
  {
    CCTK_VInfo(CCTK_THORNSTRING,"Entering CosmoLapse_CTD_dtShiftPostRHS_Body");
  }
  if (cctk_iteration % CosmoLapse_CTD_dtShiftPostRHS_calc_every != CosmoLapse_CTD_dtShiftPostRHS_calc_offset)
  {
    return;
  }
  
  const char* const groups[] = {
    "ADMBase::dtshift",
    "ADMBase::lapse",
    "ADMBase::metric",
    "ADMBase::shift",
    "ML_BSSN::ML_dtshift",
    "ML_BSSN::ML_dtshiftrhs",
    "ML_BSSN::ML_Gamma",
    "ML_BSSN::ML_Gammarhs",
    "ML_BSSN::ML_shiftrhs"};
  AssertGroupStorage(cctkGH, "CosmoLapse_CTD_dtShiftPostRHS", 9, groups);
  
  switch (fdOrder)
  {
    case 2:
    {
      EnsureStencilFits(cctkGH, "CosmoLapse_CTD_dtShiftPostRHS", 2, 2, 2);
      break;
    }
    
    case 4:
    {
      EnsureStencilFits(cctkGH, "CosmoLapse_CTD_dtShiftPostRHS", 3, 3, 3);
      break;
    }
    
    case 6:
    {
      EnsureStencilFits(cctkGH, "CosmoLapse_CTD_dtShiftPostRHS", 4, 4, 4);
      break;
    }
    
    case 8:
    {
      EnsureStencilFits(cctkGH, "CosmoLapse_CTD_dtShiftPostRHS", 5, 5, 5);
      break;
    }
    default:
      CCTK_BUILTIN_UNREACHABLE();
  }
  
  LoopOverInterior(cctkGH, CosmoLapse_CTD_dtShiftPostRHS_Body);
  if (verbose > 1)
  {
    CCTK_VInfo(CCTK_THORNSTRING,"Leaving CosmoLapse_CTD_dtShiftPostRHS_Body");
  }
}

} // namespace CosmoLapse_CTD
