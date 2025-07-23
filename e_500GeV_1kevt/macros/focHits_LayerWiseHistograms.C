//====================================================================
// Author: Monalisa Melo (maria.paulino@usp.br)
//
// This macro reads FOCAL hit data from simulation and builds a 
// detailed histogram structure for each detector layer.
//
// For each layer (in z), it creates:
// - A 2D histogram of energy loss in the XY plane
// - A set of 1D histograms (one per pad) with energy loss distribution,
//   where each histogram is titled with the XY position of the pad
//
// Layers are determined automatically based on Z hit position,
// assuming uniform thickness of 0.85 cm and using the first hit
// as Z reference.
//
// Output: A ROOT file (LayerWiseHistograms.root) with one directory 
// per layer.
//
//====================================================================

void focHits_LayerWiseHistograms()
{
    // --- Detector geometry configuration ---
    const int nPadsX = 90;         // 10 modules × 9 pads
    const int nPadsY = 88;         // 11 modules × 8 pads
    const int nPads = nPadsX * nPadsY;
    const int nLayers = 20;
    const float padSize = 1.0;     // Pad size in cm
    const float layerThickness = 0.85; // in cm

    const float xMin = -nPadsX * padSize / 2.0;
    const float yMin = -nPadsY * padSize / 2.0;

    // --- Open input ROOT file ---
    TFile* file = TFile::Open("../o2sim_HitsFOC.root");
    if (!file || file->IsZombie()) {
        std::cerr << "Failed to open 'o2sim_HitsFOC.root'" << std::endl;
        return;
    }

    TTree* tree = (TTree*)file->Get("o2sim");
    if (!tree) {
        std::cerr << "Tree 'o2sim' not found." << std::endl;
        return;
    }

    // --- Setup TTreeReader to access hit data ---
    TTreeReader reader(tree);
    TTreeReaderArray<Float_t> x(reader, "FOCHit.mPos.fCoordinates.fX");
    TTreeReaderArray<Float_t> y(reader, "FOCHit.mPos.fCoordinates.fY");
    TTreeReaderArray<Float_t> z(reader, "FOCHit.mPos.fCoordinates.fZ");
    TTreeReaderArray<Float_t> e(reader, "FOCHit.mHitValue");

    // --- Define minimum z position for reference (z0) ---
    float z0 = 700.85; // cm
   
    std::cout << "Minimum Z: " << z0 << " cm" << std::endl;

    // Reset reader to loop again
    reader.Restart();

    // --- Create histograms for each layer ---
    TH2F* hXY[nLayers];
    TH1F* hPad[nLayers][nPads];

    for (int iL = 0; iL < nLayers; ++iL) {
        TString hname = Form("hXY_Layer%d", iL);
        TString htitle = Form("XY Energy Loss for Layer %d (Z in [%.2f, %.2f] cm);X [cm];Y [cm];#Sigma E_{loss} [MeV]",
                              iL, z0 + iL * layerThickness, z0 + (iL + 1) * layerThickness);
        hXY[iL] = new TH2F(hname, htitle, nPadsX, xMin, -xMin, nPadsY, yMin, -yMin);

        for (int iY = 0; iY < nPadsY; ++iY) {
            for (int iX = 0; iX < nPadsX; ++iX) {
                int iP = iY * nPadsX + iX;
                float xCenter = xMin + (iX + 0.5f) * padSize;
                float yCenter = yMin + (iY + 0.5f) * padSize;

                TString pname = Form("hPad_L%d_P%d", iL, iP);
                TString ptitle = Form("Layer %d Pad %d @ (X=%.1f cm, Y=%.1f cm);Energy Loss [MeV];Counts",
                                      iL, iP, xCenter, yCenter);
                hPad[iL][iP] = new TH1F(pname, ptitle, 100, 0, 5e6); // Energy loss histogram
            }
        }
    }

    // --- Fill histograms from tree ---
    int nEvents = 0;
    while (reader.Next()) {
        int nHits = x.GetSize();
        for (int i = 0; i < nHits; ++i) {
            int iX = int((x[i] - xMin) / padSize);
            int iY = int((y[i] - yMin) / padSize);
            int iPad = iY * nPadsX + iX;
            int iLayer = int((z[i] - z0) / layerThickness);

            if (iX >= 0 && iX < nPadsX &&
                iY >= 0 && iY < nPadsY &&
                iPad >= 0 && iPad < nPads &&
                iLayer >= 0 && iLayer < nLayers) {

                hXY[iLayer]->Fill(x[i], y[i], e[i]);
                hPad[iLayer][iPad]->Fill(e[i]);
            }
        }
        ++nEvents;
    }

    std::cout << "Processed " << nEvents << " events." << std::endl;

    // --- Save only non-empty histograms ---
    TFile* outFile = new TFile("LayerWiseHistograms.root", "RECREATE");

    for (int iL = 0; iL < nLayers; ++iL) {
        TString layerDir = Form("Layer%d", iL);
        outFile->mkdir(layerDir);
        outFile->cd(layerDir);

        if (hXY[iL]->GetEntries() > 0) {
            hXY[iL]->Write();
        }

        int saved = 0;
        for (int iP = 0; iP < nPads; ++iP) {
            if (hPad[iL][iP]->GetEntries() > 0) {
                hPad[iL][iP]->Write();
                ++saved;
            } else {
                delete hPad[iL][iP];
            }
        }

        std::cout << "Layer " << iL << " saved with " << saved << " pads." << std::endl;
    }

    outFile->Close();
    std::cout << "Output written to 'LayerWiseHistograms.root'" << std::endl;
}
