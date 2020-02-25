from Parser import parser
import os

basepath = "/afs/hep.wisc.edu/home/ekoenig4/public_html/MonoJet/"

parser.add_argument("--path",help="Specify directory to save file in",default=basepath)
parser.add_argument("--sub",help="Specify subdirectory from path directory to save file in")

def GetOutDir(path,year,region,sub):
    args = parser.parse_args()
    if path is None: path = args.path
    if year is not None: path = os.path.join(path,'Plots'+year)
    if region is not None: path = os.path.join(path,region+'Plots_EWK')
    if sub is not None: path = os.path.join(path,sub)
    if args.sub is not None: path = os.path.join(path,args.sub)
    return path
def SaveAs(c,name,path=None,year=None,region=None,sub=None,exts='.png'):
    path = GetOutDir(path,year,region,sub)
    if not os.path.isdir(path):
        os.makedirs(path,0755)
        print 'Making %s' % path
    if not any( type(exts) == l for l in (list,tuple) ): exts = [exts]
    for ext in exts: c.SaveAs('%s/%s%s' % (path,name,ext))
