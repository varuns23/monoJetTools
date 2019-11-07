
make || exit -1

echo Dark Matter Mass 1 Gev
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx1_Mv10    postMx1_Mv10.root    -1 1000 Mx1_Mv10    split_1
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx1_Mv20    postMx1_Mv20.root    -1 1000 Mx1_Mv20    split_1
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx1_Mv50    postMx1_Mv50.root    -1 1000 Mx1_Mv50    split_1
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx1_Mv100   postMx1_Mv100.root   -1 1000 Mx1_Mv100   split_1
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx1_Mv200   postMx1_Mv200.root   -1 1000 Mx1_Mv200   split_1
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx1_Mv1000  postMx1_Mv1000.root  -1 1000 Mx1_Mv1000  split_1
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx1_Mv10000 postMx1_Mv10000.root -1 1000 Mx1_Mv10000 split_1

echo Dark Matter Mass 10 GeV
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx10_Mv10                        postMx10_Mv10.root    -1 1000 Mx10_Mv10    split_1
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx10_Mv15                        postMx10_Mv15.root    -1 1000 Mx10_Mv20    split_1
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx10_Mv50_TuneCP5_13TeV_madgraph postMx10_Mv50.root    -1 1000 Mx10_Mv50    split_1
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx10_Mv100                       postMx10_Mv100.root   -1 1000 Mx10_Mv100   split_1
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx10_Mv10000                     postMx10_Mv10000.root -1 1000 Mx10_Mv10000 split_1

echo Dark Matter Mass 50 GeV
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx50_Mv50_TuneCP5_13TeV_madgraph    postMx50_Mv50.root   -1 1000 Mx50_Mv50    split_1
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx50_Mv95_TuneCP5_13TeV_madgraph    postMx50_Mv95.root   -1 1000 Mx50_Mv95    split_1
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx50_Mv200_TuneCP5_13TeV_madgraph   postMx50_Mv200.root  -1 1000 Mx50_Mv200   split_1
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx50_Mv300_TuneCP5_13TeV_madgraph   postMx50_Mv300.root  -1 1000 Mx50_Mv300   split_1
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx50_Mv10000_TuneCP5_13TeV_madgraph postMx50_Mv1000.root -1 1000 Mx50_Mv10000 split_1

echo Dark Matter Mass 150 GeV
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx150_Mv10_TuneCP5_13TeV_madgraph    postMx150_Mv10.root    -1 1000 Mx150_Mv10    split_1
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx150_Mv200_TuneCP5_13TeV_madgraph   postMx150_Mv200.root   -1 1000 Mx150_Mv200   split_1
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx150_Mv295_TuneCP5_13TeV_madgraph   postMx150_Mv295.root   -1 1000 Mx150_Mv295   split_1
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx150_Mv500_TuneCP5_13TeV_madgraph   postMx150_Mv500.root   -1 1000 Mx150_Mv500   split_1
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx150_Mv10000_TuneCP5_13TeV_madgraph postMx150_Mv10000.root -1 1000 Mx150_Mv10000 split_1

echo Dark Matter Mass 500 GeV
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx500_Mv10_TuneCP5_13TeV_madgraph    postMx500_Mv10.root    -1 1000 Mx500_Mv10    split_1
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx500_Mv500_TuneCP5_13TeV_madgraph   postMx500_Mv500.root   -1 1000 Mx500_Mv500   split_1
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx500_Mv995_TuneCP5_13TeV_madgraph   postMx500_Mv995.root   -1 1000 Mx500_Mv995   split_1
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx500_Mv10000_TuneCP5_13TeV_madgraph postMx500_Mv10000.root -1 1000 Mx500_Mv10000 split_1

echo Dark Matter Mass 1000 GeV
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx1000_Mv10_TuneCP5_13TeV_madgraph    postMx1000_Mv10.root    -1 1000 Mx1000_Mv10    split_1
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx1000_Mv1000_TuneCP5_13TeV_madgraph  postMx1000_Mv1000.root  -1 1000 Mx1000_Mv1000  split_1
python ../SubmitCondor.py analyze /hdfs/store/user/varuns/monoZprimeMC2018/MonoZprime_V_Mx1000_Mv10000_TuneCP5_13TeV_madgraph postMx1000_Mv10000.root -1 1000 Mx1000_Mv10000 split_1
