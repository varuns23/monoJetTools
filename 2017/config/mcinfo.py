from ROOT import *

version="2017"

lumi = {"SingleEleCR":41486, #23192,
        "DoubleEleCR":41486, #23192,
        "SingleMuCR":41486,
        "DoubleMuCR":41486,
        "GammaCR":41486,
        "SignalRegion":41486/5.
        # "SignalRegion":1198
}

lumi_by_era = {"SingleEleCR":{"B":4793,"C":9631,"D":4248,"E":9314,"F":13498},
               "DoubleEleCR":{"B":4793,"C":9631,"D":4248,"E":9314,"F":13498},
               "SingleMuCR":{"B":4793,"C":9633,"D":4248,"E":9314,"F":13498},
               "DoubleMuCR":{"B":4793,"C":9633,"D":4248,"E":9314,"F":13498},
               "GammaCR":{"B":4793,"C":9633,"D":4248,"E":9314,"F":13498},
               "SignalRegion":{"B":4793/5.,"C":9633/5.,"D":4248/5.,"E":9314/5.,"F":13498/5.},
               # "SignalRegion":{"B":1198}
}

region_masks = {}

mclist = ["ZJets","WJets","DYJets","GJets","TTJets","DiBoson","QCD"]
nlomap = {"ZJets":"ZJets-NLO",
          "WJets":"WJets-NLO",
          "DYJets":"DYJets-NLO"}
filemap = {
    "ZJets":     ["postZ100to200","postZ200to400","postZ400to600","postZ600to800","postZ800to1200","postZ1200to2500","postZ2500toInf"],
    "ZJets-NLO": ["postZNLO1j50to150","postZNLO1j150to250","postZNLO1j250to400","postZNLO1j400toInf",
                  "postZNLO2j50to150","postZNLO2j150to250","postZNLO2j250to400","postZNLO2j400toInf"],
    "WJets":     ["postW100to200","postW200to400","postW400to600","postW600to800","postW800to1200","postW1200to2500","postW2500toInf","postWIncl"],
    "WJets-NLO": ["postWNLO1j0to50","postWNLO1j50to150","postWNLO1j100to150","postWNLO1j150to250","postWNLO1j250to400","postWNLO1j400toInf",
                  "postWNLO2j0to50","postWNLO2j50to150","postWNLO2j100to150","postWNLO2j150to250","postWNLO2j250to400","postWNLO2j400toInf"],
    "DYJets":    ["postDY70to100","postDY100to200","postDY200to400","postDY400to600","postDY600to800","postDY800to1200","postDY1200to2500","postDY2500toInf"],
    "DYJets-NLO":["postDYNLO1j50to150","postDYNLO1j150to250","postDYNLO1j250to400","postDYNLO1j400toInf",
                  "postDYNLO2j50to150","postDYNLO2j150to250","postDYNLO2j250to400","postDYNLO2j400toInf"],
    "GJets":     ["postGJets40to100","postGJets100to200","postGJets200to400","postGJets400to600","postGJets600toInf"],
    "TTJets":    ["postTTJetsFXFX","postST_s_4f","postST_t_antitop_4f","postST_t_top_4f","postST_tW_antitop_5f","postST_tW_top_5f"],
    "DiBoson":   ["postWW","postWWTo4Q","postWWToLNuQQ","postWZ","postZZ","postWWTo2L2Nu"],
    "QCD":       ["postQCD100to200","postQCD200to300","postQCD300to500","postQCD500to700","postQCD700to1000","postQCD1000to1500","postQCD1500to2000","postQCD2000toInf"]
}
legmap = {
    "ZJets"     :"Z#rightarrow#nu#nu",
    "ZJets-NLO" :"Z#rightarrow#nu#nu (NLO)",
    "WJets"     :"W#rightarrowl#nu",
    "WJets-NLO" :"W#rightarrowl#nu (NLO)",
    "DYJets"    :"Z#rightarrow ll",
    "DYJets-NLO":"Z#rightarrow ll (NLO)",
    "GJets"     :"#gamma+jets",  
    "TTJets"    :"Top Quark",         
    "DiBoson"   :"WW/WZ/ZZ",          
    "QCD"       :"QCD"
}
colmap = {
    "ZJets"     :kAzure+10,
    "ZJets-NLO" :kAzure+10,
    "WJets"     :kRed-10,
    "WJets-NLO" :kRed-10,
    "DYJets"    :kTeal-9,
    "DYJets-NLO":kTeal-9,
    "GJets"     :kGray,  
    "TTJets"    :kOrange-2,
    "DiBoson"   :kCyan-10, 
    "QCD"       :kGray+2
}

