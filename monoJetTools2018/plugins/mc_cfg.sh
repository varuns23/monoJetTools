MC=/hdfs/store/user/ekoenig/MonoZprimeJet/NTuples/2018/MC2018_Autumn18_June2019
MC_JECv19=/hdfs/store/user/varuns/NTuples/MC/MC2018_RunIIAutumn18_102X_JECv19

ST(){
  ${1} $MC_JECv19/ST/ST_tW_top_5f/ post_ST_0.root -1 10000 ST100_0 split_-1
}

ZJETS() {
    ${1} $MC_JECv19/ZJets/ZJetsToNuNu_HT400-600/0000/ post_Z400to600.root -1 10000 Z400_0 split_-1
}

QCD() {
    ${1} $MC/QCD/QCD_HT100to200_TuneCP5_13TeV-madgraphMLM-pythia8/0000/ postQCD100to200_0.root -1 10000 QCD100_0 split_-1
    ${1} $MC/QCD/QCD_HT100to200_TuneCP5_13TeV-madgraphMLM-pythia8/0001/ postQCD100to200_1.root -1 10000 QCD100_1 split_-1
    ${1} $MC/QCD/QCD_HT100to200_TuneCP5_13TeV-madgraphMLM-pythia8/0002/ postQCD100to200_2.root -1 10000 QCD100_2 split_-1
    ${1} $MC/QCD/QCD_HT100to200_TuneCP5_13TeV-madgraphMLM-pythia8/0003/ postQCD100to200_3.root -1 10000 QCD100_3 split_-1
    ${1} $MC/QCD/QCD_HT100to200_TuneCP5_13TeV-madgraphMLM-pythia8/0004/ postQCD100to200_4.root -1 10000 QCD100_4 split_-1
    ${1} $MC/QCD/QCD_HT100to200_TuneCP5_13TeV-madgraphMLM-pythia8/0005/ postQCD100to200_5.root -1 10000 QCD100_5 split_-1
    ${1} $MC/QCD/QCD_HT100to200_TuneCP5_13TeV-madgraphMLM-pythia8/0006/ postQCD100to200_6.root -1 10000 QCD100_6 split_-1
    ${1} $MC/QCD/QCD_HT100to200_TuneCP5_13TeV-madgraphMLM-pythia8/0007/ postQCD100to200_7.root -1 10000 QCD100_7 split_-1
    ${1} $MC/QCD/QCD_HT100to200_TuneCP5_13TeV-madgraphMLM-pythia8/0008/ postQCD100to200_8.root -1 10000 QCD100_8 split_-1
    ${1} $MC/QCD/QCD_HT100to200_TuneCP5_13TeV-madgraphMLM-pythia8/0009/ postQCD100to200_9.root -1 10000 QCD100_9 split_-1

    ${1} $MC/QCD/QCD_HT200to300_TuneCP5_13TeV-madgraphMLM-pythia8/0000/ postQCD200to300_0.root -1 10000 QCD200_0 split_-1
    ${1} $MC/QCD/QCD_HT200to300_TuneCP5_13TeV-madgraphMLM-pythia8/0001/ postQCD200to300_1.root -1 10000 QCD200_1 split_-1
    ${1} $MC/QCD/QCD_HT200to300_TuneCP5_13TeV-madgraphMLM-pythia8/0002/ postQCD200to300_2.root -1 10000 QCD200_2 split_-1
    ${1} $MC/QCD/QCD_HT200to300_TuneCP5_13TeV-madgraphMLM-pythia8/0003/ postQCD200to300_3.root -1 10000 QCD200_3 split_-1
    ${1} $MC/QCD/QCD_HT200to300_TuneCP5_13TeV-madgraphMLM-pythia8/0004/ postQCD200to300_4.root -1 10000 QCD200_4 split_-1
    ${1} $MC/QCD/QCD_HT200to300_TuneCP5_13TeV-madgraphMLM-pythia8/0005/ postQCD200to300_5.root -1 10000 QCD200_5 split_-1

    ${1} $MC/QCD/QCD_HT300to500_TuneCP5_13TeV-madgraphMLM-pythia8/0000/ postQCD300to500_0.root -1 10000 QCD300_0 split_-1
    ${1} $MC/QCD/QCD_HT300to500_TuneCP5_13TeV-madgraphMLM-pythia8/0001/ postQCD300to500_1.root -1 10000 QCD300_1 split_-1
    ${1} $MC/QCD/QCD_HT300to500_TuneCP5_13TeV-madgraphMLM-pythia8/0002/ postQCD300to500_2.root -1 10000 QCD300_2 split_-1
    ${1} $MC/QCD/QCD_HT300to500_TuneCP5_13TeV-madgraphMLM-pythia8/0003/ postQCD300to500_3.root -1 10000 QCD300_3 split_-1
    ${1} $MC/QCD/QCD_HT300to500_TuneCP5_13TeV-madgraphMLM-pythia8/0004/ postQCD300to500_4.root -1 10000 QCD300_4 split_-1
    ${1} $MC/QCD/QCD_HT300to500_TuneCP5_13TeV-madgraphMLM-pythia8/0005/ postQCD300to500_5.root -1 10000 QCD300_5 split_-1

    ${1} $MC/QCD/QCD_HT500to700_TuneCP5_13TeV-madgraphMLM-pythia8/0000/ postQCD500to700_0.root -1 10000 QCD500_0 split_-1
    ${1} $MC/QCD/QCD_HT500to700_TuneCP5_13TeV-madgraphMLM-pythia8/0001/ postQCD500to700_1.root -1 10000 QCD500_1 split_-1
    ${1} $MC/QCD/QCD_HT500to700_TuneCP5_13TeV-madgraphMLM-pythia8/0002/ postQCD500to700_2.root -1 10000 QCD500_2 split_-1
    ${1} $MC/QCD/QCD_HT500to700_TuneCP5_13TeV-madgraphMLM-pythia8/0003/ postQCD500to700_3.root -1 10000 QCD500_3 split_-1
    ${1} $MC/QCD/QCD_HT500to700_TuneCP5_13TeV-madgraphMLM-pythia8/0004/ postQCD500to700_4.root -1 10000 QCD500_4 split_-1
    ${1} $MC/QCD/QCD_HT500to700_TuneCP5_13TeV-madgraphMLM-pythia8/0005/ postQCD500to700_5.root -1 10000 QCD500_5 split_-1

    ${1} $MC/QCD/QCD_HT700to1000_TuneCP5_13TeV-madgraphMLM-pythia8/0000/ postQCD700to1000_0.root -1 10000 QCD700_0 split_-1
    ${1} $MC/QCD/QCD_HT700to1000_TuneCP5_13TeV-madgraphMLM-pythia8/0001/ postQCD700to1000_1.root -1 10000 QCD700_1 split_-1
    ${1} $MC/QCD/QCD_HT700to1000_TuneCP5_13TeV-madgraphMLM-pythia8/0002/ postQCD700to1000_2.root -1 10000 QCD700_2 split_-1
    ${1} $MC/QCD/QCD_HT700to1000_TuneCP5_13TeV-madgraphMLM-pythia8/0003/ postQCD700to1000_3.root -1 10000 QCD700_3 split_-1
    ${1} $MC/QCD/QCD_HT700to1000_TuneCP5_13TeV-madgraphMLM-pythia8/0004/ postQCD700to1000_4.root -1 10000 QCD700_4 split_-1

    ${1} $MC/QCD/QCD_HT1000to1500_TuneCP5_13TeV-madgraphMLM-pythia8/0000/ postQCD1000to1500_0.root -1 10000 QCD1000_0 split_-1
    ${1} $MC/QCD/QCD_HT1000to1500_TuneCP5_13TeV-madgraphMLM-pythia8/0001/ postQCD1000to1500_1.root -1 10000 QCD1000_1 split_-1

    ${1} $MC/QCD/QCD_HT1500to2000_TuneCP5_13TeV-madgraphMLM-pythia8/0000/ postQCD1500to2000_0.root -1 10000 QCD1500_0 split_-1
    ${1} $MC/QCD/QCD_HT1500to2000_TuneCP5_13TeV-madgraphMLM-pythia8/0001/ postQCD1500to2000_1.root -1 10000 QCD1500_1 split_-1

    ${1} $MC/QCD/QCD_HT2000toInf_TuneCP5_13TeV-madgraphMLM-pythia8/0000/ postQCD2000toInf_0.root -1 10000 QCD2000_0 split_-1
}
#TTJets
TTJETS() {
    ${1} $MC/TTJets/TTJets_DiLept_TuneCP5_13TeV-madgraphMLM-pythia8/0000/ postTTJetsDiLept_0.root -1 10000 TTJetsDiLept_0 split_-1
    ${1} $MC/TTJets/TTJets_DiLept_TuneCP5_13TeV-madgraphMLM-pythia8/0001/ postTTJetsDiLept_1.root -1 10000 TTJetsDiLept_1 split_-1
    ${1} $MC/TTJets/TTJets_DiLept_TuneCP5_13TeV-madgraphMLM-pythia8/0002/ postTTJetsDiLept_2.root -1 10000 TTJetsDiLept_2 split_-1

    ${1} $MC/TTJets/TTJets_SingleLeptFromTbar_TuneCP5_13TeV-madgraphMLM-pythia8/0000/ postTTJetsSingleLeptFromT_0.root -1 10000 TTJetsSingleLeptFromT_0 split_-1
    ${1} $MC/TTJets/TTJets_SingleLeptFromTbar_TuneCP5_13TeV-madgraphMLM-pythia8/0001/ postTTJetsSingleLeptFromT_1.root -1 10000 TTJetsSingleLeptFromT_1 split_-1
    ${1} $MC/TTJets/TTJets_SingleLeptFromTbar_TuneCP5_13TeV-madgraphMLM-pythia8/0002/ postTTJetsSingleLeptFromT_2.root -1 10000 TTJetsSingleLeptFromT_2 split_-1
    ${1} $MC/TTJets/TTJets_SingleLeptFromTbar_TuneCP5_13TeV-madgraphMLM-pythia8/0003/ postTTJetsSingleLeptFromT_3.root -1 10000 TTJetsSingleLeptFromT_3 split_-1
    ${1} $MC/TTJets/TTJets_SingleLeptFromTbar_TuneCP5_13TeV-madgraphMLM-pythia8/0004/ postTTJetsSingleLeptFromT_4.root -1 10000 TTJetsSingleLeptFromT_4 split_-1
    ${1} $MC/TTJets/TTJets_SingleLeptFromTbar_TuneCP5_13TeV-madgraphMLM-pythia8/0005/ postTTJetsSingleLeptFromT_5.root -1 10000 TTJetsSingleLeptFromT_5 split_-1
}
#GJets
GJETS() {
    ${1} $MC/GJets/GJets_HT-40To100_TuneCP5_13TeV-madgraphMLM-pythia8/0000/ postGJets40to100_0.root -1 10000 GJets40_0 split_-1
    ${1} $MC/GJets/GJets_HT-40To100_TuneCP5_13TeV-madgraphMLM-pythia8/0001/ postGJets40to100_1.root -1 10000 GJets40_1 split_-1

    ${1} $MC/GJets/GJets_HT-100To200_TuneCP5_13TeV-madgraphMLM-pythia8/0000/ postGJets100to200_0.root -1 10000 GJets100_0 split_-1
    ${1} $MC/GJets/GJets_HT-100To200_TuneCP5_13TeV-madgraphMLM-pythia8/0001/ postGJets100to200_1.root -1 10000 GJets100_1 split_-1

    ${1} $MC/GJets/GJets_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8/0000/ postGJets200to400_0.root -1 10000 GJets200_0 split_-1
    ${1} $MC/GJets/GJets_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8/0001/ postGJets200to400_1.root -1 10000 GJets200_1 split_-1
    ${1} $MC/GJets/GJets_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8/0002/ postGJets200to400_2.root -1 10000 GJets200_2 split_-1

    ${1} $MC/GJets/GJets_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8/0000/ postGJets400to600.root -1 10000 GJets400_0 split_-1

    ${1} $MC/GJets/GJets_HT-600ToInf_TuneCP5_13TeV-madgraphMLM-pythia8/0000/ postGJets600toInf.root -1 10000 GJets600_0 split_-1
}
#Diboson
DIBOSON() {
    ${1} $MC/DiBoson/WWTo2L2Nu_NNPDF31_TuneCP5_13TeV-powheg-pythia8/0000/ postWWto2L2Nu.root -1 10000 WWto2L2Nu_0 split_-1

    ${1} $MC/DiBoson/WWTo4Q_NNPDF31_TuneCP5_13TeV-powheg-pythia8/0000/ postWWto4Q.root -1 10000 WWto4Q_0 split_-1

    ${1} $MC/DiBoson/WWToLNuQQ_NNPDF31_TuneCP5_13TeV-powheg-pythia8/0000/ postWWtoLNuQQ_0.root -1 10000 WWtoLNuQQ_0 split_-1
    ${1} $MC/DiBoson/WWToLNuQQ_NNPDF31_TuneCP5_13TeV-powheg-pythia8/0001/ postWWtoLNuQQ_1.root -1 10000 WWtoLNuQQ_1 split_-1

    ${1} $MC/DiBoson/WW_TuneCP5_13TeV-pythia8/0000/ postWW.root -1 10000 WW_0 split_-1

    ${1} $MC/DiBoson/WZ_TuneCP5_13TeV-pythia8/0000/ postWZ.root -1 10000 WZ_0 split_-1

    ${1} $MC/DiBoson/ZZ_TuneCP5_13TeV-pythia8/0000/ postZZ.root -1 10000 ZZ_0 split_-1
}
#WJets
WJETS() {
    ${1} $MC/WJets/WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8/0000/ postWJets_MLM_0.root -1 10000 WMLM_0 split_-1
    ${1} $MC/WJets/WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8/0001/ postWJets_MLM_1.root -1 10000 WMLM_1 split_-1
    ${1} $MC/WJets/WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8/0002/ postWJets_MLM_2.root -1 10000 WMLM_2 split_-1
    ${1} $MC/WJets/WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8/0003/ postWJets_MLM_3.root -1 10000 WMLM_3 split_-1
    ${1} $MC/WJets/WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8/0004/ postWJets_MLM_4.root -1 10000 WMLM_4 split_-1
    ${1} $MC/WJets/WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8/0005/ postWJets_MLM_5.root -1 10000 WMLM_5 split_-1
    ${1} $MC/WJets/WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8/0006/ postWJets_MLM_6.root -1 10000 WMLM_6 split_-1
    ${1} $MC/WJets/WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8/0007/ postWJets_MLM_7.root -1 10000 WMLM_7 split_-1
    ${1} $MC/WJets/WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8/0008/ postWJets_MLM_8.root -1 10000 WMLM_8 split_-1

    ${1} $MC/WJets/WJetsToLNu_HT-100To200_TuneCP5_13TeV-madgraphMLM-pythia8/0000/ postW100to200_0.root -1 10000 W100_0 split_-1
    ${1} $MC/WJets/WJetsToLNu_HT-100To200_TuneCP5_13TeV-madgraphMLM-pythia8/0001/ postW100to200_1.root -1 10000 W100_1 split_-1
    ${1} $MC/WJets/WJetsToLNu_HT-100To200_TuneCP5_13TeV-madgraphMLM-pythia8/0002/ postW100to200_2.root -1 10000 W100_2 split_-1

    ${1} $MC/WJets/WJetsToLNu_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8/0000/ postW200to400_0.root -1 10000 W200_0 split_-1
    ${1} $MC/WJets/WJetsToLNu_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8/0001/ postW200to400_1.root -1 10000 W200_1 split_-1
    ${1} $MC/WJets/WJetsToLNu_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8/0002/ postW200to400_2.root -1 10000 W200_2 split_-1

    ${1} $MC/WJets/WJetsToLNu_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8/0000/ postW400to600_0.root -1 10000 W400_0 split_-1

    ${1} $MC/WJets/WJetsToLNu_HT-600To800_TuneCP5_13TeV-madgraphMLM-pythia8/0000/ postW600to800_0.root -1 10000 W600_0 split_-1
    ${1} $MC/WJets/WJetsToLNu_HT-600To800_TuneCP5_13TeV-madgraphMLM-pythia8/0001/ postW600to800_1.root -1 10000 W600_1 split_-1
    ${1} $MC/WJets/WJetsToLNu_HT-600To800_TuneCP5_13TeV-madgraphMLM-pythia8/0002/ postW600to800_2.root -1 10000 W600_2 split_-1

    ${1} $MC/WJets/WJetsToLNu_HT-800To1200_TuneCP5_13TeV-madgraphMLM-pythia8/0000/ postW800to1200_0.root -1 10000 W800_0 split_-1

    ${1} $MC/WJets/WJetsToLNu_HT-1200To2500_TuneCP5_13TeV-madgraphMLM-pythia8/0000/ postW1200to2500_0.root -1 10000 W1200_0 split_-1

    ${1} $MC/WJets/WJetsToLNu_HT-2500ToInf_TuneCP5_13TeV-madgraphMLM-pythia8/0000/ postW2500toInf_0.root -1 10000 W2500_0 split_-1
}
#DYJets
DYJETS() {
    ${1} $MC/DYJets/DYJetsToLL_M-50_HT-70to100_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/0000/ postDY70to100_0.root -1 10000 DY70_0 split_-1
    ${1} $MC/DYJets/DYJetsToLL_M-50_HT-70to100_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/0001/ postDY70to100_1.root -1 10000 DY70_1 split_-1

    ${1} $MC/DYJets/DYJetsToLL_M-50_HT-100to200_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/0000/ postDY100to200_0.root -1 10000 DY100_0 split_-1
    ${1} $MC/DYJets/DYJetsToLL_M-50_HT-100to200_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/0001/ postDY100to200_1.root -1 10000 DY100_1 split_-1

    ${1} $MC/DYJets/DYJetsToLL_M-50_HT-200to400_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/0000/ postDY200to400_0.root -1 10000 DY200_0 split_-1
    ${1} $MC/DYJets/DYJetsToLL_M-50_HT-200to400_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/0001/ postDY200to400_1.root -1 10000 DY200_1 split_-1

    ${1} $MC/DYJets/DYJetsToLL_M-50_HT-400to600_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/0000/ postDY400to600_0.root -1 10000 DY400_0 split_-1
    ${1} $MC/DYJets/DYJetsToLL_M-50_HT-400to600_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/0001/ postDY400to600_1.root -1 10000 DY400_1 split_-1

    ${1} $MC/DYJets/DYJetsToLL_M-50_HT-600to800_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/0000/ postDY600to800_0.root -1 10000 DY600_0 split_-1

    ${1} $MC/DYJets/DYJetsToLL_M-50_HT-800to1200_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/0000/ postDY800to1200_0.root -1 10000 DY800_0 split_-1

    ${1} $MC/DYJets/DYJetsToLL_M-50_HT-1200to2500_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/0000/ postDY1200to2500_0.root -1 10000 DY1200_0 split_-1

    ${1} $MC/DYJets/DYJetsToLL_M-50_HT-2500toInf_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/0000/ postDY2500toInf_0.root -1 10000 DY2500_0 split_-1
}

