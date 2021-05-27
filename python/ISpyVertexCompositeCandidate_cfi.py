import FWCore.ParameterSet.Config as cms

ISpyVertexCompositeCandidate = cms.EDAnalyzer(
    'ISpyVertexCompositeCandidate',
    iSpyVertexCompositeCandidateTag = cms.InputTag('generalV0Candidates:Lambda')
    )
