# Compile postAnalyzers

./rootcom postAnalyzer_mc_QCDfake_phoNum analyze_mc_QCDfake_phoNum
./rootcom postAnalyzer_data_QCDfake analyze_data_QCDfake


# Run postAnalyzers to get data and MC distributions

##-  nohup ./analyze_mc_QCDfake_phoNum /hdfs/store/user/jjbuch/ntuples_moriond17/RunIISummer16MiniAODv2-PUMoriond17/GJets_HT-40To100_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/crab_GJets_HT-40To100/170224_162416/0000/ GJets_QCDfake_phoNum_40To100.root -1 1000 >& GJets_QCDfake_phoNum_40To100.txt &
##-  nohup ./analyze_mc_QCDfake_phoNum /hdfs/store/user/jjbuch/ntuples_moriond17/RunIISummer16MiniAODv2-PUMoriond17/GJets_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/crab_GJets_HT-100To200/170224_162449/0000/ GJets_QCDfake_phoNum_100To200.root -1 1000 >& GJets_QCDfake_phoNum_100To200.txt &
##-  nohup ./analyze_mc_QCDfake_phoNum /hdfs/store/user/jjbuch/ntuples_moriond17/RunIISummer16MiniAODv2-PUMoriond17/GJets_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/crab_GJets_HT-200To400/170224_162521/0000/ GJets_QCDfake_phoNum_200To400.root -1 1000 >& GJets_QCDfake_phoNum_200To400.txt &
##-  nohup ./analyze_mc_QCDfake_phoNum /hdfs/store/user/jjbuch/ntuples_moriond17/RunIISummer16MiniAODv2-PUMoriond17/GJets_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/crab_GJets_HT-400To600/170224_162554/0000/ GJets_QCDfake_phoNum_400To600.root -1 1000 >& GJets_QCDfake_phoNum_400To600.txt &
##-  nohup ./analyze_mc_QCDfake_phoNum /hdfs/store/user/jjbuch/ntuples_moriond17/RunIISummer16MiniAODv2-PUMoriond17/GJets_HT-600ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/crab_GJets_HT-600ToInf/170224_162630/0000/ GJets_QCDfake_phoNum_600ToInf.root -1 1000 >& GJets_QCDfake_phoNum_600ToInf.txt &
##-  
##-  ./MakeCondorFiles.csh analyze_data_QCDfake /hdfs/store/user/gomber/2016ReMiniAODnutples/SinglePhoton_2016B_ver2_v2/SinglePhoton/crab_job_Singlephoton_reminiAOD_2016B_ver2_v2/170220_112756/0000/  data_QCDfake_b0000.root -1 1000 data_QCDfake_b0000
##-  ./MakeCondorFiles.csh analyze_data_QCDfake /hdfs/store/user/gomber/2016ReMiniAODnutples/SinglePhoton_2016B_ver2_v2/SinglePhoton/crab_job_Singlephoton_reminiAOD_2016B_ver2_v2/170220_112756/0001/  data_QCDfake_b0001.root -1 1000 data_QCDfake_b0001
##-  ./MakeCondorFiles.csh analyze_data_QCDfake /hdfs/store/user/gomber/2016ReMiniAODnutples/SinglePhoton_2016B_ver2_v2/SinglePhoton/crab_job_Singlephoton_reminiAOD_2016B_ver2_v2/170220_112756/0002/  data_QCDfake_b0002.root -1 1000 data_QCDfake_b0002
##-  ./MakeCondorFiles.csh analyze_data_QCDfake /hdfs/store/user/gomber/2016ReMiniAODnutples/SinglePhoton_2016B_ver2_v2/SinglePhoton/crab_job_Singlephoton_reminiAOD_2016B_ver2_v2/170220_112756/0003/  data_QCDfake_b0003.root -1 1000 data_QCDfake_b0003
##-  ./MakeCondorFiles.csh analyze_data_QCDfake /hdfs/store/user/gomber/2016ReMiniAODnutples/SinglePhoton_2016C_v1/SinglePhoton/crab_job_Singlephoton_reminiAOD_2016C_v1/170220_134811/0000/  data_QCDfake_c0000.root -1 1000 data_QCDfake_c0000
##-  ./MakeCondorFiles.csh analyze_data_QCDfake /hdfs/store/user/gomber/2016ReMiniAODnutples/SinglePhoton_2016C_v1/SinglePhoton/crab_job_Singlephoton_reminiAOD_2016C_v1/170220_134811/0001/  data_QCDfake_c0001.root -1 1000 data_QCDfake_c0001
##-  ./MakeCondorFiles.csh analyze_data_QCDfake /hdfs/store/user/gomber/2016ReMiniAODnutples/SinglePhoton_2016D_v1/SinglePhoton/crab_job_Singlephoton_reminiAOD_2016D_v1/170220_135003/0000/  data_QCDfake_d0000.root -1 1000 data_QCDfake_d0000
##-  ./MakeCondorFiles.csh analyze_data_QCDfake /hdfs/store/user/gomber/2016ReMiniAODnutples/SinglePhoton_2016D_v1/SinglePhoton/crab_job_Singlephoton_reminiAOD_2016D_v1/170220_135003/0001/  data_QCDfake_d0001.root -1 1000 data_QCDfake_d0001
##-  ./MakeCondorFiles.csh analyze_data_QCDfake /hdfs/store/user/gomber/2016ReMiniAODnutples/SinglePhoton_2016E_v1/SinglePhoton/crab_job_Singlephoton_reminiAOD_2016E_v1/170220_135129/0000/  data_QCDfake_e0000.root -1 1000 data_QCDfake_e0000
##-  ./MakeCondorFiles.csh analyze_data_QCDfake /hdfs/store/user/gomber/2016ReMiniAODnutples/SinglePhoton_2016E_v1/SinglePhoton/crab_job_Singlephoton_reminiAOD_2016E_v1/170220_135129/0001/  data_QCDfake_e0001.root -1 1000 data_QCDfake_e0001
##-  
##-  ./MakeCondorFiles.csh analyze_data_QCDfake /hdfs/store/user/gomber/2016ReMiniAODnutples/SinglePhoton_2016F_v1/SinglePhoton/crab_job_Singlephoton_reminiAOD_2016F_v1/170220_135311/0000/  data_QCDfake_f0000.root -1 1000 data_QCDfake_f0000
##-  ./MakeCondorFiles.csh analyze_data_QCDfake /hdfs/store/user/gomber/2016ReMiniAODnutples/SinglePhoton_2016F_v1/SinglePhoton/crab_job_Singlephoton_reminiAOD_2016F_v1/170220_135311/0001/  data_QCDfake_f0001.root -1 1000 data_QCDfake_f0001
##-  ./MakeCondorFiles.csh analyze_data_QCDfake /hdfs/store/user/gomber/2016ReMiniAODnutples/SinglePhoton_2016G_v1/SinglePhoton/crab_job_Singlephoton_reminiAOD_2016G_v1/170220_135510/0000/  data_QCDfake_g0000.root -1 1000 data_QCDfake_g0000
##-  ./MakeCondorFiles.csh analyze_data_QCDfake /hdfs/store/user/gomber/2016ReMiniAODnutples/SinglePhoton_2016G_v1/SinglePhoton/crab_job_Singlephoton_reminiAOD_2016G_v1/170220_135510/0001/  data_QCDfake_g0001.root -1 1000 data_QCDfake_g0001
##-  ./MakeCondorFiles.csh analyze_data_QCDfake /hdfs/store/user/gomber/2016ReMiniAODnutples/SinglePhoton_2016G_v1/SinglePhoton/crab_job_Singlephoton_reminiAOD_2016G_v1/170220_135510/0002/  data_QCDfake_g0002.root -1 1000 data_QCDfake_g0002
##-  ./MakeCondorFiles.csh analyze_data_QCDfake /hdfs/store/user/gomber/2016ReMiniAODnutples/SinglePhoton_2016H_ver2_v1/SinglePhoton/crab_job_Singlephoton_reminiAOD_2016H_ver2_v1/170220_150632/0000/  data_QCDfake_h0000.root -1 1000 data_QCDfake_h0000
##-  ./MakeCondorFiles.csh analyze_data_QCDfake /hdfs/store/user/gomber/2016ReMiniAODnutples/SinglePhoton_2016H_ver2_v1/SinglePhoton/crab_job_Singlephoton_reminiAOD_2016H_ver2_v1/170220_150632/0001/  data_QCDfake_h0001.root -1 1000 data_QCDfake_h0001
##-  ./MakeCondorFiles.csh analyze_data_QCDfake /hdfs/store/user/gomber/2016ReMiniAODnutples/SinglePhoton_2016H_ver2_v1/SinglePhoton/crab_job_Singlephoton_reminiAOD_2016H_ver2_v1/170220_150632/0002/  data_QCDfake_h0002.root -1 1000 data_QCDfake_h0002
##-  ./MakeCondorFiles.csh analyze_data_QCDfake /hdfs/store/user/gomber/2016ReMiniAODnutples/SinglePhoton_2016H_ver2_v1/SinglePhoton/crab_job_Singlephoton_reminiAOD_2016H_ver2_v1/170220_150632/0003/  data_QCDfake_h0003.root -1 1000 data_QCDfake_h0003
##-  ./MakeCondorFiles.csh analyze_data_QCDfake /hdfs/store/user/gomber/2016ReMiniAODnutples/SinglePhoton_2016H_ver3_v1/SinglePhoton/crab_job_Singlephoton_reminiAOD_2016H_ver3_v1/170220_150836/0000/  data_QCDfake_h0004.root -1 1000 data_QCDfake_h0004
##-  

# Gather distributions, perform template fits, compute the fake ratio and its systematics, and produce plots

hadd -f data_QCDfake_all.root data_QCDfake_*000*.root

root -l -q -b QCDfake_fakeratio_systematics_phopt.C
root -l -q -b QCDfake_fakeratio_denpurity_phopt.C
