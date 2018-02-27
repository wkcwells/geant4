//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// INCL++ intra-nuclear cascade model
// Alain Boudard, CEA-Saclay, France
// Joseph Cugnon, University of Liege, Belgium
// Jean-Christophe David, CEA-Saclay, France
// Pekka Kaitaniemi, CEA-Saclay, France, and Helsinki Institute of Physics, Finland
// Sylvie Leray, CEA-Saclay, France
// Davide Mancusi, CEA-Saclay, France
//
#define INCLXX_IN_GEANT4_MODE 1

#include "globals.hh"

#include "G4INCLNKbToNKbChannel.hh"
#include "G4INCLKinematicsUtils.hh"
#include "G4INCLBinaryCollisionAvatar.hh"
#include "G4INCLRandom.hh"
#include "G4INCLGlobals.hh"
#include "G4INCLLogger.hh"
#include <algorithm>
#include "G4INCLPhaseSpaceGenerator.hh"

namespace G4INCL {
	
	NKbToNKbChannel::NKbToNKbChannel(Particle *p1, Particle *p2)
		: particle1(p1), particle2(p2)
		{}
	
	NKbToNKbChannel::~NKbToNKbChannel(){}
	
	void NKbToNKbChannel::fillFinalState(FinalState *fs) {
		
		Particle *nucleon;
		Particle *kaon;
		
		if(particle1->isNucleon()){
			nucleon = particle1;
			kaon = particle2;
		}
		else{
			nucleon = particle2;
			kaon = particle1;
		}
		
// assert((ParticleTable::getIsospin(nucleon->getType()) + ParticleTable::getIsospin(kaon->getType())) == 0);
		
		ThreeVector mom_kaon = KaonMomentum(kaon,nucleon);
		
		if(kaon->getType() == KZeroBar){
			nucleon->setType(Proton);
			kaon->setType(KMinus);
		}
		else{
			nucleon->setType(Neutron);
			kaon->setType(KZeroBar);
		}
		
		G4double norm = KinematicsUtils::momentumInCM(kaon,nucleon);

		kaon->setMomentum(mom_kaon*norm);
		nucleon->setMomentum(-mom_kaon*norm);
		
		kaon->adjustEnergyFromMomentum();
		nucleon->adjustEnergyFromMomentum();
		
		
		fs->addModifiedParticle(nucleon);
		fs->addModifiedParticle(kaon);
				
	}
	
