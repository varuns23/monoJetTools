from ROOT import *

version="2016"

lumi = {"SingleEleCR":35900,
        "DoubleEleCR":35900,
        "SingleMuCR":35900,
        "DoubleMuCR":35900,
        "SignalRegion":1892}

lumi_by_era = {"SingleEleCR" :{"B":5750,"C":2573,"D":4242,"E":4025,"F":3105,"G":7576,"H":8651},
               "DoubleEleCR" :{"B":5750,"C":2573,"D":4242,"E":4025,"F":3105,"G":7576,"H":8651},
               "SingleMuCR"  :{"B":5750,"C":2573,"D":4242,"E":4025,"F":3105,"G":7576,"H":8651},
               "DoubleMuCR"  :{"B":5750,"C":2573,"D":4242,"E":4025,"F":3105,"G":7576,"H":8651},
               "SignalRegion":{"F":1892}}

mclist = ["ZJets","WJets","DYJets","GJets","TTJets","DiBoson","QCD"]
filemap = {
    "ZJets":  ["postZ100to200","postZ200to400","postZ400to600","postZ600to800","postZ800to1200","postZ1200to2500","postZ2500toInf"],
    "WJets":  ["postW100to200","postW200to400","postW400to600","postW600to800","postW800to1200","postW1200to2500","postW2500toInf","postWMLM"],
    "DYJets": ["postDY100to200","postDY200to400","postDY400to600","postDY600to800","postDY800to1200","postDY1200to2500","postDY2500toInf","postDYMLM"],
    "GJets":  ["postGJets40to100","postGJets100to200","postGJets200to400","postGJets400to600","postGJets600toInf"],
    "TTJets": ["postTTJetsMLM","postST_tW_antitop_5f"],
    "DiBoson":["postWW","postWZ","postZZ"],
    "QCD":    ["postQCD100to200","postQCD200to300","postQCD300to500","postQCD500to700","postQCD700to1000","postQCD1000to1500","postQCD1500to2000","postQCD2000toInf"]
}
nlomap = {}

legmap = {
    "ZJets"  :"Z#rightarrow#nu#nu",  
    "WJets"  :"W#rightarrowl#nu",     
    "DYJets" :"Z#rightarrow ll",   
    "GJets"  :"#gamma+jets",  
    "TTJets" :"Top Quark",         
    "DiBoson":"WW/WZ/ZZ",          
    "QCD"    :"QCD"
}
colmap = {
    "ZJets"  :kAzure+10,  
    "WJets"  :kRed-10,
    "DYJets" :kTeal-9,  
    "GJets"  :kGray+2,  
    "TTJets" :kOrange-2,
    "DiBoson":kCyan-10, 
    "QCD"    :kGray
}

xsec_new = {                                                                                                                                                                    
"postDY100to200":1.473e+02,
"postDY200to400":4.104e+01,
"postDY400to600":5.676e+00,
"postDY600to800":1.358e+00,
"postDY800to1200":6.229e-01,
"postDY1200to2500":1.512e-01,
"postDY2500toInf":3.659e-03,
"postDYFXFX":5.932e+03,
"postDYMLM":4.928e+03,
    
"postGJets40to100":1.743e+04,
"postGJets100to200":5.384e+03,
"postGJets200to400":1.176e+03,
"postGJets400to600":1.321e+02,
"postGJets600toInf":4.427e+01,
    
"postQCD100to200":2.801e+07,
"postQCD200to300":1.710e+06,
"postQCD300to500":3.473e+05,
"postQCD500to700":3.220e+04,
"postQCD700to1000":6.839e+03,
"postQCD1000to1500":1.207e+03,
"postQCD1500to2000":1.201e+02,
"postQCD2000toInf":2.524e+01,
    
"postTTJetsFXFX":7.467e+02,
"postTTJetsMLM":5.093e+02,

"postST_tW_antitop_5f":3.806e+01,
    
"postW100to200":1.349e+03,
"postW200to400":3.595e+02,
"postW400to600":4.890e+01,
"postW600to800":1.206e+01,
"postW800to1200":5.477e+00,
"postW1200to2500":1.329e+00,
"postW2500toInf":3.219e-02,
"postWFXFX":6.038e+04,
"postWMLM":5.033e+04,
    
"postZ100to200":9.350e+01,
"postZ200to400":2.585e+01,
"postZ400to600":3.584e+00,
"postZ600to800":8.536e-01,
"postZ800to1200":3.927e-01,
"postZ1200to2500":9.543e-02,
"postZ2500toInf":2.304e-03,
    
"postWW":6.430e+01,
"postWZ":2.343e+01,
"postZZ":1.016e+01 
}

xsec = {
'postZ100to200': 280.5,
'postZ200to400': 77.67,
'postZ400to600': 10.73,
'postZ600to800': 2.559,
'postZ800to1200': 1.1796,
'postZ1200to2500': 0.28633,
'postZ2500toInf': 0.0006945,

'postWMLM': 50690,
'postW100to200': 1343,
'postW200to400': 359.7,
'postW400to600': 48.91,
'postW600to800': 12.05,
'postW800to1200': 5.501,
'postW1200to2500': 1.329,
'postW2500toInf': 0.03216,

'postDYMLM':4895,
'postDY100to200': 148,
'postDY200to400': 40.94,
'postDY400to600': 5.497,
'postDY600to800': 1.354,
'postDY800to1200': 0.6250,
'postDY1200to2500': 0.1511,
'postDY2500toInf': 0.003647,

'postGJets40to100': 17420,
'postGJets100to200': 5391,
'postGJets200to400': 1168,
'postGJets400to600':132.5,
'postGJets600toInf': 44.05,

'postTTJetsMLM': 831.76,

"postST_tW_antitop_5f":3.806e+01,
    
'postQCD100to200': 27500000,
'postQCD200to300': 1735000,
'postQCD300to500': 367000,
'postQCD500to700': 29370,
'postQCD700to1000': 6524,
'postQCD1000to1500': 1064,
'postQCD1500to2000': 121.5,
'postQCD2000toInf': 25.42,


'postWW': 118.7,
'postWZ': 47.2,
'postZZ': 16.6
}
