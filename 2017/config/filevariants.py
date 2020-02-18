filevariants = {
    "postTTJetsFXFX":["postTTJetsDiLept"]
}

for era in ("B","C","D","E","D"):
    filevariants["postSingleEle_SE_%s" % era] = ["postSingleEle_%s" % era]
    filevariants["postDoubleEle_SE_%s" % era] = ["postDoubleEle_%s" % era]
