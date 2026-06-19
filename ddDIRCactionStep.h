#ifndef DDDIRCACTIONSTEP_H
#define DDDIRCACTIONSTEP_H

#include "G4EventManager.hh"
#include "G4SteppingManager.hh"
#include "DDG4/Geant4SteppingAction.h"
#include "DDG4/Geant4StepHandler.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TTree.h"
#include "TFile.h"
#include "TString.h"
#include "TMath.h"

// Namespace for the AIDA detector description toolkit
namespace dd4hep {
        
  // Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

	class ddDIRCactionStep;

    class ddDIRCactionStep : public Geant4SteppingAction {
    
	protected:
		DDG4_DEFINE_ACTION_CONSTRUCTORS(ddDIRCactionStep);
		std::string  mOutputString  { };
		int          mFileNumber { };
		int          mDetailLevel { };                   

	public:	
		//
		TFile*	steppingOutput;
		TTree*	DircIncidenceTree;
		//
		//---- parameters for monitoring histograms
		static const int MON_nev	= 20;
		TH1D *hMON_thetaC_truth[MON_nev];	// filled for each of first MON_nev events...
		//TH1D *hMON_OPtheta[MON_nev];		// filled for each of first MON_nev events...
		TH2D *ht1;
		TH2D *ht2;
		TH2D *ht3;
		TH2D *ht4;
		TH2D *ht5;
		TH2D *ht6;
		//
		//---------------------------------------------------------------------------------------------------- 
		// Standard constructor with initializing arguments
		ddDIRCactionStep(Geant4Context* context, const std::string& name = "") : Geant4SteppingAction(context, name) {
			//
			declareProperty("OutputBase" , mOutputString);  // This is the basic string for output root file
			declareProperty("fileNumber" , mFileNumber  );  // UNUSED (but keep for future)
			declareProperty("DetailLevel", mDetailLevel );  // 
			//
			std::cout<<"ddDIRCactionStep -- mDetailLevel = "<<mDetailLevel<<std::endl;
			//
		} ;
		//
		//---------------------------------------------------------------------------------------------------- 
		// Default destructor
		virtual ~ddDIRCactionStep() {
			//
			if (steppingOutput) {
				//
				steppingOutput->cd();		//---- dirc incidence output file
				//
				DircIncidenceTree->Write();	// dirc incidence output ttree
// 				if (DETAIL){
// 					ht1->Write();
// 					ht2->Write();
// 					ht3->Write();
// 					ht4->Write();
// 					ht5->Write();
// 					ht6->Write();
// 					for (int iev=0;iev<MON_nev;iev++){
// 						hMON_thetaC_truth[iev]	->Write();
// 						//hMON_OPtheta[iev]		->Write();
// 					}
// 				}
				//
 				steppingOutput->Close();
				delete steppingOutput;
				//
				std::cout << "ddDIRCactionStep -- nSeen = " << nSeen << std::endl;
				std::cout << "ddDIRCactionStep -- nFill = " << nFill << std::endl;
				//
			}		
		};

		//---------------------------------------------------------------------------------------------------- 
		void initializeOutputFile(){			
			//
			eventManager = G4EventManager::GetEventManager();
			//
			if (mDetailLevel==0){
				VERBOSE	= false;	// very quiet
				DETAIL	= false;	// just fill the incidence tree and leave as quickly as possible
				std::cout<<"ddDIRCactionStep -- Fast mode - fill incidence tree only."<<std::endl;
			} else if (mDetailLevel==1){
				VERBOSE	= false;	// mostly quiet
				DETAIL	= true;		// calculate additional things and save histograms besides the incidence tree
				std::cout<<"ddDIRCactionStep -- Detailed Calculations Enabled!"<<std::endl;
			} else {
				VERBOSE	= true;		// not quiet
				DETAIL	= true;		// calculate additional things and save histograms besides the incidence tree
				std::cout<<"ddDIRCactionStep -- Detailed Calculations & Verbosity Enabled!"<<std::endl;
			}
			//
			TString tmp(mOutputString);
			TString inputFileString;
			inputFileString.Form("./%s.root", tmp.Data() );
			if (VERBOSE) std::cout << "ddDIRCactionStep -- output file = " << inputFileString << std::endl;
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
				DircIncidenceTree->Branch("t"      , &mDircIncidence_t      ,      "t/D");
				DircIncidenceTree->Branch("mass"   , &mDircIncidence_mass   ,   "mass/D");	// convenience
				DircIncidenceTree->Branch("beta"   , &mDircIncidence_beta   ,   "beta/D");	// convenience
			//
			if (DETAIL){
				for (int iev=0;iev<MON_nev;iev++){
					hMON_thetaC_truth[iev]	= new TH1D(Form("hMON_thetaC_truth_%d",iev),
												   Form("truth #theta_{C1}, first primary, event %d",iev),
												   1000,0,2000.);
					//hMON_OPtheta[iev]		= new TH1D(Form("hMON_OPtheta_%d",iev),
					//							   Form("truth #theta_{C2}, first primary, event %d",iev),
					//							   1000,0,2000.);
				}
				ht1	= new TH2D("ht1","ht1",480,-1200,1200,480,-1200,1200);
				ht2	= new TH2D("ht2","ht2",200,-M_PI,M_PI,200,-M_PI,M_PI);
				ht3	= new TH2D("ht3","ht3",200,-M_PI,M_PI,200,-M_PI,M_PI);
				ht4	= new TH2D("ht4","ht4",200,-M_PI,M_PI,200,-M_PI,M_PI);
				ht5	= new TH2D("ht5","ht5",200,-M_PI,M_PI,200,-M_PI,M_PI);
				ht6	= new TH2D("ht6","ht6",200,-M_PI,M_PI,200,-M_PI,M_PI);
			}	// end DETAIL
			//
			initialized = true;
		}

