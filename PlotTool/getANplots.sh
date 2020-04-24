#!/bin/sh
set -e
nvariables='recoil pfMET recoilall pfMETall nJetsSkim j1etaWidth j1pT j1Eta j1Phi j1CHF j1NHF nVtxNoW nVtxReW dphimin metcut'
singleleps='LeptonPt LeptonEta LeptonPhi lepMET_MT'
doubleleps='dileptonM dileptonPt leadingLeptonPt leadingLeptonEta leadingLeptonPhi subleadingLeptonEta subleadingLeptonPt subleadingLeptonPhi'
gamma='photonPt photonEta photonPhi'
ncut='h_metcut h_dphimin'
uncertainty=''

options=$@
plot() {
    echo "./PlotTool/plotter.py" $@
    ./PlotTool/plotter.py $@
}

run() {
    subdir="AN"
    n_cut="$ncut"
    if [[ "$1" == "Single"* ]]; then
    	n_cut="$ncut h_lepMET_MT"
    fi
    run_options="$options"
    if [[ "$1" == "SignalRegion" ]]; then
	run_options="$options --blinded"
    fi
    pushd $1
    shift 1
    array="$nvariables $@"
    plot $run_options --sub $subdir -a $array
    plot $run_options --sub $subdir -a ChNemPtFrac -b res
    # plot $run_options --sub $subdir -a $cat1
    # plot $run_options --sub $subdir -a $cat2
    # plot $run_options --sub $subdir -a $cat3
    plot $run_options --sub $subdir $n_cut
    popd
}

run2() {
    region=$1
    shift 1
    array="$@ $nvariables"
    plot --run2 $region $options $array
    plot --run2 $region $options $cutvars
}

region() {
    run SignalRegion
    run SingleEleCR $singleleps
    run SingleMuCR $singleleps
    run DoubleEleCR $doubleleps
    run DoubleMuCR $doubleleps
    run GammaCR $gamma
}

region2() {
    run2 SignalRegion
    run2 SingleEleCR $singleleps
    run2 SingleMuCR $singleleps
    run2 DoubleEleCR $doubleleps
    run2 DoubleMuCR $doubleleps
    run GammaCR $gamma
}

YEARS="2017 2018"
if [ -d "2017" ]; then
    region2
else
    region
fi
