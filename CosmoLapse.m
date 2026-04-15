(* ::Package:: *)

SetEnhancedTimes[False];

maxTimelevels = 4;
thorn = "CosmoLapse";

(******************************************************************************)
(*                                 Tensors                                    *)
(******************************************************************************)

Map [DefineTensor, {
  tau, dir, g, dpsim2, gucon, Ktransition,
  a, admdtalpha, mlbalpharhs, partialtalpha,
  b, admdtbeta, mlbbetarhs, partialtbeta, mlbB, mlbBrhs, partialtB,
  mlbXt, mlbXtrhs,
  rho, eps, press
  }];

Map [AssertSymmetricIncreasing, {g[la,lb]}];
SetTensorAttribute[mlbXt, TensorWeight, +2/3];

(* ADMBase variables *)
g11=gxx; g12=gxy; g22=gyy; g13=gxz; g23=gyz; g33=gzz;
a=alp;
admdtalpha=dtalp;
b1=betax; b2=betay; b3=betaz;
admdtbeta1=dtbetax; admdtbeta2=dtbetay; admdtbeta3=dtbetaz;
bssntrK=trK;
 
(* ML_BSSN variables *)
mlbalpha=alpha;
mlbalpharhs=alpharhs;
mlbbetarhs1=beta1rhs; mlbbetarhs2=beta2rhs; mlbbetarhs3=beta3rhs;

mlbB1=B1; mlbB2=B2; mlbB3=B3;
mlbBrhs1=B1rhs; mlbBrhs2=B2rhs; mlbBrhs3=B3rhs;

mlbXt1=Xt1; mlbXt2=Xt2; mlbXt3=Xt3;
mlbXtrhs1=Xt1rhs; mlbXtrhs2=Xt2rhs; mlbXtrhs3=Xt3rhs;

(******************************************************************************)
(*                                   Groups                                   *)
(******************************************************************************)

DefineGroup1[name_, tensor_] :=
  Module[{group},
         group = CreateGroupFromTensor[tensor];
         group = SetGroupName[group, name];
         group];

inheritedImplementations =
  {"ADMBase", "ML_BSSN", "Cactus", "LocalReduce", "HydroBase"};

ThornGroups = 
  {DefineGroup1["propertime", tau], DefineGroup1["Kthreshold", Ktransition]};

extraGroups = {
  {"ADMBase::metric",        {gxx, gxy, gxz, gyy, gyz, gzz}},
  {"ADMBase::lapse",         {alp}},
  {"ADMBase::dtlapse",       {dtalp}},
  {"ADMBase::shift",         {betax, betay, betaz}},
  {"ADMBase::dtshift",       {dtbetax, dtbetay, dtbetaz}},
  {"ML_BSSN::ML_lapse",      {alpha}},
  {"ML_BSSN::ML_lapserhs",   {alpharhs}},
  {"ML_BSSN::ML_shiftrhs",   {beta1rhs, beta2rhs, beta3rhs}},
  {"ML_BSSN::ML_dtshift",    {B1, B2, B3}},
  {"ML_BSSN::ML_dtshiftrhs", {B1rhs, B2rhs, B3rhs}},
  {"ML_BSSN::ML_Gamma",      {Xt1, Xt2, Xt3}},
  {"ML_BSSN::ML_Gammarhs",   {Xt1rhs, Xt2rhs, Xt3rhs}},
  {"ML_BSSN::ML_trace_curv", {trK}},
  {"HydroBase::rho",         {rho}}, 
  {"HydroBase::press",       {press}}, 
  {"HydroBase::eps",         {eps}}
  };

declaredGroupNames = Map [First, ThornGroups];
groups = Join [ThornGroups, extraGroups];

(******************************************************************************)
(*                                 Expressions                                *)
(******************************************************************************)

(* Functions & Definitions *)

detgExpr = Det[MatrixOfComponents[g[la,lb]]];

(* Derivatives *)