	ThreeVector NKbToNKbChannel::KaonMomentum(Particle const * const kaon, Particle const * const nucleon){
		
		const G4double pLab = KinematicsUtils::momentumInLab(kaon,nucleon);
		
		if(pLab < 235.) return Random::normVector(); // isotropic
		
		G4double cos_theta = 1.;
		G4double sin_theta = 0.;
		const G4double cos_phi = std::cos(Random::shoot()*Math::twoPi);
		const G4double sin_phi = std::sqrt(1-cos_phi*cos_phi);
		
		const G4double x = kaon->getMomentum().getX();
		const G4double y = kaon->getMomentum().getY();
		const G4double z = kaon->getMomentum().getZ();
		
		const G4double r = std::sqrt(x*x+y*y+z*z);
		const G4double rho = std::sqrt(x*x+y*y);
		
		if(pLab >= 1355.){
			const G4double b = 12. * pLab/2375.; // correspond to the forward slope description at 2375 MeV/c in K- p elastic
			cos_theta = std::log(Random::shoot()*(std::exp(b)-std::exp(-b))+std::exp(-b))/b;
			sin_theta = std::sqrt(1-cos_theta*cos_theta);
			
		}
		else{
			const G4double Legendre_coef[225][9] = {
				{235,-0.30755,0,-0.04859,-0.01348,-9e-05,5e-05,0,0},
				{240,-0.3041,0,-0.03549,-0.01096,-8e-05,5e-05,0,0},
				{245,-0.30451,0,-0.02232,-0.00843,-8e-05,4e-05,0,0},
				{250,-0.31028,0,-0.00899,-0.00585,-7e-05,4e-05,0,0},
				{255,-0.32259,0,0.00462,-0.00319,-6e-05,4e-05,0,0},
				{260,-0.34185,0,0.01859,-0.00041,-5e-05,3e-05,0,0},
				{265,-0.36746,0,0.03305,0.00251,-4e-05,3e-05,0,0},
				{270,-0.39766,0,0.04809,0.00563,-3e-05,2e-05,0,0},
				{275,-0.42975,0,0.0638,0.00898,-2e-05,2e-05,0,0},
				{280,-0.46069,1e-05,0.08023,0.0126,0,1e-05,0,0},
				{285,-0.48736,5e-05,0.0973,0.0165,1e-05,0,0,0},
				{290,-0.50729,2e-04,0.11487,0.02069,4e-05,-1e-05,0,0},
				{295,-0.51855,0.00063,0.13282,0.02519,6e-05,-3e-05,0,0},
				{300,-0.52024,0.00134,0.15096,0.02998,9e-05,-4e-05,0,0},
				{305,-0.51207,0.00124,0.16892,0.03503,0.00013,-6e-05,0,0},
				{310,-0.49468,-0.002,0.18618,0.04031,0.00016,-9e-05,0,0},
				{315,-0.46892,-0.00933,0.20212,0.04568,0.00021,-0.00011,0,0},
				{320,-0.43609,-0.01586,0.21604,0.05101,0.00026,-0.00014,0,0},
				{325,-0.3973,-0.01303,0.22726,0.05617,0.00031,-0.00017,0,0},
				{330,-0.35379,0.00167,0.23511,0.06107,0.00037,-0.00021,0,0},
				{335,-0.3067,0.0219,0.23911,0.06576,0.00044,-0.00024,0,0},
				{340,-0.25739,0.04678,0.23878,0.07033,5e-04,-0.00028,0,0},
				{345,-0.20755,0.09094,0.23367,0.07488,0.00057,-0.00032,0,0},
				{350,-0.15908,0.17482,0.22334,0.0795,0.00064,-0.00037,0,0},
				{355,-0.11446,0.30682,0.20753,0.08421,0.00071,-0.00041,0,0},
				{360,-0.07625,0.48075,0.1861,0.08906,0.00078,-0.00045,0,0},
				{365,-0.04747,0.68345,0.1593,0.09421,0.00083,-0.00049,0,0},
				{370,-0.03064,0.90388,0.1275,0.09983,0.00088,-0.00052,0,0},
				{375,-0.02812,1.13397,0.09104,0.10608,0.00091,-0.00054,0,0},
				{380,-0.04087,1.3636,0.05039,0.11311,0.00092,-0.00056,0,0},
				{385,-0.06879,1.57657,0.00644,0.12093,9e-04,-0.00056,0,0},
				{390,-0.10972,1.75346,-0.03978,0.1295,0.00085,-0.00054,0,0},
				{395,-0.16022,1.87981,-0.08707,0.13859,0.00076,-5e-04,0,0},
				{400,-0.21553,1.9509,-0.1342,0.14795,0.00063,-0.00043,0,0},
				{405,-0.27043,1.97287,-0.17995,0.15732,0.00043,-0.00034,0,0},
				{410,-0.32009,1.95458,-0.22323,0.16643,0.00017,-2e-04,0,0},
				{415,-0.36095,1.89945,-0.26354,0.1749,-0.00016,-2e-05,0,0},
				{420,-0.3912,1.80489,-0.30052,0.18232,-0.00058,2e-04,0,0},
				{425,-0.4106,1.68228,-0.3338,0.18828,-0.0011,0.00048,0,0},
				{430,-0.42021,1.56358,-0.36315,0.19246,-0.00172,0.00082,0,0},
				{435,-0.42128,1.47378,-0.38888,0.19478,-0.00244,0.00123,0,0},
				{440,-0.415,1.42688,-0.41138,0.19524,-0.00329,0.0017,0,0},
				{445,-0.40201,1.39998,-0.43103,0.19376,-0.00425,0.00225,0,0},
				{450,-0.38247,1.36801,-0.44826,0.19036,-0.00534,0.00286,0,0},
				{455,-0.35589,1.33185,-0.46368,0.18542,-0.00653,0.00355,0,0},
				{460,-0.3216,1.29215,-0.47787,0.17941,-0.00782,0.0043,0,0},
				{465,-0.27917,1.2499,-0.49102,0.1727,-0.00918,0.0051,0,0},
				{470,-0.22973,1.20597,-0.50321,0.16567,-0.0106,0.00594,0,0},
				{475,-0.17713,1.16167,-0.5145,0.15865,-0.01204,0.00682,0,0},
				{480,-0.12841,1.11813,-0.52493,0.15196,-0.01349,0.00771,0,0},
				{485,-0.09149,1.07666,-0.53436,0.14575,-0.01489,0.0086,0,0},
				{490,-0.07028,1.03839,-0.5426,0.14013,-0.01618,0.00944,0,0},
				{495,-0.06296,1.00433,-0.54947,0.13518,-0.0173,0.01021,0,0},
				{500,-0.06366,0.97503,-0.55477,0.13101,-0.01819,0.01087,0,0},
				{505,-0.06664,0.95085,-0.55833,0.12773,-0.0188,0.0114,0,0},
				{510,-0.06771,0.93169,-0.55996,0.12544,-0.01906,0.01175,0,0},
				{515,-0.06476,0.91724,-0.55947,0.12423,-0.01891,0.01191,0,0},
				{520,-0.05705,0.90697,-0.55703,0.12437,-0.01831,0.01186,0,0},
				{525,-0.04488,0.90021,-0.55318,0.12594,-0.01736,0.01179,0,0},
				{530,-0.02927,0.8963,-0.54843,0.12893,-0.01624,0.01196,0,0},
				{535,-0.01183,0.8945,-0.5433,0.13334,-0.01512,0.0126,0,0},
				{540,0.00596,0.89424,-0.53831,0.13906,-0.01414,0.01394,0,0},
				{545,0.02306,0.89492,-0.5339,0.1458,-0.0134,0.01611,0,0},
				{550,0.039,0.89617,-0.53048,0.15329,-0.01298,0.01915,0,0},
				{555,0.05391,0.89758,-0.52847,0.16125,-0.01294,0.02316,0,0},
				{560,0.06806,0.89897,-0.52824,0.16947,-0.01334,0.02816,0,0},
				{565,0.08166,0.90012,-0.52974,0.17794,-0.01416,0.03398,0,0},
				{570,0.09478,0.90105,-0.53283,0.18651,-0.01529,0.0403,0,0},
				{575,0.10746,0.9017,-0.53736,0.19494,-0.01659,0.04682,0,0},
				{580,0.11985,0.90216,-0.54318,0.20297,-0.01795,0.05322,0,0},
				{585,0.13218,0.90248,-0.55002,0.21039,-0.01909,0.05908,0,0},
				{590,0.14481,0.90282,-0.55752,0.21698,-0.01964,0.06395,0,0},
				{595,0.15769,0.90328,-0.56531,0.22255,-0.01921,0.06734,0,0},
				{600,0.17046,0.90397,-0.57302,0.2269,-0.01743,0.0688,0,0},
				{605,0.18205,0.90496,-0.58026,0.23009,-0.01427,0.06815,0,0},
				{610,0.19103,0.90635,-0.58673,0.23229,-0.00999,0.06549,0,0},
				{615,0.19589,0.90814,-0.59209,0.2337,-0.00486,0.06093,0,0},
				{620,0.19545,0.91035,-0.59603,0.2345,0.00083,0.05457,0,0},
				{625,0.18873,0.91297,-0.59864,0.23485,0.00685,0.04682,0,0},
				{630,0.17514,0.91598,-0.60046,0.2349,0.01298,0.03844,0,0},
				{635,0.15461,0.91935,-0.60188,0.23486,0.0191,0.03017,0,0},
				{640,0.12791,0.92305,-0.60283,0.23506,0.0253,0.02263,0,0},
				{645,0.09694,0.92707,-0.60322,0.23576,0.03151,0.01633,0,0},
				{650,0.06435,0.9314,-0.60304,0.23717,0.03758,0.01172,0,0},
				{655,0.03278,0.93603,-0.60224,0.23951,0.04331,0.00924,0,0},
				{660,0.00403,0.94096,-0.60081,0.24298,0.04854,0.00932,0,0},
				{665,-0.02138,0.9462,-0.59882,0.24774,0.05304,0.01201,0,0},
				{670,-0.04399,0.95174,-0.59638,0.25384,0.05655,0.01685,0,0},
				{675,-0.06489,0.95757,-0.59359,0.26128,0.05879,0.02333,0,0},
				{680,-0.08497,0.96367,-0.59053,0.27011,0.05955,0.03096,-1e-05,0},
				{685,-0.10471,0.97,-0.58712,0.2802,0.0589,0.03927,-1e-05,0},
				{690,-0.12371,0.97649,-0.58315,0.29136,0.05721,0.0478,-1e-05,0},
				{695,-0.14097,0.98305,-0.57828,0.3031,0.05487,0.05603,-1e-05,0},
				{700,-0.15469,0.98957,-0.57209,0.31472,0.05231,0.06337,-1e-05,0},
				{705,-0.16271,0.99591,-0.56443,0.3256,0.04979,0.06932,-1e-05,0},
				{710,-0.16255,1.00194,-0.55574,0.33535,0.04732,0.07359,-2e-05,0},
				{715,-0.15199,1.00752,-0.54657,0.34363,0.04488,0.07591,-2e-05,0},
				{720,-0.12951,1.01251,-0.53743,0.3499,0.04245,0.07587,-2e-05,0},
				{725,-0.09529,1.0168,-0.52879,0.35376,0.03995,0.07327,-2e-05,0},
				{730,-0.05159,1.0203,-0.5212,0.35562,0.03722,0.06853,-2e-05,0},
				{735,-0.00245,1.02293,-0.51519,0.35595,0.0341,0.06213,-2e-05,0},
				{740,0.04772,1.02468,-0.51129,0.35522,0.03044,0.05455,-2e-05,0},
				{745,0.09584,1.02553,-0.51005,0.35387,0.02597,0.04627,-2e-05,0},
				{750,0.14086,1.02552,-0.51202,0.35229,0.02027,0.03776,-1e-05,0},
				{755,0.18338,1.02466,-0.51778,0.35084,0.01302,0.02948,-1e-05,0},
				{760,0.22444,1.023,-0.52787,0.34965,0.00409,0.02179,0,0},
				{765,0.2645,1.0206,-0.54268,0.34874,-0.00656,0.01494,1e-05,0},
				{770,0.30276,1.01748,-0.56211,0.34808,-0.01882,0.00896,2e-05,0},
				{775,0.33729,1.01369,-0.58582,0.34762,-0.03243,0.00376,4e-05,0},
				{780,0.36532,1.00926,-0.61343,0.34741,-0.04713,-8e-04,6e-05,0},
				{785,0.38445,1.00421,-0.64453,0.34758,-0.06254,-0.00496,8e-05,0},
				{790,0.39323,0.99857,-0.67869,0.34828,-0.07831,-0.00897,0.00011,0},
				{795,0.39129,0.99243,-0.71549,0.34967,-0.09409,-0.01306,0.00014,0},
				{800,0.37889,0.98587,-0.75456,0.35269,-0.10967,-0.01765,0.00017,0},
				{805,0.35624,0.97903,-0.79561,0.35848,-0.12507,-0.023,0.00021,0},
				{810,0.32292,0.97211,-0.83834,0.36717,-0.14022,-0.02894,0.00025,0},
				{815,0.27791,0.96533,-0.88246,0.37885,-0.15509,-0.03525,0.00029,0},
				{820,0.22029,0.95894,-0.92768,0.39359,-0.16961,-0.04173,0.00034,0},
				{825,0.15048,0.9532,-0.9737,0.41145,-0.18373,-0.04817,0.00039,0},
				{830,0.07108,0.94826,-1.02023,0.43252,-0.1974,-0.05435,0.00044,0},
				{835,-0.0136,0.94426,-1.06697,0.45687,-0.21057,-0.06007,0.00049,0},
				{840,-0.09855,0.94118,-1.11361,0.48455,-0.22318,-0.06513,0.00054,0},
				{845,-0.17951,0.9389,-1.15976,0.51525,-0.23515,-0.06938,0.00059,0},
				{850,-0.25366,0.93727,-1.20484,0.54811,-0.24638,-0.07279,0.00064,0},
				{855,-0.31965,0.93606,-1.24827,0.58227,-0.25672,-0.07535,0.00069,0},
				{860,-0.37707,0.93506,-1.28948,0.61685,-0.26608,-0.07701,0.00072,0},
				{865,-0.42612,0.93404,-1.3279,0.651,-0.27433,-0.07778,0.00075,0},
				{870,-0.46711,0.93281,-1.36298,0.68418,-0.28136,-0.0779,0.00077,0},
				{875,-0.50042,0.93129,-1.39424,0.7161,-0.28708,-0.07783,0.00078,0},
				{880,-0.52649,0.92936,-1.42118,0.74648,-0.29139,-0.07803,0.00076,0},
				{885,-0.54572,0.92699,-1.44339,0.77492,-0.29417,-0.07883,0.00072,0},
				{890,-0.55824,0.92417,-1.46054,0.801,-0.29529,-0.08047,0.00065,0},
				{895,-0.56399,0.92098,-1.47259,0.82492,-0.29497,-0.08289,0.00054,0},
				{900,-0.56271,0.91754,-1.47975,0.84731,-0.29367,-0.08579,0.00039,0},
				{905,-0.55466,0.91396,-1.48218,0.86857,-0.29166,-0.08892,0.00018,0},
				{910,-0.54086,0.91044,-1.48007,0.88907,-0.28921,-0.09206,-8e-05,0},
				{915,-0.52332,0.90717,-1.47358,0.90921,-0.28662,-0.09496,-0.00042,0},
				{920,-0.5045,0.90438,-1.46288,0.92935,-0.28415,-0.09741,-0.00084,0},
				{925,-0.48654,0.9023,-1.4482,0.94975,-0.28214,-0.09924,-0.00135,0},
				{930,-0.47053,0.90114,-1.42984,0.97058,-0.281,-0.10034,-0.00195,0},
				{935,-0.45659,0.90112,-1.40806,0.99201,-0.2811,-0.10059,-0.00266,0},
				{940,-0.44423,0.90246,-1.38314,1.01421,-0.28285,-0.09988,-0.00348,0},
				{945,-0.43287,0.90525,-1.35536,1.0371,-0.28653,-0.09816,-0.00441,0},
				{950,-0.42193,0.90965,-1.32498,1.06041,-0.29239,-0.09538,-0.00544,0},
				{955,-0.41093,0.91577,-1.29226,1.08389,-0.30066,-0.09153,-0.00659,0},
				{960,-0.39924,0.92369,-1.25746,1.10725,-0.31158,-0.08656,-0.00783,0},
				{965,-0.38604,0.93346,-1.22095,1.13008,-0.32524,-0.08039,-0.00918,0},
				{970,-0.37027,0.94514,-1.18303,1.15197,-0.34144,-0.07311,-0.01058,0},
				{975,-0.35077,0.95876,-1.14385,1.17271,-0.35991,-0.06496,-0.01199,0},
				{980,-0.32717,0.97435,-1.10358,1.19206,-0.38038,-0.05616,-0.01336,0},
				{985,-0.3004,0.99188,-1.0624,1.20979,-0.40261,-0.04696,-0.01464,0},
				{990,-0.27312,1.01135,-1.02046,1.2257,-0.42632,-0.03758,-0.01578,0},
				{995,-0.24834,1.03272,-0.97795,1.23954,-0.45126,-0.02826,-0.01674,0},
				{1000,-0.22769,1.0559,-0.93501,1.2511,-0.47716,-0.01922,-0.01745,0},
				{1005,-0.21032,1.0808,-0.89184,1.26017,-0.50377,-0.01069,-0.01787,0},
				{1010,-0.19369,1.10728,-0.84866,1.26669,-0.53095,-0.00261,-0.0179,0},
				{1015,-0.17436,1.13513,-0.80576,1.27066,-0.55859,0.00512,-0.01742,0},
				{1020,-0.1494,1.16411,-0.7634,1.27208,-0.58657,0.01264,-0.01633,0},
				{1025,-0.11749,1.19391,-0.72184,1.27097,-0.61479,0.02007,-0.01451,0},
				{1030,-0.081,1.22417,-0.68137,1.26732,-0.64314,0.02755,-0.01185,0},
				{1035,-0.04533,1.25445,-0.64225,1.26114,-0.6715,0.0352,-0.00824,0},
				{1040,-0.01646,1.28424,-0.60476,1.25244,-0.69977,0.04316,-0.00356,0},
				{1045,0.00361,1.31304,-0.56915,1.24121,-0.72785,0.05154,0.00231,0},
				{1050,0.01552,1.34024,-0.53567,1.22751,-0.75561,0.06047,0.0094,0},
				{1055,0.02103,1.36529,-0.50435,1.21159,-0.78295,0.07001,0.01752,0},
				{1060,0.02215,1.38771,-0.47519,1.19376,-0.80975,0.08022,0.02639,0},
				{1065,0.02047,1.40712,-0.4482,1.1743,-0.83589,0.09116,0.03575,0},
				{1070,0.01735,1.4233,-0.42337,1.15353,-0.86126,0.10289,0.04532,0},
				{1075,0.0137,1.43585,-0.40071,1.13173,-0.88574,0.11546,0.05485,0},
				{1080,0.00983,1.44489,-0.38022,1.10922,-0.90921,0.12892,0.06405,0},
				{1085,0.00516,1.45066,-0.3619,1.08628,-0.93157,0.14335,0.07265,0},
				{1090,-0.00091,1.45292,-0.34574,1.0632,-0.95272,0.15877,0.08044,0},
				{1095,-0.00921,1.4522,-0.33171,1.04013,-0.97273,0.17517,0.08734,0},
				{1100,-0.01996,1.44884,-0.31977,1.01722,-0.99171,0.19252,0.09332,-2e-05},
				{1105,-0.03239,1.44326,-0.30987,0.99459,-1.00975,0.2108,0.09835,-4e-05},
				{1110,-0.0447,1.43582,-0.30197,0.97238,-1.02695,0.22997,0.1024,-6e-05},
				{1115,-0.05508,1.42673,-0.29604,0.95074,-1.0434,0.25,0.10545,-9e-05},
				{1120,-0.06264,1.41667,-0.29202,0.92979,-1.05922,0.27085,0.10746,-0.00015},
				{1125,-0.06741,1.40585,-0.28988,0.90967,-1.0745,0.29251,0.10841,-0.00023},
				{1130,-0.06996,1.39476,-0.28958,0.89052,-1.08934,0.31494,0.10826,-0.00036},
				{1135,-0.07105,1.38376,-0.29107,0.87247,-1.10383,0.3381,0.107,-0.00054},
				{1140,-0.0711,1.3732,-0.29431,0.85567,-1.11808,0.36197,0.10458,-0.00087},
				{1145,-0.07085,1.36335,-0.29927,0.84024,-1.13219,0.38652,0.10099,-0.00128},
				{1150,-0.07116,1.35452,-0.30589,0.82632,-1.14626,0.41172,0.09619,-0.00185},
				{1155,-0.07304,1.34687,-0.31414,0.81405,-1.16038,0.43753,0.09015,-0.00263},
				{1160,-0.07771,1.34061,-0.32397,0.80356,-1.17466,0.46393,0.08284,-0.00367},
				{1165,-0.08664,1.3359,-0.33535,0.79499,-1.18919,0.49089,0.07424,-0.00506},
				{1170,-0.10173,1.33284,-0.34823,0.78841,-1.20404,0.51834,0.06434,-0.00686},
				{1175,-0.12509,1.33145,-0.36258,0.78356,-1.21914,0.54614,0.05318,-0.00925},
				{1180,-0.15682,1.33177,-0.37838,0.78014,-1.23438,0.57409,0.04086,-0.0122},
				{1185,-0.19464,1.33374,-0.39559,0.77782,-1.24965,0.60201,0.02744,-0.01586},
				{1190,-0.23352,1.33727,-0.41419,0.7763,-1.26484,0.62972,0.01301,-0.02035},
				{1195,-0.26876,1.34221,-0.43415,0.77525,-1.27985,0.65702,-0.00237,-0.02577},
				{1200,-0.299,1.34841,-0.45544,0.77437,-1.29457,0.68375,-0.01862,-0.0322},
				{1205,-0.32631,1.35562,-0.47804,0.77332,-1.30888,0.7097,-0.03565,-0.03976},
				{1210,-0.35505,1.36358,-0.50188,0.77186,-1.32269,0.73474,-0.05339,-0.04851},
				{1215,-0.38961,1.372,-0.52686,0.7699,-1.33587,0.75888,-0.07178,-0.0585},
				{1220,-0.43176,1.38055,-0.55282,0.76744,-1.3483,0.78219,-0.09072,-0.06977},
				{1225,-0.47936,1.38889,-0.57962,0.76446,-1.35986,0.80471,-0.11016,-0.08229},
				{1230,-0.52635,1.39666,-0.60711,0.76094,-1.37043,0.82652,-0.13,-0.09604},
				{1235,-0.56649,1.40351,-0.63516,0.75687,-1.37988,0.84766,-0.15019,-0.11094},
				{1240,-0.59709,1.40914,-0.66361,0.75222,-1.3881,0.86819,-0.17064,-0.12681},
				{1245,-0.61879,1.41323,-0.69232,0.74699,-1.39497,0.88817,-0.19128,-0.14349},
				{1250,-0.63452,1.41555,-0.72116,0.74115,-1.40036,0.90767,-0.21203,-0.16074},
				{1255,-0.6472,1.4159,-0.74996,0.73469,-1.40415,0.92673,-0.23283,-0.17827},
				{1260,-0.65841,1.4142,-0.77859,0.7276,-1.40622,0.94542,-0.25358,-0.19575},
				{1265,-0.66819,1.41043,-0.80691,0.71985,-1.40646,0.96379,-0.27423,-0.21283},
				{1270,-0.67506,1.40465,-0.83476,0.71143,-1.40474,0.9819,-0.29469,-0.22916},
				{1275,-0.67695,1.39703,-0.86202,0.70233,-1.40093,0.99981,-0.3149,-0.24439},
				{1280,-0.67215,1.38781,-0.88852,0.69252,-1.39492,1.01758,-0.33477,-0.25814},
				{1285,-0.65883,1.37728,-0.91412,0.68199,-1.38659,1.03527,-0.35423,-0.27016},
				{1290,-0.63639,1.36579,-0.93873,0.67076,-1.37587,1.05293,-0.37323,-0.28021},
				{1295,-0.60463,1.35371,-0.96237,0.65893,-1.3629,1.07059,-0.3918,-0.28812},
				{1300,-0.56527,1.34141,-0.98514,0.64665,-1.34787,1.08829,-0.41,-0.29382},
				{1305,-0.52229,1.32923,-1.0071,0.63407,-1.33099,1.10606,-0.4279,-0.29727},
				{1310,-0.4806,1.31751,-1.02834,0.62132,-1.31244,1.12394,-0.44553,-0.29853},
				{1315,-0.44384,1.3065,-1.04893,0.60855,-1.29241,1.14195,-0.46298,-0.29776},
				{1320,-0.41298,1.29641,-1.06896,0.5959,-1.27111,1.16013,-0.48028,-0.29505},
				{1325,-0.38708,1.28739,-1.0885,0.58352,-1.24873,1.17852,-0.4975,-0.29066},
				{1330,-0.36429,1.27952,-1.10764,0.57154,-1.22546,1.19714,-0.5147,-0.28485},
				{1335,-0.34347,1.27284,-1.12645,0.56012,-1.2015,1.21602,-0.53193,-0.27787},
				{1340,-0.32464,1.26732,-1.14501,0.54939,-1.17704,1.23521,-0.54926,-0.26998},
				{1345,-0.30854,1.2629,-1.1634,0.5395,-1.15228,1.25472,-0.56673,-0.26145},
				{1350,-0.29584,1.25949,-1.18169,0.53059,-1.1274,1.27461,-0.58441,-0.2525},
				{1355,-0.28655,1.25697,-1.19998,0.52281,-1.10262,1.29488,-0.60235,-0.2434}};
			
			const G4int coef_ener = G4int((pLab-Legendre_coef[0][0])/5);
			const G4double sup_ener = pLab/5. - coef_ener -Legendre_coef[0][0]/5;
			
// assert(pLab >= Legendre_coef[coef_ener][0] && pLab < Legendre_coef[coef_ener+1][0]);
			
			// Legendre coefficient normalized
			const G4double A0 = 1.;
			const G4double A1 = (1-sup_ener)*Legendre_coef[coef_ener][1] + sup_ener*Legendre_coef[coef_ener+1][1];
			const G4double A2 = (1-sup_ener)*Legendre_coef[coef_ener][2] + sup_ener*Legendre_coef[coef_ener+1][2];
			const G4double A3 = (1-sup_ener)*Legendre_coef[coef_ener][3] + sup_ener*Legendre_coef[coef_ener+1][3];
			const G4double A4 = (1-sup_ener)*Legendre_coef[coef_ener][4] + sup_ener*Legendre_coef[coef_ener+1][4];
			const G4double A5 = (1-sup_ener)*Legendre_coef[coef_ener][5] + sup_ener*Legendre_coef[coef_ener+1][5];
			const G4double A6 = (1-sup_ener)*Legendre_coef[coef_ener][6] + sup_ener*Legendre_coef[coef_ener+1][6];
			const G4double A7 = (1-sup_ener)*Legendre_coef[coef_ener][7] + sup_ener*Legendre_coef[coef_ener+1][7];
			const G4double A8 = (1-sup_ener)*Legendre_coef[coef_ener][8] + sup_ener*Legendre_coef[coef_ener+1][8];
			
			// Theoritical max if all Ai > 0 (often the case)
			const G4double A = std::fabs(A0) + std::fabs(A1) + std::fabs(A2) + std::fabs(A3) + std::fabs(A4) + std::fabs(A5) + std::fabs(A6) + std::fabs(A7) + std::fabs(A8);
			
			G4bool success = false;
			G4int maxloop = 0;
			
			while(!success && maxloop < 1000){
				
				cos_theta = Random::shoot()*2-1.; // not optimized
				
				// Legendre Polynomial
				G4double P0 = A0;
				G4double P1 = A1*cos_theta;
				G4double P2 = A2/2.*(3*std::pow(cos_theta,2)-1);
				G4double P3 = A3/2.*(5*std::pow(cos_theta,3)-3*cos_theta);
				G4double P4 = A4/8.*(35*std::pow(cos_theta,4)-30*std::pow(cos_theta,2)+3);
				G4double P5 = A5/8.*(63*std::pow(cos_theta,5)-70*std::pow(cos_theta,3)+15*cos_theta);
				G4double P6 = A6/16.*(231*std::pow(cos_theta,6)-315*std::pow(cos_theta,4)+105*std::pow(cos_theta,2)-5);
				G4double P7 = A7/16.*(429*std::pow(cos_theta,7)-693*std::pow(cos_theta,5)+315*std::pow(cos_theta,3)-35*cos_theta);
				G4double P8 = A8/128.*(6435*std::pow(cos_theta,8)-12012*std::pow(cos_theta,6)+6930*std::pow(cos_theta,4)-1260*std::pow(cos_theta,2)+35);
				
				G4double P = (P0 + P1 + P2 + P3 + P4 + P5 + P6 + P7 + P8)/2; // /2 for the normalisation
				
				if(Random::shoot()*A < P) success = true;
				maxloop +=1 ;
				if(maxloop==1000) cos_theta = std::log(Random::shoot()*(std::exp(10.)-std::exp(-10.))+std::exp(-10.))/10.; // if no success in 1E4 shoot, probably angulard distribution piked very forward 
			}
			sin_theta = std::sqrt(1-cos_theta*cos_theta);
		}
		
		if(rho == 0) return ThreeVector(sin_theta*cos_phi,sin_theta*sin_phi,cos_theta);
		// Rotation in the direction of the incident kaon
		const G4double px = x/r*cos_theta - y/rho*sin_theta*cos_phi + z/r*x/rho*sin_theta*sin_phi;
		const G4double py = y/r*cos_theta + x/rho*sin_theta*cos_phi + z/r*y/rho*sin_theta*sin_phi;
		const G4double pz = z/r*cos_theta - rho/r*sin_theta*sin_phi;
		
		return ThreeVector(px,py,pz);
	}
}