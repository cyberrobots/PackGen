#! /bin/bash

SavePath=/media/slyras/Sammy/4.Measures/11_March/
TxDevice=enx00e097005173
RxDevice=enp0s25
Proto=0x0808
PackNumber=100
DstMac=00:11:6b:67:37:6c
SrcMac=""
WriteAnalysis=1
Interval=0

ReportFile=$SavePath/Report_$(date +"%H_%M_%S")_.txt

echo "Starting Packet Generator Calibration" >> $ReportFile 

echo "Date: " $(date +"%d/%m/%y %H:%M:%S") >> $ReportFile

echo "Head last commit: " $(git rev-parse HEAD) >> $ReportFile

for i  in {1..5}
do
	filename="test_$i"
	echo "Test name: $filename"
	Interval=$(( 10 ** (i-1) ))
	cmd="sudo ./PacketGenerator path $SavePath f_name $filename devout $TxDevice devin $RxDevice dstmac $DstMac proto $Proto num $PackNumber inter $Interval write $WriteAnalysis"
	echo $cmd
	$cmd

	#AnalysisFile=$SavePath"PackGen_1514_stats_$filename.mat"
	#echo $AnalysisFile
	#Report=$(head -25 $AnalyisFile)
        #echo $Report >> $ReportFile
	
done

exit
