// This code implementation is the intellectual property of
// the RD44 GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: G4DAWNFILEScene.cc,v 2.6 1998/12/04 07:31:12 stanaka Exp $
// GEANT4 tag $Name: geant4-00 $
//
// Satoshi TANAKA
// DAWNFILE scene.


//=================//
#ifdef G4VIS_BUILD_DAWNFILE_DRIVER
//=================//


#define __G_ANSI_C__

//#define DEBUG_FR_SCENE

     //----- header files
#include <fstream.h>
#include <stdlib.h>
#include <string.h>
#include "globals.hh"
#include "G4FRConst.hh"
#include "G4DAWNFILE.hh"
#include "G4DAWNFILEScene.hh"
#include "G4DAWNFILEView.hh"
#include "G4Point3D.hh"
#include "G4VisAttributes.hh"
#include "G4Transform3D.hh"
#include "G4Polyhedron.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Polyline.hh"
#include "G4Trd.hh"
#include "G4Tubs.hh"
#include "G4Text.hh"
#include "G4Circle.hh"
#include "G4Square.hh"
#include "G4ModelingParameters.hh"
#include "G4VPhysicalVolume.hh"

//----- constants
const char  FR_ENV_CULL_INVISIBLE_OBJECTS [] = "G4DAWN_CULL_INVISIBLE_OBJECTS";
const char  G4PRIM_FILE_HEADER      [] = "g4_";
const char  DEFAULT_G4PRIM_FILE_NAME[] = "g4.prim";


///////////////////////////
// Driver-dependent part //
///////////////////////////


	//----- G4DAWNFILEScene, constructor
G4DAWNFILEScene::G4DAWNFILEScene (G4DAWNFILE& system, const G4String& name):
fSystem   (system)                        ,
G4VScene  (system, fSceneIdCount++, name) ,
fPrimDest ()                              ,
flag_in_modeling       (false)            ,
flag_saving_g4_prim    (false)            ,
COMMAND_BUF_SIZE       (G4FRofstream::SEND_BUFMAX)
{
	// count instantiated scenes
	fSceneCount++;

	// g4.prim filename and its directory
	if ( getenv( "G4DAWNFILE_DEST_DIR" ) == NULL ) {
		strcpy( fG4PrimDestDir , "" );
		strcpy( fG4PrimFileName, DEFAULT_G4PRIM_FILE_NAME );
	} else {
		strcpy( fG4PrimDestDir , getenv( "G4DAWNFILE_DEST_DIR" ) );
		strcpy( fG4PrimFileName, DEFAULT_G4PRIM_FILE_NAME        );
	}
		
	// maximum number of g4.prim files in the dest directory
	fMaxFileNum = 1 ; // initialization
	if ( getenv( "G4DAWNFILE_MAX_FILE_NUM" ) != NULL ) {	
		
		sscanf( getenv("G4DAWNFILE_MAX_FILE_NUM"), "%d", &fMaxFileNum ) ;

	} else {
		fMaxFileNum = 1 ;
	}
	if( fMaxFileNum < 1 ) { fMaxFileNum = 1 ; }

} 


	//----- G4DAWNFILEScene, destructor
G4DAWNFILEScene::~G4DAWNFILEScene () 
{
#if defined DEBUG_FR_SCENE
	G4cerr << "***** ~G4DAWNFILEScene" << endl;
#endif 
  fSceneCount--;
  ClearStore (); // clear current scene

}

void	G4DAWNFILEScene::SetG4PrimFileName() 
{
	// g4.prim, g4_1.prim, ..., g4_MAX_FILE_INDEX.prim
	const int MAX_FILE_INDEX = fMaxFileNum - 1 ;

	// dest directory (null if no environmental variables is set)
	strcpy ( fG4PrimFileName, fG4PrimDestDir) ; 

	// create full path name (default)
	strcat ( fG4PrimFileName, DEFAULT_G4PRIM_FILE_NAME );

	// Automatic updation of file names
	for( int i = 0 ; i < fMaxFileNum ; i++) { 

		// Message
		if( fMaxFileNum > 1 && i == MAX_FILE_INDEX ) {
		  G4cerr << "==========================================="   << endl; 
		  G4cerr << "WARNING MESSAGE from DAWNFILE driver:      "   << endl;
		  G4cerr << "  This file name is the final one in the   "   << endl;
		  G4cerr << "  automatic updation of the output file name." << endl; 
		  G4cerr << "  You may overwrite an existing file of   "    << endl; 
                  G4cerr << "  the same name.                          "    << endl;
		  G4cerr << "==========================================="   << endl; 
		}

		// re-determine file to G4DAWNFILE_DEST_DIR/g4_i.prim for i>0
		if( i >  0 ) { 
			sprintf( fG4PrimFileName, "%s%s%d.prim" , fG4PrimDestDir,  G4PRIM_FILE_HEADER, i );
		}

		// check validity of the file name
		ifstream  fin ; 
		fin.open(fG4PrimFileName) ;
		if(!fin) { 
			// new file	
			fin.close();  // error recovery
			break; 
		} else { 
			// already exists (try next) 
			fin.close(); 
		} 

	} // for 

	G4cerr << "===========================================" << endl; 
	G4cerr << "Output file: " <<    fG4PrimFileName         << endl; 
	G4cerr << "Muximal number of file in the destination directory: " << fMaxFileNum << endl; 
	G4cerr << "  (Customizable as: setenv G4DAWNFILE_MAX_FILE_NUM number) " << endl;
	G4cerr << "===========================================" << endl; 

} // G4DAWNFILEScene::SetG4PrimFileName()


void	G4DAWNFILEScene::BeginSavingG4Prim( void ) 
{
	if( !IsSavingG4Prim() ) 
	{ 
		SetG4PrimFileName() ; // returned to fG4PrimFileName
		fPrimDest.Open(fG4PrimFileName)   ;

		SendStr( FR_G4_PRIM_HEADER   )    ; 
		flag_saving_g4_prim = true        ; 
	} 
}

void	G4DAWNFILEScene::EndSavingG4Prim  ( void ) 
{
	if(  IsSavingG4Prim() )
	{
		fPrimDest.Close()               ;
		flag_saving_g4_prim = false ; 
	} 
}


/////////////////////////////////////////
// Common to DAWN and DAWNFILE drivers //
/////////////////////////////////////////

#define  G4FRSCENE  G4DAWNFILEScene
#include "G4FRSceneFunc.icc"
#undef   G4FRSCENE 

//////////////////////
// static variables //
//////////////////////

	//----- static variables
G4int G4DAWNFILEScene::fSceneIdCount = 0; 

G4int G4DAWNFILEScene::fSceneCount = 0;   
			// num of existing instances

#endif // G4VIS_BUILD_DAWNFILE_DRIVER