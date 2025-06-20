void drawFocalELayers_20()
{
    const int nModulesX = 10;
    const int nModulesY = 11;
    const float moduleSizeX = 9.0;
    const float moduleSizeY = 8.0;
    const float padThickness = 0.5;
    const float gap = 0.5;
    const float zStart = 0.0;
    const int nLayers = 20;

    const int centralRow = 5;
    const int centralHoleX = 4;  // entre modX=4 e 5
    const float shiftAmount = moduleSizeX / 2.0;

    const float detectorSizeX = nModulesX * moduleSizeX;
    const float detectorSizeY = nModulesY * moduleSizeY;
    const float xMin = -detectorSizeX / 2.0;
    const float yMin = -detectorSizeY / 2.0;

    std::vector<int> colors = {
        TColor::GetColor(102, 153, 0),
        TColor::GetColor(153, 204, 51),
        TColor::GetColor(204, 238, 102),
        TColor::GetColor(0, 102, 153),
        TColor::GetColor(51, 153, 204),
        TColor::GetColor(153, 0, 102),
        TColor::GetColor(204, 51, 153),
        TColor::GetColor(255, 102, 0),
        TColor::GetColor(255, 153, 0),
        TColor::GetColor(255, 204, 0)
    };

    TGeoManager *geom = new TGeoManager("FOCAL", "FOCAL-E 20 Layers Modules Only");

    TGeoMaterial *matAir = new TGeoMaterial("Air", 0, 0, 0);
    TGeoMedium *medAir = new TGeoMedium("Air", 1, matAir);

    TGeoMaterial *matSi = new TGeoMaterial("Silicon", 28.0855, 14, 2.33);
    TGeoMedium *medSi = new TGeoMedium("Si", 2, matSi);

    TGeoVolume *top = geom->MakeBox("TOP", medAir,
        detectorSizeX / 2.0 + 10, detectorSizeY / 2.0 + 10,
        (padThickness + gap) * nLayers / 2.0);
    geom->SetTopVolume(top);

    int volumeID = 0;
    for (int iLayer = 0; iLayer < nLayers; ++iLayer) {
        float z = zStart + iLayer * (padThickness + gap);

        for (int modY = 0; modY < nModulesY; ++modY) {
            for (int modX = 0; modX < nModulesX; ++modX) {
                // pula buraco entre modX=4 e 5 na linha central
                if (modY == centralRow && (modX == 4 || modX == 5)) continue;

                float x = xMin + modX * moduleSizeX + moduleSizeX / 2.0;
                float y = yMin + modY * moduleSizeY + moduleSizeY / 2.0;

                // Corrige alinhamento horizontal da linha com buraco
                if (modY == centralRow) {
                    if (modX < 4) {
                        x += shiftAmount;
                    } else if (modX > 5) {
                        x -= shiftAmount;
                    }
                }

                int colorIndex = (modX + modY * 3) % colors.size();

                TString name = Form("mod_%d_%d_%d", iLayer, modX, modY);
                TGeoVolume *mod = geom->MakeBox(name, medSi,
                    moduleSizeX / 2.0, moduleSizeY / 2.0, padThickness / 2.0);
                mod->SetLineColor(colors[colorIndex]);
                mod->SetFillColor(colors[colorIndex]);
                mod->SetTransparency(0);

                top->AddNode(mod, volumeID++, new TGeoTranslation(x, y, z));
            }
        }
    }

    geom->CloseGeometry();
    top->Draw("ogl");
}
