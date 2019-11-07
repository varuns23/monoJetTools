version = "2018"

lumi = {"SingleEleCR":13955,
        "DoubleEleCR":13955,
        "SingleMuCR":59699,
        "DoubleMuCR":59699,
        "SignalRegion":3506}

lumi_by_era = {"SingleEleCR":{"A":14024,"B":7061,"C":6895,"D":31720},
               "DoubleEleCR":{"A":14024,"B":7061,"C":6895,"D":31720},
               "SingleMuCR":{"A":14024,"B":7061,"C":6895,"D":31720},
               "DoubleMuCR":{"A":14024,"B":7061,"C":6895,"D":31720},
               "SignalRegion":{"A":3506}}

WJets_FileNames = ["postW100to200","postW200to400","postW400to600","postW600to800","postW800to1200","postW2500toInf"]

ZJets_FileNames = ["postZ100to200","postZ200to400","postZ600to800","postZ800to1200","postZ1200to2500","postZ2500toInf"];

GJets_FileNames = ["postGJets40to100","postGJets100to200","postGJets200to400","postGJets400to600","postGJets600toInf"];

DYJets_FileNames = ["postDY70to100","postDY100to200","postDY200to400","postDY400to600","postDY600to800","postDY800to1200"]

TTJets_FileNames = ["postTTJetsDiLept"];#"postTTJets_MLM"

DiBoson_FileNames = ["postWW","postWWto2L2Nu","postWWto4Q","postWWtoLNuQQ","postWZ","postZZ"]#,"postWWto2L2NuPS"]

QCD_FileNames = ["postQCD100to200","postQCD200to300","postQCD300to500","postQCD500to700","postQCD700to1000","postQCD1000to1500","postQCD1500to2000","postQCD2000toInf"];

xsec = {
"postZ100to200":3.034e+02,
"postZ200to400":9.171e+01,
"postZ400to600":1.310e+01,
"postZ600to800":3.243e+00,
"postZ800to1200":1.496e+00,
"postZ1200to2500":3.425e-01,
"postZ2500toInf":5.263e-03,
    
"postWJets_MLM":5.276e+04,
"postW100to200":1.392e+03,
"postW200to400":4.103e+02,
"postW400to600":5.785e+01,
"postW600to800":1.295e+01,
"postW800to1200":5.450e+00,
"postW1200to2500":1.084e+00,
"postW2500toInf":8.047e-03,
    
"postDY70to100":1.467e+02,
"postDY100to200":1.608e+02,
"postDY200to400":4.863e+01,
"postDY400to600":6.978e+00,
"postDY600to800":1.756e+00,
"postDY800to1200":8.094e-01,
"postDY1200to2500":1.931e-01,
"postDY2500toInf":3.516e-03,
    
"postGJets40to100":1.857e+04,
"postGJets100to200":8.64e+03,
"postGJets200to400":2.194e+03,
"postGJets400to600":2.578e+02,
"postGJets600toInf":8.492e+01,
    
"postTTJetsDiLept":5.436e+01,
"postTTJetsSingleLeptFromT":1.091e+02,
# "postTTJets_MLM":7.503e+02,
    
"postQCD100to200":2.366e+07,
"postQCD200to300":1.549e+06,
"postQCD300to500":3.230e+05,
"postQCD500to700":2.996e+04,
"postQCD700to1000":6.353e+03,
"postQCD1000to1500":1.093e+03,
"postQCD1500to2000":9.935e+01,
"postQCD2000toInf":2.025e+01,
    
"postWW":7.592e+01,
"postWWtoLNuQQ":4.599e+01,
"postWWto2L2Nu":1.108e+01,
"postWWto4Q":4.773e+01,
    
"postWZ":2.758e+01,
"postZZ":1.214e+01,
}

old_xsec = {
'postZ100to200': 302.8,
'postZ200to400': 92.59,
'postZ400to600': 13.18,
'postZ600to800': 3.257,
'postZ800to1200': 1.49,
'postZ1200to2500': 0.3419,
'postZ2500toInf': 0.005146,

'postWJets_MLM': 52940.0,
'postW100to200': 1395.0,
'postW200to400': 407.9,
'postW400to600': 57.48,
'postW600to800': 12.87,
'postW800to1200': 5.366,
'postW1200to2500': 1.074,
'postW2500toInf': 0.008001,

'postDY_MLM':4895,
'postDY70to100': '/DYJetsToLL_M-50_HT-70to100_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM',
'postDY100to200': 161.1,
'postDY200to400': 48.66,
'postDY400to600': 6.968,
'postDY600to800': 1.743,
'postDY800to1200': 0.8052,
'postDY1200to2500': 0.1933,
'postDY2500toInf': 0.003468,

'postGJets40to100': 18700.0,
'postGJets100to200': 8640.0,
'postGJets200to400': 2185.0,
'postGJets400to600':132.5,
'postGJets600toInf': 85.31,

'postTTJetsDiLept': 54.23,
'postTTJetsSingleLeptFromT': 109.6,
# 'postTTJets_MLM': 722.8,

'postQCD100to200': 23700000.0,
'postQCD200to300': 1547000.0,
'postQCD300to500': 322600.0,
'postQCD500to700': 29980.0,
'postQCD700to1000': 6334.0,
'postQCD1000to1500': 1088.0,
'postQCD1500to2000': 99.11,
'postQCD2000toInf': 20.23,


'postWW': 75.8,
'postWWtoLNuQQ': 45.99,
'postWWto2L2Nu': 11.08,
'postWWto2L2NuPS': 11.08,
'postWWto4Q': 47.73,

'postWZ': 27.6,
'postZZ': 12.14
}
