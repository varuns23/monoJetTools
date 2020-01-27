#!/usr/bin/env python
import os
import datetime
from argparse import ArgumentParser

hdfs_base = '/hdfs/store/user/ekoenig/MonoJet/PostFiles/'

valid_years = ['2016','2017','2018']
valid_regions = ['SignalRegion','SingleEleCR','SingleMuCR','DoubleEleCR','DoubleMuCR','GammaCR']

options = {
  "confirm":True
}

def confirm_all(): options["confirm"] = True; return option["confirm"]    
def valid_ans(ans): return ans == 'y' or ans == 'Y'
def valid_hdfs(directory):
    if not os.path.isdir(hdfs_base): raise ValueError("%s is not a valid hdfs base" % hdfs_base)
    subdirectories = directory.split('/')
    def build_directory(directory):
        if not os.path.isdir(directory):
            if options['confirm']: os.mkdir(directory); return
            ans = raw_input("%s not found, do you want to make it (y/Y)? " % directory)
            if valid_ans(ans): os.mkdir(directory)
            else: print 'Exiting'; exit()
    current = hdfs_base
    for subdirectory in subdirectories:
        current = os.path.join(current,subdirectory)
        build_directory(current)
    return current
def valid_year(year):
    if year not in valid_years: raise ValueError("%s is not a valid year" % year)
    return year
def valid_region(region):
    if region not in valid_regions: raise ValueError("%s is not a valid region" % region)
    return region
def valid_file(fname):
    if 'post' in fname and fname.endswith('.root'): return fname
    return None
def valid_files(fname):
    if valid_file(fname) is None: raise ValueError("%s is not a valid file" % fname)
    return fname
def valid_directory(directory):
    if any( fname for fname in os.listdir(directory) if valid_file(fname) is not None ):
        ans = raw_input("%s has root files already, do you want to overwrite it (y/Y)? " % directory)
        return valid_ans(ans)
    return True
def autodirectory(args):
    from datetime import datetime
    directory = []
    date,time = str(datetime.now()).split()
    args.date = date.replace('-','')
    from PlotTool import GetRegion
    if args.year is None:
        from config import version
        args.year = version
        directory.append(args.year)
    if args.region is None:
        args.region = GetRegion()
        if args.region is not None: directory.append(args.region)
        else:
            region = os.path.basename(os.getcwd())
            ans = raw_input("%s is not a recognized region, do you want to save to this region (y/Y)? " % region)
            if not valid_ans(ans): exit()
            directory.append(region)
    label = '%s_%s' % (args.date,args.label)
    directory.append(label)
    directory = '/'.join(directory)
    args.directory = valid_hdfs(directory)
def getargs():
    parser = ArgumentParser()
    parser.add_argument("-d","--directory",help="Specify directory in %s to save files" % hdfs_base,type=valid_hdfs)
    parser.add_argument("-y","--year",help="Specify the year of the files",type=valid_year)
    parser.add_argument("-r","--region",help="Specify the region of the files",type=valid_region)
    parser.add_argument("-l","--label",help="Specify extra information to be used in autodirectory",type=str,default='postFiles')
    parser.add_argument("-f","--files",help="Specify the files to move to specified hdfs area",nargs='+',type=valid_files,default=[ fname for fname in os.listdir('.') if valid_file(fname) is not None ])
    args = parser.parse_args()
    if args.directory is None: autodirectory(args)
    if not valid_directory(args.directory): exit()
    return args

def move_file(fname,directory):
    command = ['cp','-v',fname,directory]
    os.system(' '.join(command))

def main():
    args = getargs()
    for fname in args.files: move_file(fname,args.directory)

if __name__ == "__main__": main()
