#include "filter.hpp"
#include <r2tk\r2-assert.hpp>


StereoPanningFilter::StereoPanningFilter(const Listener& listener, const glm::vec3& position) :
	m_listener(listener),
	m_position(position) {}

Channels StereoPanningFilter::apply(const std::vector<double>& right, const std::vector<double>& left) {
	glm::vec3 displacement = m_position - m_listener.m_position;
	glm::vec3 direction = displacement;
	if (direction == glm::vec3(0,0,0))
		direction = m_listener.m_facing;
	direction = glm::normalize(direction);

	double distanceSquared = glm::dot(displacement, displacement);
	double dotRight = glm::dot(direction, m_listener.getRight());
	double dotLeft = glm::dot(direction, m_listener.getLeft());
	
	PanVolume vol = constantPower(dotRight);
	double distanceFactor = 1.0f / (1.0f + 0.005 * distanceSquared);

	Channels result;
	result.m_right.resize(right.size());
	result.m_left.resize(left.size());
	for (int i = 0; i < right.size(); ++i) {
		result.m_right[i] = right[i] * vol.right * distanceFactor;
		result.m_left[i] = left[i] * vol.left * distanceFactor;
	}

	return result;
}

StereoPanningFilter::PanVolume StereoPanningFilter::constantPower(double position) const {
	PanVolume result;
	const double SQRT2INV = 0.707107f;
	const double PIOVER4 = 0.785398f;
	
	double angle = position * PIOVER4;

	result.left = SQRT2INV * (cos(angle) - sin(angle));
	result.right = SQRT2INV * (cos(angle) + sin(angle));

	return result;
}


ConvolutionFilter::ConvolutionFilter(std::vector<double>& impulseTime) 
	: m_impulseFreq(impulseTime.size()) {

	fftw_plan p = fftw_plan_dft_r2c_1d(impulseTime.size(), &impulseTime[0], m_impulseFreq.m_array, FFTW_ESTIMATE);
	fftw_execute(p);
	fftw_destroy_plan(p);
}

Channels ConvolutionFilter::apply(const std::vector<double>& right, const std::vector<double>& left) {
	Channels result;

	result.m_right = std::move(applyImpulse(right));
	result.m_left = std::move(applyImpulse(left));

	return result;
}

std::vector<double> ConvolutionFilter::applyImpulse(std::vector<double> samplesTime) {
	// Find the closest larger power of two to the output size
	int minOutputSize = samplesTime.size() + m_impulseFreq.m_size - 1;
	int size = 2;
	while (size < minOutputSize) {
		size <<= 1;
	}

	// Zero pad the input
	FFTWArray<double> samplesTimePadded(size);
	memset(samplesTimePadded.m_array, 0, size * sizeof(double));
	memcpy(samplesTimePadded.m_array, &samplesTime[0], samplesTime.size() * sizeof(double));
	
	// Copy impulse into a proper size array and zero-pad
	FFTWArray<fftw_complex> impulseFreqPadded(size);
	memset(impulseFreqPadded.m_array, 0, size * sizeof(fftw_complex));
	memcpy(impulseFreqPadded.m_array, m_impulseFreq.m_array, m_impulseFreq.m_size * sizeof(fftw_complex));

	// Transform the samples to the frequency domain and zero-pad
	FFTWArray<fftw_complex> samplesFreq(size);
	memset(samplesFreq.m_array, 0, size * sizeof(fftw_complex));

	{
		fftw_plan p = fftw_plan_dft_r2c_1d(size, samplesTimePadded.m_array, samplesFreq.m_array, FFTW_ESTIMATE);
		fftw_execute(p);
		fftw_destroy_plan(p);
	}

	// Multiply the signal with the impulse
	FFTWArray<fftw_complex> samplesSpectral(size);
	for (int i = 0; i < size; ++i) {
		samplesSpectral[i][0] = samplesFreq[i][0] * impulseFreqPadded[i][0] - samplesFreq[i][1] * impulseFreqPadded[i][1];
		samplesSpectral[i][1] = samplesFreq[i][0] * impulseFreqPadded[i][1] - samplesFreq[i][1] * impulseFreqPadded[i][0];
	}

	// Convert signals back to time domain
	FFTWArray<double> outputSamplesTime(size);

	{
		fftw_plan p = fftw_plan_dft_c2r_1d(size, samplesSpectral.m_array, outputSamplesTime.m_array, FFTW_ESTIMATE);
		fftw_execute(p);
		fftw_destroy_plan(p);
	}

	// Copy the result into a non-zero padded vector
	std::vector<double> result = std::move(std::vector<double>(outputSamplesTime.m_array, outputSamplesTime.m_array + samplesTime.size()));

	// Normalize the samples
	double peak = 0.0;
	double inPeak = 0.0;
	for (int i = 0; i < samplesTime.size(); ++i) {
		if (fabs(samplesTime[i]) > inPeak)
			inPeak = fabs(samplesTime[i]);
		if (fabs(result[i]) > peak)
			peak = fabs(result[i]);
	}

	double fac = inPeak / peak;
	for (int i = 0; i < result.size(); ++i) {
		result[i] *= fac;
	}

	// Return the filtered samples
	return result;
}

std::vector<double> ConvolutionFilter::generateEchoImpulse(int sampleRate, double delay, double decay) {
	r2AssertM(decay >= 0.0 && decay <= 1.0, "Invalid decay");

	std::vector<double> result(size_t(sampleRate * delay), 0.0);
	result.front() = 1.0f;
	result.back() = decay;

	return result;
}
