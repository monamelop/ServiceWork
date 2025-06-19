void focHits_EnergyPerPosition() {
    TFile* file = TFile::Open("../o2sim_HitsFOC.root");
    if (!file || file->IsZombie()) {
        std::cerr << "Erro ao abrir o arquivo 'o2sim_HitsFOC.root'." << std::endl;
        return;
    }

    // Acessa a árvore de hits
    TTree* tree = (TTree*)file->Get("o2sim");
    if (!tree) {
        std::cerr << "Arvore 'o2sim' não encontrada." << std::endl;
        return;
    }

    // Cria o canvas
    TCanvas* c = new TCanvas("c", "Energy Loss per XY Position", 1000, 800);
    c->SetRightMargin(0.18); 

    // Cria e preenche o histograma: X = x, Y = y, peso = energia depositada (em MeV)
    tree->Draw(
        "FOCHit.mPos.fCoordinates.fY : FOCHit.mPos.fCoordinates.fX >> hELossXY(1000, -50, 50, 1000, -50, 50)",
        "FOCHit.mHitValue",  // peso = energia depositada
        "COLZ"
    );

    // Ajusta o histograma
    TH2F* h = (TH2F*)gDirectory->Get("hELossXY");
    if (h) {
        h->SetTitle("Total Energy Loss in XY Plane;X [cm];Y [cm];#Sigma E_{loss} [MeV]");
        h->SetMaximum(1.1e6);
        h->GetZaxis()->SetTitleOffset(2.0); 
        h->Draw("COLZ");
        gStyle->SetPalette(kRainBow);
    } else {
        std::cerr << "Histograma 'hELossXY' não foi criado." << std::endl;
    }
}