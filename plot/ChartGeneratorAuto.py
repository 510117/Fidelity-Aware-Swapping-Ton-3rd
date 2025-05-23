import numpy as np
import math
import os
import latex
import matplotlib.pyplot as plt
import matplotlib.transforms
import matplotlib
from matplotlib.offsetbox import AnchoredOffsetbox, TextArea, HPacker, VPacker

class ChartGenerator:
    # data檔名 Y軸名稱 X軸名稱 Y軸要除多少(10的多少次方) Y軸起始座標 Y軸終止座標 Y軸座標間的間隔
    def __init__(self, dataName, Xlabel, Ylabel):
        filename = '../data/ans/Greedy_' + dataName
        print("start generate", filename)

        if not os.path.exists(filename):
            print("file doesn't exist")
            return
        
        with open(filename, 'r', encoding='utf-8') as f:
            lines = f.readlines()
            
        
        
        # Ydiv, Ystart, Yend, Yinterval

        # color = [
        #     "#000000",
        #     "#006400",
        #     "#FF1493",
        #     "#7FFF00",   
        #     "#900321",
        # ]
        color = [
            "#FF0000",
            "#00FF00",   
            "#0000FF",
            "#000000",
            "#900321",
            "#FF00FF",
        ]
        # matplotlib.rcParams['text.usetex'] = True

        fontsize = 26
        Xlabel_fontsize = fontsize
        Ylabel_fontsize = fontsize
        Xticks_fontsize = fontsize
        Yticks_fontsize = fontsize
            
        # fig, ax = plt.subplots(figsize=(8, 6), dpi=600) 
        
        andy_theme = {
        # "axes.grid": True,
        # "grid.linestyle": "--",
        # "legend.framealpha": 1,
        # "legend.facecolor": "white",
        # "legend.shadow": True,
        # "legend.fontsize": 14,
        # "legend.title_fontsize": 16,
        "xtick.labelsize": 20,
        "ytick.labelsize": 20,
        "axes.labelsize": 20,
        "axes.titlesize": 20,
        "mathtext.fontset": "custom",
        "font.family": "Times New Roman",
        "mathtext.default": "default",
        "mathtext.it": "Times New Roman:italic",
        "mathtext.cal": "Times New Roman:italic",
        # "mathtext.fontset": "regular",
        # "figure.autolayout": True,
        "text.usetex": True,
        # "figure.dpi": 800
        }
        
        matplotlib.rcParams.update(andy_theme)        
        # fig, ax1 = plt.subplots(figsize = (6, 4.5), dpi = 600)
        fig, ax1 = plt.subplots(figsize = (6, 5), dpi = 600)
        # fig, ax1 = plt.subplots(figsize = (8, 6), dpi = 600)
        # ax1.spines['top'].set_position(('axes', 0.5)
        # ax1.spines['right'].set_linewidth(1.5)
        # ax1.spines['bottom'].set_linewidth(1.5)
        # ax1.spines['left'].set_linewidth(1.5)
        ax1.tick_params(direction = "in")
        ax1.tick_params(bottom = True, top = True, left = True, right = True)
        ax1.tick_params(pad = 20)

        ##data start##
        x = []
        _y = []
        numOfData = 0

        for line in lines:
            line = line.replace('\n','')
            if line[-1] == ' ':
                line = line[0 : -1]
            data = line.split(' ')
            numOfline = len(data)
            numOfData += 1

            for i in range(numOfline):
                if i == 0:
                    x.append(data[i])
                else:
                    if Ylabel.endswith("(%)"):
                        _y.append(str(float(data[i]) * 100))        
                    else:
                        _y.append(data[i])
        
        numOfAlgo = len(_y) // numOfData

        y = [[] for _ in range(numOfAlgo)]
        for i in range(numOfData * numOfAlgo):
            y[i % numOfAlgo].append(_y[i])

        # print(x)
        # print(y)

        maxData = 0
        minData = math.inf

        Ypow = 0
        Xpow = 0

        for i in range(-10, -1, 1):
            if i == 6:
                continue
            if float(x[numOfData - 1]) <= 10 ** i:
                Xpow = (i - 2)

        Ydiv = float(10 ** Ypow)
        Xdiv = float(10 ** Xpow)
        
        for i in range(numOfData):
            x[i] = float(x[i]) / Xdiv

        for i in range(numOfAlgo):
            if i == 6 or i == 1:
                continue
            for j in range(numOfData):
                y[i][j] = float(y[i][j]) / Ydiv
                maxData = max(maxData, y[i][j])
                minData = min(minData, y[i][j])


        Yend = math.ceil(maxData)
        Ystart = 0
        Yinterval = (Yend - Ystart) / 10

        if maxData > 1.1:
            Yinterval = int(math.ceil(Yinterval))
            Yend = int(Yend)
        else:
            Yend = 1
            Ystart = 0
            Yinterval = 0.2


        marker = ['x', 'v', 'o', '^', '.', '.']

        # for i in range(numOfAlgo - 1, -1, -1):
        for i in range(numOfAlgo):
            if i == 6 or i == 1:
                continue
            ax1.plot(x, y[i], color = color[i], lw = 1.5, linestyle = "-", marker = marker[i], markersize = 7, markerfacecolor = "none", markeredgewidth = 1.5, zorder = -i)
        # plt.show()

        plt.xticks(fontsize = Xticks_fontsize)
        plt.yticks(fontsize = Yticks_fontsize)
        
        # AlgoName = ["FNPR", "UB", "FLTO", "Nesting", "Linear", "ASAP"]
        AlgoName = ["FNPR", "FLTO", "Nesting", "Linear", "ASAP"]
                
        bbox_pos = (0.7, 0.15)
        leg = plt.legend(
            AlgoName,
            loc = 10,
            # bbox_to_anchor = (0.4, 1.2),
            bbox_to_anchor = bbox_pos,
            prop = {"size": fontsize - 5, "family": "Times New Roman"},
            frameon = "False",
            labelspacing = 0.2,
            handletextpad = 0.2,
            handlelength = 1,
            columnspacing = 0.8,
            ncol = 2,
            facecolor = "None",
        )

        leg.get_frame().set_linewidth(0.0)
        # Ylabel += self.genMultiName(Ypow)
        Xlabel += self.genMultiName(Xpow)
        # plt.subplots_adjust(top = 0.81)
        # plt.subplots_adjust(left = 0.22)
        # plt.subplots_adjust(right = 0.975)
        # plt.subplots_adjust(bottom = 0.23)
        plt.subplots_adjust(top = 0.97)
        plt.subplots_adjust(left = 0.2)
        plt.subplots_adjust(right = 0.975)
        plt.subplots_adjust(bottom = 0.20)

        plt.yticks(np.arange(Ystart, Yend + Yinterval, step = Yinterval), fontsize = Yticks_fontsize)
        plt.xticks(x)
        plt.ylabel(Ylabel, fontsize = Ylabel_fontsize)
        plt.xlabel(Xlabel, fontsize = Xlabel_fontsize, labelpad = 500)
        # ax1.yaxis.set_label_coords(-0.2, 0.5)
        # ax1.xaxis.set_label_coords(0.45, -0.27)
        ax1.yaxis.set_label_coords(-0.17, 0.5)
        ax1.xaxis.set_label_coords(0.45, -0.15)
        # plt.show()
        # plt.tight_layout()
        pdfName = dataName[0:-4].replace('#', '')
        # plt.savefig('../data/pdf/Fie_{}.eps'.format(pdfName)) 
        plt.savefig('../data/pdf/{}.jpg'.format(pdfName)) 
        # Xlabel = Xlabel.replace(' (%)','')
        # Xlabel = Xlabel.replace('# ','')
        # Ylabel = Ylabel.replace('# ','')
        plt.close()

    def genMultiName(self, multiple):
        if multiple == 0:
            return str()
        else:
            return "($" + "10" + "^{" + str(multiple) + "}" + "$)"

