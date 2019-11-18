RUN="python ${CMSSW_BASE}/src/monojet/CondorTools/SubmitCondor.py analyze"

. "${PWD}/../plugins/data_cfg.sh"

make || exit -1

echo "Do the CR Data samples"

file="DoubleMu"
label="Mu"

MET_A "$RUN" $file $label
MET_B "$RUN" $file $label
MET_C "$RUN" $file $label
MET_D "$RUN" $file $label
