#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllDummyInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>
#include <g4detectors/PHG4CylinderSubsystem.h>
#include <g4detectors/PHG4CylinderCellReco.h>
#include <g4trackfastsim/PHG4TrackFastSim.h>
#include <g4trackfastsim/PHG4TrackFastSimEval.h>
#include <g4main/PHG4ParticleGenerator.h>
#include <g4main/PHG4TruthSubsystem.h>
#include <g4main/PHG4Reco.h>
#include <phool/recoConsts.h>
#include <qinhua/CellNtuple.h>
#include <qinhua/HitNtuple.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4testbench.so)
R__LOAD_LIBRARY(libg4detectors.so)
R__LOAD_LIBRARY(libg4trackfastsim.so)
R__LOAD_LIBRARY(libqinhuahistos.so)

int Fun4All_G4_Qinhua(const int nEvents = 1000, const char *outfile = NULL)
{
  const bool whether_to_sim_calorimeter = false;

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
  gen->set_name("e+");
  gen->set_vtx(0, 0, 0);
  gen->set_eta_range(-0.5, +0.5);
  gen->set_mom_range(0.5, 0.5); // GeV/c
  gen->set_phi_range(0., 90. / 180. * TMath::Pi());  // 0-90 deg
  se->registerSubsystem(gen);


  PHG4Reco *g4Reco = new PHG4Reco();
  g4Reco->set_field(1.5);  // 1.5 T solenoidal field

  PHG4CylinderSubsystem *cyl =  new PHG4CylinderSubsystem("ZStrip",0);
  cyl->set_int_param("lengthviarapidity", 0);
  cyl->set_double_param("radius", 20.);
  cyl->set_string_param("material", "G4_Si");
  cyl->set_double_param("thickness", 0.1);
  cyl->SetActive();
  cyl->SuperDetector("ZStrip");
  cyl->set_double_param("length", 30.);
  g4Reco->registerSubsystem(cyl);

  cyl =  new PHG4CylinderSubsystem("PhiStrip",0);
  cyl->set_int_param("lengthviarapidity", 0);
  cyl->set_double_param("radius", 21.);
  cyl->set_string_param("material", "G4_Si");
  cyl->set_double_param("thickness", 0.1);
  cyl->SetActive();
  cyl->SuperDetector("PhiStrip");
  cyl->set_double_param("length", 30.);
  g4Reco->registerSubsystem(cyl);


  // Black hole swallows everything - prevent loopers from returning
  // to inner detectors
  cyl = new PHG4CylinderSubsystem("BlackHole", 0);
  cyl->set_double_param("radius", 80);        // 80 cm
  cyl->set_double_param("thickness", 0.1); // does not matter (but > 0)
  cyl->SetActive();
  cyl->BlackHole(); // eats everything
  g4Reco->registerSubsystem(cyl);

  PHG4TruthSubsystem *truth = new PHG4TruthSubsystem();
  g4Reco->registerSubsystem(truth);

  se->registerSubsystem(g4Reco);

  PHG4CylinderCellReco *cellrec = new PHG4CylinderCellReco("ZStripCell");
  cellrec->Detector("ZStrip");
//  cellrec->Verbosity(10);
// layer number first argument  size in z second, size in  third
  cellrec->cellsize(0,500.,1.);
  se->registerSubsystem(cellrec);



  cellrec = new PHG4CylinderCellReco("PhiStripCell");
  cellrec->Detector("PhiStrip");
//  cellrec->Verbosity(10);
  cellrec->cellsize(0,1.,500.);
  se->registerSubsystem(cellrec);


  HitNtuple *hits = new HitNtuple();
  hits->AddNode("ZStrip",0);
  hits->AddNode("PhiStrip",1);
  se->registerSubsystem(hits);

  CellNtuple *cells = new CellNtuple();
  cells->AddNode("ZStrip",0);
  cells->AddNode("PhiStrip",1);
  se->registerSubsystem(cells);

  //---------------------------

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
    std::cout << "All done" << std::endl;

    // cleanup - delete the server and exit
    delete se;
    gSystem->Exit(0);
  }
  return 0;
}

PHG4ParticleGenerator *get_gen(const char *name = "PGENERATOR")
{
  Fun4AllServer *se = Fun4AllServer::instance();
  PHG4ParticleGenerator *pgun = (PHG4ParticleGenerator *) se->getSubsysReco(name);
  return pgun;
}
