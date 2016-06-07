// This code implementation is the intellectual property of
// the RD44 GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: G4UIcmdWithoutParameter.hh,v 2.0 1998/07/02 17:07:57 gunter Exp $
// GEANT4 tag $Name: geant4-00 $
//
//

#ifndef G4UIcmdWithoutParameter_H
#define G4UIcmdWithoutParameter_H 1

#include "G4UIcommand.hh"

class G4UIcmdWithoutParameter : public G4UIcommand
{
  public:
    G4UIcmdWithoutParameter
    (const char * theCommandPath,G4UImessenger * theMessenger);
};

#endif