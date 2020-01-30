from ROOT import *

store = []
boundaries = []
def GetUsery(ndcy):
    gPad.Update(); #this is necessary!
    return ndcy*(gPad.GetY2()-gPad.GetY1()) + gPad.GetY1();
def GetUserx(ndcx):
    gPad.Update(); #this is necessary!
    return ndcx*(gPad.GetX2()-gPad.GetX1()) + gPad.GetX1();
def GetNDCy(usery):
    gPad.Update(); #this is necessary!
    return (usery - gPad.GetY1())/(gPad.GetY2()-gPad.GetY1());
def GetNDCx(userx):
    gPad.Update(); #this is necessary!
    return (userx - gPad.GetX1())/(gPad.GetX2()-gPad.GetX1());
class Box:
    def __init__(self,xmin=None,ymin=None,xmax=None,ymax=None,x=None,y=None,w=None,h=None):
        if xmin is not None: self.x1 = xmin;
        if xmax is not None: self.x2 = xmax;
        if ymin is not None: self.y1 = ymin;
        if ymax is not None: self.y2 = ymax;

        if x is not None and w is not None:
            self.x1 = x + w/2.0
            self.x2 = x - w/2.0
        if y is not None and h is not None:
            self.y1 = y + h/2.0
            self.y2 = y - h/2.0
    def userx1(self): return GetUserx(self.x1)
    def userx2(self): return GetUserx(self.x2)
    def usery1(self): return GetUsery(self.y1)
    def usery2(self): return GetUsery(self.y2)
    def userlogy1(self): return pow(10,self.usery1())
    def userlogy2(self): return pow(10,self.usery2())
    def __str__(self): return '(%f,%f) - (%f,%f)' % (self.x1,self.y1,self.x2,self.y2)

def SetYBounds(histos):
    nbins = histos[0].GetNbinsX()
    xaxis = histos[0].GetXaxis()
    min_array = [ min(hs[ibin] for hs in histos) for ibin in range(1,nbins+1) ]
    max_array = [ max(hs[ibin] for hs in histos) for ibin in range(1,nbins+1) ]
    
    for ibin,(ymin,ymax) in enumerate(zip(min_array,max_array)):
        xval = xaxis.GetBinCenter(ibin)
        for box in boundaries:
            if box.userx1() < xval and xval < box.userx2():
                tmp = ymax/box.usery1()
                scale = max(scale,tmp)

    min_y = min(min_array)
    max_y = max(max_array)
    for hs in histos:
        hs.GetYaxis().SetRangeUser(0.8*min_y,scale*max_y)
    
def DataStyle(hs_data):
    hs_data.SetLineWidth(2)
    hs_data.SetLineColor(kWhite);
    hs_data.SetTitle("");
    hs_data.GetXaxis().SetTitle("");
    hs_data.GetXaxis().SetTickLength(0);
    hs_data.GetXaxis().SetLabelOffset(999);
    hs_data.GetYaxis().SetTitle("");
    hs_data.GetYaxis().SetTickLength(0);
    hs_data.GetYaxis().SetLabelOffset(999);
    hs_data.SetLineColor(kBlack);
    hs_data.SetMarkerStyle(20);
    hs_data.SetMarkerSize(1);
###################################################################
    
def MCStyle(hs_mc,color):
    hs_mc.SetTitle("");
    hs_mc.GetXaxis().SetTitle("");
    hs_mc.GetXaxis().SetTickLength(0);
    hs_mc.GetXaxis().SetLabelOffset(999);
    # hs_mc.GetYaxis().SetTitle("");
    # hs_mc.GetYaxis().SetTickLength(0);
    # hs_mc.GetYaxis().SetLabelOffset(999);
    hs_mc.SetFillColor(color);
###################################################################

def UncBandStyle(uncband,color=kGray+1):
    uncband.SetTitle("")
    # uncband.SetFillStyle(3144)
    uncband.SetFillStyle(2001)
    uncband.SetFillColor(color)
###################################################################

def fillStack(samples,hs_datamc):
    order = [ process for name,process in samples.processes.iteritems() if process.proctype == 'bkg' ]
    if (samples.name == "Cutflow"):
        nbin = samples.processes['Data'].histo.GetNbinsX()
        order.sort(key=lambda process: process.histo.GetBinContent(nbin))
    else: order.sort(key=lambda process: process.scaled_total)
    samples.MCOrder = [ _.process for _ in reversed(order) ]
    for process in order: hs_datamc.Add(process.histo)
###################################################################

def getLegend(xmin=0.75,ymin=0.5,xmax=0.95,ymax=0.887173):
    leg = TLegend(xmin,ymin,xmax,ymax,"")
    boundaries.append( Box(xmin,ymin,xmax,ymax) )
    leg.SetFillColor(kWhite);
    leg.SetFillStyle(0);
    leg.SetTextSize(0.025);
    store.append(leg)
    return leg
###################################################################

