REGIONS=SignalRegion SingleEleCR SingleMuCR DoubleEleCR DoubleMuCR GammaCR GammaPurity/QCDTemplate

.PHONY: $(REGIONS) all

all: $(REGIONS)
	@#

$(REGIONS): 
	@$(MAKE) -C $@ $(TARGET)

%:: TARGET:=$(MAKECMDGOALS)
%:: $(REGIONS)
	@#
