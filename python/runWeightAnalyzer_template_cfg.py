import FWCore.ParameterSet.Config as cms

process = cms.Process("TEST")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.destinations = ['cout', 'cerr']
process.MessageLogger.cerr.FwkReport.reportEvery = 5000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32 ( -1 ) )

process.source = cms.Source( "PoolSource",
#### FOR TESTING
#                             fileNames = cms.untracked.vstring(
#                             'root://eoscms.cern.ch//store/mc/RunIISpring16MiniAODv2/TTWJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8/MINIAODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/50000/5AF9DA75-2E1D-E611-AD17-D4AE527EE013.root'),
#        'root://eoscms.cern.ch//store/mc/RunIISpring16MiniAODv2/TT_TuneCUETP8M1_13TeV-powheg-pythia8/MINIAODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext4-v1/00000/004A0552-3929-E611-BD44-0025905A48F0.root'),
#### FOR CONDOR
                             fileNames = cms.untracked.vstring(CONDOR_FILELIST),
                             inputCommands = cms.untracked.vstring('keep *'),
                             )


#run the producer -- USE WHICHEVER CONFIG ARGUMENTS NEEDED
process.WeightAnalyzer = cms.EDAnalyzer(
    "WeightAnalyzer",
#    LHEEPTag   = cms.InputTag('externalLHEProducer'),
#    isX53 = cms.bool(SUBX53),
#    isWJ = cms.bool(SUBWJ),
#    JetsTag            = cms.InputTag('slimmedJets'),
#    JetsTagAK8         = cms.InputTag('slimmedJetsAK8'),
#    genParticles_it    = cms.InputTag('prunedGenParticles'),
#    DiscriminatorTag   = cms.string('pfCombinedInclusiveSecondaryVertexV2BJetTags'),
#    DiscriminatorValue = cms.double(0.800),
#    PtNBins            = cms.int32(50),
#    PtMin              = cms.double(0.),
#    PtMax              = cms.double(1000.),
#    EtaNBins           = cms.int32(48),
#    EtaMin             = cms.double(-3.0),
#    EtaMax             = cms.double(3.0)
    )

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string('CONDOR_OUTFILE.root') # for condor
#                                   fileName = cms.string('test.root') # for testing
                                   )

process.p = cms.Path(process.WeightAnalyzer)



