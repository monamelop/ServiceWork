//====================================================================
// drawFocalLayerRealistic()
//
// This script builds a realistic single-layer geometric representation 
// of the FOCAL-E detector, where each module consists of a 9×8 grid of 
// pads. The full detector is composed of 10 modules in X and 11 in Y.
//
// Colors are assigned to pads based on their module position. The full 
// layout is visualized using ROOT’s OpenGL viewer.
//
// Monalisa Melo (maria.paulino@usp.br)
//====================================================================

void drawFocalLayerRealistic() {
    // Define pad dimensions and layout per module
    const int padsPerModX = 9;
    const int padsPerModY = 8;
    const float padSizeX = 1.0;
    const float padSizeY = 1.0;
    const float padThickness = 0.85;

    // Total number of modules in X and Y
    const int fullModRowX = 10;
    const int fullModColY = 11;

    // Total pad counts for full layer
    const int totalPadsX = fullModRowX * padsPerModX;
    const int totalPadsY = fullModColY * padsPerModY;

    // Lower left coordinates (origin centered)
    const float xMin = -totalPadsX * padSizeX / 2.0;
    const float yMin = -totalPadsY * padSizeY / 2.0;

    // Define a color palette for module distinction
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

    // Initialize ROOT geometry manager
    TGeoManager* geom = new TGeoManager("FOCAL", "FOCAL realistic layer with central gap");

    // Define air and silicon materials
    TGeoMaterial* matAir = new TGeoMaterial("Air", 0, 0, 0);
    TGeoMedium* medAir = new TGeoMedium("Air", 1, matAir);
    TGeoMaterial* matSi = new TGeoMaterial("Silicon", 28.0855, 14, 2.33);
    TGeoMedium* medSi = new TGeoMedium("Si", 2, matSi);

    // Create top-level volume (world volume)
    TGeoVolume* top = geom->MakeBox("TOP", medAir,
        totalPadsX * padSizeX / 2.0 + 5, totalPadsY * padSizeY / 2.0 + 5, padThickness);
    geom->SetTopVolume(top);

    int moduleCount = 0;  // Optional counter for module indexing

    // Loop over all module rows
    for (int modY = 0; modY < fullModColY; ++modY) {
        // Handle the central row with a missing module
        int modulesThisRow = (modY == 5) ? 9 : 10; // 9 modules in row 6 due to central gap
        int xOffsetPads = (modY == 5) ? padsPerModX / 2 : 0; // Shift for alignment

        for (int modX = 0; modX < modulesThisRow; ++modX) {
            if (modY == 5 && modX == 4) continue;  // Skip the central module in row 6

            // Assign color based on module position
            int color = colors[(modX + modY * 3) % colors.size()];

            // Loop over pads within the module
            for (int iX = 0; iX < padsPerModX; ++iX) {
                for (int iY = 0; iY < padsPerModY; ++iY) {
                    // Compute global pad indices
                    int globalX = modX * padsPerModX + iX + xOffsetPads;
                    int globalY = modY * padsPerModY + iY;

                    // Compute global pad position in XY
                    float x = xMin + globalX * padSizeX + padSizeX / 2;
                    float y = yMin + globalY * padSizeY + padSizeY / 2;

                    // Create and place pad volume
                    TString name = Form("pad_%d_%d", globalX, globalY);
                    TGeoVolume* pad = geom->MakeBox(name, medSi, padSizeX / 2, padSizeY / 2, padThickness / 2);
                    pad->SetFillColor(color);
                    pad->SetLineColor(color);
                    pad->SetTransparency(20);

                    int padID = globalY * totalPadsX + globalX;
                    top->AddNode(pad, padID, new TGeoTranslation(x, y, 0));
                }
            }
        }
    }

    // Finalize and display geometry
    geom->CloseGeometry();
    top->Draw("ogl");
}