if __name__ == "__main__":
    # data檔名 Y軸名稱 X軸名稱 Y軸要除多少(10的多少次方) Y軸起始座標 Y軸終止座標 Y軸座標間的間隔
    # ChartGenerator("numOfnodes_waitingTime.txt", "need #round", "#Request of a round", 0, 0, 25, 5)
    Xlabels = ["request_cnt", "time_limit", "fidelity_threshold", "path_length", "avg_memory", "min_fidelity", "tao", "swap_prob", "entangle_lambda"]
    Ylabels = ["fidelity_gain", "succ_request_cnt"]
    # Ylabels = ["fidelity_gain"]
    # vector<string> X_names = {"request_cnt", "time_limit", "fidelity_threshold", "path_length", "avg_memory", "min_fidelity"};
    # vector<string> Y_names = {"fidelity_gain", "succ_request_cnt", "utilization"};
    LabelsName = {}
    LabelsName["min_fidelity"] = "min fidelity"
    LabelsName["swap_prob"] = "swap probablity"
    LabelsName["request_cnt"] = "\\#Requests"
    LabelsName["time_limit"] = "$|T|$"
    LabelsName["avg_memory"] = "Average Memory Limit"
    LabelsName["fidelity_threshold"] = "fidelity threshold"
    LabelsName["path_length"] = "path length"
    LabelsName["tao"] = "$\\tau$"
    LabelsName["fidelity_gain"] = " Fidelity Sum"
    LabelsName["succ_request_cnt"] = "\#Accepted Requests"
    LabelsName["utilization"] = "Memory Utilization (\%)"
    LabelsName["entangle_lambda"] = "$\lambda$"


    for Xlabel in Xlabels:
        for Ylabel in Ylabels:
            dataFileName = Xlabel + '_' + Ylabel + '.ans'
            ChartGenerator(dataFileName, LabelsName[Xlabel], LabelsName[Ylabel])