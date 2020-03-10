TH1F* getHisto(TDirectory* tdir,const char* process) { return (TH1F*)tdir->Get(process); }

THStack* getBkgStack(TDirectory* tdir) {
  const char* names[] = {"ZJets","WJets","GJets","DiBoson","TTJets","QCD","DYJets"};
  TList proclist;
  for (const char* name : names) proclist.Add( getHisto(tdir,name) );
  proclist.Sort();

  const char* stack_name = ( string(tdir->GetName())+"_stack" ).c_str();
  auto stack = new THStack(stack_name,stack_name);
  for (auto hs : proclist) stack->Add( (TH1F*)hs,"PFC" );
  return stack;
}

TRatioPlot* getDataMC(TDirectory* tdir) {
  auto stack = getBkgStack(tdir);
  auto data = getHisto(tdir,"data_obs");
  data->SetMarkerStyle(20);
  data->SetMarkerSize(1);
  const char* name = ( string(tdir->GetName())+"_datamc" ).c_str();
  auto ratio = new TRatioPlot(stack,data);
  ratio->SetH1DrawOpt("HIST PFC");
  ratio->SetH2DrawOpt("PEX0");
  return ratio;
}
