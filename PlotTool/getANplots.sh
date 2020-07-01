#!/bin/sh
set -e
nvariables='ChNemPtFrac ChNemPtFracV2 recoil pfMET recoilall pfMETall nJetsSkim j1etaWidth j1pT j1Eta j1Phi j1CHF j1NHF nVtxNoW nVtxReW dphimin metcut'
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
    # if [[ "$1" == "SignalRegion" ]]; then
    # 	run_options="$options --blinded"
    # fi
    pushd $1
    shift 1
    array="$nvariables $@"

    # plot $run_options -a --sub PtFracCut recoil --soverb
    # for cut in $(echo 0.7 0.8 0.85 0.9); do
    # 	plot $run_options --branch -a --sub PtFracCut recoil -c "ChNemPtFrac>${cut}" --soverb &
    # done
    # plot $run_options -a --sub $subdir $array $uncertainty &
    # plot $run_options --sub $subdir $n_cut &
    plot $run_options -a --sub $subdir j1ChMult j1NhMult
    wait
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
