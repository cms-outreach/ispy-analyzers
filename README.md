# iSpy CMSSW Analyzers

This package is for production of event display files for the CMS experiment at the LHC. 

Briefly, the event display files, with a file extension `.ig` are zip archives of JSON files. A separate JSON file is made for each event. The JSON file is then easily read in the browser-based event display [iSpy WebGL](https://github.com/cms-outreach/ispy-webgl)

The `ig` files are created by running this code in a CMSSW environment, extracting out the necessary event and geometry information.

## Quick start

* Create a CMSSW environment: 

```
    cmsrel CMSSW_8_0_0
```

* Change to the CMSSW_8_0_0/src/ directory:

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
    cmsRun python/ispy_80X_cfg.py
```

* View the ouput in http://cern.ch/ispy-webgl-dev


## Configuration file walk-through

This is a typical cfg file for processing AOD:

```
import FWCore.ParameterSet.Config as cms

process = cms.Process("ISPY")

process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load("Configuration.StandardSequences.GeometryDB_cff")

process.GlobalTag.globaltag = '102X_dataRun2_Prompt_v11'

import FWCore.Utilities.FileUtils as FileUtils
```
Here one specifies the input file
```
process.source = cms.Source('PoolSource',
                            fileNames = cms.untracked.vstring('file://pickevents_2016_AOD.root'),
)

from FWCore.MessageLogger.MessageLogger_cfi import *
```
Here is the output file name. 
```
process.add_(
    cms.Service("ISpyService",
    outputFileName = cms.untracked.string('ig_output.ig'),
    outputIg = cms.untracked.bool(True),
```
These are the number of events to write per ig file. The file name specified above is actually a pattern.
For example, given the maximum number of events below the first 10 events will be written to `ig_output_0.ig`,
the next 10 will be written to `ig_output_1.ig`, etc.
```
    outputMaxEvents = cms.untracked.int32(10), 
    debug = cms.untracked.bool(True)
    )
)

process.options = cms.untracked.PSet(
    SkipEvent = cms.untracked.vstring('ProductNotFound')
    )
```
These are the number of events to cycle through in the input root file. If `-1` then it will
go through all of them.
```
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)
```
It is often useful to explicitly load the configurations as below. 
```
# Load things explicitly here; mostly AOD stuff
process.load("ISpy.Analyzers.ISpyEvent_cfi")
process.load('ISpy.Analyzers.ISpyCSCRecHit2D_cfi')
process.load('ISpy.Analyzers.ISpyCSCSegment_cfi')
process.load('ISpy.Analyzers.ISpyDTRecHit_cfi')
process.load('ISpy.Analyzers.ISpyDTRecSegment4D_cfi')
process.load('ISpy.Analyzers.ISpyEBRecHit_cfi')
process.load('ISpy.Analyzers.ISpyEERecHit_cfi')
process.load('ISpy.Analyzers.ISpyESRecHit_cfi')
process.load('ISpy.Analyzers.ISpyHBRecHit_cfi')
process.load('ISpy.Analyzers.ISpyHERecHit_cfi')
process.load('ISpy.Analyzers.ISpyHFRecHit_cfi')
process.load('ISpy.Analyzers.ISpyHORecHit_cfi')
process.load('ISpy.Analyzers.ISpyMET_cfi')
process.load('ISpy.Analyzers.ISpyPFMET_cfi')
process.load('ISpy.Analyzers.ISpyMuon_cfi')
process.load('ISpy.Analyzers.ISpyJet_cfi')
process.load('ISpy.Analyzers.ISpyPFJet_cfi')
process.load('ISpy.Analyzers.ISpyPhoton_cfi')
process.load('ISpy.Analyzers.ISpyRPCRecHit_cfi')
process.load('ISpy.Analyzers.ISpySuperCluster_cfi')
```
Since TrackExtras are not available in AOD the `TrackExtrapolation` collection is 
used. This handles tracks and gsfElectrons
```
process.load('ISpy.Analyzers.ISpyTrackExtrapolation_cfi')
process.load('ISpy.Analyzers.ISpyTriggerEvent_cfi')
process.load('ISpy.Analyzers.ISpyVertex_cfi')

