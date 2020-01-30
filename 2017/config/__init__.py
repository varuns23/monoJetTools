from ROOT import TFile
import os
from mcinfo import *
from cfg_saveplot import *
from filevariants import *

proc_path = os.path.dirname(__file__)
data_pileup = TFile.Open("%s/dataPileup.root" % proc_path)
