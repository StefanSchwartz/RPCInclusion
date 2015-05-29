#include <iostream>
#include<TChain.h>

using namespace std;


void merge() {

  // the name of the chain has to be the same as the one in the tree.
  // 
  // Use this for shortcut in writing out the chain lines
  //cmsLs /store/user/dcurry/rpc/2012D_reco_5_26/ | grep root | awk '{print "chain->AddFile(\"root://eoscms//eos/cms/"$5"\");"}' >> text



  TChain* chain = new TChain("CSCtree");
  
  // here you append all your files
  //chain -> AddFile("rootFiles/DYJetsToLL_11_1_1jf.root");


  


  // merge them in a single file
  std::cout << "Merging into minbias_2012D_merged.root (it may take a while)\n" << std::endl;
  chain -> Merge("root://eoscms//eos/cms/store/user/dcurry/rpc/test_merge.root");

}

