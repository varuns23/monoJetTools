from CondorConfig import CondorConfig
import sys
import os

def check_config(config):
    print "Checking",config
    config = CondorConfig(config)
    inputfiles = set([ fname for fname in os.listdir(config['inputdir'].strip()) if '.root' in fname ])
    def check_input(args):
        fnames = [ arg for arg in args.split() if '.root' in arg and '$' not in arg ]
        for fname in fnames:
            if fname in inputfiles: inputfiles.remove(fname)
    for arg in config['Arguments']: check_input(arg)
    if any(inputfiles):
        print "Missing %s Input Files" % len(inputfiles)
        for fname in inputfiles: print fname
        return
    outputfiles = []
    def check_output(iarg,args):
        output = '.output/%s_%s.root' % (config['outputfile'].strip(),iarg)
        if not os.path.isfile(output): outputfiles.append(output)
    if any(outputfiles):
        print "Missing %s Output Files" % len(outputfiles)
        for output in outputfiles: print output
for config in sys.argv[1:]: check_config(config)

