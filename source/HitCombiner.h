// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef HITCOMBINER_H
#define HITCOMBINER_H

#include <fun4all/SubsysReco.h>

#include <string>

class PHCompositeNode;

class HitCombiner : public SubsysReco
{
 public:
  HitCombiner(const std::string &name = "HitCombiner");

  virtual ~HitCombiner();

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  int Init(PHCompositeNode *topNode) override;

  /** Called for first event when run number is known.
      Typically this is where you may want to fetch data from
      database, because you know the run number.
   */
  int InitRun(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
   */
  int process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of each run.
  int EndRun(const int runnumber) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  /// Reset
  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;

  void SetInputNodes(const std::string &node1, const std::string &node2)
  {
    m_InNode1 = "G4HIT_" + node1;
    m_InNode2 = "G4HIT_" + node2;
  }
  void SetOutputNode(const std::string &node) { m_OutNode = "G4HIT_" + node; }

 private:
  std::string m_InNode1;
  std::string m_InNode2;
  std::string m_OutNode;
};

#endif  // HITCOMBINER_H
