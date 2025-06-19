void plotFOCHits() {
  TFile* myFile = TFile::Open("../o2sim_HitsFOC.root");
  if (!myFile || myFile->IsZombie()) {
    std::cerr << "Failed to open file!" << std::endl;
    return;
  }

  TTree* tree = (TTree*)myFile->Get("o2sim");
  if (!tree) {
    std::cerr << "TTree 'o2sim' not found!" << std::endl;
    return;
  }

  // Obter os leaves diretamente
  TLeaf* lTrackID  = tree->GetLeaf("FOCHit.mTrackID");
  TLeaf* lX        = tree->GetLeaf("FOCHit.mPos.fCoordinates.fX");
  TLeaf* lY        = tree->GetLeaf("FOCHit.mPos.fCoordinates.fY");
  TLeaf* lZ        = tree->GetLeaf("FOCHit.mPos.fCoordinates.fZ");
  TLeaf* lHitValue = tree->GetLeaf("FOCHit.mHitValue");

  if (!lTrackID || !lX || !lY || !lZ || !lHitValue) {
    std::cerr << "One or more leaves not found!" << std::endl;
    return;
  }

  Long64_t entries = tree->GetEntries();
  std::cout << "Total entries: " << entries << std::endl;

  for (Long64_t i = 0; i < entries; ++i) {
    tree->GetEntry(i);
    Int_t nHits = lTrackID->GetLen();

    std::cout << "\nEvent " << i << " has " << nHits << " hits:" << std::endl;

    for (Int_t j = 0; j < nHits; ++j) {
      Int_t   trackID = lTrackID->GetValue(j);
      Float_t x       = lX->GetValue(j);
      Float_t y       = lY->GetValue(j);
      Float_t z       = lZ->GetValue(j);
      Float_t val     = lHitValue->GetValue(j);

      std::cout << "  Hit " << j
                << " | TrackID: " << trackID
                << " | x: " << x
                << " | y: " << y
                << " | z: " << z
                << " | value: " << val << std::endl;
    }
  }
}
