TEMPLATES=SigTemplate BkgTemplate DenTemplate

.PHONY: $(TEMPLATES) all

all: $(TEMPLATES)
	@#

$(TEMPLATES): 
	@$(MAKE) -C $@ $(TARGET)

%:: TARGET:=$(MAKECMDGOALS)
%:: $(TEMPLATES)
	@#
