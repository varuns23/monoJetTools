plugin=${PWD}/../plugins/

rflags = -I$ROOTSYS/include -L$ROOTSYS/lib `root-config --cflags` `root-config --libs` -lFoam -lMathMore

analyze: monoJet* $(plugin)monoJet* RootFiles PlotTool
	g++ -I $(plugin) -Wno-deprecated monoJet*.C $(plugin)*.C -o analyze $(rflags)

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
	./submitDATA.sh
	./submitMC.sh

hadd: PlotTool
	./PlotTool/plotter.py

plot:
	./PlotTool/plotter.py $(variable)
