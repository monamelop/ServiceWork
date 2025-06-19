void focHits_EnergyHist() {
    TFile* file = TFile::Open("../o2sim_HitsFOC.root");
    if (!file || file->IsZombie()) {
        std::cerr << "Error opening file." << std::endl;
        return;
    }

    TTree* tree = (TTree*)file->Get("o2sim");
    if (!tree) {
        std::cerr << "Tree 'o2sim' not found." << std::endl;
        return;
    }

    // Create custom histogram with defined binning
    
    TH1F* h = new TH1F("GetEnergyLoss", "Energy Loss per Hit;Energy Loss [MeV];Number of Hits", 1000, 0, 5e6);

    // Fill it using TTree::Draw
    h->GetXaxis()->SetNoExponent(false);
    h->GetXaxis()->SetMoreLogLabels(true);
    tree->Draw("FOCHit.GetEnergyLoss()/1000 >> GetEnergyLoss(200, 0, 500)", "", "HIST");

}
