RUN="python ${CMSSW_BASE}/src/monojet/CondorTools/SubmitCondor.py analyze"

. "${PWD}/../plugins/mc_cfg.sh"

make || exit -1

echo "Do the MC samples"

# ST "$RUN"
#WJETS "$RUN"
#GJETS "$RUN"
#DIBOSON "$RUN"
#TTJETS "$RUN"
#DYJETS "$RUN"
ZJETS "$RUN"
#QCD "$RUN"
