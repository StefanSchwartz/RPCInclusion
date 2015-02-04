///////////////////////
// pt_modes.h
//
// Author: David Curry
//
// Imported ny twoHit_RPC_pt_all.C
//
// Creates a method for each track mode(2 hit csctf tracks) and used rpc to find pt  
//
//////////////////////

//#include "EffiEvent.h"



float EffiEvent::pt_13(TTree CSCtree, int iCSCTrk, int printLevel) {

  EffiEvent evt;
    
  if (printLevel>0) cout << "-----> Starting pt_13 module\n";
    
  for (int iCsc=0; iCsc < evt.NumLctsTrk[iCSCTrk]; iCsc++) {
    
    if (printLevel>0) cout << "Looping over CSC Lct # " << iCsc << " in station # " << evt.trLctStation[iCSCTrk][iCsc] << endl;
    
  }

  return 0;

} // end pt_13