SCDO = StandardCenteredDifferenceOperator;
SUDO = StandardUpwindDifferenceOperator;
SUDOsymm[p_, m1_Integer, m2_Integer, i_Integer] :=
  1/2 (SUDO[p, m1, m2, i] - SUDO[p, m2, m1, i]);
SUDOanti[p_, m1_Integer, m2_Integer, i_Integer] :=
  1/2 (SUDO[p, m1, m2, i] + SUDO[p, m2, m1, i]);

derivatives = {
  PDstandardNth[i_]    -> SCDO[1, fdOrder/2, i],
  PDstandardNth[i_,i_] -> SCDO[2, fdOrder/2, i],
  PDstandardNth[i_,j_] -> SCDO[1, fdOrder/2, i] SCDO[1, fdOrder/2, j],
  PDupwindNthSymm[i_] -> SUDOsymm[1, fdOrder/2-1, fdOrder/2+1, i],
  PDupwindNthAnti[i_] -> SUDOanti[1, fdOrder/2-1, fdOrder/2+1, i]
  };

PD  = PDstandardNth;
Upwind[dir_, var_, idx_] := dir PDupwindNthAnti[var,idx] 
                            + Abs[dir] PDupwindNthSymm[var,idx];
 
(* Split a calculation *) (* Took this from ML_BSSN *)

PartialCalculation[calc_, suffix_, updates_, vars_] :=
  Module[
    {name, calc1, replacements, calc2, vars1, patterns, eqs, calc3},
    (* Add suffix to name *)
    name  = lookupDefault[calc, Name, ""] <> suffix;
    calc1 = mapReplaceAdd[calc, Name, name];
    (* Replace some entries in the calculation *)
    replacements = updates //. (lhs_ -> rhs_) -> (mapReplaceAdd[#, lhs, rhs]&);
    calc2        = calc1 // Composition@@replacements;
    (* Remove unnecessary equations *)
    vars1    = Join[vars, lookupDefault[calc2, Shorthands, {}]];
    patterns = Replace[vars1, {Tensor[n_,__]      ->     Tensor[n,__] ,
                               dot[Tensor[n_,__]] -> dot[Tensor[n,__]]}, 1];
    eqs      = FilterRules[lookup[calc, Equations], patterns];
    calc3    = mapReplace[calc2, Equations, eqs];
    calc3];
    
(* Softplus function *)
LnExp[K_, S_, T_]:= IfThen[S (K - T) < -20, T,
                           IfThen[S (K - T) > 20, K, 
                                  Log[1 + Exp[S (K - T)]]/S + T]];

Advection[LieDeriv_, b_, X_]:= IfThen[LieDeriv!=0, 
                                      IfThen[upwind==1, Upwind[b[ub], X, lb], 
                                             b[ub] PD[X, lb]], 
                                      0];

(******************************************************************************)
(*                           Master calculations                              *)
(******************************************************************************)

(***** Gauge evolution equations *****)

