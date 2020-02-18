#!/bin/sh

nvariables='recoil nJets j1pT j1Eta j1Phi nVtx'
singleleps='LeptonPt LeptonEta LeptonPhi'
doubleleps='dileptonM dileptonPt leadingLeptonPt leadingLeptonEta leadingLeptonPhi subleadingLeptonEta subleadingLeptonPt subleadingLeptonPhi'
gamma='photonPt photonEta photonPhi'
cutvars='h_metcut h_dphimin'
uncertainty=''

options=$@
plot() {
    echo "./PlotTool/plotter.py" $@
    ./PlotTool/plotter.py $@
}

run() {
    subdir="AN"
    pushd $1
    shift 1
    array="$@ $nvariables"
    plot $options --sub $subdir -a $array $uncertainty || exit 1
    plot $options --sub $subdir $cutvars || exit 1
    popd
}

run2() {
    region=$1
    shift 1
    array="$@ $nvariables"
    plot --run2 $region $options $array || exit 1
    plot --run2 $region $options $cutvars || exit 1
}

region() {
    run SignalRegion || exit 1
    run SingleEleCR $singleleps  || exit 1
    run SingleMuCR $singleleps || exit 1
    run DoubleEleCR $doubleleps || exit 1
    run DoubleMuCR $doubleleps || exit 1
    run GammaCR $gamma || exit 1
}

region2() {
    run2 SignalRegion || exit 1
    run2 SingleEleCR $singleleps || exit 1
    run2 SingleMuCR $singleleps || exit 1
    run2 DoubleEleCR $doubleleps || exit 1
    run2 DoubleMuCR $doubleleps || exit 1
    run GammaCR $gamma || exit 1
}

YEARS="2017 2018"
if [ -d "2017" ]; then
    region2
else
    region
fi