xsec = {
"postDYNLO1j50to150":3.138e+02,
"postDYNLO1j150to250":9.528e+00,
"postDYNLO1j250to400":1.091e+00,
"postDYNLO1j400toInf":1.209e-01,

"postDYNLO2j50to150":1.703e+02,
"postDYNLO2j150to250":1.548e+01,
"postDYNLO2j250to400":2.724e+00,
"postDYNLO2j400toInf":4.487e-01,
    
"postDY70to100":1.470e+02,
"postDY100to200":1.610e+02,
"postDY200to400":4.858e+01,
"postDY400to600":6.983e+00,
"postDY600to800":1.747e+00,
"postDY800to1200":8.052e-01,
"postDY1200to2500":1.927e-01,
"postDY2500toInf":3.478e-03,
    
"postGJets40to100":1.862e+04,
"postGJets100to200":8.622e+03,
"postGJets200to400":2.193e+03,
"postGJets400to600":2.585e+02,
"postGJets600toInf":8.521e+01,
    
"postQCD100to200":2.368e+07,
"postQCD200to300":1.556e+06,
"postQCD300to500":3.236e+05,
"postQCD500to700":2.995e+04,
"postQCD700to1000":6.351e+03,
"postQCD1000to1500":1.094e+03,
"postQCD1500to2000":9.899e+01,
"postQCD2000toInf":2.023e+01,
    
"postST_s_4f":3.740e+00,
"postST_t_antitop_4f":6.791e+01,
"postST_t_top_4f":1.133e+02,
"postST_tW_antitop_5f":3.497e+01,
"postST_tW_top_5f":3.491e+01,
    
"postTTJetsFXFX":7.268e+02,
"postTTJetsMLM":4.949e+02,
    
"postWNLO1j0to50":0.000e+00,
"postWNLO1j50to150":2.661e+03,
"postWNLO1j100to150":2.861e+02,
"postWNLO1j150to250":7.19e+01,
"postWNLO1j250to400":8.05e+00,
"postWNLO1j400toInf":8.85e-01,
    
"postWNLO2j0to50":0.000e+00,
"postWNLO2j50to150":2.661e+03,
"postWNLO2j100to150":2.777e+03,
"postWNLO2j150to250":1.509e+03,
"postWNLO2j250to400":1.867e+01,
"postWNLO2j400toInf":3.037e+00,
    
"postW70to100":1.294e+03,
"postW100to200":1.395e+03,
"postW200to400":4.093e+02,
"postW400to600":5.791e+01,
"postW600to800":1.293e+01,
"postW800to1200":5.395e+00,
"postW1200to2500":1.080e+00,
"postW2500toInf":8.053e-03,
"postWIncl":5.280e+04,
    

"postZNLO1j50to150":5.964e+02,
"postZNLO1j150to250":1.798e+01,
"postZNLO1j250to400":2.057e+00,
"postZNLO1j400toInf":2.240e-01,

"postZNLO2j50to150":3.258e+02,
"postZNLO2j150to250":2.976e+01,
"postZNLO2j250to400":5.164e+00,
"postZNLO2j400toInf":8.457e-01,
    
"postZ100to200":3.045e+02,
"postZ200to400":9.185e+01,
"postZ400to600":1.311e+01,
"postZ600to800":3.257e+00,
"postZ800to1200":1.499e+00,
"postZ1200to2500":3.430e-01,
"postZ2500toInf":5.146e-03,
    
"postWW":7.590e+01,
"postWWToLNuQQ":4.599e+01,
"postWWTo2L2Nu":1.108e+01,
"postWWTo4Q":4.773e+01,
"postWZ":2.757e+01,
"postZZ":1.214e+01 
}

