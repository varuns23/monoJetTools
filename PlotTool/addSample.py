#!/usr/bin/env python
import os
from sys import argv
if len(argv) < 3 or len(argv) > 5:
    print "usage: ./PlotTool/addSample.py histoName x-axisTitle"
    exit()

script_path = os.path.dirname( os.path.realpath(__file__) )

def sortList(array):
    head = array[0]
    tail = array[-1]
    var = array[1:-1];
    for i in range(len(var)):var[i] = var[i].lstrip()
    var.sort();
    var[-1] = var[-1].replace(",\n","\n")
    var.insert(0,head)
    var.append(tail)
    return var
    
    
if len(argv) >= 3:
    histo = argv[1]
    if len(argv) == 3:
        title = argv[2]
    elif len(argv) <= 5:
        title = {}
        i = 0; axis = ['x','y','z']
        for t in argv[2:]:
            title[axis[i]] = t
            i+=1

    option = raw_input("Setting " + histo + " X-Axis Title to " + str(title) + " (y/n).")
    if not (option == "y" or option == "Y" or option == ""): print "Exiting.";exit()
    from PlotTool import samplenames
    if histo in samplenames:
        print histo+":"+str(samplenames[histo])+". Already Exists."
        if title == samplenames[histo]: print "Exiting.";exit()
        option = raw_input("Do you want to set " + histo + " X-Axis Title to " + str(title) + " (y/n).")
        if not (option == "y" or option == "Y"): print "Exiting.";exit()
    samplenames[histo] = title
    with open("%s/PlotTool/samplenames.py" % script_path,"w") as f:f.write("samplenames = "+str(samplenames)+"\n")
    # with open("PlotTool/samplenames.py","r") as f:
    #     text = f.readlines()
    #     for i in range(len(text)):
    #         if ('"'+histo+'":' in text[i]):
    #             print text[i].replace("\n","") + ". Already Exists."
    #             if (':"'+title+'"' in text[i]):print "Exiting.";exit()
    #             else:
    #                 option = raw_input("Do you want to set " + histo + " X-Axis Title to " + title + " (y/n).")
    #                 if not (option == "y" or option == "Y"): print "Exiting.";exit()
    #                 else:text[i] = '"'+histo+'":"'+title+'",\n';break
                        
    #         elif (text[i] == "}\n"):
    #             text[i-1] = text[i-1].replace("\n",",\n")
    #             text.insert(i,'"'+histo+'":"'+title+'",\n')
    #             break
    # text = sortList(text)
    # with open("PlotTool/samplenames.py","w") as f:
    #     for i in text:
    #         f.write(i)
