#!/bin/sh
set -e

if [[ "$1" == "phoiso" ]]; then
    var="photonPFIso"
    dir="phoisoV2"
    rebin=""
elif [[ "$1" == "sieie" ]]; then
    var="photonSieie"
    dir="sieie"
    rebin="--rebin 5"
fi

save="--save"
plot="--plot --sub ${dir}"

./PlotTool/purity/build_templates.py ${save} ${plot} ${var}{"",_ptbins} ${rebin}
./PlotTool/purity/build_templates.py ${save} ${var}{"",_ptbins} -a +1 --label metup ${rebin}
./PlotTool/purity/build_templates.py ${save} ${var}{"",_ptbins} -a +2 --label metdn ${rebin}

if [[ $var == "photonPFIso" ]]; then
    ./PlotTool/purity/build_templates.py ${save} ${var}{"",_ptbins} -a +3 --label sbup ${rebin}
    ./PlotTool/purity/build_templates.py ${save} ${var}{"",_ptbins} -a +4 --label sbdn ${rebin}
fi

./PlotTool/purity/fit_template.py ${save} ${plot} -t templates/template_${var}{"",_ptbins}.root 
./PlotTool/purity/fit_template.py ${save} -t templates/template_${var}*


if [[ $var == "photonPFIso" ]]; then
    ./PlotTool/purity/purity_ptbins.py -v ${var} ${plot}
elif [[ $var == "photonSieie" ]]; then
    ./PlotTool/purity/impurity_ptbins.py -v ${var} ${plot} --sys met
fi
