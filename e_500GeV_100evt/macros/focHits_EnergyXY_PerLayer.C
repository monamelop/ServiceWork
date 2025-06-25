//====================================================================
// 
// Author: Monalisa Melo (maria.paulino@usp.br)
//
// This macro processes FOCAL hit data from a ROOT simulation file,
// and creates one 2D histogram (TH2F) per detector layer. Each histogram 
// shows the energy deposited in the XY plane for a specific layer.
//
// The z range is divided into 20 layers (18 from pad detectors + 2 from pixels),
// each assumed to be 0.85 cm thick. The z position of each hit determines 
// the corresponding layer.
//
// Output: A ROOT file "XY_Energy_Layers.root" with 20 2D histograms,
// each showing the total energy deposited (in keV) per (X,Y) coordinate.
//====================================================================
void focHits_EnergyXY_PerLayer() {
    // --- Configuration ---
    const int nLayers = 20;                   // Total number of layers (18 pads + 2 pixels)
    const float layerThickness = 0.85;        // Thickness of each layer in cm
    const float zStart = 700.85;               // Approximate Z position of the first layer [cm]

    // XY limits of the detector (based on number of pads)
    const float xMin = -45.0, xMax = 45.0;    // X range in cm (90 pads × 1 cm)
    const float yMin = -44.0, yMax = 44.0;    // Y range in cm (88 pads × 1 cm)

    // --- Create 2D histograms for each layer ---
    TH2F* hXY[nLayers];
    for (int i = 0; i < nLayers; ++i) {
        // Calculate the Z range for this layer
        float zMin = zStart + i * layerThickness;
        float zMax = zMin + layerThickness;

        // Create histogram with appropriate title and axis labels
        TString name = Form("hXY_Layer%d", i);
        TString title = Form("Energy Loss in XY Plane - Layer %d (z: %.2f-%.2f cm);X [cm];Y [cm];#Sigma E_{loss} [MeV]", i, zMin, zMax);
        hXY[i] = new TH2F(name, title, 90, xMin, xMax, 88, yMin, yMax);  // 90×88 bins (1 cm per pad)
    }

    // --- Open input ROOT file with FOCAL hits ---
    TFile* file = TFile::Open("../o2sim_HitsFOC.root");
    if (!file || file->IsZombie()) {
        std::cerr << "Failed to open file" << std::endl;
        return;
    }

    // Get the TTree containing the hit data
    TTree* tree = (TTree*)file->Get("o2sim");
    if (!tree) {
        std::cerr << "Tree not found" << std::endl;
        return;
    }

    // --- Set up TTreeReader to access arrays of hit properties ---
    TTreeReader reader(tree);
    TTreeReaderArray<Float_t> x(reader, "FOCHit.mPos.fCoordinates.fX");  // X position [cm]
    TTreeReaderArray<Float_t> y(reader, "FOCHit.mPos.fCoordinates.fY");  // Y position [cm]
    TTreeReaderArray<Float_t> z(reader, "FOCHit.mPos.fCoordinates.fZ");  // Z position [cm]
    TTreeReaderArray<Float_t> e(reader, "FOCHit.mHitValue");             // Energy loss [keV]

    // --- Loop over all events and hits ---
    while (reader.Next()) {
        int nHits = x.GetSize();
        for (int i = 0; i < nHits; ++i) {
            float zVal = z[i];  // Z position of the hit

            // Determine the layer index from Z
            int layer = int((zVal - zStart) / layerThickness);

            // Check bounds and fill the corresponding histogram
            if (layer >= 0 && layer < nLayers) {
                hXY[layer]->Fill(x[i], y[i], e[i]);  // Accumulate energy loss
            }
        }
    }

    // --- Save all histograms to a ROOT file ---
    TFile* outFile = new TFile("XY_Energy_Layers.root", "RECREATE");
    for (int i = 0; i < nLayers; ++i) {
        hXY[i]->Write();
    }
    outFile->Close();

    std::cout << "Histograms saved to XY_Energy_Layers.root" << std::endl;
}
