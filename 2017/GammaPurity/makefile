REGIONS=SigTemplate BkgTemplate

.PHONY: $(REGIONS) all

all: $(REGIONS)
	@#

$(REGIONS): 
	@$(MAKE) -C $@ $(TARGET)

%:: TARGET:=$(MAKECMDGOALS)
%:: $(REGIONS)
	@#
