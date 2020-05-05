
RFLAGS:=-I$ROOTSYS/include -L$ROOTSYS/lib `root-config --cflags` `root-config --libs` -lFoam -lMathMore -Wno-deprecated

ROOT:=../../../
YEAR:=../../
SRC:=src/
INC:=inc/
BIN:=bin/
SCRIPTS:=scripts/

CR:=monoJetGammaCR

LIBRARY:=$(addprefix $(ROOT)$(BIN),ScaleUncCollection.o ShapeUncCollection.o Utilities.o Dataset.o)
INCLUDE:=$(addprefix $(ROOT)$(INC),monoJetCutConfig.h Dataset.h VariableBins.h ScaleUncCollection.h ShapeUncCollection.h Utilities.h)
OBJECTS:=$(BIN)monoJetClass.o $(YEAR)$(BIN)monoJetYear.o $(ROOT)$(BIN)$(CR).o $(ROOT)$(BIN)monoJetAnalysis.o $(LIBRARY)
SOURCES:=$(subst $(BIN),$(SRC),$(OBJECTS:.o=.C))

.PHONY:test clean condor_clean hadd bin

analyze: $(ROOT)$(SCRIPTS)analyze.C $(OBJECTS)
	@if [ ! -d RootFiles ]; then ln -s $(YEAR)RootFiles/; fi
	@if [ ! -d PlotTool ]; then ln -s $(ROOT)PlotTool/; fi
	g++ -I$(ROOT)$(INC) -I$(YEAR)$(INC) -I$(INC) $(RFLAGS) $(WARNINGS) $< $(OBJECTS) -o $@

$(BIN)monoJetClass.o: $(SRC)monoJetClass.C $(INC)monoJetClass.h $(YEAR)$(INC)monoJetYear.h $(ROOT)$(INC)monoJetAnalysis.h $(INCLUDE)
	g++ -I$(ROOT)$(INC) -I$(YEAR)$(INC) -I$(INC) $(RFLAGS) -c $< -o $@

$(YEAR)$(BIN)monoJetYear.o: $(YEAR)$(SRC)monoJetYear.C $(YEAR)$(INC)monoJetYear.h $(ROOT)$(INC)monoJetAnalysis.h $(INCLUDE)
	g++ -I$(ROOT)$(INC) -I$(YEAR)$(INC) $(RFLAGS) -c $< -o $@

$(ROOT)$(BIN)$(CR).o: $(ROOT)$(SRC)$(CR).C $(ROOT)$(INC)$(CR).h $(ROOT)$(INC)monoJetAnalysis.h $(INCLUDE)
	g++ -I$(ROOT)$(INC) $(RFLAGS) -c $< -o $@

$(ROOT)$(BIN)monoJetAnalysis.o: $(ROOT)$(SRC)monoJetAnalysis.C $(ROOT)$(INC)monoJetAnalysis.h $(INCLUDE)
	g++ -I$(ROOT)$(INC) $(RFLAGS) -c $< -o $@

$(ROOT)$(BIN)%.o: $(ROOT)$(SRC)%.C $(ROOT)$(INC)%.h
	g++ -I$(ROOT)$(INC) $(RFLAGS) -c $< -o $@

profiler: $(ROOT)$(SCRIPTS)profiler.C $(OBJECTS)
	g++ $(PROFILER) -I$(ROOT)$(INC) -I$(YEAR)$(INC) -I$(INC) $(RFLAGS) $(WARNINGS) $< $(SOURCES) -o $@

bin:
	mkdir -p $(BIN)
	mkdir -p $(YEAR)$(BIN)
	mkdir -p $(ROOT)$(BIN)

test: analyze
	./analyze

clean:
	rm post*.root

condor_clean:
	rm -r .output/
	rm -r .status/

condor_submit: analyze
	./submit.py $(options)

hadd:
	./PlotTool/plotter.py
