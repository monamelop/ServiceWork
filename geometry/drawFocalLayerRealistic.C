void drawFocalLayerRealistic() {
    const int padsPerModX = 9;
    const int padsPerModY = 8;
    const float padSizeX = 1.0;
    const float padSizeY = 1.0;
    const float padThickness = 0.5;

    const int fullModRowX = 10;
    const int fullModColY = 11;
    const int totalPadsX = fullModRowX * padsPerModX;
    const int totalPadsY = fullModColY * padsPerModY;

    const float xMin = -totalPadsX * padSizeX / 2.0;
    const float yMin = -totalPadsY * padSizeY / 2.0;

    // Paleta usando new TColor(...) com RGB normalizado
    std::vector<int> colors = {
        (new TColor(102. / 255, 153. / 255, 0. / 255))->GetNumber(),
        (new TColor(153. / 255, 204. / 255, 51. / 255))->GetNumber(),
        (new TColor(204. / 255, 238. / 255, 102. / 255))->GetNumber(),
        (new TColor(0. / 255, 102. / 255, 153. / 255))->GetNumber(),
        (new TColor(51. / 255, 153. / 255, 204. / 255))->GetNumber(),
        (new TColor(153. / 255, 0. / 255, 102. / 255))->GetNumber(),
        (new TColor(204. / 255, 51. / 255, 153. / 255))->GetNumber(),
        (new TColor(255. / 255, 102. / 255, 0. / 255))->GetNumber(),
        (new TColor(255. / 255, 153. / 255, 0. / 255))->GetNumber(),
        (new TColor(255. / 255, 204. / 255, 0. / 255))->GetNumber()
    };

    TGeoManager* geom = new TGeoManager("FOCAL", "FOCAL realistic layer with central gap");

    TGeoMaterial* matAir = new TGeoMaterial("Air", 0, 0, 0);
    TGeoMedium* medAir = new TGeoMedium("Air", 1, matAir);
    TGeoMaterial* matSi = new TGeoMaterial("Silicon", 28.0855, 14, 2.33);
    TGeoMedium* medSi = new TGeoMedium("Si", 2, matSi);

    TGeoVolume* top = geom->MakeBox("TOP", medAir,
        totalPadsX * padSizeX / 2.0 + 5, totalPadsY * padSizeY / 2.0 + 5, padThickness);
    geom->SetTopVolume(top);

    int moduleCount = 0;  // contador de módulos reais

    for (int modY = 0; modY < fullModColY; ++modY) {
        int modulesThisRow = (modY == 5) ? 9 : 10;
        int xOffsetPads = (modY == 5) ? padsPerModX / 2 : 0;

        for (int modX = 0; modX < modulesThisRow; ++modX) {
            // pula o módulo central da linha 6
            if (modY == 5 && modX == 4) continue;

            //int color = colors[moduleCount % colors.size()];
            //moduleCount++;
            int color = colors[(modX + modY * 3) % colors.size()];

            for (int iX = 0; iX < padsPerModX; ++iX) {
                for (int iY = 0; iY < padsPerModY; ++iY) {
                    int globalX = modX * padsPerModX + iX + xOffsetPads;
                    int globalY = modY * padsPerModY + iY;

                    float x = xMin + globalX * padSizeX + padSizeX / 2;
                    float y = yMin + globalY * padSizeY + padSizeY / 2;

                    TString name = Form("pad_%d_%d", globalX, globalY);
                    TGeoVolume* pad = geom->MakeBox(name, medSi, padSizeX / 2, padSizeY / 2, padThickness / 2);
                    pad->SetFillColor(color);
                    pad->SetLineColor(color);
                    pad->SetTransparency(0);

                    int padID = globalY * totalPadsX + globalX;
                    top->AddNode(pad, padID, new TGeoTranslation(x, y, 0));
                }
            }
        }
    }

    geom->CloseGeometry();
    top->Draw("ogl");
}
