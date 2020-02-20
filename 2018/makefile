REGIONS=SingleEleCR SingleMuCR DoubleEleCR DoubleMuCR GammaCR

.PHONY: $(REGIONS) all

all: $(REGIONS)
	@#

$(REGIONS): 
	@$(MAKE) -C $@ $(TARGET)

%:: TARGET:=$(MAKECMDGOALS)
%:: $(REGIONS)
	@#