```
There are default input tag names for all of the objects. Here we specify them.
```
process.ISpyCSCRecHit2D.iSpyCSCRecHit2DTag = cms.InputTag("csc2DRecHits")
process.ISpyCSCSegment.iSpyCSCSegmentTag = cms.InputTag("cscSegments")
process.ISpyDTRecHit.iSpyDTRecHitTag = cms.InputTag('dt1DRecHits')
process.ISpyDTRecSegment4D.iSpyDTRecSegment4DTag = cms.InputTag('dt4DSegments')

process.ISpyEBRecHit.iSpyEBRecHitTag = cms.InputTag('reducedEcalRecHitsEB')
process.ISpyEERecHit.iSpyEERecHitTag = cms.InputTag('reducedEcalRecHitsEE')
process.ISpyESRecHit.iSpyESRecHitTag = cms.InputTag('reducedEcalRecHitsES')

process.ISpyHBRecHit.iSpyHBRecHitTag = cms.InputTag("reducedHcalRecHits:hbhereco")
process.ISpyHERecHit.iSpyHERecHitTag = cms.InputTag("reducedHcalRecHits:hbhereco")
process.ISpyHFRecHit.iSpyHFRecHitTag = cms.InputTag("reducedHcalRecHits:hfreco")
process.ISpyHORecHit.iSpyHORecHitTag = cms.InputTag("reducedHcalRecHits:horeco")

process.ISpyJet.iSpyJetTag = cms.InputTag("iterativeCone5CaloJets")

process.ISpyMET.iSpyMETTag = cms.InputTag("htMetIC5")
process.ISpyMuon.iSpyMuonTag = cms.InputTag("muons")
process.ISpyPFJet.iSpyPFJetTag = cms.InputTag('ak4PFJets')
process.ISpyPhoton.iSpyPhotonTag = cms.InputTag('photons')
process.ISpyRPCRecHit.iSpyRPCRecHitTag = cms.InputTag("rpcRecHits")
process.ISpyVertex.iSpyVertexTag = cms.InputTag('offlinePrimaryVertices')
```
Now we specify the path and the processes.
```
process.iSpy = cms.Path(process.ISpyEvent*
                        process.ISpyCSCRecHit2D*
                        process.ISpyCSCSegment*
                        process.ISpyDTRecHit*
                        process.ISpyDTRecSegment4D*
                        process.ISpyEBRecHit*
                        process.ISpyEERecHit*
                        process.ISpyESRecHit*
                        process.ISpyHBRecHit*
                        process.ISpyHERecHit*
                        process.ISpyHFRecHit*
                        process.ISpyHORecHit*
                        process.ISpyMuon*
                        process.ISpyPFJet*
                        process.ISpyPFMET*
                        process.ISpyPhoton*
                        process.ISpyRPCRecHit*
                        process.ISpyTrackExtrapolation*
                        process.ISpyVertex)

process.schedule = cms.Schedule(process.iSpy)
```

## The ig file format

A `.ig` file is a zip archive (unzippable via unzip or similar) containing one file per event. Each event file uses a relational database-like structure, described using JSON.

ig files have a flat directory structure with a separate directory per run (run directory). Each run directory contains one or more files describing a given event (ig event file) each. The associated run number and the event number are, respectively, the file name for the run directory and for the ig event file.

The ig-event-file format is a valid JSON or python dictionary. The format defines three categories of objects: types, collections and associations-sets.

Syntax:
```
ig-event-file ::= { 'Types': type-descriptions, 'Collections': collections, 'Associations': association-sets }
```
### Types

Types are a mapping of a type-name and its associated type-description. The type-description defines the types of the objects that are to be found in the Collection which uses the same string as the type-name to be identified. The attributes (i.e. columns) of a Collection are defined by an attribute name and an attribute type. The former is just a string label, while the second one define the kind of object which is stored in the attribute, all of which have a C++ equivalent. Possible attribute types include:

<ul>
<li>simple numeric types: <em>int</em>, <em>long</em>, and <em>double</em>, which map to their C++ equivalents.</li>
<li>text-like elements: <em>string</em>, which map to <code>std::string</code>
</li>
<li>vector like quantities: <em>v2d</em>, <em>v3d</em>, <em>v4d</em> which map to <code>IgV2d</code>, <code>IgV3d</code>, <code>IgV4d</code>.</li>
</ul>

Syntax:
```
type-descriptions ::= { type-description, ... }

