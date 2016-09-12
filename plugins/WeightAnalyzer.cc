// -*- C++ -*-
//
// Package:    MonteCarloInfo/WeightAnalyzer
// Class:      WeightAnalyzer
// 
/**\class WeightAnalyzer WeightAnalyzer.cc MonteCarloInfo/WeightAnalyzer/plugins/WeightAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  clint richardson
//         Created:  Fri, 11 Sep 2015 16:29:00 GMT
//
//


// system include files
#include <memory>
#include <iostream>
// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "TLorentzVector.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "TMath.h"
#include "TH1.h"
#include "TTree.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <utility> // std::pair

//
// class declaration
//

class WeightAnalyzer : public edm::EDAnalyzer {
public:
  explicit WeightAnalyzer(const edm::ParameterSet&);
  ~WeightAnalyzer();
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;
  TH1F* weightHist;
  TH1F* NumTrueHist;
  TH1F* NumPUHist;
  TH1F* NumPVHist;

  int posweightsum = 0;
  int negweightsum = 0;
  int totalcount = 0;
  int zeroweightsum = 0;
  bool printheader = true;

  edm::EDGetTokenT<GenEventInfoProduct> GEIPtoken;  
  edm::EDGetTokenT<LHERunInfoProduct> LRIPtoken;  
  edm::EDGetTokenT<std::vector<PileupSummaryInfo>> APItoken;

  //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
  virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
  //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
  //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
  
  // ----------member data ---------------------------
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
WeightAnalyzer::WeightAnalyzer(const edm::ParameterSet& iConfig)
{
  //now do what ever initialization is needed
  edm::Service<TFileService> fs;
  weightHist = fs->make<TH1F>("weightHist","MC Weight",10,-2,2); weightHist->Sumw2();
  NumTrueHist = fs->make<TH1F>("NumTrueHist","True Interactions",60,0,60); NumTrueHist->Sumw2();
  NumPUHist = fs->make<TH1F>("NumPUHist","PU Interactions",60,0,60); NumPUHist->Sumw2();
  NumPVHist = fs->make<TH1F>("NumPVHist","NPV",60,0,60); NumPVHist->Sumw2();

  edm::InputTag GEIPtag("generator");
  GEIPtoken = consumes<GenEventInfoProduct>(GEIPtag);
  
  edm::InputTag LRIPtag("externalLHEProducer");
  LRIPtoken = consumes<LHERunInfoProduct, edm::InRun>(LRIPtag);

  edm::InputTag APItag("slimmedAddPileupInfo");
  APItoken = consumes<std::vector<PileupSummaryInfo>>(APItag);

}


WeightAnalyzer::~WeightAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void WeightAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace std;

  edm::Handle<GenEventInfoProduct> genEvtInfo;
  iEvent.getByToken(GEIPtoken,genEvtInfo);
  
  // Original method
  if(genEvtInfo->weight() > 0) weightHist->Fill(1.0);
  else weightHist->Fill(-1.0);
  double weight = genEvtInfo->weight()/fabs(genEvtInfo->weight());

  // Count
  double countweight = genEvtInfo->weight();
  if(countweight < 0) negweightsum++;
  else if(countweight > 0) posweightsum++;
  else zeroweightsum++;
  totalcount++;

  // Pileup
  edm::Handle<std::vector<PileupSummaryInfo>> PupInfo;
  iEvent.getByToken(APItoken, PupInfo);

  int NumTrueInts = -1;
  int NumPUInts = -1;
  for(std::vector<PileupSummaryInfo>::const_iterator PVI = PupInfo->begin(); PVI != PupInfo->end(); ++PVI){
    int BX = PVI->getBunchCrossing();
    if(BX == 0){
      NumTrueInts = PVI->getTrueNumInteractions();
      NumPUInts = PVI->getPU_NumInteractions();
    }
  }

  NumTrueHist->Fill(NumTrueInts,weight);
  NumPUHist->Fill(NumPUInts,weight);

  edm::Handle<std::vector<reco::Vertex> > pvHandle;
  edm::InputTag pvCollection_it("offlineSlimmedPrimaryVertices");
  iEvent.getByLabel(pvCollection_it, pvHandle);
  std::vector<reco::Vertex> goodPVs;
  goodPVs = *(pvHandle.product());
  
  int npv = (int)goodPVs.size();
  NumPVHist->Fill(npv,weight);

}

// ------------ method called once each job just before starting event loop  ------------
void 
WeightAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
WeightAnalyzer::endJob() 
{

  printf("Total events processed = %i\n",totalcount);
  printf("Positive weight = %i\n",posweightsum);
  printf("Negative weight = %i\n",negweightsum);
  printf("Zero weight = %i\n",zeroweightsum);
  printf("Adjusted count = %i\n",posweightsum - negweightsum);
}

// ------------ method called when starting to processes a run  ------------
/*
void 
WeightAnalyzer::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a run  ------------

void 
WeightAnalyzer::endRun(edm::Run const& iRun, edm::EventSetup const&)
{
  edm::Handle<LHERunInfoProduct> run; 
  typedef std::vector<LHERunInfoProduct::Header>::const_iterator headers_const_iterator;
  
  if(iRun.getByToken(LRIPtoken,run)){
    LHERunInfoProduct myLHERunInfoProduct = *(run.product());
    
    int pdfidx = run->heprup().PDFSUP.first;
    std::cout << "Nominal PDFset: " << pdfidx << std::endl;

    if(printheader){
      for (headers_const_iterator iter=myLHERunInfoProduct.headers_begin(); iter!=myLHERunInfoProduct.headers_end(); iter++){
	std::cout << iter->tag() << std::endl;
	std::vector<std::string> lines = iter->lines();
	for (unsigned int iLine = 0; iLine<lines.size(); iLine++) {
	  std::cout << lines.at(iLine);
	}
      }  
      printheader = false;
    }
  }else{ std::cout << "No externalLHEProducer" << std::endl; }
}


// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
WeightAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
WeightAnalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
WeightAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(WeightAnalyzer);

