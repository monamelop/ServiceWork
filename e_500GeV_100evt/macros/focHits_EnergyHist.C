//====================================================================
// focHits_EnergyHist()
//
// This function reads the FOCAL hit tree ("o2sim") from a ROOT file 
// and plots a histogram of the energy loss per hit. The energy loss 
// is scaled to MeV and drawn using ROOT’s TTree::Draw function.
//
// Output:
// A histogram named "GetEnergyLoss" showing the distribution of 
// energy deposited per hit in MeV.
//
// Monalisa Melo (maria.paulino@usp.br)
//====================================================================
void focHits_EnergyHist() {
    // Open the ROOT file containing the simulated FOCAL hits
    TFile* file = TFile::Open("../o2sim_HitsFOC.root");
    if (!file || file->IsZombie()) {
        std::cerr << "Error opening file." << std::endl;
        return;
    }

    // Retrieve the TTree named "o2sim"
    TTree* tree = (TTree*)file->Get("o2sim");
    if (!tree) {
        std::cerr << "Tree 'o2sim' not found." << std::endl;
        return;
    }

    // Create a 1D histogram for the energy loss per hit
    // Note: this one is unused below, kept for reference or future use
    TH1F* h = new TH1F("GetEnergyLoss", "Energy Loss per Hit;Energy Loss [MeV];Number of Hits", 500, 0, 500000000);

    h->SetTitle("Energy Loss per Hit");                    // Título geral
    h->GetXaxis()->SetTitle("Energy Loss [MeV]");          // Título do eixo X
    h->GetYaxis()->SetTitle("Number of Hits");             // Título do eixo Y

    //tree->Draw("FOCHit.GetEnergyLoss() >> GetEnergyLoss", "", "HIST");
    tree->Draw("FOCHit.mHitValue >> GetEnergyLoss", "", "HIST");

}

