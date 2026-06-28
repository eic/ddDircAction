# ddDircAction
determine position and direction of primary particles incident on dirc bars, and save this info to a tree

## get and build code
```
cd eic
git clone git@github.com:eic/ddDircAction.git
```
then start eic-shell, and then build the stepping action:
```
eic-shell
cd ddDircAction
mkdir build install
cmake -B build -S . -DCMAKE_INSTALL_PREFIX=install -DCMAKE_C_COMPILER=/usr/bin/cc -DCMAKE_CXX_COMPILER=/usr/bin/g++
cmake --build build -- install
cd .. 
```

Next you need to add the created libraries to the path! (i have the next command in a "mysetup" script that i source just after starting eic-shell):
```
export LD_LIBRARY_PATH=/Users/sp/eic/ddDircAction/install/lib:${LD_LIBRARY_PATH}
```
and you need to change the parts of this path that are in front of eic/ddDircAction/install/lib .

Running the code will produce a root file containing the "incidence tree". This tree is filled once per event with variable length arrays holding the information at incidence of every particle with parentID = 0 that enters a DIRC bar from the air outside a DIRC bar. There are "ninc" incident particles per event. The tree format is like this:
```
  event number (starting from zero),
  Number of bar-incident particles in this event ("ninc")
  integer ID of struck bar: bar[ninc]  (values range: [0,9])
  integer G4 track ID: trackID[ninc] 
  integer PDG particle code: pdgCode[ninc]
  3 position doubles: x[ninc], y[ninc], z[ninc]  (in mm)
  3 momentum doubles: px[ninc], py[ninc], pz[ninc]  (in GeV)
  1 time double: t[ninc]  (in ns)
```
plus perhaps some convenience variables like the mass of the incident particle.
  
The position and momentum information is that at the G4 step where the (truth) G4Track enters any dirc bar ("bar_vol_XX" in the geometry). The bar ID field is needed to choose the proper LUT file for the PID reconstruction. In the future this will be estimated by finding the bar closest to the acts track state at the dirc "radius". 

If no particles hit any dirc bar in a given event, that event number will be missing from the incidence tree. It's up to the user to match the incidence tree event numbers up with those in the "main" npsim output tree. Tracks in the main npsim output tree may not also appear in the incidence tree - only parentID=0 ("primary") particles that strike dirc bars are saved to the incidence tree. 

## running the code

In the first command just below, i am throwing 50 GeV/c mu+ at "bar 4" (eicdirc bar=994) with the hpDIRC sitting alone in air (no rest of epic). 

Note the syntax of the --action.step part of the following command. This syntax is very fiddly! Right now, I only use the first argument (the filename base for the output root file), the other two parameters are placeholders for further development. You'll note that i set the string parameter to "sim_dirconly_5k_bar4polarscan.incidence" which has the same base as the main output root file, defined later in the same command: "sim_dirconly_5k_bar4polarscan.edm4hep.root".

In this way, the command below will produce two files:\
sim_dirconly_5k_bar4polarscan.edm4hep.root\
sim_dirconly_5k_bar4polarscan.incidence.root

Run a simulation with one primary muon per event and call the dirc stepping action:
```
npsim --runType batch \
--action.step '{"name":"ddDIRCactionStep","parameter":{"OutputBase":"sim_dirconly_5k_bar4polarscan.incidence", "fileNumber":1, "DetailLevel":1}}' \
--compactFile $DETECTOR_PATH/epic_dirc_only.xml -G -N 5000 --gun.particle "mu+" \
--gun.momentumMin 50*GeV --gun.momentumMax 50*GeV --gun.phiMin 358.45*deg --gun.phiMax 358.45*deg \
--gun.thetaMin 30*deg --gun.thetaMax 140*deg --gun.distribution uniform --gun.position 0*cm,0*cm,0*cm \
--part.userParticleHandler='' --outputFile sim_dirconly_5k_bar4polarscan.edm4hep.root
```

The command above throws the 50 GeV/c muons at phi = 358.45 degrees. This puts those muons at eta = 0 into the very middle of the face of "bar4". That is, the y position in the epic global coordinate system of the point where the 50 GeV muons enter the dirc bar plane is very close to -1.7cm (half the width of a dirc bar).

Note the due to non-solenoidal components of the field, the tracks are not quite centered on the bar anymore as the pseudorapidity of the tracks move away from zero. The impact point moves a few mm away from the center of the bar at high-eta.




