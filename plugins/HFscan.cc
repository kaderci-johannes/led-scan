// -*- C++ -*-
//
// Package:    HFscan
// Class:      HFscan
// 
/**\class HFscan HFscan.cc UserCode/HFscan/src/HFscan.cc

   Description: [one line class summary]

   Implementation:
   [Notes on implementation]
*/
//

// user include files
#include "EventFilter/HcalRawToDigi/interface/HcalUnpacker.h"
#include "EventFilter/HcalRawToDigi/interface/HcalDCCHeader.h"
#include "EventFilter/HcalRawToDigi/interface/HcalDTCHeader.h"
#include "EventFilter/HcalRawToDigi/interface/AMC13Header.h"
#include "EventFilter/HcalRawToDigi/interface/HcalHTRData.h"
#include "EventFilter/HcalRawToDigi/interface/HcalUHTRData.h"
#include "DataFormats/HcalDetId/interface/HcalOtherDetId.h"
#include "DataFormats/HcalDigi/interface/HcalQIESample.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "EventFilter/HcalRawToDigi/interface/HcalTTPUnpacker.h"



#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "EventFilter/HcalRawToDigi/interface/HcalHTRData.h"
#include "EventFilter/HcalRawToDigi/interface/HcalDCCHeader.h"
#include "EventFilter/HcalRawToDigi/interface/HcalUnpacker.h"
#include "DataFormats/HcalDetId/interface/HcalOtherDetId.h"
#include "DataFormats/HcalDigi/interface/HcalQIESample.h"
#include "DataFormats/HcalDigi/interface/QIE10DataFrame.h"
#include "DataFormats/HcalDetId/interface/HcalSubdetector.h"
#include "DataFormats/HcalDetId/interface/HcalCalibDetId.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "DataFormats/FEDRawData/interface/FEDHeader.h"
#include "DataFormats/FEDRawData/interface/FEDTrailer.h"
#include "DataFormats/FEDRawData/interface/FEDNumbering.h"
#include "DataFormats/FEDRawData/interface/FEDRawData.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "CalibFormats/HcalObjects/interface/HcalDbService.h"
#include "CalibFormats/HcalObjects/interface/HcalDbRecord.h"
#include "CalibFormats/HcalObjects/interface/HcalCalibrations.h"
#include "CalibFormats/HcalObjects/interface/HcalCoderDb.h"

//#include "HFcommissioning/Analysis/interface/ADC_Conversion.h"
#include "RecoTBCalo/HcalTBObjectUnpacker/interface/HcalTBSlowDataUnpacker.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TF1.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TFile.h"
#include "TSystem.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include "unpack_name.h"
#include <bitset>

using namespace std;


#define NUMADCS 256


//#define nQIEs   0
#define iPHIe  36
#define iETAe  26
#define nD      4   // # of depths
#define nP     20   // # of points

