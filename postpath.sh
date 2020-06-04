#!/bin/sh

# Fetches the most recent Postfile paths in the hdfs path

path=/hdfs/store/user/ekoenig/MonoZprimeJet/PostFiles

YEARS="2016 2017 2018"
YEARS="2017"
REGIONS="SignalRegion SingleEleCR SingleMuCR DoubleEleCR DoubleMuCR GammaCR"
PURITY="$(echo GammaPurity/{Den,QCD}Template GammaPurity/{Sig,Bkg}Template/{sieie,iso}_purity)"

get_path() {
    postpath=$(ls -td $path/$1/*/ | head -n 1)
    echo "$postpath --> $1/"
    echo $postpath > $1/postpath.txt
}

for y in $YEARS; do
    for r in $REGIONS; do get_path $y/$r; done
    for p in $PURITY; do get_path $y/$p; done
done
