#include "HitCombiner.h"

#include <g4main/PHG4Hitv1.h>
#include <g4main/PHG4HitContainer.h>


#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>    // for PHIODataNode
#include <phool/getClass.h>

#include <TSystem.h>

using namespace std;

HitCombiner::HitCombiner(const std::string &name):
 SubsysReco(name)
{}

HitCombiner::~HitCombiner()
{
}


int HitCombiner::Init(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int HitCombiner::InitRun(PHCompositeNode *topNode)
{
// just checking if nodenames exist
  if (m_OutNode.empty() || m_InNode1.empty() || m_InNode2.empty())
  {
    cout << "No or not all nodes given in macro" << endl;
    gSystem->Exit(1);
  }
  PHNodeIterator iter(topNode);
  PHCompositeNode *dstNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "DST"));

      PHG4HitContainer *out_hits = findNode::getClass<PHG4HitContainer>(topNode,m_OutNode);
      if (!out_hits)
      {
        out_hits = new PHG4HitContainer(m_OutNode);
       dstNode->addNode(new PHIODataNode<PHObject>(out_hits, m_OutNode, "PHObject"));
      }
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int HitCombiner::process_event(PHCompositeNode *topNode)
{
      PHG4HitContainer *out_hits = findNode::getClass<PHG4HitContainer>(topNode,m_OutNode);
      PHG4HitContainer *in_hits1 = findNode::getClass<PHG4HitContainer>(topNode,m_InNode1);
      PHG4HitContainer *in_hits2 = findNode::getClass<PHG4HitContainer>(topNode,m_InNode2);
      if (! in_hits1 || !in_hits2)
      {
	cout << "could not find one or both input nodes: " << m_InNode1
	     << " and/or " << m_InNode2 << endl;
	gSystem->Exit(1);
      }
  PHG4HitContainer::ConstRange hit_range1 = in_hits1->getHits();
 for (PHG4HitContainer::ConstIterator hit_iter1 = hit_range1.first; hit_iter1 != hit_range1.second; ++hit_iter1)
 {
   PHG4Hit *outhit = new PHG4Hitv1(hit_iter1->second);
   out_hits->AddHit(0,outhit);
// print input and output hit to see if they are identical
   cout << "Input Hit:" << endl;
   hit_iter1->second->identify();
   cout << "Output Hit:" << endl;
   outhit->identify();
 }
  return Fun4AllReturnCodes::EVENT_OK;
}

int HitCombiner::ResetEvent(PHCompositeNode *topNode)
{
//  cout << "Resetting internal structures after event processing" << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

int HitCombiner::EndRun(const int runnumber)
{
//  cout << "Ending Run for run " << runnumber << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

int HitCombiner::End(PHCompositeNode *topNode)
{
//  cout << "This is the End..." << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

int HitCombiner::Reset(PHCompositeNode *topNode)
{
//  cout << "being Reset()" << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

void HitCombiner::Print(const std::string &what) const
{
  cout << "Printing info for " << what << endl;
}
