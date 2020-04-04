#pragma once

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllDummyInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>
#include <g4detectors/PHG4CylinderSubsystem.h>
#include <g4trackfastsim/PHG4TrackFastSim.h>
#include <g4trackfastsim/PHG4TrackFastSimEval.h>
#include <g4main/PHG4ParticleGenerator.h>
#include <g4main/PHG4TruthSubsystem.h>
#include <g4main/PHG4Reco.h>
#include <phool/recoConsts.h>
#include <qinhua/HitCombiner.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4testbench.so)
R__LOAD_LIBRARY(libg4detectors.so)
R__LOAD_LIBRARY(libqinhuahistos.so)

int Fun4All_G4_2Cylinders(const int nEvents = 1000, const char *outfile = NULL)
{
  ///////////////////////////////////////////
  // Make the Server
  //////////////////////////////////////////
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);

  recoConsts *rc = recoConsts::instance();
  //  rc->set_IntFlag("RANDOMSEED", 12345); // if you want to use a fixed seed
  // PHG4ParticleGenerator generates particle
  // distributions in eta/phi/mom range
  PHG4ParticleGenerator *gen = new PHG4ParticleGenerator("PGENERATOR");
//  gen->set_name("e+");
// use geantinos for development/debugging (straight tracks, no decays or interactions)
  gen->set_name("geantino");
  gen->set_vtx(0, 0, 0);
  gen->set_eta_range(-0.5, +0.5);
  gen->set_mom_range(0.5, 0.5); // GeV/c
  gen->set_phi_range(0., 90. / 180. * TMath::Pi());  // 0-90 deg
  se->registerSubsystem(gen);


  PHG4Reco *g4Reco = new PHG4Reco();
  g4Reco->set_field(1.5);  // 1.5 T solenoidal field

// by default the node name is <Name>_<layer>
// so this will be ZSTRIP_0
  PHG4CylinderSubsystem *cyl = new PHG4CylinderSubsystem("ZSTRIP",0);
  cyl->set_double_param("radius", 20.); // 20 cm
  cyl->set_string_param("material", "G4_MYLAR");
  cyl->set_double_param("thickness", 0.2);
  cyl->SetActive();
  cyl->set_color(1.,0.,0.1,0.6);
  cyl->set_double_param("length", 100.); // 1 m long
  g4Reco->registerSubsystem(cyl);

// by default the node name is <Name>_<layer>
// so this will be PHISTRIP_1
  cyl = new PHG4CylinderSubsystem("PHISTRIP",1);
  cyl->set_double_param("radius", 21.); // 21 cm
  cyl->set_string_param("material", "G4_MYLAR");
  cyl->set_double_param("thickness", 0.2);
  cyl->SetActive();
  cyl->set_color(0.,1.,0.5,0.2);
  cyl->set_double_param("length", 100.); // 1 m long
  g4Reco->registerSubsystem(cyl);

  // Black hole swallows everything - prevent loopers from returning
  // to inner detectors
  cyl = new PHG4CylinderSubsystem("BlackHole", 0);
  cyl->set_double_param("radius", 40);        // 40 cm
  cyl->set_double_param("thickness", 0.1); // does not matter (but > 0)
//  cyl->SetActive();
  cyl->set_double_param("length", 110.); // 1.1 m long
  cyl->BlackHole(); // eats everything
  g4Reco->registerSubsystem(cyl);

  PHG4TruthSubsystem *truth = new PHG4TruthSubsystem();
  g4Reco->registerSubsystem(truth);

  se->registerSubsystem(g4Reco);

  HitCombiner *hitcmb = new HitCombiner();
  hitcmb->SetInputNodes("ZSTRIP_0","PHISTRIP_1");
  hitcmb->SetOutputNode("ZPHISTRIPS");
  se->registerSubsystem(hitcmb);
  //---------------------------
  // output DST file for further offlien analysis
  //---------------------------
  if (outfile)
  {
    Fun4AllOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", outfile);
    se->registerOutputManager(out);
  }
  Fun4AllInputManager *in = new Fun4AllDummyInputManager("JADE");
  se->registerInputManager(in);

  if (nEvents > 0)
  {
    se->run(nEvents);
    // finish job - close and save output files
    se->End();
    // std::cout << "All done" << std::endl;

    // // cleanup - delete the server and exit
    delete se;
    gSystem->Exit(0);
  }
  return 0;
}