type-description ::= "type-name": [attributes-description, ...]

type-name ::= string-literal

attribute-description ::= ["attribute-name", "attribute-type"]

attribute-name ::= string-literal

string-literal ::= [A-Za-z_][A-Za-z_/0-9]*

attribute-type ::= int OR long OR double OR string OR v2d OR v3d OR v4d
```
### Collections

Collections are arrays of rows, identified by a collection-name. The collection-name is a string-literal which matches one of the type-names defined in types. Rows are arrays of values, where the type of the value matches the attribute-type of the attribute-description with the same position in the type-description which has its type-name matching the collection name.

This is the actual data contained in a file.

Syntax:
```
collections ::= {collection, ... }

collection ::= "type-name": [row, ...]

row ::= [ value, ...]

value ::= string-literal OR numeric-literal OR vector-literal

numeric-literal ::= [0-9]+[.][0-9]

vector-literal ::= [numeric-literal, numeric-literal, ...]
```

### Association sets

An association defines a relationship between two rows found in any (possibly the same) collections. They are uniquely identified by two couples of numbers, the first one uniquely identifying the left object of the association, the other identifying the right object. The first element of this couple of numbers is the so called collection id while the second one is called object id. The collection id is the index of the collection type within the Types table, while the object id is the index of the object within the collection. Such a couple of numbers is called object reference. An association-set is an array of associations which share some common meaning decided by the writer of the ig-file.

Syntax:

```
associations-sets ::= { association-set, ... }

association-set ::= "association-set-name": [association, ...]

association ::= [left-object, right-object]

left-object ::= object-reference

right-object ::= object-reference

object-reference ::= [collection-id, object-id]

collection-id ::= index-literal

object-id ::= index-literal

index-literal ::= [0-9]+
```

### Example

Here is a simple example of what an ig file looks like.
```
{
 "Types": {"Tracks_V1": [["x", "double"],["y", "double"],["z", "double"],
                         ["px", "double"],["py","double"],["pz", "double"]],
           "Clusters_V1": [["x", "double"],["y", "double"],["z", "double"],
                           ["e", "double"]]
          },
 "Collections": {"Tracks_V1": [[0, 0, 0, 0, 0, 0],
                               [1, 1, 1, 1, 1, 1],
                               [2, 2, 2, 2, 2, 2],
                               [3, 3, 3, 3, 3, 3],
                               [4, 4, 4, 4, 4, 4],
                               [5, 5, 5, 5, 5, 5],
                               [6, 6, 6, 6, 6, 6],
                               [7, 7, 7, 7, 7, 7],
                               [8, 8, 8, 8, 8, 8],
                               [9, 9, 9, 9, 9, 9]],
                 "Clusters_V1": [[0, 0, 0, 0],
                                 [1, 1, 1, 1],
                                 [2, 2, 2, 2],
                                 [3, 3, 3, 3],
                                 [4, 4, 4, 4],
                                 [5, 5, 5, 5],
                                 [6, 6, 6, 6],
                                 [7, 7, 7, 7],
                                 [8, 8, 8, 8],
                                 [9, 9, 9, 9]]
                 },
 "Associations": {"TrackClusters_V1": [[[0, 0], [1, 0]],
                                       [[0, 1], [1, 1]],
                                       [[0, 2], [1, 2]],
                                       [[0, 3], [1, 3]],
                                       [[0, 4], [1, 4]],
                                       [[0, 5], [1, 5]],
                                       [[0, 6], [1, 6]],
                                       [[0, 7], [1, 7]],
                                       [[0, 8], [1, 8]],
                                       [[0, 9], [1, 9]]],
                  "TrackClusters2_V1": [[[0, 0], [1, 0]],
                                        [[0, 1], [1, 0]],
                                        [[0, 2], [1, 0]],
                                        [[0, 3], [1, 0]],
                                        [[0, 4], [1, 0]],
                                        [[0, 5], [1, 0]],
                                        [[0, 6], [1, 0]],
                                        [[0, 7], [1, 0]],
                                        [[0, 8], [1, 0]],
                                        [[0, 9], [1, 0]]]
                  }
}
```