old_xsec = {
"postZ100to200":3.045e+02,
"postZ200to400":9.185e+01,
"postZ400to600":1.311e+01,
"postZ600to800":3.257e+00,
"postZ800to1200":1.499e+00,
"postZ1200to2500":3.430e-01,
"postZ2500toInf":5.146e-03,

"postZNLO1j50to150":5.964e+02,
"postZNLO1j150to250":1.798e+01,
"postZNLO1j250to400":2.057e+00,
"postZNLO1j400toInf":2.240e-01,

"postZNLO2j50to150":3.258e+02,
"postZNLO2j150to250":2.976e+01,
"postZNLO2j250to400":5.164e+00,
"postZNLO2j400toInf":8.457e-01,
    
"postWIncl":5.279e+04,
"postW70to100":0, #TODO: Calculate XSEC
"postW100to200":1.395e+03,
"postW200to400":4.093e+02,
"postW400to600":5.791e+01,
"postW600to800":1.293e+01,
"postW800to1200":5.395e+00,
"postW1200to2500":1.080e+00,
"postW2500toInf":8.053e-03,

"postWNLO1j0to50":0.000e+00,
"postWNLO1j50to150":2.661e+03,
"postWNLO1j100to150":2.861e+02,
"postWNLO1j150to250":7.19e+01,
"postWNLO1j250to400":8.05e+00,
"postWNLO1j400toInf":8.85e-01,
    
"postWNLO2j0to50":0.000e+00,
"postWNLO2j50to150":2.661e+03,
"postWNLO2j100to150":2.777e+03,
"postWNLO2j150to250":1.509e+03,
"postWNLO2j250to400":1.867e+01,
"postWNLO2j400toInf":3.037e+00,
    
"postDY70to100":1.471e+02,
"postDY100to200":1.610e+02,
"postDY200to400":4.858e+01,
"postDY400to600":6.982e+00,
"postDY600to800":1.747e+00,
"postDY800to1200":8.052e-01,
"postDY1200to2500":1.927e-01,
"postDY2500toInf":3.478e-03,

"postDYNLO1j50to150":3.138e+02,
"postDYNLO1j150to250":9.528e+00,
"postDYNLO1j250to400":1.091e+00,
"postDYNLO1j400toInf":1.209e-01,

"postDYNLO2j50to150":1.703e+02,
"postDYNLO2j150to250":1.548e+01,
"postDYNLO2j250to400":2.724e+00,
"postDYNLO2j400toInf":4.487e-01,
    
"postGJets40to100":1.862e+04,
"postGJets100to200":8.622e+03,
"postGJets200to400":2.193e+03,
"postGJets400to600":2.585e+02,
"postGJets600toInf":8.521e+01,
    
"postTTJetsDiLept":5.424e+01,
"postTTJetsFXFX":7.228e+02,

"postST_s_4f":0, #TODO: Calculate XSEC
"postST_tW_antitop_5f":3.806e+01,
"postST_tW_top_5f":0,
"postST_t_antitop_4f":0,
"postST_t_top_4f":0,
    
"postQCD100to200":2.369e+07,
"postQCD200to300":1.556e+06,
"postQCD300to500":3.236e+05,
"postQCD500to700":2.999e+04,
"postQCD700to1000":6.351e+03,
"postQCD1000to1500":1.094e+03,
"postQCD1500to2000":9.901e+01,
"postQCD2000toInf":2.023e+01,
    
"postWW":7.590e+01,
"postWWToLNuQQ":4.599e+01,
"postWWTo2L2Nu":1.108e+01,
"postWWTo4Q":4.773e+01,
    
"postWZ":2.757e+01,
"postZZ":1.214e+01
}

