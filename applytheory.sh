for d in ${@}; do
    pushd $d
    for f in $(ls post*); do
	python $CMSSW_BASE/src/monoJetTools/macros/ApplyTheory.py $f &
    done
    wait $(jobs -p)
    popd;
done
