DATA1=/hdfs/store/user/varuns/Run2018_17Sep2018_May2019
DATA2=/hdfs/store/user/varuns/Run2018D_Prompt

MET_A() {
    ${1} $DATA1/MET/crab_job_MET2018A/190511_011526/0000/ post${2}_0_0.root -1 10000 ${3}_0_0 split_-1
    ${1} $DATA1/MET/crab_job_MET2018A/190511_011526/0001/ post${2}_0_1.root -1 10000 ${3}_0_1 split_-1
    ${1} $DATA1/MET/crab_job_MET2018A/190511_011526/0002/ post${2}_0_2.root -1 10000 ${3}_0_2 split_-1
    ${1} $DATA1/MET/crab_job_MET2018A/190511_011526/0003/ post${2}_0_3.root -1 10000 ${3}_0_3 split_-1
    ${1} $DATA1/MET/crab_job_MET2018A/190511_011526/0004/ post${2}_0_4.root -1 10000 ${3}_0_4 split_-1
    ${1} $DATA1/MET/crab_job_MET2018A/190511_011526/0005/ post${2}_0_5.root -1 10000 ${3}_0_5 split_-1
    ${1} $DATA1/MET/crab_job_MET2018A/190511_011526/0006/ post${2}_0_6.root -1 10000 ${3}_0_6 split_-1
    ${1} $DATA1/MET/crab_job_MET2018A/190511_011526/0007/ post${2}_0_7.root -1 10000 ${3}_0_7 split_-1
    ${1} $DATA1/MET/crab_job_MET2018A/190511_011526/0008/ post${2}_0_8.root -1 10000 ${3}_0_8 split_-1
    ${1} $DATA1/MET/crab_job_MET2018A/190511_011526/0009/ post${2}_0_9.root -1 10000 ${3}_0_9 split_-1
}
MET_B() {
    ${1} $DATA1/MET/crab_job_MET2018B/190511_011551/0000/ post${2}_1_0.root -1 10000 ${3}_1_0 split_-1
    ${1} $DATA1/MET/crab_job_MET2018B/190511_011551/0001/ post${2}_1_1.root -1 10000 ${3}_1_1 split_-1
    ${1} $DATA1/MET/crab_job_MET2018B/190511_011551/0002/ post${2}_1_2.root -1 10000 ${3}_1_2 split_-1
    ${1} $DATA1/MET/crab_job_MET2018B/190511_011551/0003/ post${2}_1_3.root -1 10000 ${3}_1_3 split_-1
    ${1} $DATA1/MET/crab_job_MET2018B/190511_011551/0004/ post${2}_1_4.root -1 10000 ${3}_1_4 split_-1
}
MET_C() {
    ${1} $DATA1/MET/crab_job_MET2018C/190511_011627/0000/ post${2}_2_0.root -1 10000 ${3}_2_0 split_-1
    ${1} $DATA1/MET/crab_job_MET2018C/190511_011627/0001/ post${2}_2_1.root -1 10000 ${3}_2_1 split_-1
    ${1} $DATA1/MET/crab_job_MET2018C/190511_011627/0002/ post${2}_2_2.root -1 10000 ${3}_2_2 split_-1
    ${1} $DATA1/MET/crab_job_MET2018C/190511_011627/0003/ post${2}_2_3.root -1 10000 ${3}_2_3 split_-1
    ${1} $DATA1/MET/crab_job_MET2018C/190511_011627/0004/ post${2}_2_4.root -1 10000 ${3}_2_4 split_-1
}
MET_D() {
    ${1} $DATA2/MET/crab_job_MET2018Dv2/190512_164040/0000/ post${2}_3_0.root -1 10000 ${3}_3_0 split_-1
    ${1} $DATA2/MET/crab_job_MET2018Dv2/190512_164040/0001/ post${2}_3_1.root -1 10000 ${3}_3_1 split_-1
    ${1} $DATA2/MET/crab_job_MET2018Dv2/190512_164040/0002/ post${2}_3_2.root -1 10000 ${3}_3_2 split_-1
    ${1} $DATA2/MET/crab_job_MET2018Dv2/190512_164040/0003/ post${2}_3_3.root -1 10000 ${3}_3_3 split_-1
    ${1} $DATA2/MET/crab_job_MET2018Dv2/190512_164040/0004/ post${2}_3_4.root -1 10000 ${3}_3_4 split_-1
    ${1} $DATA2/MET/crab_job_MET2018Dv2/190512_164040/0005/ post${2}_3_5.root -1 10000 ${3}_3_5 split_-1
    ${1} $DATA2/MET/crab_job_MET2018Dv2/190512_164040/0006/ post${2}_3_6.root -1 10000 ${3}_3_6 split_-1
    ${1} $DATA2/MET/crab_job_MET2018Dv2/190512_164040/0007/ post${2}_3_7.root -1 10000 ${3}_3_7 split_-1
    ${1} $DATA2/MET/crab_job_MET2018Dv2/190512_164040/0008/ post${2}_3_8.root -1 10000 ${3}_3_8 split_-1
    ${1} $DATA2/MET/crab_job_MET2018Dv2_2/190516_203151/0000/ post${2}_3_9.root -1 10000 ${3}_3_9 split_-1
    ${1} $DATA2/MET/crab_job_MET2018Dv2_2/190516_203151/0001/ post${2}_3_10.root -1 10000 ${3}_3_10 split_-1
    ${1} $DATA2/MET/crab_job_MET2018Dv2_2/190516_203151/0002/ post${2}_3_11.root -1 10000 ${3}_3_11 split_-1
    ${1} $DATA2/MET/crab_job_MET2018Dv2_2/190516_203151/0003/ post${2}_3_12.root -1 10000 ${3}_3_12 split_-1
    ${1} $DATA2/MET/crab_job_MET2018Dv2_2/190516_203151/0004/ post${2}_3_13.root -1 10000 ${3}_3_13 split_-1
    ${1} $DATA2/MET/crab_job_MET2018Dv2_2/190516_203151/0005/ post${2}_3_14.root -1 10000 ${3}_3_14 split_-1
    ${1} $DATA2/MET/crab_job_MET2018Dv2_2/190516_203151/0006/ post${2}_3_15.root -1 10000 ${3}_3_15 split_-1
    ${1} $DATA2/MET/crab_job_MET2018Dv2_2/190516_203151/0007/ post${2}_3_16.root -1 10000 ${3}_3_16 split_-1
    ${1} $DATA2/MET/crab_job_MET2018Dv2_2/190516_203151/0008/ post${2}_3_17.root -1 10000 ${3}_3_17 split_-1
    ${1} $DATA2/MET/crab_job_MET2018Dv2_2/190516_203151/0009/ post${2}_3_18.root -1 10000 ${3}_3_18 split_-1
    ${1} $DATA2/MET/crab_job_MET2018Dv2_3/190519_232032/0000/ post${2}_3_19.root -1 10000 ${3}_3_19 split_-1
}

