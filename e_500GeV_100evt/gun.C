FairGenerator *gun(double energy) {
	std::cout << "Single electron generator shooting at EMCAL with Energy " << energy << "GeV/c" << std::endl;
	auto elecgen = new FairBoxGenerator(11, 1);
	elecgen->SetEtaRange(3.4, 5.8);
	elecgen->SetPhiRange(0, 360);
	elecgen->SetPRange(energy, energy);
	return elecgen;
}
