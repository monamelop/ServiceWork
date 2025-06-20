void drawFocalLayer()
{
    const int nModulesX = 10;
    const int nModulesY = 11;
    const int padsPerModX = 9;
    const int padsPerModY = 8;
    const int nPadsX = nModulesX * padsPerModX;
    const int nPadsY = nModulesY * padsPerModY;

    const float padSizeX = 1.0;     // cm
    const float padSizeY = 1.0;     // cm
    const float padThickness = 1.0; // cm

    const float xMin = -nPadsX * padSizeX / 2.0;
    const float yMin = -nPadsY * padSizeY / 2.0;

    // Define as cores RGB da paleta Coolors AZUL e VINHO
    //std::vector<int> colors = {
    //    TColor::GetColor(3, 50, 112),
    //    TColor::GetColor(19, 104, 170),
    //    TColor::GetColor(64, 145, 201),
    //    TColor::GetColor(157, 206, 226),
    //    TColor::GetColor(254, 223, 212),
    //    TColor::GetColor(242, 148, 121),
    //    TColor::GetColor(242, 106, 79),
    //    TColor::GetColor(239, 60, 45),
    //    TColor::GetColor(203, 27, 22),
    //    TColor::GetColor(101, 1, 12)
    //};
    // Define as cores RGB da paleta Coolors VERDE e ROSA
    std::vector<int> colors = {
    TColor::GetColor(102, 153, 0),    // verde escuro
    TColor::GetColor(153, 204, 51),   // verde claro
    TColor::GetColor(204, 238, 102),  // verde limão
    TColor::GetColor(0, 102, 153),    // azul escuro
    TColor::GetColor(51, 153, 204),   // azul claro
    TColor::GetColor(153, 0, 102),    // roxo
    TColor::GetColor(204, 51, 153),   // rosa
    TColor::GetColor(255, 102, 0),    // laranja escuro
    TColor::GetColor(255, 153, 0),    // laranja médio
    TColor::GetColor(255, 204, 0)     // amarelo
    };

    // Setup geometry
    TGeoManager *geom = new TGeoManager("FOCAL", "FOCAL Layer Colorful");

    TGeoMaterial *matAir = new TGeoMaterial("Air", 0, 0, 0);
    TGeoMedium *medAir = new TGeoMedium("Air", 1, matAir);

    TGeoMaterial *matSi = new TGeoMaterial("Silicon", 28.0855, 14, 2.33);
    TGeoMedium *medSi = new TGeoMedium("Si", 2, matSi);

    TGeoVolume *top = geom->MakeBox("TOP", medAir, nPadsX * padSizeX / 2.0 + 5, nPadsY * padSizeY / 2.0 + 5, padThickness);
    geom->SetTopVolume(top);

    // Laço para desenhar cada pad com cor conforme módulo
    for (int iX = 0; iX < nPadsX; ++iX) {
        for (int iY = 0; iY < nPadsY; ++iY) {
            int modX = iX / padsPerModX;
            int modY = iY / padsPerModY;
            int modIndex = modY * nModulesX + modX;
            int colorIndex = (modX + modY * 3) % colors.size();
            //int colorIndex = modIndex % colors.size();

            TString name = Form("pad_%d_%d", iX, iY);
            TGeoVolume *pad = geom->MakeBox(name, medSi, padSizeX / 2, padSizeY / 2, padThickness / 2);
            pad->SetLineColor(colors[colorIndex]);
            pad->SetFillColor(colors[colorIndex]);
            pad->SetTransparency(0); // 0 opaco 100 transparente

            float x = xMin + iX * padSizeX + padSizeX / 2;
            float y = yMin + iY * padSizeY + padSizeY / 2;
            int padID = iY * nPadsX + iX;

            top->AddNode(pad, padID, new TGeoTranslation(x, y, 0));
        }
    }

    geom->CloseGeometry();
    top->Draw("ogl");
}