// NEEDS UPDATING
double adc2fC_QIE10[NUMADCS]={
  // - - - - - - - range 0 - - - - - - - -
  //subrange0 
  1.58, 4.73, 7.88, 11.0, 14.2, 17.3, 20.5, 23.6, 
  26.8, 29.9, 33.1, 36.2, 39.4, 42.5, 45.7, 48.8,
  //subrange1
  53.6, 60.1, 66.6, 73.0, 79.5, 86.0, 92.5, 98.9,
  105, 112, 118, 125, 131, 138, 144, 151,
  //subrange2
  157, 164, 170, 177, 186, 199, 212, 225,
  238, 251, 264, 277, 289, 302, 315, 328,
  //subrange3
  341, 354, 367, 380, 393, 406, 418, 431,
  444, 464, 490, 516, 542, 568, 594, 620,

  // - - - - - - - range 1 - - - - - - - -
  //subrange0
  569, 594, 619, 645, 670, 695, 720, 745,
  771, 796, 821, 846, 871, 897, 922, 947,
  //subrange1
  960, 1010, 1060, 1120, 1170, 1220, 1270, 1320,
  1370, 1430, 1480, 1530, 1580, 1630, 1690, 1740,
  //subrange2
  1790, 1840, 1890, 1940,  2020, 2120, 2230, 2330,
  2430, 2540, 2640, 2740, 2850, 2950, 3050, 3150,
  //subrange3
  3260, 3360, 3460, 3570, 3670, 3770, 3880, 3980,
  4080, 4240, 4450, 4650, 4860, 5070, 5280, 5490,
  
  // - - - - - - - range 2 - - - - - - - - 
  //subrange0
  5080, 5280, 5480, 5680, 5880, 6080, 6280, 6480,
  6680, 6890, 7090, 7290, 7490, 7690, 7890, 8090,
  //subrange1
  8400, 8810, 9220, 9630, 10000, 10400, 10900, 11300,
  11700, 12100, 12500, 12900, 13300, 13700, 14100, 14500,
  //subrange2
  15000, 15400, 15800, 16200, 16800, 17600, 18400, 19300,
  20100, 20900, 21700, 22500, 23400, 24200, 25000, 25800,
  //subrange3
  26600, 27500, 28300, 29100, 29900, 30700, 31600, 32400,
  33200, 34400, 36100, 37700, 39400, 41000, 42700, 44300,

  // - - - - - - - range 3 - - - - - - - - -
  //subrange0
  41100, 42700, 44300, 45900, 47600, 49200, 50800, 52500,
  54100, 55700, 57400, 59000, 60600, 62200, 63900, 65500,
  //subrange1
  68000, 71300, 74700, 78000, 81400, 84700, 88000, 91400,
  94700, 98100, 101000, 105000, 108000, 111000, 115000, 118000,
  //subrange2
  121000, 125000, 128000, 131000, 137000, 145000, 152000, 160000,
  168000, 176000, 183000, 191000, 199000, 206000, 214000, 222000,
  //subrange3
  230000, 237000, 245000, 253000, 261000, 268000, 276000, 284000,
  291000, 302000, 316000, 329000, 343000, 356000, 370000, 384000};
////////////////////////////////////////////////////////////////////////////////////////////
static const float adc2fC[128]={-0.5,0.5,1.5,2.5,3.5,4.5,5.5,6.5,7.5,8.5,9.5, 10.5,11.5,12.5,
		   13.5,15.,17.,19.,21.,23.,25.,27.,29.5,32.5,35.5,38.5,42.,46.,50.,54.5,59.5,
		   64.5,59.5,64.5,69.5,74.5,79.5,84.5,89.5,94.5,99.5,104.5,109.5,114.5,119.5,
		   124.5,129.5,137.,147.,157.,167.,177.,187.,197.,209.5,224.5,239.5,254.5,272.,
		   292.,312.,334.5,359.5,384.5,359.5,384.5,409.5,434.5,459.5,484.5,509.5,534.5,
		   559.5,584.5,609.5,634.5,659.5,684.5,709.5,747.,797.,847.,897.,947.,997.,
		   1047.,1109.5,1184.5,1259.5,1334.5,1422.,1522.,1622.,1734.5,1859.5,1984.5,
		   1859.5,1984.5,2109.5,2234.5,2359.5,2484.5,2609.5,2734.5,2859.5,2984.5,
		   3109.5,3234.5,3359.5,3484.5,3609.5,3797.,4047.,4297.,4547.,4797.,5047.,
		   5297.,5609.5,5984.5,6359.5,6734.5,7172.,7672.,8172.,8734.5,9359.5,9984.5};
////////////////////////////////////////////////////////////////////////////////////////////







TFile *_file;
int EventNumber;
int point;

TH1F *AllSum[iETAe][iPHIe][nD][nP];			// A 1D histogram for each (ieta, iphi, depth). x axis: Charge, y axis: counts
TProfile *psd[iETAe][iPHIe][nD][nP];			// An array of profile histograms that contain pulse shape distributions. x axis: Time slice, y: Average ADC, y_err: STDEV ADC
TProfile2D *hfp[nD][nP];				// An array of 2D histograms for various depths of HF+. x axis: ieta, y: iphi, z: Mean charge
TProfile2D *hfm[nD][nP];				// An array of 2D histograms for various depths of HF-. x axis: ieta, y: iphi, z: Mean charge
TProfile2D *stdevp[nD][nP];				// An array of 2D histograms for various depths of HF+. x axis: ieta, y: iphi, z: Stdev of the pulse shape distribution.
TProfile2D *stdevm[nD][nP];				// An array of 2D histograms for various depths of HF-. x axis: ieta, y: iphi, z: Stdev of the pulse shape distribution.

//TF1 *fit;						// Fit function for the pulse shape distribution.

//
// class declaration
//

