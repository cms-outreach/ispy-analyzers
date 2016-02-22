iSpy CMSSW Analyzers

This package is for production of event display files for the CMS experiment at the LHC. More information on the output format
can be found here: http://cms-outreach.github.io/ispy/

To use:

* Create a CMSSW environment: 

```
    cmsrel CMSSW_8_0_0
```

* Change to the CMSSW_5_3_30/src/ directory:

```
    cd CMSSW_8_0_0/src/
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
    cmsRun ispy_80X_cfg.py
```

* View the ouput in http://cern.ch/ispy-webgl

