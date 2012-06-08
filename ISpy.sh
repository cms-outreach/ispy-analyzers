#!/bin/bash
#------------------------------------Original Author: Jinzhong Zhang-----------------------------------------------
#E-mail: zhangjin@cern.ch
#The main function is: build ISpy ig files from CMSSW data files automatically

CMSSW_Default="CMSSW_4_3_0"
AUTOCONDCFGFILE="Configuration.AlCa.autoCond"
ISpyBranchTag="use_zlib"

PYFileNamePrefix="ISpy_tmp"
IgFileName="ISpy.ig"
OutputFileName="Skim.root"
BatchSh="tmp_batch"

maxEvents=-1
MaxEventsEachIgFile=50

Load_RAWtoDigi_Data='process.load("Configuration.StandardSequences.RawToDigi_Data_cff")'
Load_RAWtoDigi_MC='process.load("Configuration.StandardSequences.RawToDigi_cff")'
Process_RAWtoDigi_All="process.RawToDigi*"

Load_Reco_Cosmic='process.load("Configuration.StandardSequences.ReconstructionCosmics_cff")'
Process_Reco_Cosmic="process.reconstructionCosmics*"
Load_Reco_MCData='process.load("Configuration.StandardSequences.Reconstruction_cff")'
Process_Reco_MCData="process.reconstruction*"

HLTTAG="\"TriggerResults\",\"\",\"HLT\""

IgFileList=
RunNumber=
RenewCode=FALSE
STAGEDONLY=FALSE
SKIM=UNKNOWN
Debug=FALSE
HLTalgorithm=
HLTProcessLoader=
process_HLT=
DATAFormat=0
CpToCastor=
EventSelection=
SaveTo=
TestFile=
System=`uname -a`
CurrentDir=`pwd`
CRABMODE="glite"
AUTOCOND='startup'

Logo ()
{
echo -e "\e[00m"
echo "=================================================================================="
echo "=                                                                                ="
echo -e "=                      \e[07m* Data Skimmer and ISpy IgMaker *\e[00m                         ="
echo -e "=                  \e[04m* Author:Jinzhong Zhang (zhangjin@cern.ch) *\e[00m                  ="
echo "=                                                                                ="
echo "= Help:                                                                          ="
echo -e "= \e[00;33m./ISpy.sh\e[00m or \e[00;33m./ISpy.sh -h\e[00m or \e[00;33m./ISpy.sh help\e[00m                                    ="
echo "=                                                                                ="
echo "= Usage:                                                                         ="
echo -e "= \e[00;33m./ISpy.sh [local or castor root files path. "\*" and "\?" are OK] [options]\e[00m          ="
echo -e "= \e[00;33m./ISpy.sh [local or castor root files path. "\*" and "\?" are OK] skimreco [options]\e[00m ="
echo -e "= \e[00;33m./ISpy.sh DAS:[DAS path] skim [options]\e[00m                                        ="
echo -e "= \e[33;07mPlease do not mix different root formats in one job.\e[00m                           ="
echo "=                                                                                ="
echo -e "= \e[33;07mFor MC RAW data with events pileup, please include \"MCRAW\" in the options to\e[00m   ="
echo -e "= \e[33;07mmask out TrackingParticle collection building.\e[00m                                 ="
echo "=                                                                                ="
echo "= To reconstruct cosmic RAW data correctly, please include \"CosmicRAW\" in the    ="                            
echo "= options.                                                                       ="
echo "=                                                                                ="
echo -e "= ./ISpy.sh with \e[00;33mskimreco\e[00m option is designed for providing Fireworks (\e[00;33mcmsShow\e[00m)   ="
echo "= files quickly and easily.                                                      ="
echo "=                                                                                ="
echo "=================================================================================="
echo
}

