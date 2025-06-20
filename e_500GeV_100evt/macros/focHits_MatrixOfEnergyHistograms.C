void focHits_MatrixOfEnergyHistograms() {
    // --- Geometry configuration ---
    const int nPadsX = 9 * 10;     // 90 pads along X
    const int nPadsY = 8 * 11;     // 88 pads along Y
    const int nPads = nPadsX * nPadsY;
    const int nLayers = 18;

    const float padSizeX = 1.0;  // cm
    const float padSizeY = 1.0;  // cm
    const float xMin = -nPadsX * padSizeX / 2.0;
    const float yMin = -nPadsY * padSizeY / 2.0;
    const float zMin = 700.0;   // mm
    const float dz = 6.6;       // mm

    // --- Mask function: excludes the central gap on layer 6 ---
    auto isValidPad = [](int iX, int iY) {
        return !(iY >= 44 && iY < 48 && iX >= 40 && iX < 50);
    };

    // --- Create histogram matrix ---
    TH1F* hist[nPads][nLayers] = {{nullptr}};
    for (int i = 0; i < nPads; ++i) {
        for (int j = 0; j < nLayers; ++j) {
            TString name = Form("h_pad%d_layer%d", i, j);
            hist[i][j] = new TH1F(name, name, 100, 0, 5e6); // energy in MeV
        }
    }

    // --- Open input file ---
    TFile* file = TFile::Open("../o2sim_HitsFOC.root");
    if (!file || file->IsZombie()) {
        std::cerr << "❌ Failed to open 'o2sim_HitsFOC.root'" << std::endl;
        return;
    }

    TTree* tree = (TTree*)file->Get("o2sim");
    if (!tree) {
        std::cerr << "❌ Tree 'o2sim' not found." << std::endl;
        return;
    }

    // --- Setup TTreeReader ---
    TTreeReader reader(tree);
    TTreeReaderArray<Float_t> x(reader, "FOCHit.mPos.fCoordinates.fX");
    TTreeReaderArray<Float_t> y(reader, "FOCHit.mPos.fCoordinates.fY");
    TTreeReaderArray<Float_t> z(reader, "FOCHit.mPos.fCoordinates.fZ");
    TTreeReaderArray<Float_t> e(reader, "FOCHit.mHitValue");

    // --- Loop over hits and fill corresponding histograms ---
    int nEvents = 0;
    while (reader.Next()) {
        int nHits = x.GetSize();
        for (int i = 0; i < nHits; ++i) {
            int iX = int((x[i] - xMin) / padSizeX);
            int iY = int((y[i] - yMin) / padSizeY);
            int iPad = iY * nPadsX + iX;
            int iLayer = int((z[i] - zMin) / dz);

            if (iX >= 0 && iX < nPadsX &&
                iY >= 0 && iY < nPadsY &&
                iPad < nPads &&
                iLayer >= 0 && iLayer < nLayers &&
                isValidPad(iX, iY)) {
                hist[iPad][iLayer]->Fill(e[i]);
            }
        }
        ++nEvents;
    }

    std::cout << "✅ Processed " << nEvents << " events." << std::endl;

    // --- Save all histograms to a ROOT file ---
    TFile* outFile = new TFile("PadLayerHistos.root", "RECREATE");
    for (int i = 0; i < nPads; ++i) {
        for (int j = 0; j < nLayers; ++j) {
            if (hist[i][j]) hist[i][j]->Write();
        }
    }
    outFile->Close();

    std::cout << "✅ Saved all pad-layer histograms to PadLayerHistos.root" << std::endl;
}