MasterCalc = {
    Shorthands -> {dir[ua],
                   partialtalpha, Ka, KaTransitionVal, Kb, eosw, Kth, fBM,
                   detg, psi, psim2, dpsim2[la], gucon[ua,ub], eta,
                   partialtbeta[ua], partialtB[ua]},
    Equations -> {
        dir[ua] -> Sign[b[ua]],
        (* background extrinsic curvature *)
        eosw -> press / (rho (1 + eps)),
        Kth -> - 2.0 / (tau (1 + eosw)),
      
        (***** alpha *****)
        fBM -> fBMa + fBMb a^fBMc,
        KaTransitionVal -> IfThen[KaTransitionExp==1, Ktransition, 
                                  KaTransition],
        Ka -> IfThen[KaExpression==2, Exp[- a] bssntrK,
                     IfThen[KaExpression==1, LnExp[bssntrK, KaSteepness, 
                                                   KaTransitionVal], 
                            bssntrK]],
        Ktransition -> - Max[-Ktransition, Ka],
        Kb -> IfThen[KbExpression==2, - Sqrt[24 Pi (rho (1 + eps))], 
                     IfThen[KbExpression==1, Kth, 
                            0.0]],
        partialtalpha  -> (- a^2 fBM (Ka - Kb) 
                           + Advection[alphaFullLieDeriv, b, a]),
        
        (***** beta *****)
        (* evolving eta with eq 4 of 0912.3125*)
        detg -> detgExpr,
        psi -> detg^(1/12),
        psim2 -> psi^(-2),
        dpsim2[la] -> - (1/6) detg^(-7/6) PD[detgExpr, la],
        gucon[ua,ub] -> (psi^(4)) detgExpr/detg MatrixInverse[g[ua,ub]],
        eta -> IfThen[varyingEta==1, betaEta Sqrt[gucon[ua,ub] dpsim2[la] dpsim2[lb]] 
                                     / ( 1 - psim2 )^2, 
                      betaEta],
        (* eq 4.3.33 and 4.3.34 in Alcubierre*)
        partialtbeta[ua] -> betaXi1 mlbB[ua] + Advection[betaFullLieDeriv, b, b[ua]],
        partialtB[ua] -> (betaXi2 a^betaP (mlbXtrhs[ua] 
                                           - Advection[betaFullLieDeriv, b, mlbXt[ua]])
                          - eta mlbB[ua]
                          + Advection[betaFullLieDeriv, b, mlbB[ua]]),
                                  
        (***** Update ADMBase and ML_BSSN *****)
        admdtalpha  -> partialtalpha,
        mlbalpharhs -> partialtalpha,
        admdtbeta[ua] -> partialtbeta[ua],
        mlbbetarhs[ua] -> partialtbeta[ua],
        mlbBrhs[ua] -> partialtB[ua]
    }};

(********************* Lapse *********************)
 
dtLapsePostStep = PartialCalculation[
    MasterCalc, "",
     {
         Name -> thorn <> "_dtLapsePostStep",
         Schedule -> {"IN MoL_PostStep AFTER ML_BSSN_ADMBaseBoundaryScalar BEFORE ADMBase_SetADMVars"},
         Where -> InteriorNoSync
     },
     {admdtalpha, mlbalpharhs, Ktransition}];
 
dtLapsePostRHS = PartialCalculation[
    MasterCalc, "",
     {
         Name -> thorn <> "_dtLapsePostRHS",
         Schedule -> {"IN MoL_PostRHS"},
         Where -> InteriorNoSync
     },
     {admdtalpha, mlbalpharhs, Ktransition}];

dtLapsePostStepBoundary = 
    {
         Name -> thorn <> "_dtLapsePostStepBoundary",
         Schedule -> {"IN MoL_PostStep AFTER " <> thorn <> "_dtLapsePostStep"},
         Where -> Boundary,
         Equations ->
         {
            admdtalpha -> 0.0,
            mlbalpharhs -> 0.0,
            Ktransition -> 0.0
         }
     };
 
dtLapsePostRHSBoundary =
    {
         Name -> thorn <> "_dtLapsePostRHSBoundary",
         Schedule -> {"IN MoL_PostRHS AFTER " <> thorn <> "_dtLapsePostRHS"},
         Where -> Boundary,
         Equations ->
         {
            admdtalpha -> 0.0,
            mlbalpharhs -> 0.0,
            Ktransition -> 0.0
         }
     };

(********************* Shift *********************)

dtShiftPostStep = PartialCalculation[
    MasterCalc, "",
     {
         Name -> thorn <> "_dtShiftPostStep",
         Schedule -> {"IN MoL_PostStep AFTER ML_BSSN_ADMBaseBoundaryScalar BEFORE ADMBase_SetADMVars"},
         Where -> InteriorNoSync
     },
     {admdtbeta[ua], mlbbetarhs[ua], mlbBrhs[ua]}];
 