		//
		//	bars polar max approx	 22.65 deg
		//	bars polar min approx	164.3  deg
		//
		//	y to inner face of bar:	757.25 * mm
		//
		//	shortbar length		1140.025   (includes one glue layer)
		//	shortbar length/2	 570.0125  (includes one-half glue layer)
		//
		//  double barsZmin_expected   = -2729.075 * mm;
		//  double barsZmax_expected   =  1831.025 * mm;
		//
		//	-2729.075 +   1140.025	= -1589.05		high-z end of bar3
		//	-2729.075 + 2*1140.025	=  -449.025		high-z end of bar2
		//	-2729.075 + 3*1140.025	=   691.0		high-z end of bar1
		//	-2729.075 + 4*1140.025	=  1831.025		high-z end of bar0
		//
		//	shortbar center coords:
		//		765.875 -17.575 -2,159.0625
		//		765.875 -17.575 -1,019.0375
		//		765.875 -17.575    120.9875
		//		765.875 -17.575  1,261.0125
		//
		//---------------------------------------------------------------------------------------------------- 
		// Pre-track action callback
		virtual void operator()(const G4Step* step, G4SteppingManager* mgr) override {
      		//
			if(!initialized){initializeOutputFile();}
			//
			G4Track* track	= step->GetTrack();
			int trackID 	= track->GetTrackID();
			int stepNumber	= track->GetCurrentStepNumber();
			if (trackID==1 && stepNumber==1 ){	// this is the first step of new event!
				++nSeen;
			}
			int parentID	= track->GetParentID();
			int pdgCode		= track->GetDefinition()->GetPDGEncoding();
			//
			currentEventID = eventManager->GetConstCurrentEvent()->GetEventID();
			//
			const G4ThreeVector preStepLocation  = step->GetPreStepPoint()->GetPosition();
			const G4ThreeVector postStepLocation = step->GetPostStepPoint()->GetPosition();
			const G4ThreeVector stepLocation     = 0.5*(preStepLocation + postStepLocation); //midpoint of step
			int                 stepStatus       = step->GetPreStepPoint()->GetStepStatus();
			//
			prevname = postname = "";
			if ( step->GetPreStepPoint()->GetPhysicalVolume() 
			  && step->GetPostStepPoint()->GetPhysicalVolume() ) {
				prevname = step->GetPreStepPoint()->GetPhysicalVolume()->GetName();
				postname = step->GetPostStepPoint()->GetPhysicalVolume()->GetName();
			}
// 			if (stepNumber > 50000 ){
// 				track->SetTrackStatus(fStopAndKill);		// KILL TRACK
// 				return;
// 			}
// 			if (trackLen > 30000./CLHEP::cm ){
// 				track->SetTrackStatus(fStopAndKill);		// KILL TRACK
// 				return;
// 			}
			//
			if (parentID==0 && stepStatus==1 	// parentID==0 taken to indicate PRIMARY GENERATED particle
			 &&  postname.Contains("bar_vol") 	// enters bar_vol...
			 && !prevname.Contains("bar_vol") 	// ...from some other volume
			 ){									//--> this track enters a DIRC bar in this step!!!
				//
				//DircIncidence_pos		= postStepLocation;
				DircIncidence_pos		= track->GetPosition();
				//
				if (DETAIL && !VERBOSE && currentEventID<20){
					std::cout<<"dirc incidence: "<<DircIncidence_pos.x()<<" "
							 <<DircIncidence_pos.y()<<" "
							 <<DircIncidence_pos.z()<<" "
							 <<std::endl;
				}
				//
				//double trackLen	= track->GetTrackLength();
				double mass		= track->GetDefinition()->GetPDGMass() / CLHEP::GeV;
				double charge	= track->GetDefinition()->GetPDGCharge()/eplus;
				double ptot		= track->GetMomentum().mag() / CLHEP::GeV;
				double etot		= sqrt(ptot*ptot + mass*mass);
				double beta		= ptot/etot;
				//
				if (fabs(charge)>0. && mass>0. && beta>1./1.5){
					//
					DircIncidence_mom		= track->GetMomentum();
					DircIncidence_momdir	= track->GetMomentumDirection();
					mDircIncidence_trackID	= trackID;
					mDircIncidence_pdgCode	= pdgCode;
					mDircIncidence_x		= postStepLocation.x() / CLHEP::mm;			// mm
					mDircIncidence_y		= postStepLocation.y() / CLHEP::mm;			// mm
					mDircIncidence_z		= postStepLocation.z() / CLHEP::mm;			// mm
					mDircIncidence_r		= std::sqrt(mDircIncidence_x*mDircIncidence_x+mDircIncidence_y*mDircIncidence_y);	// mm
					mDircIncidence_px		= track->GetMomentum().x() / CLHEP::GeV;	// GeV
					mDircIncidence_py		= track->GetMomentum().y() / CLHEP::GeV;	// GeV
					mDircIncidence_pz		= track->GetMomentum().z() / CLHEP::GeV;	// GeV
					mDircIncidence_t		= track->GetGlobalTime() / CLHEP::ns;		// ns
					mDircIncidence_mass		= mass;										// GeV
					mDircIncidence_beta		= beta;										// GeV
					mDircIncidence_evt		= currentEventID;
					DircIncidenceTree	->Fill();
					++nFill;
					//
					if (VERBOSE){
						std::cout	<<"DIRC Incidence: evt="<<currentEventID<<" nFill="<<nFill<<" trkID="<<trackID
									//<<" parentID="<<parentID
									<<" step="<<stepNumber
									<<" pdg="<<pdgCode
									<<" mass="<<mass
									<<" beta= "<<mDircIncidence_beta
									<<" chg="<<charge
									<<" posn: "<<mDircIncidence_x <<" "<<mDircIncidence_y<<" "<<mDircIncidence_z<<" r: "<<mDircIncidence_r
									<<" mom: "<<mDircIncidence_px<<" "<<mDircIncidence_py<<" "<<mDircIncidence_pz
									<<" t: "<<mDircIncidence_t
									<<std::endl;
					}
					//
				}
				//
			}	// end check: primary entering bar?
			//
//			//---- just a cout whenever the primary changes volumes...
//			if (DETAIL){
//				//
// 				if (VERBOSE){				
// 					if (trackID==1 && (stepStatus!=4 || prevname!=postname) ){	// primary track, but don't show steps w/in same volume
// 						//
// 						double stepphi	= stepLocation.phi()*(180./M_PI);
// 						printf("PRIMARY\t trackID= %6d \t pdg= %6d \t parentID= %6d \t step x,y,z = %6.1f %6.1f %6.1f \t phi= %5.1f \t %d %8.2f %d \t %s %s\n", 
// 							trackID, pdgCode, parentID,
// 							stepLocation.x(), stepLocation.y(), stepLocation.z(),
// 							stepphi,
// 							stepNumber,trackLen,stepStatus,
// 							prevname.Data(),postname.Data()
// 							);
// 						//
// 						//double xpos		= postStepLocation.x() / CLHEP::mm;	// mm
// 						//double ypos		= postStepLocation.y() / CLHEP::mm;	// mm
// 						//double zpos		= postStepLocation.z() / CLHEP::mm;	// mm
// 						//double rpos		= std::sqrt(xpos*xpos+ypos*ypos);	// mm
// 						//double xpre		= preStepLocation.x() / CLHEP::mm;	// mm
// 						//double ypre		= preStepLocation.y() / CLHEP::mm;	// mm
// 						//double zpre		= preStepLocation.z() / CLHEP::mm;	// mm
// 						//double rpre		= std::sqrt(xpre*xpre+ypre*ypre);	// mm
// 						//std::cout	<<"trkID="<<trackID<<" parentID="<<parentID
// 						//			<<" step="<<stepNumber<<" pdg="<<pdgCode
// 						//			//<<" ss= "<<istatstep
// 						//			<<"\t posn pre: "<<xpre <<" "<<ypre<<" "<<zpre<<" r: "<<rpre
// 						//			<<"\t posn pos: "<<xpos <<" "<<ypos<<" "<<zpos<<" r: "<<rpos
// 						//			<<std::endl;
// 						//
// 					}	// end trackID==1 at volume boundary...
// 				}	// end VERBOSE
				//
				//---- get info from optical photons at creation!
// 				double Cframe_OPtheta	= 0;
// 				double Cframe_OPphi		= 0;
// 				if (pdgCode==-22 && stepNumber==1 ){	// OP at creation
// 					//
// 					auto thisStepPoint	= step->GetPreStepPoint();
// 					//
// 					G4ThreeVector OPpos	= thisStepPoint->GetPosition();	//!!!!!!! same as preStepLocation
// 					G4ThreeVector OPdir	= thisStepPoint->GetMomentumDirection();
// 					double thetaC_truth	= 1000.*OPdir.angle(DircIncidence_momdir);	// mrad
// 					//
// 					double primthe			= DircIncidence_momdir.theta();		// rad
// 					double primphi			= DircIncidence_momdir.phi();		// rad
// 					G4ThreeVector norm		= DircIncidence_momdir.cross(axisz);
// 					G4ThreeVector OPdirRot	= OPdir; OPdirRot.rotate(primthe,norm);
// 					Cframe_OPtheta			= 1000.*OPdirRot.theta();				// mrad
// 					Cframe_OPphi		 	= OPdirRot.phi();						// rad
// 					//
// 					if (nSeen-1 < MON_nev){
// 						hMON_thetaC_truth[nSeen-1]	->Fill(thetaC_truth);
// 						//hMON_OPtheta[nSeen-1]		->Fill(Cframe_OPtheta);
// 		 			}
// 					double ringplot_x	= Cframe_OPtheta*cos(Cframe_OPphi);
// 					double ringplot_y	= Cframe_OPtheta*sin(Cframe_OPphi);
// 					ht1->Fill(ringplot_x,ringplot_y);
// 					//
// 					//---- now get polarization in the cerenkov frame
// 					G4ThreeVector POLdir	= track->GetPolarization();
// 					G4ThreeVector POLdirRot	= POLdir; POLdirRot.rotate(primthe,norm);
// 					double dphi	= POLdirRot.phi() - Cframe_OPphi;
// 					//		if (dphi <   -M_PI/2.)dphi += 2.*M_PI;
// 					//		if (dphi >= 3*M_PI/2.)dphi -= 2.*M_PI;
// 					//
// //std::cout<<POLdir.x()<<" "<<POLdir.y()<<" "<<POLdir.z()
// //		 <<"\t "<<(180/M_PI)*POLdir.phi()
// //		 <<"\t OPdir.angle(POLdir)= "<<OPdir.angle(POLdir)
// //		 <<"\t DircIncidence_momdir.angle(POLdir)= "<<DircIncidence_momdir.angle(POLdir)
// //		 <<std::endl;
//  					//
// 					if (VERBOSE){
// 						if (parentID==1){	//!!!!!!!!!! SINGLE TRACK EVENTS ASSUMED
// 							std::cout<<"IDs: "<<trackID<<" "<<parentID
// 									 <<"\t thetaC_truth (mrad) = "<<thetaC_truth
// 									 <<" "<<Cframe_OPtheta
// 									 <<"\t\t "<<POLdirRot.phi()
// 									 <<" "<<Cframe_OPphi
// 									 <<" "<<dphi
// 									 <<std::endl;
// 						}
// 					}
// 					//	
// 					//---- let's see if these cerenkov photons are ~radially polarized...
// // 					G4ThreeVector OPpol		= track->GetPolarization();
// // 	 				G4ThreeVector p_X_g		= DircIncidence_momdir.cross(OPdir);
// // 	 				G4ThreeVector p_X_g_X_p	= p_X_g.cross(DircIncidence_momdir);	// should be radial about path.
// // 	 				double radpolang		= p_X_g_X_p.angle(OPpol);				// angle between radial dir and photon dir
// // 					if (VERBOSE){
// // 						if (parentID==1){	//!!!!!!!!!! SINGLE TRACK EVENTS ASSUMED
// // 							std::cout<<"IDs: "<<trackID<<" "<<parentID
// // 									 <<"\t radpolang (rad) = "<<radpolang
// // 									 <<"\t OPpol: "<<OPpol.theta()<<" "<<OPpol.phi()
// // 									 <<"\t OPdir: "<<OPdir.theta()<<" "<<OPdir.phi()
// // 									 <<"\t p_X_g_X_p: "<<p_X_g_X_p.theta()<<" "<<p_X_g_X_p.phi()
// // 									 <<"\t angle(p,pol): "<<DircIncidence_momdir.angle(OPpol)
// // 									 <<std::endl;
// // 						}	// end parentID check
// // 					}	// end verbose
// 					//
// 	// 				if (parentID==1){		//!!!!!!!!!! SINGLE TRACK EVENTS ASSUMED
// 	// 					ht1->Fill(OPdir.theta(),OPpol.theta());
// 	// 					ht2->Fill(OPdir.phi(),OPpol.phi());
// 	// 					ht3->Fill(p_X_g.phi(),OPpol.theta());
// 	// 					ht4->Fill(p_X_g.theta(),OPpol.phi());
// 	// 					ht5->Fill(p_X_g_X_p.theta(),OPpol.theta());
// 	// 					ht6->Fill(p_X_g_X_p.phi(),OPpol.phi());
// 	// 				}	// end parentID==1 check
// 
// 
// 					//
// 					//
// 					//std::cout<<"OPdir: "<<OPdir.x()<<" "<<OPdir.y()<<" "<<OPdir.z()
// 					//		 <<"\t OPpol: "<<OPpol.x()<<" "<<OPpol.y()<<" "<<OPpol.z()
// 					//		 <<std::endl;
// 					//	printf("OP     \t trackID= %6d \t pdg= %6d \t parentID= %6d \t step x,y,z = %6.1f %6.1f %6.1f \t phi= %5.1f \t %d %8.2f %d \n", 
// 					//		trackID, pdgCode, parentID,
// 					//		stepLocation.x(), stepLocation.y(), stepLocation.z(),
// 					//		stepphi,
// 					//		stepNumber,trackLen,stepStatus);
// 					//
// 					//
// 				}	// end OP at creation
//			}	// end DETAIL
			//
			//if (pdgCode==-22 && stepNumber==1 && parentID!=1 ){		// OP but created by OTHER THAN THE PRIMARY
			//	//
			//	//
			//	printf("OP BG  \t trackID= %6d \t pdg= %6d \t parentID= %6d \t step x,y,z = %6.1f %6.1f %6.1f \t phi= %5.1f \t %d %8.2f %d \n", 
			//		trackID, pdgCode, parentID,
			//		stepLocation.x(), stepLocation.y(), stepLocation.z(),
			//		stepphi,
			//		stepNumber,trackLen,stepStatus);
			//	//
			//}
			//
		}	// end operator function
 
