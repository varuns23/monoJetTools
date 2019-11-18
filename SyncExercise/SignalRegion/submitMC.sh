RUN="python ${CMSSW_BASE}/src/monojet/CondorTools.py analyze"

. "${CMSSW_BASE}/src/ZprimeTools2018/plugins/mc_cfg.sh"

make || exit -1

echo "Do the MC samples"

WJETS "$RUN"
GJETS "$RUN"
DIBOSON "$RUN"
TTJETS "$RUN"
DYJETS "$RUN"
ZJETS "$RUN"
QCD "$RUN"