dtShiftPostRHS = PartialCalculation[
    MasterCalc, "",
     {
         Name -> thorn <> "_dtShiftPostRHS",
         Schedule -> {"IN MoL_PostRHS"},
         Where -> InteriorNoSync
     },
     {admdtbeta[ua], mlbbetarhs[ua], mlbBrhs[ua]}];

dtShiftPostStepBoundary =
    {
         Name -> thorn <> "_dtShiftPostStepBoundary",
         Schedule -> {"IN MoL_PostStep AFTER " <> thorn <> "_dtShiftPostStep"},
         Where -> Boundary,
         Equations ->
         {
            admdtbeta[ua] -> 0.0,
            mlbbetarhs[ua] -> 0.0,
            mlbBrhs[ua] -> 0.0
         }
     };
 
dtShiftPostRHSBoundary =
    {
         Name -> thorn <> "_dtShiftPostRHSBoundary",
         Schedule -> {"IN MoL_PostRHS AFTER " <> thorn <> "_dtShiftPostRHS"},
         Where -> Boundary,
         Equations ->
         {
            admdtbeta[ua] -> 0.0,
            mlbbetarhs[ua] -> 0.0,
            mlbBrhs[ua] -> 0.0
         }
     };

(******************* Proper time *******************)

initialise =
{
  Name -> thorn <> "_InitialTau",
  Schedule -> {"AT initial after ML_BSSN_InitialADMBase1Everywhere"},
  Where -> Everywhere,
  Equations ->
  {
    tau -> t,
    Ktransition -> KaTransition
  }
};
       
initRHSCalc =
{
  Name -> thorn <> "_InitRHSTau",
  Schedule -> {"AT analysis BEFORE " <> thorn <> "_RHSTau"},
  Where -> Everywhere,
  Equations ->
  {
    dot[tau] -> 0,
    dot[Ktransition] -> 0
  }
};

evolCalc =
{
  Name -> thorn <> "_RHSTau",
  Schedule -> {"IN MoL_CalcRHS"},
  Where -> Everywhere,
  Equations ->
  {
    dot[tau] -> Abs[a^2 - b[ua] b[ub] g[la,lb]]^(1/2),
    dot[Ktransition] -> 0
  }
};
 
calculations = {
    dtLapsePostStep,
    dtLapsePostRHS,
    dtLapsePostStepBoundary,
    dtLapsePostRHSBoundary,
    dtShiftPostStep,
    dtShiftPostRHS,
    dtShiftPostStepBoundary,
    dtShiftPostRHSBoundary,
    initialise,
    initRHSCalc,
    evolCalc};

(******************************************************************************)
(*                                 Parameters                                 *)
(******************************************************************************)

realParameters = {
    {
        Name -> fBMa,
        Description -> "f(alpha) = fBMa + fBMb alpha^fBMc",
        Steerable -> Always,
        Default -> 0.0
    },
    {
        Name -> fBMb,
        Description -> "f(alpha) = fBMa + fBMb alpha^fBMc",
        Steerable -> Always,
        Default -> 0.0
    },
    {
        Name -> KaSteepness,
        Description -> "Softplus steepness (S), only used if KaExpression = 1",
        Steerable -> Always,
        Default -> 1.0
    },
    {
        Name -> KaTransition,
        Description -> "Softplus transition (T), only used if KaExpression = 1",
        Steerable -> Always,
        Default -> 0.0
    },
    {
        Name -> betaXi1,
        Description -> "d/dt beta = Xi_1 B^i",
        Steerable -> Always,
        Default -> 0.0
    },
    {
        Name -> betaXi2,
        Description -> "d/dt B = alpha^P Xi_2 d/dt Xt^i - Eta B^i",
        Steerable -> Always,
        Default -> 0.0
    },
    {
        Name -> betaEta,
        Description -> "d/dt B = alpha^P Xi_2 d/dt Xt^i - Eta B^i",
        Steerable -> Always,
        Default -> 0.0
    }};
  
