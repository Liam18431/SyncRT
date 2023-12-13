#include "PrimaryGeneratorAction.hh"

SRT::PrimaryGeneratorAction::PrimaryGeneratorAction(const std::map<double, double>* spectrum_cdf) : 
	spectrum_cdf_(spectrum_cdf)
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

inline G4ThreeVector SampleRectangularSource(double field_width, double field_height)
{
	double x = 0;
	double y = G4UniformRand() * field_width - field_width / 2.;
	double z = G4UniformRand() * field_height - field_height / 2.;

	return G4ThreeVector(x, y, z);
}

inline G4ThreeVector SampleCircularSource(double radius)
{
	double x = 0;

	double sample_radius = G4UniformRand() * radius;
	double sample_angle = G4UniformRand() * 2 * SRT::pi_;

	double y = std::cos(sample_angle) * sample_radius;
	double z = std::sin(sample_angle) * sample_radius;

	return G4ThreeVector(x, y, z);
}

inline G4ThreeVector SampleMRTSource(double field_width, double field_height, double microbeam_width, double ctc)
{
	double x = 0;

	int n_microbeams = static_cast<int>(field_width / ctc);

	if (n_microbeams % 2 == 0) n_microbeams += 1;

	double new_field_width = (n_microbeams - 1) * ctc + microbeam_width;

	int microbeam = static_cast<int>(G4UniformRand() * n_microbeams);

	double microbeam_centre_coord = microbeam * ctc + microbeam_width / 2. - new_field_width / 2.;

	double y = (microbeam_centre_coord - microbeam_width / 2.) + G4UniformRand() * microbeam_width;

	double z = G4UniformRand() * field_height - field_height / 2.;

	return G4ThreeVector(x, y, z);
}

void SRT::PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
	double field_width = 10.0 *mm;
	double field_height = 10.0 *mm;
	double microbeam_width = 50 *um;
	double ctc = 400 *um;
	double radius = 5.0f *mm;

	/* Set particle position*/
	G4ThreeVector offset = G4ThreeVector(-12.0 * cm, 0.0 *cm, 0.0 *cm);

	/* MRT */
	G4ThreeVector sample = SampleMRTSource(field_width, field_height, microbeam_width, ctc);

	/* BB */
	//G4ThreeVector sample = SampleRectangularSource(field_width, field_height);
	//G4ThreeVector sample = SampleCircularSource(radius);

	G4ThreeVector position = sample + offset;
	this->particle_gun->SetParticlePosition(position);

	/* Set particle direction*/
	G4ThreeVector direction = G4ThreeVector(1.0, 0.0, 0.0);
	this->particle_gun->SetParticleMomentumDirection(direction);

	/* Set particle polarisation*/
	G4ThreeVector polarisation = G4ThreeVector(0.0, 1.0, 0.0);
	this->particle_gun->SetParticlePolarization(polarisation);

	/* Set particle energy*/
	double energy = this->spectrum_cdf_->lower_bound(G4UniformRand())->second *keV;
	this->particle_gun->SetParticleEnergy(energy);

	this->particle_gun->GeneratePrimaryVertex(event);
}