class HFscan : public edm::EDAnalyzer {
public:
  explicit HFscan(const edm::ParameterSet&);
  ~HFscan();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  virtual void beginJob() ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);

  virtual void endJob() ;


  int numChannels;
  string _outFileName;
  int _verbosity;


  virtual void beginRun(edm::Run const&, edm::EventSetup const&);
  virtual void endRun(edm::Run const&, edm::EventSetup const&);
  virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
  virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

  edm::EDGetTokenT<HcalDataFrameContainer<QIE10DataFrame> > tok_QIE10DigiCollection_;
  edm::Handle<QIE10DigiCollection> qie10DigiCollection;

  edm::EDGetTokenT<HFDigiCollection> hf_token;

  //Slow data stuffs - raw_toiken is slow data
  bool _sequencer_flag;
  edm::EDGetTokenT<FEDRawDataCollection> raw_token;
  edm::Handle<FEDRawDataCollection> raw_collection;

  hcaltb::HcalTBSlowDataUnpacker sdp;

};




HFscan::HFscan(const edm::ParameterSet& iConfig) :
  _outFileName(iConfig.getUntrackedParameter<string>("OutFileName")),
  _verbosity(iConfig.getUntrackedParameter<int>("Verbosity")),
  _sequencer_flag(iConfig.getUntrackedParameter<int>("Sequencer_Flag"))
{ 

  char hName[1024];	// Histogram name
  char hTitle[1024];	// Histogram title
  char fName[1024];	// Folder name

  EventNumber = 0;
  point = 0;

  _file = new TFile(_outFileName.c_str(), "recreate");

  _file->cd();


  tok_QIE10DigiCollection_ = consumes<HcalDataFrameContainer<QIE10DataFrame> >(edm::InputTag("hcalDigis"));
  hf_token = consumes<HFDigiCollection>(edm::InputTag("hcalDigis"));
  raw_token = consumes<FEDRawDataCollection>(edm::InputTag("source")); //Slow data

  _file->mkdir("AllSumCh");
  _file->mkdir("2D");
  _file->mkdir("PSD");
  for(int i=0;i<nP;i++){
    sprintf(fName,"AllSumCh/Point_%i",i+1);
    _file->mkdir(fName);
    sprintf(fName,"2D/Point_%i",i+1);
    _file->mkdir(fName);
    sprintf(fName,"PSD/Point_%i",i+1);
    _file->mkdir(fName);
  }


  for(int i=0;i<iETAe;i++) for(int j=0;j<iPHIe;j++) for(int k=0;k<nD;k++) for(int l=0;l<nP;l++) psd[i][j][k][l] = new TProfile;	
 // This is needed for the psd[i][j][k]->GetEntries() that will come up not to crash. nTS is needed to define the # of bins and the x-axis range. Therefore, the ranges, etc. of psd[i][j][k] are defined in the HFscan::analyze part.

  for(int i=0;i<iETAe;i++){
    for(int j=0;j<iPHIe;j++){
      for(int k=0;k<nD;k++){
        for(int l=0;l<nP;l++){
          if(i<13){
            sprintf(hName,"SumCharge_m%i_%i_%i_%i",41-i,2*j+1,k+1,l+1);
            sprintf(hTitle,"PMT Charge (ieta: %i, iphi: %i, Depth: %i, Point: %i)",i-41,2*j+1,k+1,l+1);
          }
          else{
            sprintf(hName,"SumCharge_p%i_%i_%i_%i",i+16,2*j+1,k+1,l+1);
            sprintf(hTitle,"PMT Charge (ieta: %i, iphi: %i, Depth: %i, Point: %i)",i+16,2*j+1,k+1, l+1);
          }
          AllSum[i][j][k][l] = new TH1F(hName,hTitle,200,0,1000);
        }
      }
    }
  }

  for(int i=0;i<nD;i++){
    for(int l=0;l<nP;l++){
      sprintf(hName,"hfp_%i_%i",i+1,l+1);
      sprintf(hTitle,"PMT Mean Charge (HF+ Depth %i, Point: %i)",i+1, l+1);
      hfp[i][l] = new TProfile2D(hName,hTitle,13,28.5,41.5,36,0,72);
      sprintf(hName,"hfm_%i_%i",i+1,l+1);
      sprintf(hTitle,"PMT Mean Charge (HF- Depth %i, Point: %i)",i+1, l+1);
      hfm[i][l] = new TProfile2D(hName,hTitle,13,-41.5,-28.5,36,0,72);
      sprintf(hName,"stdevp_%i_%i",i+1,l+1);
      sprintf(hTitle,"Stdev of the PMT Charge (HF+ Depth %i, Point: %i)",i+1, l+1);
      stdevp[i][l] = new TProfile2D(hName,hTitle,13,28.5,41.5,36,0,72);
      sprintf(hName,"stdevm_%i",i+1);
      sprintf(hTitle,"Stdev of the PMT Charge (HF- Depth %i, Point: %i)",i+1, l+1);
      stdevm[i][l] = new TProfile2D(hName,hTitle,13,-41.5,-28.5,36,0,72);
    }
  }

// for histo stuff
  numChannels=0;
}