CheckOptions ()
{
    Option=${1//\"/}
    Option=${Option//\'/}
#0-Unknown;1-MC RECO;2-MC RAW;3-FEVT;4-Cosmic/data RECO;5-Cosmic RAW;6-Data RAW
    case $Option in
	[Mm][Cc][Rr][Ee][Cc][Oo])
	    return 1;;
	[Mm][Cc][Rr][Aa][Ww])
	    return 2;;
	[Ff][Ee][Vv][Tt])
	    return 3;;
	[Rr][Ee][Cc][Oo])
	    return 4;;
	[Dd][Aa][Tt][Aa][Rr][Ee][Cc][Oo])
	    return 4;;
	[Cc][Oo][Ss][Mm][Ii][Cc][Rr][Aa][Ww])
	    return 5;;
	[Cc][Oo][Ll][Rr][Aa][Ww])
	    return 6;;
	[Dd][Aa][Tt][Aa][Rr][Aa][Ww])
	    return 6;;
	[Dd][Ee][Bb][Uu][Gg])
	    Debug=TRUE;;
	[Ss][Kk][Ii][Mm])
	    if [ "$SKIM" == "FALSE" ]; then
		OutputFileName=${IgFileName%.*}.root
		echo -e $'\a'"\e[0;31mWarning: Contradictory options --- Data Skimmer should output a root file. The file name is changed to $OutputFileName.\e[00m"
	    fi
	    SKIM=TRUE
	    PYFileNamePrefix="Skim_tmp";;
	[Ss][Kk][Ii][Mm][Rr][Ee][Cc][Oo])
	    if [ "$SKIM" == "FALSE" ]; then
		OutputFileName=${IgFileName%.*}.root
		echo -e $'\a'"\e[0;31mWarning: Contradictory options --- Data Skimmer should output a root file. The file name is changed to $OutputFileName.\e[00m"
	    fi
	    SKIM=TRUE
	    OutputFileName="SkimandReco.root"
	    PYFileNamePrefix="Skimreco_tmp";;
	[Cc][Oo][Nn][Dd][Oo][Rr])
	    CRABMODE="condor";;
	[Rr][Ee][Nn][Ee][Ww][Cc][Oo][Dd][Ee])
	    RenewCode=TRUE;;
        REDIGI[0-9]*[XY])
	    HLTTAG="\"TriggerResults\",\"\",\"$Option\"";;
	[Ss][Tt][Aa][Gg][Ee][Dd][Oo][Nn][Ll][Yy])
	    STAGEDONLY=TRUE;;
	CMSSW_[0-9_]*)
	    CMSSW_Default=$Option
	    if [ "$RunType" != "15" ]; then
		CMSSWEnvironment
	    fi;;
	[[:graph:]]*.root)
	    if [ `expr index "$Option" '/'` != "0" ]; then
		temp=`expr match "$Option" '.*/\([[:graph:]]*\)'`
	    else
		temp=$Option
	    fi
	    OutputFileName=$temp
	    SaveTo=${Option%"$OutputFileName"}
	    SKIM=TRUE
	    PYFileNamePrefix="Skim_tmp";;
	[[:graph:]]*.ig)
	    if [ `expr index "$Option" '/'` != "0" ]; then
		temp=`expr match "$Option" '.*/\([[:graph:]]*\)'`
	    else
		temp=$Option
	    fi
	    IgFileName=$temp
	    SaveTo=${Option%"$IgFileName"}
	    if [ "$SKIM" == "TRUE" ]; then
		OutputFileName=${IgFileName%.*}.root
		echo -e $'\a'"\e[0;31mWarning: Contradictory options --- Data Skimmer should output a root file. The file name is changed to $OutputFileName.\e[00m"
	    else
		SKIM=FALSE	
	    fi;;
	[[:graph:]]*/)
	    SaveTo=$Option;;
	[[:graph:]]*/[[:graph:]]*)
	    SaveTo=$Option"/";;
        HLT_[A-Za-z0-9_,]*)
	    HLTProcessLoader="process.load(\"HLTrigger.HLTfilters.hltHighLevel_cfi\")"
	    temp="'${Option//,/','}'"
	    if [ -n "$HLTalgorithm" ]; then
		HLTalgorithm=$HLTalgorithm" $temp"
	    else
		HLTalgorithm=$temp
	    fi;;
	[0-9]*:[max0-9,:-]*)
	    SegmentLeft=$Option
	    until [ ${#SegmentLeft} -le 0 ]
	      do
	      Segment=`expr match "$SegmentLeft" '\([max0-9:-]*\)'`
	      SegmentLeft=${SegmentLeft#$Segment}
	      SegmentLeft=${SegmentLeft#,}
	      if [ `expr index "$Segment" ':'` != "0" ]; then
		  NewRunNumber=`expr match "$Segment" '\([0-9]*\):.*'`
		  if [ "$NewRunNumber" != "$RunNumber" ]; then
		      strlength=${#NewRunNumber}
		      (( strlength=8-$strlength ))
		      strlength="%"$strlength"s"
		      strlength=$(printf "$strlength")
		      strlength=${strlength// /0}
		      Digi8RunNumber=$strlength$NewRunNumber
		      IgFileList=$IgFileList${IgFileName%.ig}"_R-"$Digi8RunNumber"_T00000001.ig, "${IgFileName%.ig}"_R0"$Digi8RunNumber"_T00000001.ig, "
		      RunNumber=$NewRunNumber
		  fi
		  if [ `expr index "$Segment" '-'` == "0" ]; then
		      Option=${Option/$Segment/$Segment-$Segment}
		  else
		      Segment1=`expr match "$Segment" '\([0-9:]*\)-.*'`
		      Segment2=${Segment#$Segment1-}
		      if [ `expr index "$Segment2" ':'` == "0" ]; then
			  Option=${Option/$Segment/$Segment1-$RunNumber:$Segment2}
		      fi
		  fi
	      else
		  if [ `expr index "$Segment" '-'` == "0" ]; then
		      Option=${Option/$Segment/$RunNumber:$Segment-$RunNumber:$Segment}
		  else
		      Segment1=`expr match "$Segment" '\([0-9]*\)-.*'`
		      Segment2=${Segment#$Segment1-}
		      Option=${Option/$Segment/$RunNumber:$Segment1-$RunNumber:$Segment2}
		  fi
	      fi
	    done
	    Option=\"${Option%,}\"
	    if [ -n "$EventSelection" ]; then
		EventSelection=$EventSelection,${Option//,/\",\"}
	    else
		EventSelection=${Option//,/\",\"}
	    fi;;
        [a-zA-Z0-9]*_[a-zA-Z0-9_]*)
	    GlobalTag=${Option%::All}::All
	    echo -e "I will use GlobalTag: \e[00;33m$GlobalTag\e[00m.";;
        *)
 	    Option=${1//\"/\\\"}
	    echo -e "\e[31;07mError: \"$Option\" cannot be recognized by the script.\e[00m"
	    HELPOptions
	    return 0;;
    esac
    return 0;
}

CheckAvailability ()
{
    #check CMSSW environment
    if [ -z "$CMSSW_BASE" ]; then
	#check current directory
	CMSSW_BASE=`expr match "$CurrentDir/" "\(.*CMSSW_[0-9]_[0-9]*_[[:alnum:]_]*/\).*"`
	if [ -z "$CMSSW_BASE" ]; then
	    echo "Please setup CMSSW environment before running this code."
	    Choice="S"
	    until [[ "$Choice" == [YyNn] ]]
	      do
	      echo -en $'\a'"\e[33;07mDo you want to set up $CMSSW_Default in the current directory?(y/n)\e[00m"
	      read -n 1 Choice
	      echo
	    done
	    if [[ "$Choice" == [Yy] ]]; then
		CMSSWEnvironment
	    else
		return 0
	    fi
	else
	    cd $CMSSW_BASE/src/
	    eval `scramv1 runtime -sh`
	    cd $CurrentDir
	fi
    fi
    VersionNum1=`expr match "$CMSSW_VERSION" ".*CMSSW_\([0-9]*\)_.*"`
    VersionNum2=`expr match "$CMSSW_VERSION" ".*CMSSW_[0-9]*_\([0-9]*\).*"`
    VersionNum3=`expr match "$CMSSW_VERSION" ".*CMSSW_[0-9]*_[0-9]*_\([0-9]*\).*"`
    if [ `expr match "$CMSSW_VERSION" ".*pre"` != "0" ]; then
	(( VersionNum3 -= 1 ))
    fi
    #Compatibility check
    #<CMMSW_3_3_0 is not supported (onone uses)
    if [ $VersionNum1 -lt 3 ]||([ $VersionNum1 -eq 3 ]&&[ $VersionNum2 -lt 3 ]); then
	echo -en $'\a'"\e[33;07mWarning: $CMSSW_BASE is NOT supported."
    fi
    #the location of autocond cfg file is changed in CMSSW_4_3_0
    if [ $VersionNum1 -lt 4 ]||([ $VersionNum1 -eq 4 ]&&[ $VersionNum2 -lt 3 ])||([ $VersionNum1 -eq 4 ]&&[ $VersionNum2 -eq 3 ]&&[ $VersionNum3 -lt 0 ]); then
	AUTOCONDCFGFILE="Configuration.PyReleaseValidation.autoCond"
    fi
    #Under <CMSSW_4_X, ISpy releases with CMSSW
    if [ $VersionNum1 -lt 4 ]; then 
	ISpyBranchTag=$CMSSW_VERSION
    else
	#4_0_0< <4_2_5, use_classlib, use_zlib for ISpyCSCRecHits2D;4_2_5< <?, use_zlib
	if ([ $VersionNum1 -eq 4 ]&&[ $VersionNum2 -lt 2 ])||([ $VersionNum1 -eq 4 ]&&[ $VersionNum2 -eq 2 ]&&[ $VersionNum3 -lt 5 ]); then
	    ISpyBranchTag="use_classlib"
	fi
	if [ $VersionNum1 -ge 5 ]||([ $VersionNum1 -eq 5 ]&&[ $VersionNum2 -ge 2 ]); then
	    ISpyBranchTag="postCMSSW_5_2"
	fi
    fi
    if [ `expr match "$System" ".*lxplus.*"` != "0" ]; then
	echo -e "\e[07mIt is running on lxplus.cern.ch. Run it on Batch? How long will it take?\e[00m"
	echo -n "0 - Run it locally."
	if [ -f $ISpyBin ]&&[ -n "$DISPLAY" ]; then
	    echo "The first event in the last produced ig file will be shown up."
	else
	    echo
	fi
	echo "1 - 1nh (1 hour)"
	echo "2 - 8nh (8 hours)"
	echo "3 - 1nd (1 day)"
	echo "4 - 2nd (2 days)"
	echo "5 - 1nw (1 week)"
	echo "6 - 2nw (2 weeks)"
	RunType=10
	until [[ "$RunType" == [0-6] ]]
	  do
	  echo -en $'\a'"\e[07m(0-6)?\e[00m"
	  read -n 1 RunType
	done
	echo
    else
	RunType="0"
    fi
    if [ `expr match "$CurrentDir" ".*/afs/.*"` != "0" ]&&[ -z "$SaveTo" ]; then
	Choice="S"
	until [[ "$Choice" == [YyNn] ]]
	  do
	  echo -en $'\a'"\e[33;07mWarning: It may exceed the AFS disk quota. Save the outputs to $CASTOR_HOME?(y/n)\e[00m"
	  read -n 1 Choice
	  echo
	done
	if [[ "$Choice" == [Yy] ]]; then
	    SaveTo=$CASTOR_HOME"/"
	fi
    fi
    return 1
}

CheckoutCode ()
{
#check out CVS code
    echo "========================== Checking the ig producer codes ========================"
    cd $CMSSW_BASE/src/
    CVSStatus=`cvs checkout -r $ISpyBranchTag ISpy/Analyzers 2>&1`  #output stderr as stdout
    echo "Checking out codes with branch $ISpyBranchTag"
    if [ `expr match "$CVSStatus" ".*GSSAPI authentication failed"` != "0" ]; then
        kinit
	TeaTime
 	cvs checkout -r $ISpyBranchTag ISpy/Analyzers
    else
	TeaTime
    fi
    #less than CMSSW_5_X, use_zlib for ISpyCSCRecHit2D.cc
    if [ $VersionNum1 -lt 5 ]||([ $VersionNum1 -eq 5 ]&&[ $VersionNum2 -lt 2 ]); then
	cvs checkout -r use_zlib ISpy/Analyzers/src/ISpyCSCRecHit2D.cc # for the fix of the crashes in ISpyCSCRecHit2D
    else
	cvs checkout -r use_zlib ISpy/Services #temporary!
    fi
    #cvs checkout -r $ISpyBranchTag ISpy/Services
    cd ISpy
    scramv1 b
    cd $CurrentDir
}

Client ()
{
    if [ `expr match "$System" ".*x86_64"` != "0" ]; then
	ISpyBin=ispy-x86_64
    else
	ISpyBin=ispy
    fi
    if [ -f $ISpyBin ]; then
	Choice="N"
    else
	Choice="S"
    fi
    until [[ "$Choice" == [YyNn] ]]
    do
      echo -en $'\a'"\e[07mYou do not have an ISpy Client in the current directory. Download it?(y/n)\e[00m"
      read -n 1 Choice
      echo
    done
    if [[ "$Choice" == [Yy] ]]; then
	wget http://iguana.web.cern.ch/iguana/ispy/bin/linux/$ISpyBin
	chmod +x $ISpyBin
    fi
}

CMSSWEnvironment()
{
    if [ -z "$CMSSW_VERSION" ]; then
	if [ `expr match "$System" ".*cmslpc"` != "0" ]; then
	    . /uscmst1/prod/sw/cms/shrc prod
	else
	    if [ `expr match "$System" ".*lxplus.*.cern.ch"` == "0" ]; then
		source /afs/cern.ch/cms/sw/cmsset_default.sh
	    fi
	fi
	if [ ! -d $CMSSW_Default ]; then
	    echo "Building CMSSW directory"
	    scramv1 project CMSSW $CMSSW_Default
	fi
	CMSSW_BASE="$CurrentDir/$CMSSW_Default"
    fi
    cd $CMSSW_BASE/src/
    eval `scramv1 runtime -sh`
    cd $CurrentDir
}

CRABEnvironment ()
{
    if [ -n "$CRABDIR" ]; then 
	return
    fi
    if [ `expr match "$System" ".*cmslpc"` != "0" ]; then
	source /uscmst1/prod/grid/gLite_SL5_CRAB_27x.sh
	CMSSWEnvironment
	source /uscmst1/prod/grid/CRAB/crab.sh
    else
	source /afs/cern.ch/cms/LCG/LCG-2/UI/cms_ui_env.sh
	CMSSWEnvironment
	source /afs/cern.ch/cms/ccs/wm/scripts/Crab/crab.sh
    fi
    CRABStatus=`voms-proxy-info -identity 2>&1`
    CRABTimeLeft=`voms-proxy-info -timeleft`
    if [ `expr match "$CRABStatus" ".*Couldn't find a valid proxy."` != "0" ]||[ `expr match "$CRABStatus" ".*expired"` != "0" ]||[ "$CRABTimeLeft" == "0" ]; then
	voms-proxy-init -voms cms
    fi
}

CREATECRABJOB ()
{
    echo "[CRAB]">crab.cfg
    echo "jobtype = cmssw">>crab.cfg
    echo "scheduler = $CRABMODE">>crab.cfg
    echo "[CMSSW]">>crab.cfg
    echo "datasetpath=$1">>crab.cfg
    echo "pset = $PYFileName">>crab.cfg
    if [ "$SKIM" == "TRUE" ]; then
	echo "output_file = $OutputFileName">>crab.cfg
    else
	echo "output_file = ${IgFileList%, }">>crab.cfg
    fi
    echo "total_number_of_events=-1">>crab.cfg
    echo "events_per_job = 50000">>crab.cfg
    echo "[USER]">>crab.cfg
    if [ `expr match "$GRIDOutputPath" '/castor/'` != "0" ]||[ `expr match "$GRIDOutputPath" 'rfio:/castor/'` != "0" ]; then
	echo "return_data = 0">>crab.cfg
	echo "copy_data = 1">>crab.cfg
	echo "storage_element=srm-cms.cern.ch">>crab.cfg
	echo "storage_path=/srm/managerv2?SFN=/castor/cern.ch">>crab.cfg
	echo "user_remote_dir = ${GRIDOutputPath#/castor/cern.ch/}">>crab.cfg
    else
	GRIDOutputPath="Return outputs to local"
	echo "return_data = 1">>crab.cfg
	echo "copy_data = 0">>crab.cfg
    fi
    echo "publish_data=0">>crab.cfg
    cat crab.cfg
    crab -create | tee Results.txt
    WorkingDir=`grep 'working directory' Results.txt`
    WorkingDir="/"`expr match "$WorkingDir" ".*working directory [ ]*/\(.*/\)"`
    NumJobs=`grep 'Total of' Results.txt`
    NumJobs=`expr match "$NumJobs" ".*Total of \([0-9]*\) jobs created.*"`
    rm Results.txt
    echo "======================== CRAB Job Summary (crab.log) ================================="
    echo "Total $NumJobs jobs"
    echo "Source: $1"
    echo "Python configuration file: $PYFileName"
    echo "Save to: $GRIDOutputPath"
    echo "Working Directory: " $WorkingDir
    if [ -n "$NumJobs" ]&&[ -n "$WorkingDir" ]&&[ "$Debug" != "TRUE" ]; then 
	echo "$NumJobs jobs are going to be submitted in $WorkingDir"
	for i in $(seq 0 1 $(( NumJobs/500 )) )
	do
	  crab -submit 500 -c $WorkingDir
	done
    fi
}

DATAFormatDef () 
{
#0-Unknown;1-MC RECO;2-MC RAW;3-FEVT;4-Cosmic/data RECO;5-Cosmic RAW;6-Data RAW;7-MC RAWDebug;
   EventContent=`edmDumpEventContent $1`
   HLTTAG=`printf "$EventContent" | grep --max-count=1 edm::TriggerResults`
   HLTTAG=`expr match "$HLTTAG" ".*edm::TriggerResults[ ]*\(\".*\"\)"`
   HLTTAG=${HLTTAG// /}
   HLTTAG=${HLTTAG//\"\"/\",\"}
   if [ `expr match "$EventContent" ".*reco.*"` != "0" ]; then
       if [ `expr match "$EventContent" ".*FEDRawDataCollection.*"` != "0" ]; then
	   return 3;
       else
	   if [ `expr match "$EventContent" ".*generator.*"` != "0" ]; then
	       return 1;
	   else
	       return 4;
	   fi
       fi
   else
       if [ `expr match "$EventContent" ".*FEDRawDataCollection.*"` != "0" ]; then
	   if [ `expr match "$EventContent" ".*generator.*"` != "0" ]; then
	       if [ `expr match "$EventContent" ".*TrackingParticle.*"` != "0" ]; then
		   return 7;
	       else
		   return 2;
	       fi
	   else
	       return 6;
	   fi
       fi
   fi
   until [ $DATAFormat -gt 0 ]&&[ $DATAFormat -lt 8 ]
     do
     echo "1-Monte Carlo RECO"
     echo "2-Monte Carlo RAW"
     echo "3-Full EVenT: FEVT"
     echo "4-Cosmic/data RECO"
     echo "5-Cosmic RAW"
     echo "6-Data RAW"
     echo "7-Monte Carlo RAWDEBUG"
     echo -en $'\a'"\e[07mPlease Specify Data Format(1-6)\e[00m"
     read -n 1 DATAFormat
     echo ""
   done
   return $DATAFormat
}

HELP ()
{
    echo -e "\e[00m"
    echo -e "\e[07m[usage]\e[00m"
    echo -e "\e[04m(I)   For RAW, RECO and FEVT root files on local disks and CASTOR:\e[00m"
    echo -e "      \e[00;33m./ISpy.sh [local or castor root files path. "\*" and "\?" are OK] [options]\e[00m"
    echo
    echo -e "      1. Quick example: \e[00;33m./ISpy.sh /castor/cern.ch/cms/store/data/Run2010A/Mu/RECO/v2/000/136/294/* 136294:2202867,1000-2000\e[00m"
    echo "         It will go through RUN136294, Event 2202867 and RUN136294, from Event 1000 to 2000 and produce ig files. If the X display is on and the ISpy client is in the current directory, the first event in the last produced ig file will be shown up."
    echo
    echo -e "      2. Full example: \e[00;33m./ISpy.sh /castor/cern.ch/cms/store/data/Run2010A/Mu/RECO/v2/000/136/294/* 136294:2200000-3000000,1000-2000 HLT_Mu9 HLT_IsoMu3,HLT_L2Mu3 CMSSW_3_5_4 GR_R_35X_V8 \$CASTOR_HOME/output datareco debug\e[00m"
    echo "         It will (1) read events from run 136294 event 2200000 to 3000000 and 1000 to 2000; (2) use HLT algorithm \"HLT_Mu9 and ( HLT_IsoMu3 OR HLT_L2Mu3 )\" to skim events; (3) setup and use CMSSW_3_5_4; (4) use GlobalTag GR_R_35X_V8; (5) save the output to CASTOR, the name will be output.root; (6) treat it as RECO collision data (rarely needed); (7) create all the configuration files and codes without submitting the job."
    echo
    echo -e "\e[04m(II)   For files on DAS:\e[00m"
    echo "      It can only skim events from DAS. The ig files could not be produced. Running"
    echo -e "      \e[00;33m./ISpy.sh DAS:[DAS path] [options] skim\e[00m"
    echo "      will skim events based on the algorithm stated in the options. The default output path is your CASTOR home directory. You can change the output path in the options."
    echo
    echo -e "\e[07m[options]:\e[00m"
    HELPOptions
}

HELPOptions ()
{
    echo "   One can directly list all the options. The script will recognize the meanings of your options."
    echo -e "\e[04m(I)   Reserved words (not case sensitive):\e[00m"
    echo "      1. To specify the data type:"
    echo -e "         It is rarely necessary since the script can recognize most data types. You have the following options: \e[00;33mMCRECO\e[00m for reconstructed (RECO) Monte Carlo (MC); \e[00;33mMCRAW\e[00m for RAW MC; \e[00;33mRECO\e[00m or \e[00;33mDATARECO\e[00m for cosmic or data RECO; \e[00;33mCOSMICRAW\e[00m for cosmic RAW; \e[00;33mCOLRAW\e[00m or \e[00;33mDATARAW\e[00m for collision RAW data;"    
    echo -e "      2. \e[00;33mdebug\e[00m: The configure files (*.py, crab.cfg, or *.sh) will be written without submitting/running the job"
    echo -e "      3. \e[00;33mskim\e[00m: The root files will be skimmed and outputted as root files"
    echo -e "      4. \e[00;33mskimreco\e[00m: The root files will be skimmed, reconstructed and outputted as root files."
    echo -e "      5. \e[00;33mcondor\e[00m: CRAB condor mode, for data in T1-FNAL."
    echo -e "      6. \e[00;33mrenewcode\e[00m: match the ISpy/Analyzer and ISpy/Services codes with the ones on CVS."
    echo -e "      7. \e[00;33mstagedonly\e[00m: only investigate the staged files on CASTOR."
    echo -e "\e[04m(II)  To specify a CMSSW version:\e[00m Please list it directly. It should look like \e[00;33mCMSSW_......\e[00m"
    echo -e "\e[04m(III) To specify a GlobalTag:\e[00m Please list it directly. It should look like \e[00;33m......_......\e[00m"
    echo -e "\e[04m(IV)  To specify the save path and the file name (default is your current directory):\e[00m"
    echo -e "      Please list it directly. A file name should either end with '\e[00;33m.ig\e[00m' or '\e[00;33m.root\e[00m'. If it contains '\e[00;33m/\e[00m' but does not end with '\e[00;33m.ig\e[00m' or '\e[00;33m.root\e[00m', it will be treated as a directory. In other words, a directory must contain a '\e[00;33m/\e[00m' to be treated as a directory. The directory does not have to exist. The script will create it recursively."
    echo -e "\e[04m(V)   Make an event selector:\e[00m"
    echo -e "      It should not be confusing. Both run number and event number should be specified, separated by a colon \e[00;33m:\e[00m, like"
    echo -e "      1. \e[00;33m123456:1423556-123456:2345243,123478:2354231-123478:2355342,......\e[00m, which means in run 123456, it will take from event 1423556 to 2345243 and in run 123478, it will take event 2345231 to 2355342."
    echo -e "   or 2. \e[00;33m123456:1423556-2345445,1235472,2455667-max,......\e[00m, which means in run 123456, it will take events from 1423556 to 2345445 and event 1235472 and all events whose numbers are bigger than 2455667."
    echo -e "\e[04m(VI)  Make a selector based on HLT algorithm:\e[00m"
    echo -e "      It should look like \e[00;33mHLT_A,HLT_B HLT_C\e[00m. The comma means OR. The space means AND. Here the OR is prior to AND. It means \"(HLT_A OR HLT_B) AND HLT_C\" in the regular expression."
    echo
}

NameMatch () 
{
    Str=${2//\./\\.}
    Str=${Str//\?/.}
    Str=${Str//\*/.\*}
    if [ `expr match "$1" "$Str"` = ${#1} ]; then
	return 1;
    else
	return 0;
    fi
}

rfmkdir_r () 
{
    Exist=`nsls $1 2>&1`
    if [ `expr match "$Exist" ".*No such file or directory"` != "0" ]; then
	rfmkdir_r ${1%/*}
	rfmkdir $1
	rfchmod 755 $1
	echo $1 is created.
    fi
}

mkdir_r () 
{
    Exist=`ls $1 2>&1`
    if [ `expr match "$Exist" ".*No such file or directory"` != "0" ]; then
	mkdir_r ${1%/*}
	mkdir $1
	echo $1 is created.
    fi
}

TeaTime ()
{
    echo -e "\e[32;07m================== No further options for the script. Please wait for the result. ==============\e[00m"
}

VisLatestIgFile()
{
    if [ -f $ISpyBin ]; then
	for i in `ls -t $SaveTo*.ig`
	do
	  chmod +x $ISpyBin
	  ./$ISpyBin $i
	  break
	done
    fi
}

Main()
{
    if [ "$SKIM" == "TRUE" ]; then
	echo "The output file will be saved to $SaveTo$OutputFileName"
	if [ -n "$GlobalTag" ]; then
	    echo -e "\e[0;32mWarning: GlobalTags will not be used in data skim.\e[00m"
	fi
	LoadRawtoDigi=
	ProcessRawtoDigi=
	LoadRECO=
	ProcessRECO=
    else
	echo "The output ig file will be saved to $SaveTo$IgFileName"
	if [ ! -d "$CMSSW_BASE/src/ISpy" ]||[ "$RenewCode" == "TRUE" ]; then
	    CheckoutCode
	fi
    fi
    if [ "$SKIM" != "TRUE" ]||[ "$PYFileNamePrefix"=="Skimreco_tmp" ]; then
	case $DATAFormat in
	    1)
		AUTOCOND='mc'
		LoadRawtoDigi=
		LoadRECO=
		ProcessRawtoDigi=
		ProcessRECO=
		echo -n "The data format is Monte Carlo RECO" ;;
	    2)
		AUTOCOND='mc'
		LoadRawtoDigi=$Load_RAWtoDigi_MC
		ProcessRawtoDigi=$Process_RAWtoDigi_All"process.trackingTruth*"
		LoadRECO=$Load_Reco_MCData
		ProcessRECO=$Process_Reco_MCData
		echo -n "The data format is Monte Carlo RAW" ;;
	    3) 
		AUTOCOND='com10'
		LoadRawtoDigi=$Load_RAWtoDigi_Data
		ProcessRawtoDigi=$Process_RAWtoDigi_All
		LoadRECO=
		ProcessRECO=
		echo -n "The data format is FEVT" ;;
	    4)
		AUTOCOND='com10'
		LoadRawtoDigi=
		ProcessRawtoDigi=
		LoadRECO=
		ProcessRECO=
		echo -n "The data format is Cosmic/data RECO" ;;
	    5)
		AUTOCOND='com10'
		LoadRawtoDigi=$Load_RAWtoDigi_Data
		ProcessRawtoDigi=$Process_RAWtoDigi_All
		LoadRECO=$Load_Reco_Cosmic
		ProcessRECO=$Process_Reco_Cosmic
		echo -n "The data format is Cosmic RAW" ;;
	    6)
		AUTOCOND='com10'
		LoadRawtoDigi=$Load_RAWtoDigi_Data
		ProcessRawtoDigi=$Process_RAWtoDigi_All
		LoadRECO=$Load_Reco_MCData
		ProcessRECO=$Process_Reco_MCData
		echo -n "The data format is Collision RAW" ;;
	    7)
		AUTOCOND='mc'
		LoadRawtoDigi=$Load_RAWtoDigi_MC
		ProcessRawtoDigi=$Process_RAWtoDigi_All
		LoadRECO=$Load_Reco_MCData
		ProcessRECO=$Process_Reco_MCData
		echo -n "The data format is Monte Carlo RAWDEBUG (include TrackingParticle collection)" ;;
	esac
	echo ". If the format is not correct, please push Ctrl-C and send a bug report."
    fi
    TeaTime
    PyNum=0
    PYFileName=$PYFileNamePrefix"_"$PyNum".py"
    until ! [ -f $PYFileName ]
      do
      (( PyNum += 1 ))
      PYFileName=$PYFileNamePrefix"_"$PyNum".py"
    done
    echo "import FWCore.ParameterSet.Config as cms">$PYFileName
    if [ "$SKIM" == "TRUE" ]; then
	echo 'process = cms.Process("SKIM")'>>$PYFileName
    else
	echo 'process = cms.Process("ISPY")
process.load("ISpy/Analyzers/ISpy_Producer_cff")'>>$PYFileName
    fi
    if [ "$SKIM" != "TRUE" ]||[ "$PYFileNamePrefix"=="Skimreco_tmp" ]; then
	echo 'process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")'>>$PYFileName
	if [ -z "$GlobalTag" ]; then
	    echo "from $AUTOCONDCFGFILE import autoCond">>$PYFileName
	    echo "process.GlobalTag.globaltag = autoCond['$AUTOCOND']">>$PYFileName
	else
	    echo "process.GlobalTag.globaltag = \"$GlobalTag\"">>$PYFileName
	fi
	echo 'process.load("Configuration.StandardSequences.Geometry_cff")'>>$PYFileName
	echo 'process.load("Configuration.StandardSequences.MagneticField_cff")'>>$PYFileName
	if [ "$SKIM" != "TRUE" ]; then
	    echo 'from ISpy.Analyzers.ISpyTriggerEvent_cfi import *'>>$PYFileName
	    HLTTAGProcessName=`expr match "$HLTTAG" "\".*\",\".*\",\"\(.*\)\""`
	    echo "ISpyTriggerEvent.processName=('$HLTTAGProcessName')">>$PYFileName
        fi
	echo $LoadRawtoDigi>>$PYFileName
	echo $LoadRECO>>$PYFileName
	if [ $DATAFormat -eq 2 ]; then
	    echo 'process.load("Configuration.StandardSequences.Digi_cff")
process.load("SimGeneral.MixingModule.mixNoPU_cfi")
process.load("SimGeneral.TrackingAnalysis.trackingParticles_cfi")
process.load("Configuration.StandardSequences.Services_cff")
del process.RandomNumberGeneratorService.generator
process.RandomNumberGeneratorService.restoreStateLabel = cms.untracked.string("randomEngineStateProducer")
process.mix.playback = cms.untracked.bool(True)
process.trackingTruth = cms.Sequence(process.mix*process.doAllDigi*process.mergedtruth)'>>$PYFileName
	fi
    fi
    if [ -n "$HLTalgorithm" ]; then
	if [ -z "$EventContent" ]&&[ "$RunType" != "GRID" ]; then
	    temp=$IOName${Files[0]}
	    EventContent=`edmDumpEventContent ${temp#file:}`
	fi
	HLTFilterNum=0
	for i in $HLTalgorithm
	  do 
	  (( HLTFilterNum += 1 ))
	  echo "process.hltHighLevel$HLTFilterNum = cms.EDFilter(\"HLTHighLevel\",TriggerResultsTag = cms.InputTag($HLTTAG),HLTPaths = cms.vstring($i),eventSetupPathsKey = cms.string(''),andOr = cms.bool(True),throw = cms.bool(True))">>$PYFileName
	  process_HLT=$process_HLT"process.hltHighLevel$HLTFilterNum*"
	done
    fi
    if [ "$RunType" == "GRID" ]; then 
	echo -n "process.source = cms.Source(\"PoolSource\",fileNames=cms.untracked.vstring()">>$PYFileName
    else
	echo "filelist = cms.untracked.vstring()">>$PYFileName
	echo -n "filelist.extend([">>$PYFileName
	for i in $(seq 0 1 $(( $NumberFiles-2 )) )
	  do
	  echo "'$IOName${Files[i]}',">>$PYFileName
	done
	echo "'$IOName${Files[$NumberFiles-1]}'])">>$PYFileName
	echo -n "process.source = cms.Source('PoolSource',fileNames = filelist">>$PYFileName
    fi
    if [ -n "$EventSelection" ]; then
	echo ",eventsToProcess = cms.untracked.VEventRange($EventSelection))">>$PYFileName
    else
	echo ")">>$PYFileName
    fi
    echo "from FWCore.MessageLogger.MessageLogger_cfi import *">>$PYFileName
    echo "process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32($maxEvents))">>$PYFileName
#    echo 'process.load("FWCore.MessageLogger.MessageLogger_cfi")'>>$PYFileName
#    echo "process.MessageLogger.cerr.FwkReport.reportEvery = 1000   #quench the message logger (optional)">>$PYFileName
    if [ `expr match "$SaveTo" '/castor/'` != "0" ]||[ `expr match "$SaveTo" 'rfio:/castor/'` != "0" ]; then
	CpToCastor=${SaveTo#rfio:}
	rfmkdir_r ${CpToCastor%/}
	if [ -n "$TMPDIR" ]; then
	    SaveTo=$TMPDIR/
	else
	    SaveTo=/tmp/
	fi
	if [ "$SKIM" == "TRUE" ]; then
	    CpToCastor="rfcp $SaveTo$OutputFileName $CpToCastor"
	else
	    CpToCastor="for f in $SaveTo${IgFileName%.ig}*.ig; do rfcp \$f $CpToCastor;rm \$f; done"
	fi
    else
	mkdir_r ${SaveTo%/}
    fi
    if [ "$SKIM" == "TRUE" ]; then
	process_HLT=${process_HLT%\*}
	if [ ${#process_HLT} -gt 0 ]; then
	    echo "process.p1=cms.Path($process_HLT)">>$PYFileName
	    echo "process.output = cms.OutputModule(\"PoolOutputModule\",">>$PYFileName
	    echo "fileName = cms.untracked.string(\"$SaveTo$OutputFileName\"),">>$PYFileName
	    echo "SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('p1')))">>$PYFileName
	else
	    echo "process.output = cms.OutputModule(\"PoolOutputModule\",">>$PYFileName
	    echo "fileName = cms.untracked.string(\"$SaveTo$OutputFileName\"))">>$PYFileName
	fi
	if [ "$PYFileNamePrefix"=="Skimreco_tmp" ]; then
	    echo "process.p1=cms.Path($process_HLT$ProcessRawtoDigi${ProcessRECO%\*})">>$PYFileName
	fi
	echo "process.outpath = cms.EndPath(process.output)">>$PYFileName
    else
	echo "process.add_(">>$PYFileName
	echo 'cms.Service("ISpyService",'>>$PYFileName
	echo "outputFileName = cms.untracked.string('$SaveTo$IgFileName'),">>$PYFileName
	echo "outputMaxEvents = cms.untracked.int32($MaxEventsEachIgFile),">>$PYFileName
	echo "online = cms.untracked.bool(False),">>$PYFileName
	echo "debug = cms.untracked.bool(False)))">>$PYFileName
	echo "process.p1=cms.Path($process_HLT$ProcessRawtoDigi$ProcessRECO""process.iSpy_sequence)">>$PYFileName
    fi
    if [ "$Debug" == "TRUE" ]; then
	echo "========================Python Code ($PYFileName)================================="
	cat $PYFileName
    fi
    if [ "$RunType" == "GRID" ]; then 
	echo "=================================== crab.cfg ====================================="
	CREATECRABJOB $1
    else 
	if [ "$Debug" != "TRUE" ]&&[ "$RunType"=="0" ]; then 
	    echo "============================= cmsRun $PYFileName ========================================"
	    cmsRun $PYFileName
	    eval "$CpToCastor"
	    if [ "$SKIM" == "TRUE" ]; then
		rm $PYFileName
	    else
		if [ -n "$DISPLAY" ]; then
		    VisLatestIgFile
		fi
		rm $PYFileName
	    fi
	else
	    BatchShNum=0
	    BatchShFileName=$BatchSh"_"$BatchShNum".sh"
	    until ! [ -f $BatchShFileName ]
	      do
	      (( BatchShNum += 1 ))
	      BatchShFileName=$BatchSh"_"$BatchShNum".sh"
	    done
	    echo "========================= Batch Code ($BatchShFileName) ======================================"
	    case $RunType in
		1)
		    Queue="1nh";;
		2)
		    Queue="8nh";;
		3)
		    Queue="1nd";;
		4)
		    Queue="2nd";;
		5)
		    Queue="1nw";;
		6)
		    Queue="2nw";;
	    esac
	    echo "#!/bin/bash" > $BatchShFileName
	    echo "cd $CMSSW_BASE/src/">>$BatchShFileName
	    echo "eval \`scramv1 runtime -sh\`">>$BatchShFileName
	    echo "cd $CurrentDir">>$BatchShFileName
	    echo "cmsRun $PYFileName">>$BatchShFileName
	    echo "rm $CurrentDir/$PYFileName">>$BatchShFileName
	    echo "rm $CurrentDir/$BatchShFileName">>$BatchShFileName
	    if [ "$SKIM" == "TRUE" ]; then
		echo "ls -lrt $SaveTo$OutputFileName">>$BatchShFileName
	    else
		echo "ls -lrt $SaveTo${IgFileName%.ig}*.ig">>$BatchShFileName
	    fi
	    if [ -n "$CpToCastor" ]; then
		echo "$CpToCastor">>$BatchShFileName
		if [ "$SKIM" == "TRUE" ]; then
		    echo "rm $SaveTo$OutputFileName">>$BatchShFileName
		else
		    echo "rm $SaveTo${IgFileName%.ig}[0-9_RT]*.ig">>$BatchShFileName
		fi
	    fi
	    cat $BatchShFileName
	    chmod +x $BatchShFileName
	    if [ "$Debug" != "TRUE" ]; then
		bsub -q $Queue $BatchShFileName
	    fi
	fi
    fi
}

if [ -z "$1" ]||[ `expr match "$1" '[Hh][Ee][Ll][Pp]'` == 4 ]||[ "$1" == "-h" ]||[ "$1" == "-H" ]; then
    HELP
    Client
else
    Logo
    if [ `expr match "$1" '[Dd][Aa][Ss]:'` == 4 ]; then
	RunType=GRID
	if [ -n "$CASTOR_HOME" ]; then
	    SaveTo=$CASTOR_HOME
	else
	    SaveTo=
	fi
	for i in `seq 2 $#`
	  do
	  eval temp=\${$i}
	  CheckOptions "$temp";DATAFormat=$?
	done
	GRIDOutputPath=$SaveTo
	SaveTo=
	if [ `expr match "$System" ".*cmslpc"` == "0" ]; then
	    rfmkdir_r $GRIDOutputPath
	    rfchmod 775 $GRIDOutputPath
	fi
	if [ "$SKIM" != "TRUE" ]; then
	    Client
	fi
	if [ "$SKIM" != "TRUE" ]&&[ -z "$RunNumber" ]; then
	    echo "You must specify the run number and event numbers by including e.g. 112343:234233-112343:235233 in the options."
	else
	    MaxEventsEachIgFile=-1
	    CRABEnvironment
	    Main ${1#[Dd][Bb][Ss]:}
	fi
    else
	if [ `expr match "$1" '/castor/'` != "0" ]||[ `expr match "$1" 'rfio:/castor/'` != "0" ]; then
	    for i in `seq 2 $#`
	      do
	      eval temp=\${$i}
	      CheckOptions "$temp";DATAFormat=$?
	    done
	    InputName=${1#"rfio:"}
	    IOName="rfio:"
	    FileName=`expr match "$InputName" '.*/\([[:graph:]]*\)'`
	    Dir=${InputName%"$FileName"}
	    NumberFiles=0
	    for i in `nsls $Dir`
	      do
	      NameMatch "$i" "$FileName";
	      if [ $? -eq 1 ]; then
		  if [ -z "$TestFile" ]; then
		      TestFile="rfio:"$Dir$i
		  fi
		  ISSTAGED=`stager_qry -M $Dir$i`
		  if [ `expr match "$ISSTAGED" '.*STAGED'` == "0" ]; then
		      if [ "$STAGEDONLY" == "TRUE" ]; then
			  continue
		      fi
		      stager_get -M $Dir$i
		  else
		      TestFile="rfio:"$Dir$i
		  fi
		  Files[$NumberFiles]=$Dir$i
		  (( NumberFiles += 1 ))
	      fi
	    done
	else
	    NumberFiles=0
	    IOName="file:"
	    IsFileName=TRUE
	    for i in `seq 1 $#`
	      do
	      eval temp=\${$i}
	      if [ -f "$temp" ]&&[ "$IsFileName" == "TRUE" ]; then
		  Files[$NumberFiles]=$temp
		  (( NumberFiles += 1 ))
	      else
		  IsFileName=FALSE
		  CheckOptions "$temp";DATAFormat=$?
	      fi
	    done
	    TestFile=${Files[0]}
	fi
	if [ "$SKIM" != "TRUE" ]&&[ -n "$DISPLAY" ]; then
	    Client
	fi
	if [ $NumberFiles -eq 0 ]; then
	    echo "No matching files were found"
	else
	    CheckAvailability;temp=$?
	    if [ $temp -ne 0 ]; then
		if [ `expr match "$SaveTo" '/tmp/'` != "0" ]&&[ $RunType -ge 0 ]&&[ $RunType -lt 7 ]; then
		    echo -e $'\a'"\e[0;31mWarning: You will not be able to find the output files.\e[00m"
		else
		    if ([ "$SKIM" != "TRUE" ]||[ "$PYFileNamePrefix"=="Skimreco_tmp" ])&&[ $DATAFormat -eq 0 ]; then
			DATAFormatDef ${TestFile};DATAFormat=$?
		    fi
		    Main
		fi
	    else
		echo -e $'\a'"\e[33;07mCommand \"eval \`scramv1 runtime -sh\`\" is needed.\e[00m"
	    fi
	fi
    fi
fi
