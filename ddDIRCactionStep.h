#ifndef DDDIRCACTIONSTEP_H
#define DDDIRCACTIONSTEP_H

#include "DDG4/Geant4SteppingAction.h"
#include "DDG4/Geant4StepHandler.h"
#include "TH3.h"
#include "TTree.h"
#include "TFile.h"
#include "TString.h"
#include "TMath.h"
#include "TGraph.h"
#include "THnSparse.h"

// Namespace for the AIDA detector description toolkit
namespace dd4hep {
        
  // Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

	class ddDIRCactionStep;

    class ddDIRCactionStep : public Geant4SteppingAction {
    
	protected:
		DDG4_DEFINE_ACTION_CONSTRUCTORS(ddDIRCactionStep);
		// Property: filename to output radiation map
		std::string  mRadiationMapOutputString  { };
		int          mFileNumber { };
		int          mNumEvents { };                   

	public:	
		//
		TFile*	steppingOutput;
		TTree*	DircIncidenceTree;
		//
		//---------------------------------------------------------------------------------------------------- 
		// Standard constructor with initializing arguments
		ddDIRCactionStep(Geant4Context* context, const std::string& name = "") : Geant4SteppingAction(context, name) {
			//
			declareProperty("radiationOutputString", mRadiationMapOutputString); //This is the basic string for radiation output
			declareProperty("fileNumber", mFileNumber); //file number to append to radiation output string
			declareProperty("numEvents", mNumEvents); 	//number of simulated events... needed for normalization
			//		
		} ;
		//
		//---------------------------------------------------------------------------------------------------- 
		// Default destructor
		virtual ~ddDIRCactionStep() {
			if (steppingOutput) {
				//
				//---- catch last event info!
				++nFill;
				mDircIncidence_evt	= nFill - 1;
				DircIncidenceTree	->Fill();
				std::cout << "Stepping radiation dose ---> nEvents seen = " << nFill << std::endl;
				//
				steppingOutput->cd();		// dirc incidence output file
				DircIncidenceTree->Write();	// dirc incidence output ttree
				//
 				steppingOutput->Close();
				delete steppingOutput;
			}		
		};

		//---------------------------------------------------------------------------------------------------- 
		void initializeOutputFile(){			
			//
			TString tmp(mRadiationMapOutputString);
			TString inputFileString;
			//
			//change here to write output to correct directory for your own simulations 
			//inputFileString.Form("/gpfs02/eic/ajentsch/ePIC_dd4hep_simulations/radSimOutput/%s_%d.root", tmp.Data(), mFileNumber);
			//inputFileString.Form("./radsim_%s_%d.root", tmp.Data(), mFileNumber);
			inputFileString.Form("./%s.root", tmp.Data() );
			//			
			std::cout << "ddDIRCactionStep ---> output file = " << inputFileString << std::endl;
			//
			steppingOutput    = new TFile(inputFileString, "RECREATE");
			//
			DircIncidenceTree	= new TTree("DircIncidenceTree","tree with kinematics of particles hitting dirc");			
				DircIncidenceTree->Branch("evt"    , &mDircIncidence_evt    ,    "evt/I");
				DircIncidenceTree->Branch("trackID", &mDircIncidence_trackID,"trackID/I");
				DircIncidenceTree->Branch("pdgCode", &mDircIncidence_pdgCode,"pdgCode/I");
				DircIncidenceTree->Branch("x"      , &mDircIncidence_x      ,      "x/D");
				DircIncidenceTree->Branch("y"      , &mDircIncidence_y      ,      "y/D");
				DircIncidenceTree->Branch("z"      , &mDircIncidence_z      ,      "z/D");
				DircIncidenceTree->Branch("px"     , &mDircIncidence_px     ,     "px/D");
				DircIncidenceTree->Branch("py"     , &mDircIncidence_py     ,     "py/D");
				DircIncidenceTree->Branch("pz"     , &mDircIncidence_pz     ,     "pz/D");
			//
			initialized = true;
		}