     private:
		bool initialized = false;
		//
		G4EventManager* eventManager;
		//
		G4ThreeVector DircIncidence_pos;
		G4ThreeVector DircIncidence_mom;
		G4ThreeVector DircIncidence_momdir;
		TString prevname="",postname="";
		int    mDircIncidence_evt     = 0; 
		int    mDircIncidence_trackID = 0; 
		int    mDircIncidence_pdgCode = 0; 
		double mDircIncidence_x       = 0; 
		double mDircIncidence_y       = 0; 
		double mDircIncidence_z       = 0; 
		double mDircIncidence_r       = 0; 
		double mDircIncidence_px      = 0;
		double mDircIncidence_py      = 0;
		double mDircIncidence_pz      = 0;
		double mDircIncidence_t       = 0;
		double mDircIncidence_mass    = 0;
		double mDircIncidence_beta    = 0;
		int	currentEventID	= 0;
		int nSeen	= 0;
		int nFill	= 0;
		//
		bool VERBOSE	= false;
		bool DETAIL		= false;
		//double CerenkovAngleExpected = 0;
		G4ThreeVector axisy		= G4ThreeVector(0.,1.,0.);
		G4ThreeVector axisz		= G4ThreeVector(0.,0.,1.);

  };	// End class definition
  }		// End namespace sim
}		// End namespace dd4hep

#endif

