#!/usr/bin/env bash

CMSSW_RELEASE="/cvmfs/cms.cern.ch/slc6_amd64_gcc700/cms/cmssw/CMSSW_10_2_10/src/"
source /cvmfs/cms.cern.ch/cmsset_default.sh
pushd $CMSSW_RELEASE
eval `scramv1 runtime -sh`
popd

export LD_LIBRARY_PATH=$PWD/lib:$LD_LIBRARY_PATH
export X509_USER_PROXY=userproxy

./$@ || exit $?
