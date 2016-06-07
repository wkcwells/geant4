// This code implementation is the intellectual property of
// the RD44 GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: G4Ions.hh,v 2.1 1998/07/13 17:17:58 urbi Exp $
// GEANT4 tag $Name: geant4-00 $
//
// 
// ------------------------------------------------------------
//      GEANT 4 class header file
//
//      For information related to this code contact:
//      CERN, CN Division, ASD group
//      History: first implementation, based on object model of
//      Hisaya Kurashige, 27 June 1998
// ----------------------------------------------------------------


#ifndef G4Ions_h
#define G4Ions_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleWithCuts.hh"

// ######################################################################
// ###                          Ions                                 ###
// ######################################################################

class G4Ions : public G4ParticleWithCuts
{
 private:
   G4double  theIonsLengthCut;
   G4double* theIonsKineticEnergyCuts;

 public:
   G4Ions(
       const G4String&     aName,        G4double            mass,
       G4double            width,        G4double            charge,   
       G4int               iSpin,        G4int               iParity,    
       G4int               iConjugation, G4int               iIsospin,   
       G4int               iIsospin3,    G4int               gParity,
       const G4String&     pType,        G4int               lepton,      
       G4int               baryon,       G4int               encoding,
       G4bool              stable,       G4double            lifetime,
       G4DecayTable        *decaytable
   );
   G4Ions*    IonsDefinition(){return this;};
   G4Ions*    Ions(){return this;};
   G4double   GetCuts() {return theIonsLengthCut;}   
   G4double*  GetCutsInEnergy() {return theIonsKineticEnergyCuts;};

   void SetCuts(G4double aCut); 
};

inline void G4Ions::SetCuts(G4double aCut)
{
  CalcEnergyCuts(aCut);
  theIonsLengthCut = theCutInMaxInteractionLength;  
  theIonsKineticEnergyCuts = theKineticEnergyCuts;
}

#endif