def getCMSText(lumi,year,scale=1):
    global boundaries
    x1,y1 = 0.62,0.907173
    texS = TLatex(x1,y1,("%s (13 TeV, %s)" % (lumi,year)));#VS
    texS.SetNDC();
    texS.SetTextFont(42);
    texS.SetTextSize(0.040*scale);
    texS.Draw();
    box1 = Box(x=x1,y=y1,w=texS.GetXsize(),h=texS.GetYsize())

    x2,y2 = 0.15,0.837173
    texS1 = TLatex(x2,y2,"#bf{CMS} #it{Preliminary}");
    # texS1 = TLatex(x2,y2,"#bf{CMS} #it{Analysis in Progress}"); 
    texS1.SetNDC();
    texS1.SetTextFont(42);
    texS1.SetTextSize(0.040*scale);
    texS1.Draw();
    box2 = Box(x=x2,y=y2,w=texS1.GetXsize(),h=texS1.GetYsize())

    boundaries += [box1,box2]
    return texS,texS1
###################################################################

def RatioStyle(ratio,rymin=0.65,rymax=1.35):
    gPad.SetGridy();
    ratio.GetYaxis().SetRangeUser(rymin,rymax);
    ratio.SetStats(0);
    ratio.GetYaxis().CenterTitle();
    ratio.SetMarkerStyle(20);
    ratio.SetMarkerSize(1);
    ratio.GetYaxis().SetLabelSize(0.14);
    ratio.GetYaxis().SetTitleSize(0.12);
    ratio.GetYaxis().SetLabelFont(42);
    ratio.GetYaxis().SetTitleFont(42);
    ratio.GetYaxis().SetTitleOffset(0.25);
    ratio.GetYaxis().SetNdivisions(4);
    ratio.GetYaxis().SetTickLength(0.05);
    
    ratio.GetXaxis().SetLabelSize(0.15);
    ratio.GetXaxis().SetTitleSize(0.12);
    ratio.GetXaxis().SetLabelFont(42);
    ratio.GetXaxis().SetTitleFont(42);
    ratio.GetXaxis().SetTitleOffset(0.9);
    ratio.GetXaxis().SetTickLength(0.05);
###################################################################

def getRatioLine(xmin,xmax):
    line = TLine(xmin, 1.,xmax, 1.);
    line.SetLineStyle(8);
    line.SetLineColor(kBlack);
    store.append(line)
    return line
###################################################################

def StackStyle(hs_stack,ymin=None,ymax=None,scaleWidth=False):
    hs_stack.GetYaxis().SetTitle("Events" if not scaleWidth else "Events / GeV");
    hs_stack.GetYaxis().SetTitleOffset(1.5);
    hs_stack.SetTitle("");
    def checkbin(x,y,box):
        if x > box.userx1() and x < box.userx2() and y > box.usery1():
            # print '%f < %f < %f && %f > %f' % (box.userx1(),x,box.userx2(),y,box.usery1())
            return y/box.usery1()
        return 1;
    hs = hs_stack
    if type(hs) == THStack: hs = hs.GetStack().Last()
    scale = 1
    ymax = max( hs[ibin] for ibin in range(1,hs.GetNbinsX()+1) if hs[ibin] != 0 )
    for box in boundaries:
        for ibin in range(1,hs.GetNbinsX()+1):
            scale = max(scale,checkbin(hs.GetBinCenter(ibin),hs[ibin],box))
    ymax *= scale*pow(10,2.5)
    ymin = pow(10,-2)
    
    hs_stack.SetMaximum(ymax)
    hs_stack.SetMinimum(ymin)
###################################################################

def makeXaxis(xmin,xmax,ymin,ndiv,name=None):
    xaxis = TGaxis(xmin,ymin,xmax,ymin,xmin,xmax,ndiv);
    if name != None: xaxis.SetTitle(name);
    xaxis.SetLabelFont(42);
    xaxis.SetLabelSize(0.10);
    xaxis.SetTitleFont(42);
    xaxis.SetTitleSize(0.12);
    xaxis.SetTitleOffset(1.2);
    return xaxis
###################################################################

def XaxisCutflowStyle(xaxis,hs):
    xaxis.SetLabelOffset(-999)
    xaxis.SetTitle("");
    for i in range(1,hs.GetXaxis().GetNbins()+1):
        label = TLatex(i-0.5,xaxis.GetY1()-0.2,hs.GetXaxis().GetBinLabel(i));
	label.SetTextSize(0.065);
	label.SetTextAngle(-30.);
	label.Draw("SAME");
        store.append(label)
###################################################################

def makeYaxis(ymin,ymax,xmin,ndiv,name=None):
    if name == None: name == 'YAxis'
    yaxis = TGaxis(xmin,ymin,xmin,ymax,ymin,ymax,ndiv,"");
    yaxis.SetTitle(name);
    yaxis.SetLabelFont(42);
    yaxis.SetLabelSize(0.10);
    yaxis.SetTitleFont(42);
    yaxis.SetTitleSize(0.12);
    yaxis.SetTitleOffset(0.35);
    yaxis.SetNdivisions(4)
    yaxis.CenterTitle()
    return yaxis
###################################################################
