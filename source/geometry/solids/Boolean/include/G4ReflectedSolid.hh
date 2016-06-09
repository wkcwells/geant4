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
// $Id: G4ReflectedSolid.hh,v 1.6 2002/10/28 11:36:28 gcosmo Exp $
// GEANT4 tag $Name: geant4-05-01 $
//
//
// class G4ReflectedSolid
//
// Class description:
//
// A Reflected solid is a solid that has been shifted from its original
// frame of reference to a new one.

// History:
//
// 23.07.01 V.Grichine: created
// 15.02.02 V.Grichine: get/set methods for fPtr(Direct)Transform3D
//
// ********************************************************************

#ifndef G4ReflectedSolid_HH
#define G4ReflectedSolid_HH

#include "G4VSolid.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4AffineTransform.hh"

class G4ReflectedSolid : public G4VSolid
{
  public:  // with description

    G4ReflectedSolid( const G4String& pName,
                            G4VSolid* pSolid ,
                      const G4Transform3D& transform  ) ;
      // For use in instantiating a transient instance.

    virtual ~G4ReflectedSolid() ;
      // Virtual destructor.

  public:  // without description 

    // Includes all the methods that a solid requires.

    EInside Inside( const G4ThreeVector& p ) const ; 

    G4bool CalculateExtent( const EAxis pAxis,
                            const G4VoxelLimits& pVoxelLimit,
                            const G4AffineTransform& pTransform,
                                  G4double& pMin, G4double& pMax) const ;

    G4ThreeVector SurfaceNormal( const G4ThreeVector& p ) const ;

    G4double DistanceToIn( const G4ThreeVector& p,
                           const G4ThreeVector& v  ) const ;

    G4double DistanceToIn( const G4ThreeVector& p) const ;

    G4double DistanceToOut( const G4ThreeVector& p,
                            const G4ThreeVector& v,
                            const G4bool calcNorm=false,
                                  G4bool *validNorm=0,
                                  G4ThreeVector *n=0      ) const ;

    G4double DistanceToOut( const G4ThreeVector& p ) const ;

    void ComputeDimensions(       G4VPVParameterisation* p,
                            const G4int n,
                            const G4VPhysicalVolume* pRep ) ;
                                   
  public:  // with description 

    virtual G4GeometryType  GetEntityType() const;

    virtual const G4ReflectedSolid* GetReflectedSolidPtr() const;
    virtual       G4ReflectedSolid* GetReflectedSolidPtr();
      // If the Solid is a "G4ReflectedSolid",
      // return a self pointer else return 0.

    G4VSolid*                GetConstituentMovedSolid() const;

    G4Transform3D        GetTransform3D() const; 
    void       SetTransform3D(G4Transform3D&);
    G4Transform3D        GetDirectTransform3D() const; 
    void       SetDirectTransform3D(G4Transform3D&);
      // Accessors methods.

    G4std::ostream& StreamInfo(G4std::ostream& os) const;

  public:  // without description

    void DescribeYourselfTo ( G4VGraphicsScene& scene ) const ;
    G4Polyhedron* CreatePolyhedron () const ;
    G4NURBS*      CreateNURBS      () const ;
      // For creating graphical representations (ie for visualisation).

  protected:

    G4AffineTransform        GetTransform() const; 
    void       SetTransform(G4AffineTransform&);
    G4AffineTransform        GetDirectTransform() const; 
    void       SetDirectTransform(G4AffineTransform&);
    G4RotationMatrix         GetFrameRotation() const;
    void  SetFrameRotation(const G4RotationMatrix&);
    G4ThreeVector            GetFrameTranslation() const; 
    void  SetFrameTranslation(const G4ThreeVector&); 
      // Get/Set the rotation/translation, as applied to the
      // frame of reference.

    G4RotationMatrix         GetObjectRotation() const;
    void  SetObjectRotation(const G4RotationMatrix&);
    G4ThreeVector            GetObjectTranslation() const; 
    void  SetObjectTranslation(const G4ThreeVector&); 
      // Get/Set the rotation/translation, as applied to the object.

    G4VSolid*          fPtrSolid        ;
    G4AffineTransform* fPtrTransform    ;
    G4AffineTransform* fDirectTransform ;

    G4Transform3D*     fPtrTransform3D    ;
    G4Transform3D*     fDirectTransform3D ;

  private:

    G4ReflectedSolid(const G4ReflectedSolid&);
    G4ReflectedSolid& operator=(const G4ReflectedSolid&);
      // Private copy constructor and assignment operator.
} ;

#endif