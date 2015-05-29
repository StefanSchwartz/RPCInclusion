##################################
#  L1TMuon_ntuple_data.py
#  - python module that runs /plugins/rpc_tuplizer.cc
#  - creates ntuple of L1TMuon trigger primitives and tracks
#  - currently built for real data
#
#  - written by David Curry
##################################


import FWCore.ParameterSet.Config as cms


# Load all process ===================================================
process = cms.Process('L1TMUONtuple')
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.Geometry.GeometryIdeal_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('L1TriggerDPGUpgrade.L1TMuon.L1TMuonTriggerPrimitiveProducer_cfi')
process.load('L1TriggerDPGUpgrade.L1TMuon.L1CSCTFTrackConverter_cfi')
process.load('L1TriggerDPGUpgrade.L1TMuon.L1DTTFTrackConverter_cfi')
process.load('L1TriggerDPGUpgrade.L1TMuon.L1RPCTFTrackConverter_cfi')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')

# load reco process
process.load("RecoMuon.TrackingTools.MuonServiceProxy_cff")
process.load("RecoMuon.TrackingTools.MuonTrackLoader_cff")
process.load('Configuration/StandardSequences/SimL1Emulator_cff')
process.load("Configuration.StandardSequences.RawToDigi_Data_cff")
process.load('Configuration/StandardSequences/EndOfProcess_cff')
process.load("Configuration.StandardSequences.Generator_cff")

#=======================================================================


# Global Tags
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag.globaltag = 'GR_P_V41::All'

# Display event progress during runtime
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

# How many events to run over
process.maxEvents = cms.untracked.PSet( input=cms.untracked.int32(100))

# ------- Main Process: rpc_tuplizer.cc ----------------------------------------------------------

process.tuple = cms.EDAnalyzer("rpc_tuplizer",
                               genSrc       = cms.InputTag(""),
                               csctfTag     = cms.InputTag("L1CSCTFTrackConverter"),
                               cscSegTag    = cms.InputTag("cscSegments"),
                               rpcTPTag     = cms.InputTag("L1TMuonTriggerPrimitives","RPC"),
                               muonsTag     = cms.InputTag("muons"),
                               outputFile   = cms.string("csc_raw_test.root"),
                               printLevel   = cms.untracked.int32(3),
                               isMC         = cms.untracked.int32(0),
                               lutParam     = cms.PSet(
                                                       isBeamStartConf = cms.untracked.bool(True),
                                                       ReadPtLUT = cms.bool(False),
                                                       PtMethod = cms.untracked.uint32(32)
                                                       )
                               )

#--------------------------------------------------------------------------------------------------

# ------------- import Data -----------------------------------------------------------------------
readFiles = cms.untracked.vstring()
secFiles  = cms.untracked.vstring()
process.source = cms.Source(
        'PoolSource',
            fileNames = readFiles,
            secondaryFileNames= secFiles
            #                            , eventsToProcess = cms.untracked.VEventRange('201196:265380261')
            )

readFiles.extend([
    
    #'file:/afs/cern.ch/work/e/evka/public/local_run_223145/csctf.root'
    #'root://eoscms//eos/cms/store/data/Run2012D/SingleMu/RAW-RECO/ZMu-22Jan2013-v1/10000/FC9D8210-E2A7-E211-8E8B-485B39800C34.root'
    'file:/exports/uftrig01a/dcurry/data/rpc/78EF511A-94EB-E111-86F6-00237DDC5BBC.root'
    
    ])

secFiles.extend([

    'file:/exports/uftrig01a/dcurry/data/rpc/084C3CD1-8AE9-E111-BD4F-001D09F25109.root'
    
    ])








#--------------------------------------------------------------------------------------------------


# --------- L1TMuonTriggerPrimitives --------------------------------------------------------------
process.L1TMuonTriggerPrimitives.CSC.src = cms.InputTag('csctfDigis')
process.L1TMuonTriggerPrimitives.RPC.src = cms.InputTag('muonRPCDigis')
process.L1TMuonTriggerPrimitives.DT.src  = cms.InputTag('dttfDigis')

# --------- L1TrackConverters ---------------------------------------------------------------------
process.L1CSCTFTrackConverter.CSCTrackSrc = cms.InputTag('csctfDigis')

#-----------Final Process -------------------------------------------------------------------------

process.L1TMuonSeq = cms.Sequence(
                                   process.csctfDigis               +
                                   process.dttfDigis                +
                                   process.muonRPCDigis             +
                                   process.L1TMuonTriggerPrimitives +
                                   process.L1CSCTFTrackConverter    +
                                   process.tuple
                                   )
#---------------------------------------------------------------------------------------------------

process.L1TMuonPath = cms.Path(process.L1TMuonSeq)
process.Schedule = cms.Schedule(process.L1TMuonPath)



        