		//---------------------------------------------------------------------------------------------------- 
		// Pre-track action callback
		virtual void operator()(const G4Step* step, G4SteppingManager* mgr) override {
      		//
			if(!initialized){initializeOutputFile();}
			//
			G4Track* track	= step->GetTrack();
			int trackID 	= track->GetTrackID();
			int parentID	= track->GetParentID();
			int stepNumber	= track->GetCurrentStepNumber();
			int pdgCode		= track->GetDefinition()->GetPDGEncoding();
			double trackLen	= track->GetTrackLength();
			//
			//
			if (trackID==1 && stepNumber==1 && mDircIncidence_r>0.){	// this is the first step of new event!
				//
				++nFill;
				mDircIncidence_evt	= nFill - 1;
				DircIncidenceTree	->Fill();
				//
			}	// end new event block
			//
			//
			const G4ThreeVector preStepLocation  = step->GetPreStepPoint()->GetPosition();
			const G4ThreeVector postStepLocation = step->GetPostStepPoint()->GetPosition();
			const G4ThreeVector stepLocation     = 0.5*(preStepLocation + postStepLocation); //midpoint of step
			int stepStatus = step->GetPreStepPoint()->GetStepStatus();
			//
			if ( step->GetPreStepPoint()->GetPhysicalVolume() 
			  && step->GetPostStepPoint()->GetPhysicalVolume() ) {
				prevname = step->GetPreStepPoint()->GetPhysicalVolume()->GetName();
				postname = step->GetPostStepPoint()->GetPhysicalVolume()->GetName();
			}
			if (stepNumber > 50000 ){
				track->SetTrackStatus(fStopAndKill);		// KILL TRACK
				return;
			}
			if (trackLen > 30000./CLHEP::cm ){
				track->SetTrackStatus(fStopAndKill);		// KILL TRACK
				return;
			}
			//
			if (parentID==0 && stepStatus==1 	// parentID==0 taken to indicate PRIMARY GENERATED particle
			 &&  postname.Contains("bar_vol") 	// enters bar_vol...
			 && !prevname.Contains("bar_vol") 	// ...from some other volume
			 ){									//--> this track enters a DIRC bar in this step!!!
				DircIncidence_pos		= postStepLocation;
				DircIncidence_mom		= track->GetMomentum();
				mDircIncidence_trackID	= trackID;
				mDircIncidence_pdgCode	= pdgCode;
				mDircIncidence_x		= DircIncidence_pos.x() / CLHEP::mm;	// mm
				mDircIncidence_y		= DircIncidence_pos.y() / CLHEP::mm;	// mm
				mDircIncidence_z		= DircIncidence_pos.z() / CLHEP::mm;	// mm
				mDircIncidence_r		= std::sqrt(mDircIncidence_x*mDircIncidence_x+mDircIncidence_y*mDircIncidence_y);	// mm
				mDircIncidence_px		= DircIncidence_mom.x() / CLHEP::GeV;	// GeV/c
				mDircIncidence_py		= DircIncidence_mom.y() / CLHEP::GeV;	// GeV/c
				mDircIncidence_pz		= DircIncidence_mom.z() / CLHEP::GeV;	// GeV/c
				std::cout<<"DIRC Incidence:  trkID="<<trackID<<" parentID="<<parentID<<" step="<<stepNumber<<" pdg="<<pdgCode
						<<"\t pos: "<<mDircIncidence_x <<" "<<mDircIncidence_y <<" "<<mDircIncidence_z
						<<"\t mom: "<<mDircIncidence_px<<" "<<mDircIncidence_py<<" "<<mDircIncidence_pz
						<<std::endl;
			}
			//
//			double stepphi	= stepLocation.phi()*(180./M_PI);
// 			if (trackID==1 && (stepStatus!=4 || prevname!=postname) ){	// primary track, but don't show steps w/in same volume
// 				//
// 				printf("PRIMARY\t trackID= %6d \t pdg= %6d \t parentID= %6d \t step x,y,z = %6.1f %6.1f %6.1f \t phi= %5.1f \t %d %8.2f %d \t %s %s\n", 
// 					trackID, pdgCode, parentID,
// 					stepLocation.x(), stepLocation.y(), stepLocation.z(),
// 					stepphi,
// 					stepNumber,trackLen,stepStatus,
// 					prevname.Data(),postname.Data()
// 					);
// 				//
// 			}
			//
// 			if (pdgCode==-22 && stepNumber==1 ){	// OP at creation?
// 				//
// 				printf("OP     \t trackID= %6d \t pdg= %6d \t parentID= %6d \t step x,y,z = %6.1f %6.1f %6.1f \t phi= %5.1f \t %d %8.2f %d \n", 
// 					trackID, pdgCode, parentID,
// 					stepLocation.x(), stepLocation.y(), stepLocation.z(),
// 					stepphi,
// 					stepNumber,trackLen,stepStatus);
// 				//
// 			}
// 			if (pdgCode==-22 && stepNumber==1 && parentID!=1 ){		// OP but created by OTHER THAN THE PRIMARY
// 				//
// 				//
// 				printf("OP BG  \t trackID= %6d \t pdg= %6d \t parentID= %6d \t step x,y,z = %6.1f %6.1f %6.1f \t phi= %5.1f \t %d %8.2f %d \n", 
// 					trackID, pdgCode, parentID,
// 					stepLocation.x(), stepLocation.y(), stepLocation.z(),
// 					stepphi,
// 					stepNumber,trackLen,stepStatus);
// 				//
// 			}
			//
		}	// end operator function
 
     private:
		//std::string mRadiationMapOutputString = "default_rad.root";
		bool initialized = false;
		//
		//int previousTrackID = 0;
		//
		G4ThreeVector DircIncidence_pos;
		G4ThreeVector DircIncidence_mom;
		TString prevname="",postname="";
		int    mDircIncidence_evt     = 0; 
		int    mDircIncidence_trackID = 0; 
		int    mDircIncidence_pdgCode = 0; 
		double mDircIncidence_x   = 0; 
		double mDircIncidence_y   = 0; 
		double mDircIncidence_z   = 0; 
		double mDircIncidence_r   = 0; 
		double mDircIncidence_px  = 0;
		double mDircIncidence_py  = 0;
		double mDircIncidence_pz  = 0;
		//
		int nFill	= 0;
		//
	 
  };	// End class definition
  }		// End namespace sim
}		// End namespace dd4hep

#endif
