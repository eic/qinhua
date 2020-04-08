#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/Fun4AllServer.h>
#include <g4main/PHG4Reco.h>
#endif

void CreateGdmlFile(const string &filename)
{
  Fun4AllServer *se = Fun4AllServer::instance();
  PHG4Reco *g4 = (PHG4Reco *) se->getSubsysReco("PHG4RECO");
  g4->InitRun(se->topNode());
  g4->Dump_GDML(filename);
}
