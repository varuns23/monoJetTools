REGIONS=SignalRegion SingleEleCR SingleMuCR DoubleEleCR DoubleMuCR

.PHONY: $(REGIONS) all

all: $(REGIONS)
	@#

$(REGIONS): 
	@$(MAKE) -C $@ $(TARGET)

%:: TARGET:=$(MAKECMDGOALS)
%:: $(REGIONS)
	@#
