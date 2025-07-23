// Function that returns a FairGenerator for a single electron beam
// Monalisa Melo (maria.paulino@usp.br)
FairGenerator* gun(double energy) {
    // Print information about the generator to the console
    std::cout << "Single electron generator shooting at EMCAL with Energy "
              << energy << " GeV/c" << std::endl;

    // Create a FairBoxGenerator for electrons (PDG code 11), generating one per event
    auto elecgen = new FairBoxGenerator(11, 1);

    //elecgen->SetXYZ(13.5, 10.0, 0.0); //definicao da posicao do vertex
    //elecgen->SetBoxSize(0.5, 0.5, 0.01); //largura do feixe
    elecgen->SetBoxXYZ(13.5, 10.0, 1.0, 1.0, 0.0);
    //elecgen->SetVertex(13.5, 10.0, 0.0, 1.0, 1.0, 1.0, kBox);



    // Set pseudorapidity range for the generated electron
    //elecgen->SetEtaRange(3.4, 5.8);
    elecgen->SetEtaRange(4.4, 5.8);

    // Set azimuthal angle (phi) range in degrees
    elecgen->SetPhiRange(0, 360);

    // Set momentum range
    elecgen->SetPRange(energy, energy);

    // Origin: targeting the pad at (x, y) = (-44.5, 43.5) cm in the FOCAL plane
    
    //elecgen->SetBoxXYZ(-44.5, 43.5, 700.0, 0.05, 0.05, 0.05);


    return elecgen;
}
