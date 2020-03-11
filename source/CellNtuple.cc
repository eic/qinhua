#include "CellNtuple.h"

#include <g4detectors/PHG4Cell.h>
#include <g4detectors/PHG4CellContainer.h>
#include <g4detectors/PHG4CylinderCellGeom.h>
#include <g4detectors/PHG4CylinderCellGeomContainer.h>

#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/SubsysReco.h>  // for SubsysReco

#include <phool/getClass.h>

#include <TFile.h>
#include <TH1.h>
#include <TNtuple.h>

#include <cassert>
#include <cmath>     // for isfinite
#include <iostream>  // for operator<<
#include <sstream>
#include <utility>  // for pair

using namespace std;

CellNtuple::CellNtuple(const std::string &name, const std::string &filename)
  : SubsysReco(name)
  , nblocks(0)
  , hm(nullptr)
  , _filename(filename)
  , ntup(nullptr)
  , outfile(nullptr)
{
}

CellNtuple::~CellNtuple()
{
  //  delete ntup;
  delete hm;
}

int CellNtuple::Init(PHCompositeNode *)
{
  hm = new Fun4AllHistoManager(Name());
  outfile = new TFile(_filename.c_str(), "RECREATE");
  ntup = new TNtuple("cellntup", "Cells", "detid:layer:phi:z:edep");
  //  ntup->SetDirectory(0);
  TH1 *h1 = new TH1F("edep1GeV", "edep 0-1GeV", 1000, 0, 1);
  eloss.push_back(h1);
  h1 = new TH1F("edep100GeV", "edep 0-100GeV", 1000, 0, 100);
  eloss.push_back(h1);
  return 0;
}

int CellNtuple::process_event(PHCompositeNode *topNode)
{
  ostringstream nodename;
  ostringstream geonodename;
  for (set<string>::const_iterator iter = _node_postfix.begin(); iter != _node_postfix.end(); ++iter)
  {
    int detid = (_detid.find(*iter))->second;
    nodename.str("");
    nodename << "G4CELL_" << *iter;
    geonodename.str("");
    geonodename << "CYLINDERCELLGEOM_" << *iter;
    PHG4CylinderCellGeomContainer *cellgeos = findNode::getClass<PHG4CylinderCellGeomContainer>(topNode, geonodename.str());
    if (!cellgeos)
    {
      cout << "no geometry node " << geonodename.str() << " for " << *iter << endl;
      continue;
    }
    PHG4CellContainer *cells = findNode::getClass<PHG4CellContainer>(topNode, nodename.str());
    if (!cells)
    {
      cout << "could not locate " << nodename.str() << endl;
    }
    if (cells)
    {
      int previouslayer = -1;
      PHG4CylinderCellGeom *cellgeom = nullptr;
      double esum = 0;
      //          double numcells = cells->size();
      //          ncells[i]->Fill(numcells);
      //	  cout << "number of cells: " << cells->size() << endl;
      PHG4CellContainer::ConstRange cell_range = cells->getCells();
      for (PHG4CellContainer::ConstIterator cell_iter = cell_range.first; cell_iter != cell_range.second; cell_iter++)

      {
        double edep = cell_iter->second->get_edep();
        if (!isfinite(edep))
        {
          cout << "invalid edep: " << edep << endl;
        }
        esum += cell_iter->second->get_edep();
        if (!cell_iter->second->has_binning(PHG4CellDefs::sizebinning))
        {
          cout << "this code assumes binning in sizes, invalid binning scheme" << endl;
          continue;
        }
        int zbin = PHG4CellDefs::SizeBinning::get_zbin(cell_iter->second->get_cellid());

        int phibin = PHG4CellDefs::SizeBinning::get_phibin(cell_iter->second->get_cellid());
        cell_iter->second->get_zbin();
        int layer = cell_iter->second->get_layer();
        // to search the map fewer times, cache the geom object until the layer changes
        if (layer != previouslayer)
        {
          cellgeom = cellgeos->GetLayerCellGeom(layer);
          previouslayer = layer;
        }
        assert(cellgeom != nullptr);
        double phi = cellgeom->get_phicenter(phibin);
        double z = cellgeom->get_zcenter(zbin);
        ntup->Fill(detid,
                   cell_iter->second->get_layer(),
                   phi,
                   z,
                   cell_iter->second->get_edep());
        // looping over G4Hits which contributed to thie cell:
        // 	PHG4Cell::EdepConstRange hitpair = cell_iter->second->get_g4hits();
        // for (	PHG4Cell::EdepConstIterator hititer = hitpair.first;  hititer != hitpair.second; ++hititer)
        // {
        //     cout <<"\t PHG4hit "<<hititer->first<<" -> "<<hititer->second<<" GeV"<<endl;
        //   }
      }
      for (vector<TH1 *>::const_iterator eiter = eloss.begin(); eiter != eloss.end(); ++eiter)
      {
        (*eiter)->Fill(esum);
      }
    }
  }
  return 0;
}

int CellNtuple::End(PHCompositeNode *topNode)
{
  outfile->cd();
  ntup->Write();
  outfile->Write();
  outfile->Close();
  delete outfile;
  hm->dumpHistos(_filename, "UPDATE");
  return 0;
}

void CellNtuple::AddNode(const std::string &name, const int detid)
{
  _node_postfix.insert(name);
  _detid[name] = detid;
  return;
}
