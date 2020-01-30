
# sh PlotTool/getANplots.sh -s Mx1_Mv1000 || exit
for y in $YEARS; do
    pushd $y
    # sh PlotTool/getANplots.sh -s Mx1_Mv1000 || exit
    python PlotTool/plotTransfer.py -b incu40 -c '"ChNemPtFrac>0.5"' || exit
    python PlotTool/plotTransfer.py recoil || exit
    # for r in $REGIONS; do
    	# pushd $r
    	# python PlotTool/makePileup.py || exit
    	# python PlotTool/makeBosonPt.py || exit
    	# python PlotTool/plotShapeUnc.py -b incu40 -c '"ChNemPtFrac>0.5"' || exit
    	# python PlotTool/plotShapeUnc.py recoil || exit
     	# popd;
    # done
    popd
done
