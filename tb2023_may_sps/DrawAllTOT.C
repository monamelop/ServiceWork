void DrawAllTOT() {
    const int nLayers = 20;
    const int nPads = 72;
    const int nBins = 200;

    TFile* f = TFile::Open("focaldataevents_02949.root");
    TTree* t = (TTree*)f->Get("FOCALEvents");
    TLeaf* adc = t->GetLeaf("TOT");

    // Criar histogramas
    TH1F* hTOT[nLayers][nPads];
    for (int i = 0; i < nLayers; ++i) {
        for (int j = 0; j < nPads; ++j) {
            TString name = Form("hTOT_L%02d_P%02d", i, j);
            hTOT[i][j] = new TH1F(name, name, nBins, 0, 10000);
        }
    }

    // Preencher histogramas
    Long64_t nEvents = t->GetEntries();
    for (Long64_t ev = 0; ev < nEvents; ++ev) {
        t->GetEntry(ev);
        for (int i = 0; i < nLayers; ++i) {
            for (int j = 0; j < nPads; ++j) {
                int idx = 72 * i + j;
                hTOT[i][j]->Fill(adc->GetValue(idx));
            }
        }
    }

    // Encontrar o histograma com maior média
    TH1F* maxHist = nullptr;
    float maxMean = -1.0;
    for (int i = 0; i < nLayers; ++i) {
        for (int j = 0; j < nPads; ++j) {
            float mean = hTOT[i][j]->GetMean();
            if (mean > maxMean) {
                maxMean = mean;
                maxHist = hTOT[i][j];
            }
        }
    }

    // Clonar como o primeiro histograma salvo
    TH1F* hMax = (TH1F*)maxHist->Clone("hTOT_MaxMean");

    // Salvar tudo no arquivo
    TFile* out = new TFile("AllTOT_Hists.root", "RECREATE");
    hMax->Write();  // Salvo primeiro
    for (int i = 0; i < nLayers; ++i)
        for (int j = 0; j < nPads; ++j)
            hTOT[i][j]->Write();
    out->Close();

    std::cout << "✅ Histogramas salvos com sucesso!" << std::endl;
    std::cout << "⭐ Canal com maior média: " << maxHist->GetName()
              << " (média = " << maxMean << ")" << std::endl;
}

