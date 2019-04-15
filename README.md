# ISpy Analyzers

This package is for production of event display files for the CMS experiment
at the LHC. This particular branch is for use with the release of the 2012 CMS public dataset. 
See http://opendata.cern.ch for more information (and context for the instructions below).

## Setup

### Virtual machine

To produce files in the [VM](http://opendata.cern.ch/record/252) open a terminal with the X terminal emulator (an icon bottom-left of the VM screen)
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

Then proceed to installation below.

### Docker

To produce files in a Docker container first [install Docker](https://docs.docker.com/install/) and then fetch a CMSSW image and create and start a container with the `docker run` command:

```
$ docker run --name opendata -it clelange/cmssw:5_3_32 /bin/bash
Unable to find image 'clelange/cmssw:5_3_32' locally
5_3_32: Pulling from clelange/cmssw
e8114d4b0d10: Pull complete 
a3eda0944a81: Pull complete 
a88502447863: Pull complete 
Digest: sha256:6b9a12992ba088a168b87df98a841d3c56dede326684f5551368fd359acfb43c
Status: Downloaded newer image for clelange/cmssw:5_3_32
Setting up CMSSW_5_3_32
CMSSW should now be available.
[15:10:10] cmsusr@eb9ecf54fd2a ~/CMSSW_5_3_32/src $
```

which when finished, should bring you to the command line prompt in the `CMSSW_5_3_32/src` directory. Now proceed with the installation instructions below.

## Installation

* Clone the necessary source code:

```
    git clone git://github.com/cms-outreach/ispy-analyzers.git ISpy/Analyzers 
    git clone git://github.com/cms-outreach/ispy-services.git ISpy/Services
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

## Run

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
