RUN="python ${CMSSW_BASE}/src/SyncExercise/monoJetTools2018/SubmitCondor.py analyze"

. "${CMSSW_BASE}/src/SyncExercise/monoJetTools2018/plugins/mc_cfg.sh"

make || exit -1

echo "Do the MC samples"

ST "$RUN"
#WJETS "$RUN"
#GJETS "$RUN"
#DIBOSON "$RUN"
#TTJETS "$RUN"
#DYJETS "$RUN"
#ZJETS "$RUN"
#QCD "$RUN"
