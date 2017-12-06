This package is for production of event display files for the CMS experiment
at the LHC. This particular branch is for use with the release of the 2012 CMS public dataset. 
See http://opendata.cern.ch for more information (and context for the instructions below).

To produce files in the VM open a terminal with the X terminal emulator (an icon bottom-left of the VM screen)
and input the commands as explained below.

* Create a CMSSW environment: 

```
    cmsrel CMSSW_5_3_32
```

* Change to the CMSSW_5_3_32/src/ directory:

```
    cd CMSSW_5_3_32/src/
```
* Initialize the CMSSW environment:

```
    cmsenv
```
* Clone the necessary source code:

```
    git clone https://github.com/cms-outreach/ispy-analyzers.git ISpy/Analyzers 
    git clone https://github.com/cms-outreach/ispy-services.git ISpy/Services
```

* Change to the ISpy/Analyzers/ directory:

```
    cd ISpy/Analyzers/ 
```

* Checkout the appropriate branch:

```
    git checkout Run2012 
```

* Change to the ISpy/Services/ directory:

```
    cd ../../ISpy/Services/ 
```

* Checkout the appropriate branch:

```
    git checkout Run2012
```

* Change back to src directory

```
    cd ../.. 
```

* Compile the code with the command:

```
    scram b
```

* Once compiled, change to ISpy/Analyzers:

```
    cd ISpy/Analyzers
```

* Run the example configuration file:

```
    cmsRun produceIg.py
```

which will produce a file with the name DoubleMuParked_Run2012B_0.ig

The file produceIg.py is annotated; make your own changes as appropriate.
