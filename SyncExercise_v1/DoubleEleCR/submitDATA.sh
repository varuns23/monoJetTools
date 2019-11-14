RUN="python ${CMSSW_BASE}/src/SyncExercise/monoJetTools2018/SubmitCondor.py analyze"

. "${CMSSW_BASE}/src/SyncExercise/monoJetTools2018/plugins/data_cfg.sh"

make || exit -1

echo "Do the CR Data samples"

file="DoubleEle"
label="Ele"

ELE_B "$RUN" $file $label
ELE_C "$RUN" $file $label
