#ifndef DDDIRCACTIONSTEP_H
#define DDDIRCACTIONSTEP_H

#include "DDG4/Geant4SteppingAction.h"
#include "DDG4/Geant4StepHandler.h"
#include "G4SteppingManager.hh"
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
		} ;
		//
		//---------------------------------------------------------------------------------------------------- 
		// Default destructor
		virtual ~ddDIRCactionStep() {
			//
			std::cout<<"ddDIRCactionStep -- mDetailLevel = "<<mDetailLevel<<std::endl;
			//
			if (steppingOutput) {
				//
				//---- catch last event info!
				++nFill;
				mDircIncidence_evt	= nSeen - 1;
				DircIncidenceTree	->Fill();
				//
				steppingOutput->cd();		//---- dirc incidence output file
				//
				DircIncidenceTree->Write();	// dirc incidence output ttree
				if (DETAIL){
					ht1->Write();
					ht2->Write();
					ht3->Write();
					ht4->Write();
					ht5->Write();
					ht6->Write();
					for (int iev=0;iev<MON_nev;iev++){
						hMON_thetaC_truth[iev]	->Write();
						//hMON_OPtheta[iev]		->Write();
					}
				}
				//
 				steppingOutput->Close();
				delete steppingOutput;
				//
				std::cout << "ddDIRCactionStep -- nEvents seen = " << nSeen << std::endl;
				std::cout << "ddDIRCactionStep -- nEvents fill = " << nFill << std::endl;
				//
			}		
		};

		//---------------------------------------------------------------------------------------------------- 
		void initializeOutputFile(){			
			//
			if (mDetailLevel==0){
				VERBOSE	= false;	// STFU & GBTW
				DETAIL	= false;	// just fill the incidence tree and leave as quickly as possible
				std::cout<<"ddDIRCactionStep -- Fast mode - fill incidence tree only."<<std::endl;
			} else if (mDetailLevel==1){
				VERBOSE	= false;	// shut it.
				DETAIL	= true;		// calculate additional things and save histograms besides the incidence tree
				std::cout<<"ddDIRCactionStep -- Detailed Calculations Enabled!"<<std::endl;
			} else {
				VERBOSE	= true;		// yap about it
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
			//int iprocess	= mgr->GetProcessNumber();
			//G4StepStatus istatstep	= mgr->Stepping();
			//
			G4Track* track	= step->GetTrack();
			int trackID 	= track->GetTrackID();
			int parentID	= track->GetParentID();
			int stepNumber	= track->GetCurrentStepNumber();
			int pdgCode		= track->GetDefinition()->GetPDGEncoding();
			double trackLen	= track->GetTrackLength();
			//
			if (trackID==1 && stepNumber==1 ){	// this is the first step of new event!
				++nSeen;
				if (nSeen%100==0) std::cout<<"ddDIRCactionStep::operator processing "<<nSeen<<std::endl;
				if (mDircIncidence_r>0.){		// fill (unless very first step of code run)
					++nFill;
					mDircIncidence_evt	= nSeen - 1;
					DircIncidenceTree	->Fill();
				}
				//
			}	// end new event block
			//
			//
			const G4ThreeVector preStepLocation  = step->GetPreStepPoint()->GetPosition();
			const G4ThreeVector postStepLocation = step->GetPostStepPoint()->GetPosition();
			const G4ThreeVector stepLocation     = 0.5*(preStepLocation + postStepLocation); //midpoint of step
			int stepStatus = step->GetPreStepPoint()->GetStepStatus();
			//
			//G4TouchableHandle postTouchable = step->GetPostStepPoint()->GetTouchableHandle();
			//G4ThreeVector postLocalPos = postTouchable->GetHistory()->GetTopTransform().TransformPoint(postPos);
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
				//G4TouchableHandle touchable = step->GetPostStepPoint()->GetTouchableHandle();
				//G4ThreeVector globalCenter = touchable->GetHistory()->GetTopTransform().Inverse().TransformPoint(G4ThreeVector());
				//std::cout<<"bar volume center: "<<globalCenter.x()<<" "<<globalCenter.y()<<" "<<globalCenter.z()<<" "<<std::endl;
				//double shortbar_Zmin	= globalCenter.z() - 1225./2.;
				//double shortbar_Zmax	= globalCenter.z() + 1225./2.;
				//
				//DircIncidence_pos		= postStepLocation;
				DircIncidence_pos		= track->GetPosition();
				//
				if (nSeen<20){
					std::cout<<"incidence: "<<DircIncidence_pos.x()<<" "
							 <<DircIncidence_pos.y()<<" "
							 <<DircIncidence_pos.z()<<" "
							 <<std::endl;
				}
				//
				DircIncidence_mom		= track->GetMomentum();
				DircIncidence_momdir	= track->GetMomentumDirection();
				mDircIncidence_trackID	= trackID;
				mDircIncidence_pdgCode	= pdgCode;
				mDircIncidence_x		= postStepLocation.x() / CLHEP::mm;			// mm
				mDircIncidence_y		= postStepLocation.y() / CLHEP::mm;			// mm
				mDircIncidence_z		= postStepLocation.z() / CLHEP::mm;			// mm
				mDircIncidence_r		= std::sqrt(mDircIncidence_x*mDircIncidence_x+mDircIncidence_y*mDircIncidence_y);	// mm
				mDircIncidence_px		= track->GetMomentum().x() / CLHEP::GeV;	// GeV/c
				mDircIncidence_py		= track->GetMomentum().y() / CLHEP::GeV;	// GeV/c
				mDircIncidence_pz		= track->GetMomentum().z() / CLHEP::GeV;	// GeV/c
				mDircIncidence_t		= track->GetGlobalTime() / CLHEP::ns;		// ns
				//
				//std::cout<<mDircIncidence_t<<"\t "
				//		 <<step->GetPreStepPoint()->GetGlobalTime()<<"\t "
				//		 <<step->GetPostStepPoint()->GetGlobalTime()<<std::endl;
				//
				//
				if (VERBOSE){
					std::cout	<<"DIRC Incidence:  trkID="<<trackID<<" parentID="<<parentID
								<<" step="<<stepNumber<<" pdg="<<pdgCode
								//<<" ss= "<<istatstep
								<<"\t pospos: "<<mDircIncidence_x <<" "<<mDircIncidence_y<<" "<<mDircIncidence_z<<" r: "<<mDircIncidence_r
								<<"\t mom: "<<mDircIncidence_px<<" "<<mDircIncidence_py<<" "<<mDircIncidence_pz
								<<std::endl;
				}
				//
				//if (DETAIL){
				//	const G4ParticleDefinition* particleDefinition	= track->GetParticleDefinition();
				//	double amass	= particleDefinition->GetPDGMass() / CLHEP::GeV;
				//	double mom		= DircIncidence_mom.mag() / CLHEP::GeV;
				//	G4double ve		= sqrt(mom*mom + amass*amass);
				//	G4double vg		= ve/amass;
				//	G4double vb		= sqrt(1.0 - (1.0/vg/vg));
				//	if (vb!=0.&&fabs(1.0/1.47/vb)<1.0){
				//	 CerenkovAngleExpected	= 1000.*acos(1.0/1.47/vb);	// mrad
				//	} else {
				//	 CerenkovAngleExpected	= 0;
				//	}
				//}	// end DETAIL
			}	// end check: primary entering bar?
			//
			//---- just a cout whenever the primary changes volumes...
			if (DETAIL){
				//
				if (VERBOSE){				
					if (trackID==1 && (stepStatus!=4 || prevname!=postname) ){	// primary track, but don't show steps w/in same volume
						//
						double stepphi	= stepLocation.phi()*(180./M_PI);
						printf("PRIMARY\t trackID= %6d \t pdg= %6d \t parentID= %6d \t step x,y,z = %6.1f %6.1f %6.1f \t phi= %5.1f \t %d %8.2f %d \t %s %s\n", 
							trackID, pdgCode, parentID,
							stepLocation.x(), stepLocation.y(), stepLocation.z(),
							stepphi,
							stepNumber,trackLen,stepStatus,
							prevname.Data(),postname.Data()
							);
						//
						//double xpos		= postStepLocation.x() / CLHEP::mm;	// mm
						//double ypos		= postStepLocation.y() / CLHEP::mm;	// mm
						//double zpos		= postStepLocation.z() / CLHEP::mm;	// mm
						//double rpos		= std::sqrt(xpos*xpos+ypos*ypos);	// mm
						//double xpre		= preStepLocation.x() / CLHEP::mm;	// mm
						//double ypre		= preStepLocation.y() / CLHEP::mm;	// mm
						//double zpre		= preStepLocation.z() / CLHEP::mm;	// mm
						//double rpre		= std::sqrt(xpre*xpre+ypre*ypre);	// mm
						//std::cout	<<"trkID="<<trackID<<" parentID="<<parentID
						//			<<" step="<<stepNumber<<" pdg="<<pdgCode
						//			//<<" ss= "<<istatstep
						//			<<"\t pospre: "<<xpre <<" "<<ypre<<" "<<zpre<<" r: "<<rpre
						//			<<"\t pospos: "<<xpos <<" "<<ypos<<" "<<zpos<<" r: "<<rpos
						//			<<std::endl;
						//
					}	// end trackID==1 at volume boundary...
				}	// end VERBOSE
				//
				//---- get info from optical photons at creation!
				double Cframe_OPtheta	= 0;
				double Cframe_OPphi		= 0;
				if (pdgCode==-22 && stepNumber==1 ){	// OP at creation
					//
					auto thisStepPoint	= step->GetPreStepPoint();
					//
					G4ThreeVector OPpos	= thisStepPoint->GetPosition();	//!!!!!!! same as preStepLocation
					G4ThreeVector OPdir	= thisStepPoint->GetMomentumDirection();
					double thetaC_truth	= 1000.*OPdir.angle(DircIncidence_momdir);	// mrad
					//
					double primthe			= DircIncidence_momdir.theta();		// rad
					double primphi			= DircIncidence_momdir.phi();		// rad
					G4ThreeVector norm		= DircIncidence_momdir.cross(axisz);
					G4ThreeVector OPdirRot	= OPdir; OPdirRot.rotate(primthe,norm);
					Cframe_OPtheta			= 1000.*OPdirRot.theta();				// mrad
					Cframe_OPphi		 	= OPdirRot.phi();						// rad
					//
					if (nSeen-1 < MON_nev){
						hMON_thetaC_truth[nSeen-1]	->Fill(thetaC_truth);
						//hMON_OPtheta[nSeen-1]		->Fill(Cframe_OPtheta);
		 			}
					double ringplot_x	= Cframe_OPtheta*cos(Cframe_OPphi);
					double ringplot_y	= Cframe_OPtheta*sin(Cframe_OPphi);
					ht1->Fill(ringplot_x,ringplot_y);
					//
					//---- now get polarization in the cerenkov frame
					G4ThreeVector POLdir	= track->GetPolarization();
					G4ThreeVector POLdirRot	= POLdir; POLdirRot.rotate(primthe,norm);
					double dphi	= POLdirRot.phi() - Cframe_OPphi;
					//		if (dphi <   -M_PI/2.)dphi += 2.*M_PI;
					//		if (dphi >= 3*M_PI/2.)dphi -= 2.*M_PI;
					//
//std::cout<<POLdir.x()<<" "<<POLdir.y()<<" "<<POLdir.z()
//		 <<"\t "<<(180/M_PI)*POLdir.phi()
//		 <<"\t OPdir.angle(POLdir)= "<<OPdir.angle(POLdir)
//		 <<"\t DircIncidence_momdir.angle(POLdir)= "<<DircIncidence_momdir.angle(POLdir)
//		 <<std::endl;
 					//
					if (VERBOSE){
						if (parentID==1){	//!!!!!!!!!! SINGLE TRACK EVENTS ASSUMED
							std::cout<<"IDs: "<<trackID<<" "<<parentID
									 <<"\t thetaC_truth (mrad) = "<<thetaC_truth
									 <<" "<<Cframe_OPtheta
									 <<"\t\t "<<POLdirRot.phi()
									 <<" "<<Cframe_OPphi
									 <<" "<<dphi
									 <<std::endl;
						}
					}
					//	
					//---- let's see if these cerenkov photons are ~radially polarized...
// 					G4ThreeVector OPpol		= track->GetPolarization();
// 	 				G4ThreeVector p_X_g		= DircIncidence_momdir.cross(OPdir);
// 	 				G4ThreeVector p_X_g_X_p	= p_X_g.cross(DircIncidence_momdir);	// should be radial about path.
// 	 				double radpolang		= p_X_g_X_p.angle(OPpol);				// angle between radial dir and photon dir
// 					if (VERBOSE){
// 						if (parentID==1){	//!!!!!!!!!! SINGLE TRACK EVENTS ASSUMED
// 							std::cout<<"IDs: "<<trackID<<" "<<parentID
// 									 <<"\t radpolang (rad) = "<<radpolang
// 									 <<"\t OPpol: "<<OPpol.theta()<<" "<<OPpol.phi()
// 									 <<"\t OPdir: "<<OPdir.theta()<<" "<<OPdir.phi()
// 									 <<"\t p_X_g_X_p: "<<p_X_g_X_p.theta()<<" "<<p_X_g_X_p.phi()
// 									 <<"\t angle(p,pol): "<<DircIncidence_momdir.angle(OPpol)
// 									 <<std::endl;
// 						}	// end parentID check
// 					}	// end verbose
					//
	// 				if (parentID==1){		//!!!!!!!!!! SINGLE TRACK EVENTS ASSUMED
	// 					ht1->Fill(OPdir.theta(),OPpol.theta());
	// 					ht2->Fill(OPdir.phi(),OPpol.phi());
	// 					ht3->Fill(p_X_g.phi(),OPpol.theta());
	// 					ht4->Fill(p_X_g.theta(),OPpol.phi());
	// 					ht5->Fill(p_X_g_X_p.theta(),OPpol.theta());
	// 					ht6->Fill(p_X_g_X_p.phi(),OPpol.phi());
	// 				}	// end parentID==1 check


					//
					//
					//std::cout<<"OPdir: "<<OPdir.x()<<" "<<OPdir.y()<<" "<<OPdir.z()
					//		 <<"\t OPpol: "<<OPpol.x()<<" "<<OPpol.y()<<" "<<OPpol.z()
					//		 <<std::endl;
					//	printf("OP     \t trackID= %6d \t pdg= %6d \t parentID= %6d \t step x,y,z = %6.1f %6.1f %6.1f \t phi= %5.1f \t %d %8.2f %d \n", 
					//		trackID, pdgCode, parentID,
					//		stepLocation.x(), stepLocation.y(), stepLocation.z(),
					//		stepphi,
					//		stepNumber,trackLen,stepStatus);
					//
					//
				}	// end OP at creation
			}	// end DETAIL
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
		G4ThreeVector DircIncidence_pos;
		G4ThreeVector DircIncidence_mom;
		G4ThreeVector DircIncidence_momdir;
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
		double mDircIncidence_t   = 0;
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





// 	int nbou_n=0; double nbou=0.;		// to get <Nbounce> over all OPs *in this event*
// 	for (unsigned long i=0; i<nhits; i++){
// 		//
//  		auto hit = static_cast<dircTrackerHit *>(hc->GetHit(i));
//  		int trackID				= hit->GetTrackID();	
//  		int globalID			= hit->GetChamberNb();
//  		double ekin				= hit->GetEkin();
//  		G4ThreeVector pos		= hit->GetPos();		//  position of OP at readout end of bar in bar coordinates
//  		G4ThreeVector dir		= hit->GetDir();		// direction of OP at readout end of bar in bar coordinates
//  		G4ThreeVector OPpos		= hit->GetOPposbar();	//  position of OP at creation in bar coordinates
//  		G4ThreeVector OPdir		= hit->GetOPdirbar();	// direction of OP at creation in bar coordinates
//  		G4double time			= hit->GetTime();
//  		G4int Nbounce			= hit->GetNbounce();
//  		G4double cerenkovtheta	= hit->GetCerenkovTheta();		// true result
//  		G4double cerenkovphi	= hit->GetCerenkovPhi();		// true result
//  		G4double cerenkovphix	= cos(cerenkovphi);
//  		G4double cerenkovphiy	= sin(cerenkovphi);
//  		G4double ringx			= (cerenkovtheta/mrad)*cerenkovphix;
//  		G4double ringy			= (cerenkovtheta/mrad)*cerenkovphiy;
//  		++nbou_n; nbou+=Nbounce;						// running variables to produce event-avg. Nbounces
// 		//
// 		G4ThreeVector globalzaxis	= G4ThreeVector(0.,0.,1.);	
// 		G4double barangledepth		= atan2(OPdir.y(),OPdir.z());	// at creation
// 		G4double baranglewidth		= atan2(OPdir.x(),OPdir.z());	// at creation
// 		G4double baranglelength		= OPdir.angle(globalzaxis);		// at creation
// 		G4double barangletheta		= OPdir.theta();				// at creation
//  		G4double baranglephi		= OPdir.phi();					// at creation
// 		//
// 		G4ThreeVector k	= dir;		// direction of OP at bar exit in bar coordinates
// 		G4double cerenkovtheta_end	= acos(k.dot(fPrimaryDircDir));
// 		//
// 		G4ThreeVector	AMBdir[8];
// 		G4ThreeVector	fact;
// 		G4double		cerenkovtheta_end_AMB[8] = {0};
// 		int     indbest		=   -1;
// 		double diffbest		= 9999;
// 		G4double cerenkovtheta_end_AMBbest	= 0;
// 		//std::cout<<"AMB: Theta = "<<cerenkovtheta/mrad<<"\t "<<cerenkovtheta_end/mrad<<"\t ";
// 		for (int iamb=0;iamb<8;iamb++){
// 			if(iamb==0){ AMBdir[iamb]	= G4ThreeVector(  dir.x(),  dir.y(),  dir.z() ); } 
// 			if(iamb==1){ AMBdir[iamb]	= G4ThreeVector( -dir.x(),  dir.y(),  dir.z() ); }
// 			if(iamb==2){ AMBdir[iamb]	= G4ThreeVector(  dir.x(), -dir.y(),  dir.z() ); }
// 			if(iamb==3){ AMBdir[iamb]	= G4ThreeVector(  dir.x(),  dir.y(), -dir.z() ); }
// 			if(iamb==4){ AMBdir[iamb]	= G4ThreeVector( -dir.x(), -dir.y(),  dir.z() ); }
// 			if(iamb==5){ AMBdir[iamb]	= G4ThreeVector(  dir.x(), -dir.y(), -dir.z() ); }
// 			if(iamb==6){ AMBdir[iamb]	= G4ThreeVector( -dir.x(),  dir.y(), -dir.z() ); }
// 			if(iamb==7){ AMBdir[iamb]	= G4ThreeVector( -dir.x(), -dir.y(), -dir.z() ); }
// 			cerenkovtheta_end_AMB[iamb]	= acos(AMBdir[iamb].dot(fPrimaryDircDir));
// 			if ( fabs(cerenkovtheta_end_AMB[iamb]-CerenkovAngleExpected) < diffbest){
// 				indbest						= iamb;
// 				diffbest					= fabs(cerenkovtheta_end_AMB[iamb]-CerenkovAngleExpected);
// 				cerenkovtheta_end_AMBbest	= cerenkovtheta_end_AMB[iamb];
// 			}
// 			//std::cout<<std::setw(8)<<cerenkovtheta_end_AMB[iamb]/mrad<<" ";
// 		}	//std::cout<<"\t best="<<cerenkovtheta_end_AMBbest/mrad<<std::endl;
// 		//
// 		//
// 		//std::cout<<i<<" "<<trackID<<" "<<globalID<<"\t "<<cerenkovtheta/mrad<<" "<<cerenkovtheta_end/mrad<<std::endl;
// 		//
//  		//if (trackID<40){
// 		//	std::cout<<"EVENT ... "<<globalID<<" "<<trackID
// 		//		<<"\t "<<OPpos
// 		//		<<"\t "<<pos
// 		//		<<std::endl;
// 		//}
// 		//
// 		double exitx	= pos.x();			// at exit, in bar coordinates!
// 		double exity	= pos.y();			// at exit, in bar coordinates!
// 		double exitz	= pos.z();			// at exit, in bar coordinates!
// 		double theta	= dir.theta();		// at exit, in bar coordinates!
// 		double phi		= dir.phi();		// at exit, in bar coordinates!
// 		double zs		= 10. * centimeter;
// 		double rhos		= zs*tan(theta);	// tan(theta) = rhos/zs
// 		double xs		= rhos*cos(phi);
// 		double ys		= rhos*sin(phi);
// 		double screenx	= exitx + xs;
// 		double screeny	= exity + ys;
// 		double screenz	= exitz + zs;
// 		//
// 		//std::cout<<theta<<" "<<eta<<" "<<phi/CLHEP::deg
// 		//	<<"\t "<<pos.x()<<" "<<pos.y()<<" "<<pos.z()
// 		//	<<"\t "<<xs<<" "<<ys<<" "<<zs
// 		//	<<"\t "<<x<<" "<<y<<" "<<z
// 		//	<<std::endl;
// 		//
// 		analysisManager	->FillH1( analysisManager->GetH1Id("hOPexittheta") ,theta,  1.0);
// 		analysisManager	->FillH1( analysisManager->GetH1Id("hOPexitphi")   ,phi,    1.0);
// 		analysisManager	->FillH2( analysisManager->GetH2Id("hExitPoint")   ,exitx  ,exity  ,1.0);
// 		analysisManager	->FillH1( analysisManager->GetH1Id("hTime")        ,time,   1.0);
// 		analysisManager	->FillH1( analysisManager->GetH1Id("hNbounce")     ,std::min(Nbounce,4999),1.0);
// 		analysisManager	->FillH2( analysisManager->GetH2Id("hTimeNbounce")    ,std::min(Nbounce,499),time,1.0);
// 		analysisManager	->FillH2( analysisManager->GetH2Id("hTimeNbounceWide"),std::min(Nbounce,4999),time,1.0);
// 		analysisManager	->FillH2( analysisManager->GetH2Id("hPattern")     ,screenx,screeny,1.0);
// 		if (ieta>=0&&ieta<72){
// 			snprintf(buf,200,"hPattern_eta%d",ieta);
// 			analysisManager	->FillH2( analysisManager->GetH2Id(buf) ,screenx,screeny,1.0);
// 			snprintf(buf,200,"htimePattern_eta%d",ieta);
// 			analysisManager	->FillH2( analysisManager->GetH2Id(buf) ,screenx,screeny,time);
// 			snprintf(buf,200,"hNbouncePattern_eta%d",ieta);
// 			analysisManager	->FillH2( analysisManager->GetH2Id(buf) ,screenx,screeny,Nbounce);
// 			snprintf(buf,200,"hTimeNbounce_eta%d",ieta);
// 			analysisManager	->FillH2( analysisManager->GetH2Id(buf) ,Nbounce,time);
// 			//
// 			snprintf(buf,200,"hAngDepPattern_eta%d",ieta);
// 			analysisManager	->FillH2( analysisManager->GetH2Id(buf) ,screenx,screeny, barangledepth/deg );
// 			snprintf(buf,200,"hAngWidPattern_eta%d",ieta);
// 			analysisManager	->FillH2( analysisManager->GetH2Id(buf) ,screenx,screeny, baranglewidth/deg );
// 			snprintf(buf,200,"hAngLenPattern_eta%d",ieta);
// 			analysisManager	->FillH2( analysisManager->GetH2Id(buf) ,screenx,screeny, baranglelength/deg );
// 			snprintf(buf,200,"hAngThetaPattern_eta%d",ieta);
// 			analysisManager	->FillH2( analysisManager->GetH2Id(buf) ,screenx,screeny, barangletheta/mrad );
// 			snprintf(buf,200,"hAngPhiPattern_eta%d",ieta);
// 			analysisManager	->FillH2( analysisManager->GetH2Id(buf) ,screenx,screeny, baranglephi/deg );
// 			//
// 			snprintf(buf,200,"hCerenkovPhiPattern_eta%d",ieta);
// 			analysisManager	->FillH2( analysisManager->GetH2Id(buf) ,screenx,screeny, cerenkovphi/deg );
// 			//
// 			snprintf(buf,200,"hCerenkovRing_eta%d",ieta);
// 			analysisManager	->FillH2( analysisManager->GetH2Id(buf) ,ringx,ringy, 1.0 );
// 			snprintf(buf,200,"hCerenkovTheta_eta%d",ieta);
// 			analysisManager	->FillH1( analysisManager->GetH1Id(buf) ,cerenkovtheta/mrad, 1.0 );
// 			snprintf(buf,200,"hCerenkovPhi_eta%d",ieta);
// 			analysisManager	->FillH1( analysisManager->GetH1Id(buf) ,cerenkovphi/deg, 1.0 );
// 			//
// 			snprintf(buf,200,"hCerenkovThetaEndVsTheta_eta%d",ieta);
// 			analysisManager	->FillH2( analysisManager->GetH2Id(buf) ,cerenkovtheta/mrad, cerenkovtheta_end/mrad, 1.0 );
// 			snprintf(buf,200,"hCerenkovThetaEndMinusTheta_eta%d",ieta);
// 			analysisManager	->FillH1( analysisManager->GetH1Id(buf) ,cerenkovtheta_end/mrad - cerenkovtheta/mrad,  1.0 );
// 			snprintf(buf,200,"hCerenkovThetaEnd_eta%d",ieta);
// 			analysisManager	->FillH1( analysisManager->GetH1Id(buf) ,cerenkovtheta_end/mrad,  1.0 );
// 			//if (cerenkovtheta_end<M_PI/2.){
// 			//	snprintf(buf,200,"hCerenkovThetaEndA_eta%d",ieta);
// 			//	analysisManager	->FillH1( analysisManager->GetH1Id(buf) ,cerenkovtheta_end/mrad,  1.0 );
// 			//} else if (cerenkovtheta_end>=M_PI/2.){
// 			//	snprintf(buf,200,"hCerenkovThetaEndB_eta%d",ieta);
// 			//	analysisManager	->FillH1( analysisManager->GetH1Id(buf) ,180.-(cerenkovtheta_end/mrad),  1.0 );
// 			//}
// 			snprintf(buf,200,"hBestAmbiguity_eta%d",ieta);
// 			analysisManager	->FillH1( analysisManager->GetH1Id(buf) ,indbest,  1.0 );
// 			snprintf(buf,200,"hCerenkovThetaEndBest_eta%d",ieta);
// 			analysisManager	->FillH1( analysisManager->GetH1Id(buf) ,cerenkovtheta_end_AMBbest/mrad,  1.0 );
// 			snprintf(buf,200,"hCerenkovThetaEndBestVsTheta_eta%d",ieta);
// 			analysisManager	->FillH2( analysisManager->GetH2Id(buf) ,cerenkovtheta/mrad, cerenkovtheta_end_AMBbest/mrad, 1.0 );
// 			//
// 		}
// 		//
//  	}
// 	if (nbou_n){ nbou /= nbou_n; }else{ nbou = 0; }
