//====================================================================
// focHits_EnergyPerPosition()
//
// Author: Monalisa Melo (maria.paulino@usp.br)
//
// This function opens a ROOT file containing FOCAL simulation hits 
// and creates a 2D histogram of energy loss as a function of the 
// XY position of the hits. The color scale represents the total 
// energy deposited in each (x, y) bin, in MeV.
//
// Output:
// A 2D color map histogram ("hELossXY") showing spatial distribution 
// of energy deposition in the FOCAL detector plane.
//
//====================================================================

void focHits_EnergyPerPosition() {
    // Open the input ROOT file
    TFile* file = TFile::Open("../o2sim_HitsFOC.root");
    if (!file || file->IsZombie()) {
        std::cerr << "Error opening file 'o2sim_HitsFOC.root'." << std::endl;
        return;
    }

    // Access the TTree containing hit information
    TTree* tree = (TTree*)file->Get("o2sim");
    if (!tree) {
        std::cerr << "Tree 'o2sim' not found." << std::endl;
        return;
    }

    // Create a canvas to display the histogram
    TCanvas* c = new TCanvas("c", "Energy Loss per XY Position", 1000, 800);
    c->SetRightMargin(0.18);  // Extra space for color palette

    // Create and fill the 2D histogram:
    // X = x position, Y = y position, Weight = energy deposited (in MeV)
    tree->Draw(
        "FOCHit.mPos.fCoordinates.fY : FOCHit.mPos.fCoordinates.fX >> hELossXY(1000, -50, 50, 1000, -50, 50)",
        "FOCHit.mHitValue",  // weight: energy loss per hit
        "COLZ"               // 2D colored histogram
    );

    // Retrieve the histogram from the current directory and customize it
    TH2F* h = (TH2F*)gDirectory->Get("hELossXY");
    if (h) {
        h->SetTitle("Total Energy Loss in XY Plane;X [cm];Y [cm];#Sigma E_{loss} [MeV]");
        h->SetMaximum(1.1e6);  // Optional upper limit for color scale
        h->GetZaxis()->SetTitleOffset(2.0);
        h->Draw("COLZ");       // Redraw for style updates
        gStyle->SetPalette(kRainBow);  // Apply rainbow color palette
    } else {
        std::cerr << "Histogram 'hELossXY' was not created." << std::endl;
    }
}
