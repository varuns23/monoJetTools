RUN="python ${CMSSW_BASE}/src/ZprimeTools2018/SubmitCondor.py analyze"

. "${CMSSW_BASE}/src/ZprimeTools2018/plugins/data_cfg.sh"

make || exit -1

echo "Do the CR Data samples"

file="METdata"
label="MET"

MET_A "$RUN" $file $label
