// Function that returns a FairGenerator for a single electron beam
// Monalisa Melo (maria.paulino@usp.br)
FairGenerator* gun(double energy) {
    // Print information about the generator to the console
    std::cout << "Single electron generator shooting at EMCAL with Energy "
              << energy << " GeV/c" << std::endl;

    // Create a FairBoxGenerator for electrons (PDG code 11), generating one per event
    auto elecgen = new FairBoxGenerator(11, 1);

    // Set pseudorapidity range for the generated electron
    elecgen->SetEtaRange(3.4, 5.8);

    // Set azimuthal angle (phi) range in degrees
    elecgen->SetPhiRange(0, 360);

    // Set momentum range
    elecgen->SetPRange(energy, energy);

	return elecgen;
}