ELE_B() {
    ${1} $DATA1/EGamma/crab_job_EGamma2018B/190528_233825/0000/ post${2}_1_0.root -1 10000 ${3}_1_0 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018B/190528_233825/0001/ post${2}_1_1.root -1 10000 ${3}_1_1 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018B/190528_233825/0002/ post${2}_1_2.root -1 10000 ${3}_1_2 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018B/190528_233825/0003/ post${2}_1_3.root -1 10000 ${3}_1_3 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018B/190528_233825/0004/ post${2}_1_4.root -1 10000 ${3}_1_4 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Bv2/190529_100508/0000/ post${2}_1_5.root -1 10000 ${3}_1_5 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Bv2/190529_100508/0001/ post${2}_1_6.root -1 10000 ${3}_1_6 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Bv2/190529_100508/0002/ post${2}_1_7.root -1 10000 ${3}_1_7 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Bv2/190529_100508/0003/ post${2}_1_8.root -1 10000 ${3}_1_8 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Bv2/190529_100508/0004/ post${2}_1_9.root -1 10000 ${3}_1_9 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Bv2/190529_100508/0005/ post${2}_1_10.root -1 10000 ${3}_1_10 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Bv2/190529_100508/0006/ post${2}_1_11.root -1 10000 ${3}_1_11 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Bv2/190529_100508/0007/ post${2}_1_12.root -1 10000 ${3}_1_12 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Bv2/190529_100508/0008/ post${2}_1_13.root -1 10000 ${3}_1_13 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Bv3/190602_052700/0000/ post${2}_1_14.root -1 10000 ${3}_1_14 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Bv3/190602_052700/0001/ post${2}_1_15.root -1 10000 ${3}_1_15 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Bv3/190602_052700/0002/ post${2}_1_16.root -1 10000 ${3}_1_16 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Bv3/190602_052700/0003/ post${2}_1_17.root -1 10000 ${3}_1_17 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Bv3/190602_052700/0004/ post${2}_1_18.root -1 10000 ${3}_1_18 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Bv3/190602_052700/0005/ post${2}_1_19.root -1 10000 ${3}_1_19 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Bv3/190602_052700/0006/ post${2}_1_20.root -1 10000 ${3}_1_20 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Bv3/190602_052700/0007/ post${2}_1_21.root -1 10000 ${3}_1_21 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Bv3/190602_052700/0008/ post${2}_1_22.root -1 10000 ${3}_1_22 split_-1
}
ELE_C() {
    ${1} $DATA1/EGamma/crab_job_EGamma2018C/190528_233953/0000/ post${2}_2_0.root -1 10000 ${3}_2_0 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018C/190528_233953/0001/ post${2}_2_1.root -1 10000 ${3}_2_1 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018C/190528_233953/0002/ post${2}_2_2.root -1 10000 ${3}_2_2 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018C/190528_233953/0003/ post${2}_2_3.root -1 10000 ${3}_2_3 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018C/190528_233953/0004/ post${2}_2_4.root -1 10000 ${3}_2_4 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Cv2/190529_100921/0000/ post${2}_2_5.root -1 10000 ${3}_2_5 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Cv2/190529_100921/0001/ post${2}_2_6.root -1 10000 ${3}_2_6 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Cv2/190529_100921/0002/ post${2}_2_7.root -1 10000 ${3}_2_7 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Cv2/190529_100921/0003/ post${2}_2_8.root -1 10000 ${3}_2_8 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Cv2/190529_100921/0004/ post${2}_2_9.root -1 10000 ${3}_2_9 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Cv2/190529_100921/0005/ post${2}_2_10.root -1 10000 ${3}_2_10 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Cv2/190529_100921/0006/ post${2}_2_11.root -1 10000 ${3}_2_11 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Cv2/190529_100921/0007/ post${2}_2_12.root -1 10000 ${3}_2_12 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Cv3/190530_145804/0000/ post${2}_2_13.root -1 10000 ${3}_2_13 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Cv3/190530_145804/0001/ post${2}_2_14.root -1 10000 ${3}_2_14 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Cv3/190530_145804/0002/ post${2}_2_15.root -1 10000 ${3}_2_15 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Cv3/190530_145804/0003/ post${2}_2_16.root -1 10000 ${3}_2_16 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Cv3/190530_145804/0004/ post${2}_2_17.root -1 10000 ${3}_2_17 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Cv4/190604_144802/0000/ post${2}_2_18.root -1 10000 ${3}_2_18 split_-1
    ${1} $DATA1/EGamma/crab_job_EGamma2018Cv4/190604_144802/0001/ post${2}_2_19.root -1 10000 ${3}_2_19 split_-1
}
