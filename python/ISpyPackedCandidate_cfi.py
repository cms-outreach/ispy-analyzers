import FWCore.ParameterSet.Config as cms

ISpyPackedCandidate = cms.EDAnalyzer('ISpyPackedCandidate',
                            iSpyPackedCandidateTag = cms.InputTag('packedPFCandidates')
                           )
