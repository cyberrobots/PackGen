#! /bin/bash

SavePath=./
TxDevice=eth0
RxDevice=eth1
Proto=0x0808
PackNumber=10000000
DstMac=00:aa:bb:cc:11:22
SrcMac=""
WriteAnalysis=1
Interval=1000

ReportFile=$SavePath/Report_$(date +"%H_%M_%S")_.txt

echo "Starting Packet Generator Calibration" >> $ReportFile 

echo "Date: " $(date +"%d/%m/%y %H:%M:%S") >> $ReportFile

echo "Head last commit: " $(git rev-parse HEAD) >> $ReportFile

for i  in {1..5}
do
	filename="test_$i"
	echo "Test name: $filename"
	Interval=$(( 10 ** (i-1) )) ## Increase Interval exponentially
	cmd="sudo ./PacketGenerator path $SavePath f_name $filename devout $TxDevice devin $RxDevice dstmac $DstMac proto $Proto num $PackNumber inter $Interval write $WriteAnalysis"
	echo $cmd
	$cmd

	#AnalysisFile=$SavePath"PackGen_1514_stats_$filename.mat"
	#echo $AnalysisFile
	#Report=$(head -25 $AnalyisFile)
        #echo $Report >> $ReportFile
	
done

exit
