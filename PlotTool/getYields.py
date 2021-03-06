#!/usr/bin/env python

"""
Print table of events from cutflow for each process
Usage: python PlotTool/getYields.py histnumbers #(i.e. 0 1 2 3 4 5 ...etc)
"""

from sys import argv, path
from PlotTool import *
from ROOT import *

import config
import os
           
group = parser.add_group(__file__,__doc__,"Script")
group.add_argument('--yields',help='print yield table of current region',action='store_true',default=False)
group.add_argument('--percent',help='print percentage of background table of current region',action='store_true',default=False)
group.add_argument('--efficiency',help='print cut efficiecies table of current region',action='store_true',default=False)
group.add_argument('--no-weight',help='use the none weighted cutflow',action='store_true',default=False)

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

if __name__ == "__main__":
    parser.parse_args()
    
    samples = Region(show=False)
    if parser.args.no_weight:
        samples.initiate('h_cutflowNoWt')
    else:
        samples.initiate('h_cutflow')
    hslist = []
    for name in samples.SampleList:
        process = samples.processes[name]
        if name == 'Signal': process = process[0]
        hslist.append( (name,process.histo) )
    samples.setSumOfBkg()
    if not any( 'Data' in proc[0] for proc in hslist ):
        hslist.insert(0, ('SumOfBkg',samples.processes['SumOfBkg'].histo) )
    else:
        hslist.insert(1, ('SumOfBkg',samples.processes['SumOfBkg'].histo) )

    if not any(parser.args.argv): parser.args.argv = range(int(config.regions[samples.region])+1)
    binlist = [ int(ibin) for ibin in parser.args.argv ]
    tableSet = TableSet()
    if parser.args.yields: tableSet.add(getYields(hslist,binlist))
    if parser.args.percent: tableSet.add(getPercBkg(hslist,binlist))
    if parser.args.efficiency: tableSet.add(getCutEff(hslist,binlist))
    if not parser.args.yields and not parser.args.percent and not parser.args.efficiency:
        tableSet.add(getYields(hslist,binlist))

    print tableSet
        
