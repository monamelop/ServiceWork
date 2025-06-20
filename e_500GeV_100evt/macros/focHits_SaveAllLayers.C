#include <TFile.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderArray.h>
#include <TH1F.h>
#include <TString.h>
#include <iostream>

// Função auxiliar que retorna o título do histograma com a posição física do pad
TString getPadTitle(int padIndex, int nPadsX = 90, int nPadsY = 88, float padSizeX = 1.0, float padSizeY = 1.0) {
    float xMin = -nPadsX * padSizeX / 2.0;
    float yMin = -nPadsY * padSizeY / 2.0;

    int iY = padIndex / nPadsX;
    int iX = padIndex % nPadsX;

    float x = xMin + iX * padSizeX;
    float y = yMin + iY * padSizeY;

    return Form("Pad %d (x = %.1f cm, y = %.1f cm)", padIndex, x, y);
}

void focHits_SaveAllLayers() {
    // --- Geometria do detector: 10×11 módulos de 9×8 pads cada ---
    const int nPadsX = 90; // 10 módulos × 9 pads
    const int nPadsY = 88; // 11 módulos × 8 pads
    const int nPads = nPadsX * nPadsY;

    const float padSizeX = 1.0; // cm
    const float padSizeY = 1.0; // cm
    const float xMin = -nPadsX * padSizeX / 2.0;
    const float yMin = -nPadsY * padSizeY / 2.0;

    // --- Range de Z da camada 1 (confirmado em cm) ---
    const float zMinLayer1 = 700.0;  // cm
    const float zMaxLayer1 = 720.0;  // cm

    // --- Cria histogramas 1D para cada pad para toadas as layers ---
    TH1F* hist[nPads] = {nullptr};
    for (int i = 0; i < nPads; ++i) {
        TString name = Form("h_pad%d_AllLayers", i);
        TString title = getPadTitle(i);  // título com posição física
        hist[i] = new TH1F(name, title, 1000, 0, 5e6); // energia em MeV
        hist[i]->GetXaxis()->SetTitle("Energy loss [MeV]");
        hist[i]->GetYaxis()->SetTitle("Number of hits");
    }

    // --- Abre o arquivo de entrada ---
    TFile* file = TFile::Open("../o2sim_HitsFOC.root");
    if (!file || file->IsZombie()) {
        std::cerr << "Could not open 'o2sim_HitsFOC.root'" << std::endl;
        return;
    }

    TTree* tree = (TTree*)file->Get("o2sim");
    if (!tree) {
        std::cerr << "Tree 'o2sim' not found." << std::endl;
        return;
    }

    // --- Leitura dos dados ---
    TTreeReader reader(tree);
    TTreeReaderArray<Float_t> x(reader, "FOCHit.mPos.fCoordinates.fX");
    TTreeReaderArray<Float_t> y(reader, "FOCHit.mPos.fCoordinates.fY");
    TTreeReaderArray<Float_t> z(reader, "FOCHit.mPos.fCoordinates.fZ");
    TTreeReaderArray<Float_t> e(reader, "FOCHit.mHitValue");

    int nEvents = 0, nHitsSaved = 0;
    while (reader.Next()) {
        int nHits = x.GetSize();
        for (int i = 0; i < nHits; ++i) {
            if (z[i] < zMinLayer1 || z[i] >= zMaxLayer1) continue;

            int iX = int((x[i] - xMin) / padSizeX);
            int iY = int((y[i] - yMin) / padSizeY);
            int iPad = iY * nPadsX + iX;

            if (iX >= 0 && iX < nPadsX &&
                iY >= 0 && iY < nPadsY &&
                iPad >= 0 && iPad < nPads) {
                hist[iPad]->Fill(e[i]);
                ++nHitsSaved;
            }
        }
        ++nEvents;
    }

    std::cout << "Processed " << nEvents << " events and " << nHitsSaved << " hits in all layers." << std::endl;

    // --- Salva apenas histogramas preenchidos ---
    TFile* outFile = new TFile("PadLayerHistos_L1_90x88.root", "RECREATE");
    for (int i = 0; i < nPads; ++i) {
        if (hist[i] && hist[i]->GetEntries() > 0) {
            hist[i]->Write();
        }
    }
    outFile->Close();

    std::cout << "Saved to PadLayerHistos_L1_90x88.root" << std::endl;
}
