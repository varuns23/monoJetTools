plugin=${CMSSW_BASE}/src/ZprimeTools2018/plugins/

rflags = -I$ROOTSYS/include -L$ROOTSYS/lib `root-config --cflags` `root-config --libs` -lFoam -lMathMore

analyze: ZprimeJets* $(plugin)/ZprimeJets* RootFiles PlotTool
	g++ -I $(plugin) -Wno-deprecated ZprimeJets*.C $(plugin)/*.C -o analyze $(rflags)

RootFiles:
	if [ ! -d RootFiles ]; then ln -s ../RootFiles/; fi

PlotTool:
	if [ ! -d PlotTool ]; then ln -s ../PlotTool/; fi

test: analyze
	./analyze

clean:
	rm post*.root
	rm -r DataEra

condor_clean:
	rm -r .output
	rm -r .status

condor_submit: analyze
	./signalsubmit.sh
	./submitDATA.sh
	./submitMC.sh

hadd: PlotTool
	./PlotTool/plotter.py

plot:
	./PlotTool/plotter.py $(variable)
