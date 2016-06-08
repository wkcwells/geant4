//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: G4VPhysVolRefArray.hh,v 1.1.4.1 2001/06/28 19:11:28 gunter Exp $
// GEANT4 tag $Name:  $
//

// class description:
//
//	This is a class which implements G4RWTPtrVector with
//      G4VPhysicalVolume.
//

// History:
// 00.11.17 Y.Morita  Initial version

#ifndef G4VPhysVolRefArray_hh
#define G4VPhysVolRefArray_hh 1

#include "g4rw/tpvector.h"
#include "globals.hh"

#include "G4VPhysicalVolume.hh"

typedef G4RWTPtrVector<G4VPhysicalVolume> G4VPhysVolRefVArray;

class G4VPhysVolRefArray 
{
  public: // with description
      G4VPhysVolRefArray();
      ~G4VPhysVolRefArray();
      //  The constructor and the destructor.

  private:
      G4VPhysVolRefVArray  transPhysVolPtrs;

  public: // with description
      inline void Insert(G4int i, G4VPhysicalVolume* aVol)
      { transPhysVolPtrs[i] = aVol; }
      // Insert aVol as i-th element.
      inline G4VPhysicalVolume* Get(G4int i)
      { return transPhysVolPtrs[i]; }
      // return the i-th element.
      inline void Resize(size_t n)
      { transPhysVolPtrs.resize(n); }
      // Resize the vector with n.

};

G4VPhysVolRefArray::G4VPhysVolRefArray()
{;}

G4VPhysVolRefArray::~G4VPhysVolRefArray()
{;}

#endif