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
#include "TVector3.h"
#include <iomanip>

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
		TH2D*	hOPcr_yx;
		TH1D*	hOPcr_z;
		//TH1D *hMON_thetaC_truth[MON_nev];	// filled for each of first MON_nev events...
		//TH1D *hMON_OPtheta[MON_nev];		// filled for each of first MON_nev events...
		//TH2D *ht1;
		//TH2D *ht2;
		//TH2D *ht3;
		//TH2D *ht4;
		//TH2D *ht5;
		//TH2D *ht6;
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
			Zbarsend	= -2729.075*mm;		// mm	!!!!!!!!!TEMPORARY!!!!!!!!! 
			std::cout<<"ddDIRCactionStep -- Killing OPs created with Z < "<<Zbarsend<<std::endl;
			//
		};	//---- end ctor
		//
		//---------------------------------------------------------------------------------------------------- 
		//---- Default destructor
		virtual ~ddDIRCactionStep() {
			//
			if (steppingOutput) {
				//
				steppingOutput->cd();		//---- dirc incidence output file
				//
				DircIncidenceTree->Write();	// dirc incidence output ttree
 				if (DETAIL){
 					hOPcr_yx->Write();
 					hOPcr_z->Write();
 					//ht2->Divide(ht1);	
 					//ht1->Write();
 					//ht2->Write();
					//ht3->Write();
					//ht4->Write();
					//ht5->Write();
					//ht6->Write();
					//for (int iev=0;iev<MON_nev;iev++){
					//	hMON_thetaC_truth[iev]	->Write();
					//	//hMON_OPtheta[iev]		->Write();
					//}
 				}	// end detail
				//
 				steppingOutput->Close();
				delete steppingOutput;
				//
				std::cout << "ddDIRCactionStep -- nSeen = " << nSeen << std::endl;
				std::cout << "ddDIRCactionStep -- nFill = " << nFill << std::endl;
				//
			}
			//
		};	//---- end dtor

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
				DircIncidenceTree->Branch("evt"    , &mDircIncidence_evt    ,           "evt/I");
				DircIncidenceTree->Branch("ninc"   , &mDircIncidence_ninc   ,          "ninc/I");
				DircIncidenceTree->Branch("box"    , mDircIncidence_box     ,     "box[ninc]/I");
				DircIncidenceTree->Branch("bar"    , mDircIncidence_bar     ,     "bar[ninc]/I");
				DircIncidenceTree->Branch("trackID", mDircIncidence_trackID , "trackID[ninc]/I");
				DircIncidenceTree->Branch("pdgCode", mDircIncidence_pdgCode , "pdgCode[ninc]/I");
				DircIncidenceTree->Branch("x"      , mDircIncidence_x       ,       "x[ninc]/D");
				DircIncidenceTree->Branch("y"      , mDircIncidence_y       ,       "y[ninc]/D");
				DircIncidenceTree->Branch("z"      , mDircIncidence_z       ,       "z[ninc]/D");
				DircIncidenceTree->Branch("px"     , mDircIncidence_px      ,      "px[ninc]/D");
				DircIncidenceTree->Branch("py"     , mDircIncidence_py      ,      "py[ninc]/D");
				DircIncidenceTree->Branch("pz"     , mDircIncidence_pz      ,      "pz[ninc]/D");
				DircIncidenceTree->Branch("t"      , mDircIncidence_t       ,       "t[ninc]/D");
				DircIncidenceTree->Branch("mass"   , mDircIncidence_mass    ,    "mass[ninc]/D");	// convenience
				DircIncidenceTree->Branch("beta"   , mDircIncidence_beta    ,    "beta[ninc]/D");	// convenience
			//
			if (DETAIL){
				//
				hOPcr_yx	= new TH2D("hOPcr_yx","hOPcr_yx",500,-1000,1000,500,-1000,1000);
				hOPcr_z		= new TH1D("hOPcr_z" ,"hOPcr_z" ,416,-3200,2000);
				//
				//for (int iev=0;iev<MON_nev;iev++){
				//	hMON_thetaC_truth[iev]	= new TH1D(Form("hMON_thetaC_truth_%d",iev),
				//								   Form("truth #theta_{C1}, first primary, event %d",iev),
				//								   1000,0,2000.);
				//	//hMON_OPtheta[iev]		= new TH1D(Form("hMON_OPtheta_%d",iev),
				//	//							   Form("truth #theta_{C2}, first primary, event %d",iev),
				//	//							   1000,0,2000.);
				//}
				//ht1	= new TH2D("ht1","baridN vs (detphi,deteta)",126,-M_PI,M_PI,150,20*(M_PI/180.),170*(M_PI/180.));
				//ht2	= new TH2D("ht2","barid  vs (detphi,deteta)",126,-M_PI,M_PI,150,20*(M_PI/180.),170*(M_PI/180.));
				//ht3	= new TH2D("ht3","ht3",200,-M_PI,M_PI,200,-M_PI,M_PI);
				//ht4	= new TH2D("ht4","ht4",200,-M_PI,M_PI,200,-M_PI,M_PI);
				//ht5	= new TH2D("ht5","ht5",200,-M_PI,M_PI,200,-M_PI,M_PI);
				//ht6	= new TH2D("ht6","ht6",200,-M_PI,M_PI,200,-M_PI,M_PI);
			}	// end DETAIL
			//
			initialized = true;
			//
		}	//---- end initializeOutputFile()

		//--------------------------------------------------------------------------------
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
		//---- Pre-track action callback
		//
		virtual void operator()(const G4Step* step, G4SteppingManager* mgr) override {
      		//
			if(!initialized){initializeOutputFile();}
			//
			currentEventID	= eventManager->GetConstCurrentEvent()->GetEventID();
			//
			if (currentEventIDprev>=0 && currentEventID!=currentEventIDprev && iEntryIncCurrent>0){
				//---- Fill tree!!!
				mDircIncidence_ninc	= iEntryIncCurrent;
				if (VERBOSE){
					std::cout<<"  ...Filling!   evt="<<mDircIncidence_evt<<"\t Ninc="<<mDircIncidence_ninc<<std::endl;
				}
				DircIncidenceTree->Fill();
				++nFill;
				iEntryIncCurrent	=  0;
			}
			//
			G4Track* track	= step->GetTrack();
			trackID 		= track->GetTrackID();
			int stepNumber	= track->GetCurrentStepNumber();
			if (trackID==1 && stepNumber==1 ){ ++nSeen; }
			int parentID	= track->GetParentID();
			int pdgCode		= track->GetDefinition()->GetPDGEncoding();
			const G4ThreeVector preStepLocation  = step->GetPreStepPoint()->GetPosition();
			const G4ThreeVector postStepLocation = step->GetPostStepPoint()->GetPosition();
			const G4ThreeVector stepLocation     = 0.5*(preStepLocation + postStepLocation); //midpoint of step
			int                 stepStatus       = step->GetPreStepPoint()->GetStepStatus();
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
			 && !prevname.Contains("bar_vol") 	// ...from some other volume (but not glue_vol)
			 && !prevname.Contains("glue_vol") 	// crossing glue layers only happens INSIDE bars, so not a new incidence!!!
			 ){									//--> this track enters a DIRC bar in this step!!!
				//
				//DircIncidence_pos		= postStepLocation;
				DircIncidence_pos		= track->GetPosition();
				//
				//double trackLen	= track->GetTrackLength();
				double mass		= track->GetDefinition()->GetPDGMass() / CLHEP::GeV;
				double charge	= track->GetDefinition()->GetPDGCharge()/eplus;
				double ptot		= track->GetMomentum().mag() / CLHEP::GeV;
				double etot		= sqrt(ptot*ptot + mass*mass);
				double beta		= ptot/etot;
				//
				if (fabs(charge)>0. && mass>0. && beta>1./1.4738){
					//
					if (iEntryIncCurrent < MAXDircIncidence){
						//
						int kstrindex		= postname.Index("bar_vol_");
						TString barvol		= postname;
								barvol.Remove(0,kstrindex+8);
						int		kbarvol	= atoi(barvol.Data());	// step->GetPostStepPoint()->GetTouchableHandle()->GetCopyNumber(0);
						TVector3 DetPos		= TVector3(postStepLocation.x(),postStepLocation.y(),postStepLocation.z());
						double DetPhi		= DetPos.Phi();
						double dphi			= DetPhi + 15.*(M_PI/180.);
						int    kBarBox		= -1;
						if (dphi< 0.     ) dphi += 2.*M_PI;
						if (dphi>=2.*M_PI) dphi -= 2.*M_PI;
						kBarBox				= dphi/(2.*M_PI/12.);	
						if (kBarBox<0||kBarBox>=12){ std::cout<<"error kBarBox"<<std::endl; exit(0); }			
						int loop_counter	= (kbarvol - 1) / 2;	// placing glue layers increments the copy number!!!!
						int kBar			= loop_counter / 4;		// y_index (0 to 9)
						//int kSection		= loop_counter % 4;		// z_index (0 to 3)
						//
						DircIncidence_mom		= track->GetMomentum();
						DircIncidence_momdir	= track->GetMomentumDirection();
						mDircIncidence_evt							= currentEventID;
						mDircIncidence_box[iEntryIncCurrent]		= kBarBox;
						mDircIncidence_bar[iEntryIncCurrent]		= kBar;
						mDircIncidence_trackID[iEntryIncCurrent]	= trackID;
						mDircIncidence_pdgCode[iEntryIncCurrent]	= pdgCode;
						mDircIncidence_x[iEntryIncCurrent]			= postStepLocation.x() / CLHEP::mm;			// mm
						mDircIncidence_y[iEntryIncCurrent]			= postStepLocation.y() / CLHEP::mm;			// mm
						mDircIncidence_z[iEntryIncCurrent]			= postStepLocation.z() / CLHEP::mm;			// mm
						mDircIncidence_r[iEntryIncCurrent]			= std::sqrt(mDircIncidence_x[iEntryIncCurrent]*mDircIncidence_x[iEntryIncCurrent]
																			   +mDircIncidence_y[iEntryIncCurrent]*mDircIncidence_y[iEntryIncCurrent]);	// mm
						mDircIncidence_px[iEntryIncCurrent]			= track->GetMomentum().x() / CLHEP::GeV;	// GeV
						mDircIncidence_py[iEntryIncCurrent]			= track->GetMomentum().y() / CLHEP::GeV;	// GeV
						mDircIncidence_pz[iEntryIncCurrent]			= track->GetMomentum().z() / CLHEP::GeV;	// GeV
						mDircIncidence_t[iEntryIncCurrent]			= track->GetGlobalTime() / CLHEP::ns;		// ns
						mDircIncidence_mass[iEntryIncCurrent]		= mass;										// GeV
						mDircIncidence_beta[iEntryIncCurrent]		= beta;										// GeV
						//
						// std::cout<<DetEta<<"\t "<<DetTheta<<" "<<DetPhi<<"\t barbox="<<kBarBox<<" kbarvol="<<kbarvol
						// 	<<"\t bar_num="<<bar_num<<" section_num="<<section_num
						// // 	<<"\t"<<touchable->GetVolume(0)->GetName()
						// // 	<<" "<<touchable->GetVolume(1)->GetName()
						// // 	<<" "<<touchable->GetVolume(2)->GetName()
						// 	<<std::endl;
						//ht1->Fill(DetPhi,DetTheta,1.0);
						//ht2->Fill(DetPhi,DetTheta,(double)kbarvol);
						//
						//
						if (DETAIL && !VERBOSE && currentEventID<20){
							std::cout<<"evt: "<<std::setw(6)<<currentEventID
									 <<"\t dirc incidence "<<std::setw(2)<<iEntryIncCurrent<<": "
									 <<std::setw(10)<<DircIncidence_pos.x()<<" "
									 <<std::setw(10)<<DircIncidence_pos.y()<<" "
									 <<std::setw(10)<<DircIncidence_pos.z()
									 <<"\t box= "<<kBarBox
									 <<"\t bar= "<<kBar
									 <<std::endl;
						}
						if (VERBOSE){
							TString prevname2 = prevname; if (prevname2.Contains("AV_25!DIRC_0#0!")){ prevname2.ReplaceAll("AV_25!DIRC_0#0!",""); prevname2.ReplaceAll("#0",""); }
							std::cout	<<"DIRC Incidence: evt="<<currentEventID<<" iFill="<<iEntryIncCurrent<<" trkID="<<trackID
										<<" Volumes="<<prevname2.Data()<<"->"<<postname.Data()
										<<" Box="<<kBarBox<<" Bar="<<kBar
										//<<" step="<<stepNumber
										<<" pdg="<<pdgCode
										//<<" mass="<<mass
										<<" beta= "<<mDircIncidence_beta[iEntryIncCurrent]
										<<" chg="<<charge
										<<" posn: "<<mDircIncidence_x[iEntryIncCurrent] <<" "<<mDircIncidence_y[iEntryIncCurrent]<<" "<<mDircIncidence_z[iEntryIncCurrent]<<" r: "<<mDircIncidence_r[iEntryIncCurrent]
										<<" mom: "<<mDircIncidence_px[iEntryIncCurrent]<<" "<<mDircIncidence_py[iEntryIncCurrent]<<" "<<mDircIncidence_pz[iEntryIncCurrent]
										<<" t: "<<mDircIncidence_t[iEntryIncCurrent]
										<<std::endl;
						}
						++iEntryIncCurrent;
						currentEventIDprev	= currentEventID;
					} else {
						std::cout<<"Incidence tree overflow!!   ...increase MAXDircIncidence: "<<MAXDircIncidence<<std::endl;
						exit(0);
					}
					//
				}	//---- end charge,mass,beta check...
				//
			}	// end check: primary entering bar?
			//
			//
			if (pdgCode==-22 && stepNumber==1 ){	// OP at creation
				auto thisStepPoint	= step->GetPreStepPoint();
				G4ThreeVector OPpos	= thisStepPoint->GetPosition();
				double OPcr_x	= OPpos.x();
				double OPcr_y	= OPpos.y();
				double OPcr_z	= OPpos.z();
				hOPcr_yx		->Fill(OPcr_x,OPcr_y);
				hOPcr_z			->Fill(OPcr_z);
				if (OPcr_z < Zbarsend){
					track->SetTrackStatus(fStopAndKill);
				}
			}

			//---> commented code block at end of file below went HERE <---			
			//
		}	// end operator function
 
     private:
		bool initialized = false;
		//
		G4EventManager* eventManager;
		//
		double Zbarsend;
		//
		G4ThreeVector DircIncidence_pos;
		G4ThreeVector DircIncidence_mom;
		G4ThreeVector DircIncidence_momdir;
		TString prevname="",postname="";
		static const int MAXDircIncidence		= 100;
		int    mDircIncidence_evt     			=   0; 
		int    mDircIncidence_ninc				=   0;
		inline static int    mDircIncidence_box[MAXDircIncidence]     = {0}; 
		inline static int    mDircIncidence_bar[MAXDircIncidence]     = {0}; 
		inline static int    mDircIncidence_trackID[MAXDircIncidence] = {0}; 
		inline static int    mDircIncidence_pdgCode[MAXDircIncidence] = {0}; 
		inline static double mDircIncidence_x[MAXDircIncidence]       = {0}; 
		inline static double mDircIncidence_y[MAXDircIncidence]       = {0}; 
		inline static double mDircIncidence_z[MAXDircIncidence]       = {0}; 
		inline static double mDircIncidence_r[MAXDircIncidence]       = {0}; 
		inline static double mDircIncidence_px[MAXDircIncidence]      = {0};
		inline static double mDircIncidence_py[MAXDircIncidence]      = {0};
		inline static double mDircIncidence_pz[MAXDircIncidence]      = {0};
		inline static double mDircIncidence_t[MAXDircIncidence]       = {0};
		inline static double mDircIncidence_mass[MAXDircIncidence]    = {0};
		inline static double mDircIncidence_beta[MAXDircIncidence]    = {0};
		int	currentEventID		= 0;
		int	currentEventIDprev	= -1;
		int iEntryIncCurrent	= 0;
		int nSeen	= 0;
		int nFill	= 0;
		int trackID;
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

