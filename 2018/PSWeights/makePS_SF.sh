#!/bin/sh
set -e 
REGIONS="SignalRegion SingleEleCR SingleMuCR DoubleEleCR DoubleMuCR GammaCR"
for r in $REGIONS; do mkdir -p $r; done

makePSW() {
    r=$1
    shift 1
    python PlotTool/makePS_SF.py $@
    mv -v PSW_SF.root ../PSWeights/$r/
}

pushd ../
for r in $REGIONS; do
    pushd $r;
    makePSW $r $@ &
    popd;
done
wait
popd

python createPSW_SF.py 
    
