//====================================================================
// focHits_MatrixOfEnergyHistograms()
//
// Author: Monalisa Melo (maria.paulino@usp.br)
//
// Description:
// This function processes the FOCAL hit simulation file and creates 
// a large matrix of 1D histograms, one for each (pad, layer) combination.
//
// For each pad (in a 90×88 grid) and for each of 20 layers along Z,
// a histogram is created to store the distribution of energy deposited 
// in that location.
//
// Due to the high number of pads (7920) and layers (20), this script 
// creates 158,400 histograms and may be memory-intensive or slow 
// to run.
//
// The histograms are saved to a ROOT file: "PadLayerHistos.root".
//====================================================================

void focHits_MatrixOfEnergyHistograms() {
    // --- Geometry configuration ---
    const int nPadsX = 9 * 10;     // 90 pads along X (10 modules of 9 pads)
    const int nPadsY = 8 * 11;     // 88 pads along Y (11 modules of 8 pads)
    const int nPads = nPadsX * nPadsY;  // Total number of pads
    const int nLayers = 20;             // Number of detector layers

    const float padSizeX = 1.0;  // Pad size in X [cm]
    const float padSizeY = 1.0;  // Pad size in Y [cm]

    // Minimum coordinate values (centered geometry)
    const float xMin = -nPadsX * padSizeX / 2.0;
    const float yMin = -nPadsY * padSizeY / 2.0;

    // Z configuration (in mm)
    const float zMin = 700.0;   // Z start position [mm]
    const float dz = 8.5;       // Distance between layers [mm]

    // --- Create a matrix of histograms: one for each (pad, layer) ---
    TH1F* hist[nPads][nLayers] = {{nullptr}};
    for (int i = 0; i < nPads; ++i) {
        for (int j = 0; j < nLayers; ++j) {
            TString name = Form("h_pad%d_layer%d", i, j);
            hist[i][j] = new TH1F(name, name, 100, 0, 5e6); // Energy loss in MeV
        }
    }

    // --- Open input ROOT file containing FOCAL hits ---
    TFile* file = TFile::Open("../o2sim_HitsFOC.root");
    if (!file || file->IsZombie()) {
        std::cerr << "Failed to open 'o2sim_HitsFOC.root'" << std::endl;
        return;
    }

    // --- Get the TTree with hit data ---
    TTree* tree = (TTree*)file->Get("o2sim");
    if (!tree) {
        std::cerr << "Tree 'o2sim' not found." << std::endl;
        return;
    }

    // --- Setup TTreeReader to loop over hit arrays ---
    TTreeReader reader(tree);
    TTreeReaderArray<Float_t> x(reader, "FOCHit.mPos.fCoordinates.fX");
    TTreeReaderArray<Float_t> y(reader, "FOCHit.mPos.fCoordinates.fY");
    TTreeReaderArray<Float_t> z(reader, "FOCHit.mPos.fCoordinates.fZ");
    TTreeReaderArray<Float_t> e(reader, "FOCHit.mHitValue");

    // --- Loop over events and fill the corresponding histograms ---
    int nEvents = 0;
    while (reader.Next()) {
        int nHits = x.GetSize();
        for (int i = 0; i < nHits; ++i) {
            // Compute pad indices from position
            int iX = int((x[i] - xMin) / padSizeX);
            int iY = int((y[i] - yMin) / padSizeY);
            int iPad = iY * nPadsX + iX;

            // Compute layer index from Z position
            int iLayer = int((z[i] - zMin) / dz);

            // Check bounds and fill histogram
            if (iX >= 0 && iX < nPadsX &&
                iY >= 0 && iY < nPadsY &&
                iPad < nPads &&
                iLayer >= 0 && iLayer < nLayers) {
                hist[iPad][iLayer]->Fill(e[i]);
            }
        }
        ++nEvents;
    }

    std::cout << "Processed " << nEvents << " events." << std::endl;

    // --- Save all histograms to an output ROOT file ---
    TFile* outFile = new TFile("PadLayerHistos.root", "RECREATE");
    for (int i = 0; i < nPads; ++i) {
        for (int j = 0; j < nLayers; ++j) {
            if (hist[i][j]) hist[i][j]->Write();
        }
    }
    outFile->Close();

    std::cout << "Saved all pad-layer histograms to PadLayerHistos.root" << std::endl;
}
