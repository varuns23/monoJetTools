#!/bin/sh

# Fetches the most recent Postfile paths in the hdfs path

path=/hdfs/store/user/ekoenig/MonomonoJetJet/PostFiles

YEARS="2016 2017 2018"
REGIONS="SignalRegion SingleEleCR SingleMuCR DoubleEleCR DoubleMuCR"

for y in $YEARS; do
    for r in $REGIONS; do
	postpath=$(ls -td $path/$y/$r/*/ | head -n 1)
	echo "$postpath --> $y/$r/"
	echo $postpath > $y/$r/postpath.txt
    done
done