HFscan::~HFscan()
{
  int chan=1;
  char fName[1024];
  for(int i=0;i<iETAe;i++){
    for(int j=0;j<iPHIe;j++){
      for(int k=0;k<nD;k++){
        for(int l=0;l<nP;l++){
          if(AllSum[i][j][k][l]->GetEntries()!=0){
            sprintf(fName,"AllSumCh/Point_%i",l+1);
            _file->cd(fName);
            AllSum[i][j][k][l]->SetXTitle("Charge (fC)");
            AllSum[i][j][k][l]->SetYTitle("Counts");
            AllSum[i][j][k][l]->SetMinimum(0.);
            AllSum[i][j][k][l]->Write();
            sprintf(fName,"PSD/Point_%i",l+1);
            _file->cd(fName);
            psd[i][j][k][l]->SetXTitle("TS");
            psd[i][j][k][l]->SetYTitle("Mean ADC");
            psd[i][j][k][l]->SetMinimum(0.);
            psd[i][j][k][l]->Write();
            //fit = new TF1("fit","gaus",-20.,20.);
            //psd[i][j][k]->Fit("fit","Q");
            if(i<13){
              hfm[k][l]->Fill(i-41,2*j+1,AllSum[i][j][k][l]->GetMean());
              stdevm[k][l]->Fill(i-41,2*j+1,AllSum[i][j][k][l]->GetStdDev());
            }
            else{
              hfp[k][l]->Fill(i+16,2*j+1,AllSum[i][j][k][l]->GetMean());
              stdevp[k][l]->Fill(i+16,2*j+1,AllSum[i][j][k][l]->GetStdDev());
              //stdevp[k][l]->Fill(i+16,2*j+1,fit->GetParameter("Sigma"));
            }
            if(_verbosity) cout<<chan<<" "<<point+1<<endl;
            chan++;
          }
        }
      }
    }
  }

  for(int k=0;k<nD;k++){
    for(int l=0;l<nP;l++){
      sprintf(fName,"2D/Point_%i",l+1);
      _file->cd(fName);
      hfp[k][l]->SetStats(0);
      hfp[k][l]->SetXTitle("ieta");
      hfp[k][l]->SetYTitle("iphi");
      hfp[k][l]->SetZTitle("Mean Charge (fC)");
      hfp[k][l]->Write();
      hfm[k][l]->SetStats(0);
      hfm[k][l]->SetXTitle("ieta");
      hfm[k][l]->SetYTitle("iphi");
      hfm[k][l]->SetZTitle("Mean Charge (fC)");
      hfm[k][l]->Write();
    }
  }

  for(int k=0;k<nD;k++){
    for(int l=0;l<nP;l++){
      sprintf(fName,"2D/Point_%i",l+1);
      _file->cd(fName);
      stdevp[k][l]->SetStats(0);
      stdevp[k][l]->SetXTitle("ieta");
      stdevp[k][l]->SetYTitle("iphi");
      stdevp[k][l]->SetZTitle("#sigma_{TS}");
      stdevp[k][l]->Write();
      stdevm[k][l]->SetStats(0);
      stdevm[k][l]->SetXTitle("ieta");
      stdevm[k][l]->SetYTitle("iphi");
      stdevm[k][l]->SetZTitle("#sigma_{TS}");
      stdevm[k][l]->Write();
    }
  }

  _file->Write();
  _file->Close();

}



/**************************************************************************/

//
// member functions
//


