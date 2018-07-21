#! /bin/bash

SavePath=./
TxDevice=eth0
RxDevice=eth1
Proto=0x0808
PackNumber=1000
DstMac=28:d2:44:b0:58:b9
SrcMac=""
WriteAnalysis=1
Interval=2000

ReportFile=$SavePath/Report_$(date +"%H_%M_%S")_.txt

echo "Starting Packet Generator Calibration" >> $ReportFile 

echo "Date: " $(date +"%d/%m/%y %H:%M:%S") >> $ReportFile

echo "Head last commit: " $(git rev-parse HEAD) >> $ReportFile

for i  in {1..1}
do
	filename="test_$i"
	echo "Test name: $filename"
	#Interval=$(( 10 ** (i) )) ## Increase Interval exponentially
	cmd="sudo ./PacketGenerator path $SavePath f_name $filename devout $TxDevice devin $RxDevice dstmac $DstMac proto $Proto num $PackNumber inter $Interval write $WriteAnalysis"
	echo $cmd
	$cmd

	#AnalysisFile=$SavePath"PackGen_1514_stats_$filename.mat"
	#echo $AnalysisFile
	#Report=$(head -25 $AnalyisFile)
        #echo $Report >> $ReportFile
	
done

exit
