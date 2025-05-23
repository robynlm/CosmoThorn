(* ::Package:: *)

SetEnhancedTimes[False];

maxTimelevels = 4;
thorn = "CosmoLapse";

(******************************************************************************)
(*                                 Tensors                                    *)
(******************************************************************************)

Map [DefineTensor, {
  tau, dir, g,
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
  {DefineGroup1["propertime", tau]};

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

derivatives = {
  PDstandardNth[i_]    -> SCDO[1, fdOrder/2, i],
  PDstandardNth[i_,i_] -> SCDO[2, fdOrder/2, i],
  PDstandardNth[i_,j_] -> SCDO[1, fdOrder/2, i] SCDO[1, fdOrder/2, j]
  };

PD  = PDstandardNth;
 
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
    
(* Smooth max *)
LnExp[expr_]:= IfThen[expr>40, expr, Log[1 + Exp[expr]]];

(******************************************************************************)
(*                           Master calculations                              *)
(******************************************************************************)

(***** Gauge evolution equations *****)

MasterCalc = {
    Shorthands -> {dir[ua],
                   partialtalpha, Ka, Kb, Kc, eosw, Kth, fBM, expterm, 
                   partialtbeta[ua], partialtB[ua]},
    Equations -> {
        dir[ua] -> Sign[b[ua]],
        eosw -> press / (rho (1 + eps)),
        Kth -> - 2.0 / (tau (1 + eosw)),
      
        (***** alpha *****)
        fBM -> fBMa + fBMb a^fBMc,
        expterm -> KaSteepness (bssntrK - KaTransition),
        Ka -> IfThen[KaExpression==1, ((LnExp[expterm] / KaSteepness) 
                                      + (KaTransition / ( 1 + Exp[-expterm]))), bssntrK],
        Kb -> IfThen[KbExpression==1, Kth, 0.0],
        Kc -> IfThen[KcExpression==1, -Kth, 1.0],
        partialtalpha  -> (- a^2 fBM (Ka - Kb) / Kc
                      + IfThen[alphaFullLieDeriv!=0, b[ua] PD[a,la], 0]),
        
        (***** beta *****)
        (* eq 4.3.33 and 4.3.34 in Alcubierre*)
        partialtbeta[ua] -> mlbB[ua] + IfThen[betaFullLieDeriv!=0, b[ub] PD[b[ua],lb], 0],
        partialtB[ua] -> (betaXi a^betaP
                          (mlbXtrhs[ua] - IfThen[betaFullLieDeriv!=0, b[ub] PD[mlbXt[ua],lb], 0])
                          - betaEta mlbB[ua]
                          + IfThen[betaFullLieDeriv!=0, b[ub] PD[mlbB[ua],lb], 0]),

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
     {admdtalpha, mlbalpharhs}];
 
dtLapsePostRHS = PartialCalculation[
    MasterCalc, "",
     {
         Name -> thorn <> "_dtLapsePostRHS",
         Schedule -> {"IN MoL_PostRHS"},
         Where -> InteriorNoSync
     },
     {admdtalpha, mlbalpharhs}];

dtLapsePostStepBoundary = 
    {
         Name -> thorn <> "_dtLapsePostStepBoundary",
         Schedule -> {"IN MoL_PostStep AFTER " <> thorn <> "_dtLapsePostStep"},
         Where -> Boundary,
         Equations ->
         {
            admdtalpha -> 0.0,
            mlbalpharhs -> 0.0
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
            mlbalpharhs -> 0.0
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
    tau -> t
  }
};
       
initRHSCalc =
{
  Name -> thorn <> "_InitRHSTau",
  Schedule -> {"AT analysis BEFORE " <> thorn <> "_RHSTau"},
  Where -> Everywhere,
  Equations ->
  {
    dot[tau] -> 0
  }
};

evolCalc =
{
  Name -> thorn <> "_RHSTau",
  Schedule -> {"IN MoL_CalcRHS"},
  Where -> Everywhere,
  Equations ->
  {
    dot[tau] -> (a^2 - b[ua] b[ub] g[la,lb])^(1/2)
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
        Default -> 0.0
    },
    {
        Name -> fBMb,
        Description -> "f(alpha) = fBMa + fBMb alpha^fBMc",
        Default -> 0.0
    },
    {
        Name -> KaSteepness,
        Description -> "Steepnes of smooth step function, only used if KaExpression = 1",
        Default -> 1.0
    },
    {
        Name -> KaTransition,
        Description -> "Where smooth step function goes through its' transition, only used if KaExpression = 1",
        Default -> 0.0
    },
    {
        Name -> betaXi,
        Description -> "d/dt B = alpha^P Xi d/dt Xt^i - Eta B^i",
        Default -> 0.0
    },
    {
        Name -> betaEta,
        Description -> "d/dt B = alpha^P Xi d/dt Xt^i - Eta B^i",
        Default -> 0.0
    }};
  
intParameters = {
    {
        Name -> fBMc,
        Description -> "f(alpha) = fBMa + fBMb alpha^fBMc",
        Default -> 0
    },
    {
       Name -> KaExpression,
       Description -> "d/dt alpha = - alpha^2 f(alpha) (Ka - Kb) / Kc",
       AllowedValues -> {{Value -> 0, Description -> "K"},
                         {Value -> 1, Description -> "ln(1 + exp(steepness (K - transition))) / steepness"}},
       Default -> 0
    },
    {
       Name -> KbExpression,
       Description -> "d/dt alpha = - alpha^2 f(alpha) (Ka - Kb) / Kc",
       AllowedValues -> {{Value -> 0, Description -> "zero"},
                         {Value -> 1, Description -> "background_K"}},
       Default -> 0
    },
    {
       Name -> KcExpression,
       Description -> "d/dt alpha = - alpha^2 f(alpha) (Ka - Kb) / Kc",
       AllowedValues -> {{Value -> 0, Description -> "one"},
                         {Value -> 1, Description -> "- background_K"}},
       Default -> 0
    },
    {
        Name -> alphaFullLieDeriv,
        Description -> "Full Lie derivative, include beta^k partial_k term in alpha evo eq",
        AllowedValues -> {{Value -> 0, Description -> "no, off"},
                          {Value -> 1, Description -> "yes, on"}},
        Default -> 0
     },
     {
        Name -> betaP,
        Description -> "d/dt B = alpha^P Xi d/dt Xt^i - Eta B^i",
        Default -> 1
     },
     {
        Name -> betaFullLieDeriv,
        Description -> "Full Lie derivative, include beta^k partial_k term in beta evo eq",
        AllowedValues -> {{Value -> 0, Description -> "no off"},
                          {Value -> 1, Description -> "yes on"}},
        Default -> 0
     },
     {
        Name -> fdOrder,
        Description -> "Finite differencing order",
        AllowedValues -> {2, 4, 6, 8},
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
