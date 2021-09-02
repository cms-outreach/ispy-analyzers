This package is for production of event display files for the CMS experiment
at the LHC. This particular branch is for use with the CMS public data release
of [2.76 TeV PbPb collision data from 2011](http://opendata.cern.ch/search?page=1&size=20&q=&subtype=Collision&type=Dataset&year=2011&experiment=CMS&keywords=heavy-ion%20physics&collision_type=PbPb).

See [the page for the HIDiMuon dataset](http://opendata.cern.ch/record/14012) for more information (and context for
the instructions below).

The following may be done in either the Virtual Machine (see links [here](http://opendata.cern.ch/record/467)) or Docker container. The instructions are essentially same but the starting points are different. The Docker container will already have a `CMSSW_4_4_7` release.  See below.

Also, in the case of the Docker container you may fetch an image and start a container using the command
```
docker run -it cmscloud/cmssw:4_4_7 /bin/bash
```
which will bring you right to `CMSSW_4_4_7/src` in the container.

**In the VM start from here.**

* Create a CMSSW environment:

```
    cmsrel CMSSW_4_4_7
```

* Change to the CMSSW_4_4_7/src/ directory:

```
    cd CMSSW_4_4_7/src/
```

* Initialize the CMSSW environment:

```
    cmsenv
```

**In the Docker container start from here.**

* Clone the necessary source code:

```
    git clone -b heavyion2011 git://github.com/cms-outreach/ispy-analyzers.git ISpy/Analyzers
    git clone git://github.com/cms-outreach/ispy-services.git ISpy/Services
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
    cmsRun HIDiMuon_cfg.py
```

which will produce a file with the name HIDiMuon_0.ig

You may do something similar for other datasets. The file produceIg.py file has useful notes as well.

Load up the `.ig` file [here](https://cern.ch/ispy-webgl-dev). 
