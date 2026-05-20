/*  File produced by Kranc */

#include "cctk.h"

extern "C" int CosmoLapse_CTD_Startup(void)
{
  const char* banner CCTK_ATTRIBUTE_UNUSED = "CosmoLapse_CTD";
  CCTK_RegisterBanner(banner);
  return 0;
}