intParameters = {
    {
        Name -> fBMc,
        Description -> "f(alpha) = fBMa + fBMb alpha^fBMc",
        Steerable -> Always,
        Default -> 0
    },
    {
       Name -> KaExpression,
       Description -> "d/dt alpha = - alpha^2 f(alpha) (Ka - Kb)",
       AllowedValues -> {{Value -> 0, Description -> "Ka = K"},
                         {Value -> 1, Description -> "Ka = Softplus: Ln[1 + Exp[S (K - T)]] / S + T"},
                         {Value -> 2, Description -> "Ka = exp(-alpha) K"}},
       Steerable -> Always,
       Default -> 0
    },
    {
       Name -> KaTransitionExp,
       Description -> "Softplus transition (T) expression, only used if KaExpression = 1",
       AllowedValues -> {{Value -> 0, Description -> "T = Katransition"},
                         {Value -> 1, Description -> "T = Katransition then updated as = - max(-T, Ka)"}},
       Steerable -> Always,
       Default -> 0
    },
    {
       Name -> KbExpression,
       Description -> "d/dt alpha = - alpha^2 f(alpha) (Ka - Kb)",
       AllowedValues -> {{Value -> 0, Description -> "Kb = 0.0"},
                         {Value -> 1, Description -> "Kb = background_K: - 2 / (tau (1 + eosw))"},
                         {Value -> 2, Description -> "Kb = - sqrt{ 24 pi rho (1 + eps) }"}},
       Steerable -> Always,
       Default -> 0
    },
     {
        Name -> betaP,
        Description -> "d/dt B = alpha^P Xi d/dt Xt^i - Eta B^i",
        Steerable -> Always,
        Default -> 1
     },
    {
        Name -> varyingEta,
        Description -> "Varying eta parameter in shift condition",
        AllowedValues -> {{Value -> 0, Description -> "eta = betaEta"},
                          {Value -> 1, Description -> "eta = betaEta * Eq 4 of https://arxiv.org/pdf/0912.3125"}},
        Steerable -> Always,
        Default -> 0
     },
    {
        Name -> alphaFullLieDeriv,
        Description -> "Include Lie derivative advection terms, beta^k partial_k term in lapse evo",
        AllowedValues -> {{Value -> 0, Description -> "no, off"},
                          {Value -> 1, Description -> "yes, on"}},
        Steerable -> Always,
        Default -> 0
     },
     {
        Name -> betaFullLieDeriv,
        Description -> "Include Lie derivative advection terms, beta^k partial_k term in shift evo",
        AllowedValues -> {{Value -> 0, Description -> "no, off"},
                          {Value -> 1, Description -> "yes, on"}},
        Steerable -> Always,
        Default -> 0
     },
    {
        Name -> upwind,
        Description -> "Use upwind differencing for Lie derivative advection terms",
        AllowedValues -> {{Value -> 0, Description -> "no, use centered schemes"},
                          {Value -> 1, Description -> "yes, use upwind"}},
        Steerable -> Always,
        Default -> 0
     },
     {
        Name -> fdOrder,
        Description -> "Finite differencing order",
        AllowedValues -> {2, 4, 6, 8},
        Steerable -> Always,
        Default -> 4
     }};

(******************************************************************************)
(*                             Construct the thorn                            *)
(******************************************************************************)

CreateKrancThornTT[
  groups, ".", thorn,
  Calculations -> calculations,
  PartialDerivatives -> derivatives,
  DeclaredGroups -> declaredGroupNames,
  EvolutionTimelevels -> maxTimelevels,
  DefaultEvolutionTimelevels -> Min[3,maxTimelevels],
  UseJacobian -> True,
  UseLoopControl -> True,
  UseVectors -> False,
  InheritedImplementations -> inheritedImplementations,
  IntParameters -> intParameters,
  RealParameters -> realParameters
];