#ZJets
# ZJETS() {
#     ${1} $MC/ZJets/ZJetsToNuNu_HT-100To200_13TeV-madgraph/0000/ postZ100to200_0.root -1 10000 Z100_0 split_-1
#     ${1} $MC/ZJets/ZJetsToNuNu_HT-100To200_13TeV-madgraph/0001/ postZ100to200_1.root -1 10000 Z100_1 split_-1
#     ${1} $MC/ZJets/ZJetsToNuNu_HT-100To200_13TeV-madgraph/0002/ postZ100to200_2.root -1 10000 Z100_2 split_-1

#     ${1} $MC/ZJets/ZJetsToNuNu_HT-200To400_13TeV-madgraph/0000/ postZ200to400_0.root -1 10000 Z200_0 split_-1
#     ${1} $MC/ZJets/ZJetsToNuNu_HT-200To400_13TeV-madgraph/0001/ postZ200to400_1.root -1 10000 Z200_1 split_-1
#     ${1} $MC/ZJets/ZJetsToNuNu_HT-200To400_13TeV-madgraph/0002/ postZ200to400_2.root -1 10000 Z200_2 split_-1

#     ${1} $MC/ZJets/ZJetsToNuNu_HT-400To600_13TeV-madgraph/0000/ postZ400to600_0.root -1 10000 Z400_0 split_-1

#     ${1} $MC/ZJets/ZJetsToNuNu_HT-600To800_13TeV-madgraph/0000/ postZ600to800_0.root -1 10000 Z600_0 split_-1

#     ${1} $MC/ZJets/ZJetsToNuNu_HT-800To1200_13TeV-madgraph/0000/ postZ800to1200_0.root -1 10000 Z800_0 split_-1

#     ${1} $MC/ZJets/ZJetsToNuNu_HT-1200To2500_13TeV-madgraph/0000/ postZ1200to2500_0.root -1 10000 Z1200_0 split_-1

#     ${1} $MC/ZJets/ZJetsToNuNu_HT-2500ToInf_13TeV-madgraph/0000/ postZ2500toInf_0.root -1 10000 Z2500_0 split_-1
# }
