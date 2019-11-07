regions = SignalRegion SingleEleCR DoubleEleCR SingleMuCR DoubleMuCR


all:
	@$(MAKE) -C SignalRegion
	@$(MAKE) -C SingleEleCR
	@$(MAKE) -C DoubleEleCR
	@$(MAKE) -C SingleMuCR
	@$(MAKE) -C DoubleMuCR

clean:
	@$(MAKE) -C SignalRegion clean
	@$(MAKE) -C SingleEleCR clean
	@$(MAKE) -C DoubleEleCR clean
	@$(MAKE) -C SingleMuCR clean
	@$(MAKE) -C DoubleMuCR clean

condor_clean:
	@$(MAKE) -C SignalRegion condor_clean
	@$(MAKE) -C SingleEleCR condor_clean
	@$(MAKE) -C DoubleEleCR condor_clean
	@$(MAKE) -C SingleMuCR condor_clean
	@$(MAKE) -C DoubleMuCR condor_clean

condor_submit:
	@$(MAKE) -C SignalRegion condor_submit
	@$(MAKE) -C SingleEleCR condor_submit
	@$(MAKE) -C DoubleEleCR condor_submit
	@$(MAKE) -C SingleMuCR condor_submit
	@$(MAKE) -C DoubleMuCR condor_submit

hadd:
	@$(MAKE) -C SignalRegion hadd
	@$(MAKE) -C SingleEleCR hadd
	@$(MAKE) -C DoubleEleCR hadd
	@$(MAKE) -C SingleMuCR hadd
	@$(MAKE) -C DoubleMuCR hadd
