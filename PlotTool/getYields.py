#!/usr/bin/env python

from ROOT import *
from sys import argv, path
from PlotTool import *
import config
import os

optionmap = {
    'raw-output':False
}
                
def getCutflowHeader(histo,binlist):
    return [ histo.GetXaxis().GetBinLabel(ibin+1) for ibin in binlist ]
    
def getYields(hslist,binlist):
    header = ['Yields']
    header += getCutflowHeader( next( histo for _,histo in hslist ),binlist )
    table = Table(header)
    data = sumofbkg = signal = None
    for name,histo in hslist:
        if name == 'Data': data = histo
        if name == 'SumOfBkg': sumofbkg = histo
        if name == 'Signal': signal = histo
        row = [name]
        row += [ histo.GetBinContent(ibin+1) for ibin in binlist ]
        table.addRow(row)

    if data != None and sumofbkg != None:
        datamc = ['Data/MC']
        for ibin in binlist:
            if sumofbkg.GetBinContent(ibin+1) != 0: datamc.append( data.GetBinContent(ibin+1)/sumofbkg.GetBinContent(ibin+1) )
            else:                                   datamc.append('NaN')
        table.addRow(datamc)
    if sumofbkg != None and signal != None:
        s_rootb = ['S/Root(B)']
        s_rootsb = ['S/Root(S+B)']
        for ibin in binlist:
            if sumofbkg.GetBinContent(ibin+1) != 0:
                  s_rootb.append( signal.GetBinContent(ibin+1)/ sumofbkg.GetBinContent(ibin+1)**0.5 )
            else: s_rootb.append('NaN')
            if sumofbkg.GetBinContent(ibin+1) != 0 and signal.GetBinContent(ibin+1) != 0:
                  s_rootsb.append( signal.GetBinContent(ibin+1)/ (signal.GetBinContent(ibin+1) + sumofbkg.GetBinContent(ibin+1))**0.5 )
            else: s_rootsb.append('NaN')
        table.addRow(s_rootb)
        table.addRow(s_rootsb)
    return table

def getPercBkg(hslist,binlist):
    header = ['% of Background']
    header += getCutflowHeader( next( histo for _,histo in hslist ),binlist )
    table = Table(header)
    sumofbkg = next( histo for name,histo in hslist if name == 'SumOfBkg' )
    for name,histo in hslist:
        if name == 'Data' or name == 'Signal' or name == 'SumOfBkg': continue
        row = [name]
        for ibin in binlist:
            if sumofbkg.GetBinContent(ibin+1) != 0:
                  row.append( '%.6g%%' % (100*histo.GetBinContent(ibin+1)/sumofbkg.GetBinContent(ibin+1)) )
            else: row.append('NaN')
        table.addRow(row)
    return table

def getCutEff(hslist,binlist):
    header = ['Cut Efficiency']
    header += getCutflowHeader( next( histo for _,histo in hslist ),binlist )
    table = Table(header)
    for name,histo in hslist:
        row = [name]
        for ibin in binlist:
            if histo.GetBinContent(ibin) != 0:
                row.append( histo.GetBinContent(ibin+1)/histo.GetBinContent(ibin) )
            else: row.append('NaN')
        table.addRow(row)
    return table

parser.add_argument('--yields',help='print yield table of current region',action='store_true',default=False)
parser.add_argument('--percent',help='print percentage of background table of current region',action='store_true',default=False)
parser.add_argument('--efficiency',help='print cut efficiecies table of current region',action='store_true',default=False)
parser.add_argument('--raw-output',help='print tables without fancy separators (perhaps better for importing to excel)',action='store_true',default=False)
parser.add_argument('--all',help='get yields for all cuts',action='store_true',default=False)
if __name__ == "__main__":
    args = parser.parse_args()
    if args.raw_output: optionmap['raw-output'] = True
    
    samples = Region(show=False)
    samples.initiate('h_cutflow')
    hslist = []
    for name in samples.SampleList:
        process = samples.processes[name]
        if name == 'Signal': process = process[0]
        hslist.append( (name,process.histo) )
    samples.setSumOfBkg()
    hslist.insert(1, ('SumOfBkg',samples.processes['SumOfBkg'].histo) )

    if args.all: args.argv = range(int(config.regions[samples.region])+1)
    binlist = [ int(ibin) for ibin in args.argv ]
    tableSet = TableSet()
    if args.yields: tableSet.add(getYields(hslist,binlist))
    if args.percent: tableSet.add(getPercBkg(hslist,binlist))
    if args.efficiency: tableSet.add(getCutEff(hslist,binlist))
    if not args.yields and not args.percent and not args.efficiency:
        tableSet.add(getYields(hslist,binlist))

    print tableSet
        
