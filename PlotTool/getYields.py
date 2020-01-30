#!/usr/bin/env python

from ROOT import *
from sys import argv, path
from PlotTool import *
import os

optionmap = {
    'raw-output':False
}

class Table:
    def __init__(self,header):
        # header = [SampleNames,Column1,Column2,...]
        self.header = header
        self.table = []
        self.nspaces = [ len(h) for h in header ]
        self.num_temp = '%.6g'
    def addRow(self,row):
        if len(row) != len(self.header):
            print 'Current header length %i' % len(self.header)
            print 'Input row has length  %i' % len(row)
            return
        for i,c in enumerate(row):
            v = c if type(c) == str else self.num_temp % c
            self.nspaces[i] = max( self.nspaces[i],len(v) )
        self.table.append(row)
        
    def __str__(self):
        str_table = []
        for row in self.table:
            str_row = []
            for v in row:
                if type(v) == str: str_row.append(v)
                else:              str_row.append( self.num_temp % v )
            str_table.append(str_row)
        def RowStr(row,sep,raw=False):
            rowstr = []
            for i,col in enumerate(row):
                if not raw:rowstr.append( col.center(self.nspaces[i]) )
                else: rowstr.append(col)
            return sep.join(rowstr)
        def RowSep(char): return char*( sum(self.nspaces)+3*len(self.nspaces) )
        lines = [ ]
        if optionmap['raw-output']:
            rowdivide = ' '
            coldivide = '|'
        else:
            rowdivide = '-'
            coldivide = ' | '
            lines.append( RowSep('=') )
            
        rowsep = RowSep(rowdivide)
        lines.append(RowStr(self.header,coldivide,raw=optionmap['raw-output']))
        if not optionmap['raw-output']: lines.append( rowsep )
        for row in str_table:
            lines.append( RowStr(row,coldivide,raw=optionmap['raw-output']) )
            if not optionmap['raw-output']: lines.append( rowsep )
        return '\n'.join(lines)

class TableSet:
    tablelist = []
    nspaces = []
    def add(self,table): self.tablelist.append(table)
    def __str__(self):
        for table in self.tablelist:
            for i,nspace in enumerate(table.nspaces):
                if len(self.nspaces) <= i: self.nspaces.append(nspace)
                self.nspaces[i] = max( self.nspaces[i],nspace )
        for table in self.tablelist:
            table.onspace = [ nspace for nspace in table.nspaces ]
            for i,nspace in enumerate(table.nspaces):
                table.nspaces[i] = self.nspaces[i]
        tablestr = [ str(table) for table in self.tablelist ]
        return '\n'.join(tablestr)
                
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
if __name__ == "__main__":
    args = parser.parse_args()
    if args.raw_output: optionmap['raw-output'] = True
    binlist = [ int(ibin) for ibin in args.argv ]
    
    samples = Region(show=False)
    samples.initiate('h_cutflow')
    hslist = []
    for name in samples.SampleList:
        process = samples.processes[name]
        if name == 'Signal': process = process[0]
        hslist.append( (name,process.histo) )
    samples.setSumOfBkg()
    hslist.insert(1, ('SumOfBkg',samples.processes['SumOfBkg'].histo) )

    tableSet = TableSet()
    if args.yields: tableSet.add(getYields(hslist,binlist))
    if args.percent: tableSet.add(getPercBkg(hslist,binlist))
    if args.efficiency: tableSet.add(getCutEff(hslist,binlist))
    if not args.yields and not args.percent and not args.efficiency:
        tableSet.add(getYields(hslist,binlist))

    print tableSet
        
