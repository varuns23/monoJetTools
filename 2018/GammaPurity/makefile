TEMPLATES=SigTemplate BkgTemplate

.PHONY: $(TEMPLATES) all

all: $(TEMPLATES)
	@#

$(TEMPLATES): 
	@$(MAKE) -C $@ $(TARGET)

%:: TARGET:=$(MAKECMDGOALS)
%:: $(TEMPLATES)
	@#
