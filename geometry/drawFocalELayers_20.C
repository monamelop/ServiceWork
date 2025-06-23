//=======================================================================
// drawFocalELayers_20()
// 
// This script builds a simplified geometric model of the FOCAL-E 
// detector, consisting of 20 layers (18 pad layers and 2 pixels layers) 
// of modules arranged in a grid of 10 modules along X and 11 along Y. 
//
// The result is drawn using ROOT’s OpenGL viewer.
// Monalisa Melo (maria.paulino@usp.br)
//=======================================================================


void drawFocalELayers_20()
{
    // Define module layout and geometry parameters
    const int nModulesX = 10;               // Number of modules horizontally
    const int nModulesY = 11;               // Number of modules vertically
    const float moduleSizeX = 9.0;          // Module width (along X)
    const float moduleSizeY = 8.0;          // Module height (along Y)
    const float padThickness = 0.85;         // Thickness of a pad in Z
    const float gap = 0.5;                  // Gap between layers
    const float zStart = 0.0;               // Starting Z position for layers
    const int nLayers = 20;                 // Total number of layers

    // Parameters to handle the central gap in one specific row
    const int centralRow = 5;               // Index of the row with the central hole
    const int centralHoleX = 4;             // Modules 4 and 5 are skipped in this row
    const float shiftAmount = moduleSizeX / 2.0;  // Shift amount to realign modules

    // Calculate full detector dimensions for positioning
    const float detectorSizeX = nModulesX * moduleSizeX;
    const float detectorSizeY = nModulesY * moduleSizeY;
    const float xMin = -detectorSizeX / 2.0;    // Lower bound X
    const float yMin = -detectorSizeY / 2.0;    // Lower bound Y

    // Define a color palette to color modules
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
    int brown = TColor::GetColor(139, 69, 19);  // Special brown color (used in selected layers)

    // Create geometry manager
    TGeoManager *geom = new TGeoManager("FOCAL", "FOCAL-E 20 Layers Modules Only");

    // Define materials: air (for world) and silicon (for modules)
    TGeoMaterial *matAir = new TGeoMaterial("Air", 0, 0, 0);
    TGeoMedium *medAir = new TGeoMedium("Air", 1, matAir);

    TGeoMaterial *matSi = new TGeoMaterial("Silicon", 28.0855, 14, 2.33);
    TGeoMedium *medSi = new TGeoMedium("Si", 2, matSi);

    // Create top volume (world box)
    TGeoVolume *top = geom->MakeBox("TOP", medAir,
        detectorSizeX / 2.0 + 10, detectorSizeY / 2.0 + 10,
        (padThickness + gap) * nLayers / 2.0);
    geom->SetTopVolume(top);

    int volumeID = 0;  // Unique ID for each module

    // Loop over layers (along Z direction)
    for (int iLayer = 0; iLayer < nLayers; ++iLayer) {
        float z = zStart + iLayer * (padThickness + gap);

        // Loop over module grid (Y and X)
        for (int modY = 0; modY < nModulesY; ++modY) {
            for (int modX = 0; modX < nModulesX; ++modX) {

                // Skip central hole in the middle row (modules 4 and 5)
                if (modY == centralRow && (modX == 4 || modX == 5)) continue;

                // Compute module center coordinates
                float x = xMin + modX * moduleSizeX + moduleSizeX / 2.0;
                float y = yMin + modY * moduleSizeY + moduleSizeY / 2.0;

                // Shift modules left or right in central row to close the hole gap
                if (modY == centralRow) {
                    if (modX < 4) {
                        x += shiftAmount;
                    } else if (modX > 5) {
                        x -= shiftAmount;
                    }
                }

                // Pick color based on position
                int colorIndex = (modX + modY * 3) % colors.size();

                // Create module volume with unique name
                TString name = Form("mod_%d_%d_%d", iLayer, modX, modY);
                TGeoVolume *mod = geom->MakeBox(name, medSi,
                    moduleSizeX / 2.0, moduleSizeY / 2.0, padThickness / 2.0);

                // Set brown color for specific layers (e.g., 5th and 10th layers)
                if (iLayer == 4 || iLayer == 9) {
                    mod->SetLineColor(brown);
                    mod->SetFillColor(brown);
                } else {
                    mod->SetLineColor(colors[colorIndex]);
                    mod->SetFillColor(colors[colorIndex]);
                }
                mod->SetTransparency(0);  // Fully opaque

                // Add the module to the top volume at the calculated position
                top->AddNode(mod, volumeID++, new TGeoTranslation(x, y, z));
            }
        }
    }

    // Finalize geometry and draw with OpenGL
    geom->CloseGeometry();
    top->Draw("ogl");
}
