#include "PrimaryGeneratorAction.hh"

SRT::PrimaryGeneratorAction::PrimaryGeneratorAction(const std::map<double, double>* spectrum) : 
	spectrum_(spectrum)
{
	this->particle_gun = new G4ParticleGun();

	G4ParticleTable* particle_table = G4ParticleTable::GetParticleTable();
	G4ParticleDefinition* particle = particle_table->FindParticle("gamma");

	this->particle_gun->SetParticleDefinition(particle);
}

SRT::PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
	delete this->particle_gun;
}

/* NOTE: Beam coord sys:
	+x = direction
	+y = horizontal
	+z = vertical
*/
void SRT::PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
	static constexpr double sad = 40768.0 * mm;
	static constexpr double scd = 40648.0 * mm;
	static constexpr double field_width = 10.0 *mm;
	static constexpr double field_height = 10.0 *mm;
	static constexpr double sigma_width = 0.078 *mm;
	static constexpr double sigma_height = 0.0 * mm;
	static constexpr double microbeam_width = 50 * um;
	static constexpr double ctc = 400 * um;

	/* Sample the source*/
	G4ThreeVector source = SampleSource(sigma_width, sigma_height);
	source.setX(-sad);

	/* Set particle position*/
	G4ThreeVector position = SampleRectangularMRTField(field_width, field_height, microbeam_width, ctc);
	position.setX(-(sad - scd));
	this->particle_gun->SetParticlePosition(position);

	/* Set particle direction*/
	G4ThreeVector direction = position - source;
	direction.setZ(0);
	this->particle_gun->SetParticleMomentumDirection(direction);

	/* Set particle polarisation*/
	G4ThreeVector polarisation = direction.cross(G4ThreeVector(0.0, 0.0, 1.0));
	this->particle_gun->SetParticlePolarization(polarisation);

	/* Set particle energy*/
	double energy = SampleSpectrum(this->spectrum_);
	this->particle_gun->SetParticleEnergy(energy);

	this->particle_gun->GeneratePrimaryVertex(event);

	return;
}