// ------------ method called for each event  ------------
void HFscan::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{

  using namespace edm;

  EventNumber++;

  //
  //  Extracting All the Collections containing useful Info
  iEvent.getByToken(tok_QIE10DigiCollection_,qie10DigiCollection);
  const QIE10DigiCollection& qie10dc=*(qie10DigiCollection);

  //  -----------------------------------------------------

  //cout << "### QIE10 Digis=" << qie10DigiCollection->size() << endl;

  //This main stuff - how to extract QIE10 information

  float SumCharge;
  for (unsigned int j=0; j < qie10dc.size(); j++){	// qie10dc.size() is 3456 for HF

    QIE10DataFrame qie10df = static_cast<QIE10DataFrame>(qie10dc[j]);
    DetId detid = qie10df.detid();
    HcalDetId hcaldetid = HcalDetId(detid);

    int depth = hcaldetid.depth(); 
    int ieta = hcaldetid.ieta();
    int iphi = hcaldetid.iphi();
    int nTS  = qie10df.samples();


    //How to extract information per time slice (nTS) and how to sum up the total charge in 10 ts

    char hName[1024];
    char hTitle[1024];

    sprintf(hTitle,"Pulse Shape Distribution (ieta: %i, iphi: %i, Depth: %i, Point: %i)",ieta,iphi,depth,point+1);

    if(ieta<0){
      if(psd[ieta+41][(iphi-1)/2][depth-1][point]->GetEntries()==0){
        sprintf(hName,"psd_m%i_%i_%i_%i",-ieta,iphi,depth,point+1);
        psd[ieta+41][(iphi-1)/2][depth-1][point] = new TProfile(hName,hTitle,nTS,-.5,nTS-.5);
      }
    }
    else{
      if(psd[ieta-16][(iphi-1)/2][depth-1][point]->GetEntries()==0){
        sprintf(hName,"psd_p%i_%i_%i_%i",ieta,iphi,depth,point+1);
        psd[ieta-16][(iphi-1)/2][depth-1][point] = new TProfile(hName,hTitle,nTS,-.5,nTS-.5);
      }
    }


    SumCharge = 0;
    for(int i=0; i<nTS; ++i){

        
      int   adc = qie10df[i].adc();    //adc counts per TS
      //int   tdc = qie10df[i].le_tdc(); //Leading edge of TDC per TS
      //int tetdc = qie10df[i].te_tdc(); //Not uset for now
      //int capid = qie10df[i].capid();  //CAPid per TS 
      //int   soi = qie10df[i].soi();  //Sample of interest - not used for now
      float charge = adc2fC_QIE10[ adc ]; //ADC count to charge conversion accordin the general conversion table


      if(ieta<0){
        psd[ieta+41][(iphi-1)/2][depth-1][point]->Fill(i,adc);
      }
      else{
        psd[ieta-16][(iphi-1)/2][depth-1][point]->Fill(i,adc);
      }

      SumCharge+=charge;
    }


    if(ieta<0){
      AllSum[ieta+41][(iphi-1)/2][depth-1][point]->Fill(SumCharge);
      //stdevm[depth-1]->Fill(ieta,iphi,psd[ieta+41][(iphi-1)/2][depth-1]->GetStdDev());
      if(_verbosity)cout<<ieta<<" "<<iphi<<" "<<depth<<" "<<point+1<<" "<<AllSum[ieta+41][(iphi-1)/2][depth-1][point]->GetName()<<" Charge: "<<SumCharge<<endl;
    }
    else{
      AllSum[ieta-16][(iphi-1)/2][depth-1][point]->Fill(SumCharge);
      //stdevp[depth-1]->Fill(ieta,iphi,psd[ieta-16][(iphi-1)/2][depth-1]->GetStdDev());
      if(_verbosity)cout<<ieta<<" "<<iphi<<" "<<depth<<" "<<point+1<<" "<<AllSum[ieta-16][(iphi-1)/2][depth-1][point]->GetName()<<" Charge: "<<SumCharge<<endl;
    } 
  }
  if(!(EventNumber%1000)) point+=1;
}

    

  



// ------------ method called once each job just before starting event loop  ------------
void 
HFscan::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
HFscan::endJob() 
{
  //      _file->Write();
  //      _file->Close();
}

// ------------ method called when starting to processes a run  ------------
void 
HFscan::beginRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
HFscan::endRun(edm::Run const&, edm::EventSetup const&)
{
  //_file->Write();
  //_file->Close();
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
HFscan::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
HFscan::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
HFscan::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(HFscan);
