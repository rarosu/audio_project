#include "filter.hpp"
#include <fftw3.h>



ConvolutionFilter::ConvolutionFilter(std::vector<double>& impulseTime) 
	: m_impulseFreq(impulseTime.size()) {

	fftw_plan p = fftw_plan_dft_r2c_1d(impulseTime.size(), &impulseTime[0], m_impulseFreq.m_array, FFTW_ESTIMATE);
	fftw_execute(p);
	fftw_destroy_plan(p);
}

Channels ConvolutionFilter::apply(const std::vector<double>& right, const std::vector<double>& left) {
	Channels result;

	// find the closest larger power of two to the output size
	int minOutputSize = right.size() + m_impulseFreq.m_size - 1;
	int size = 2;
	while (size < minOutputSize / 4) {
		size <<= 1;
	}

	// zero-pad time domain data
	FFTWArray<double> rightTime(size);
	FFTWArray<double> leftTime(size);
	memset(rightTime.m_array, 0, size * sizeof(double));
	memcpy(rightTime.m_array, &right[0], size * sizeof(double));
	memset(leftTime.m_array, 0, size * sizeof(double));
	memcpy(leftTime.m_array, &left[0], size * sizeof(double));

	// transform the samples to the frequency domain and zero-pad
	FFTWArray<fftw_complex> rightFreq(size);
	FFTWArray<fftw_complex> leftFreq(size);
	memset(rightFreq.m_array, 0, size * sizeof(fftw_complex));
	memset(leftFreq.m_array, 0, size * sizeof(fftw_complex));

	{
		fftw_plan p = fftw_plan_dft_r2c_1d(size, &rightTime[0], rightFreq.m_array, FFTW_ESTIMATE);
		fftw_execute(p);
		fftw_destroy_plan(p);
	}

	{
		fftw_plan p = fftw_plan_dft_r2c_1d(size, &leftTime[0], leftFreq.m_array, FFTW_ESTIMATE);
		fftw_execute(p);
		fftw_destroy_plan(p);
	}

	// Copy impulse into a proper size array and zero-pad
	FFTWArray<fftw_complex> impulseFreqPadded(size);
	memset(impulseFreqPadded.m_array, 0, size * sizeof(fftw_complex));
	memcpy(impulseFreqPadded.m_array, m_impulseFreq.m_array, m_impulseFreq.m_size * sizeof(fftw_complex));


	// Multiply the two complex signals with the impulse
	FFTWArray<fftw_complex> rightSpectral(size);
	FFTWArray<fftw_complex> leftSpectral(size);
	for (int i = 0; i < size; ++i) {
		rightSpectral[i][0] = rightFreq[i][0] * impulseFreqPadded[i][0] - rightFreq[i][1] * impulseFreqPadded[i][1];
		rightSpectral[i][1] = rightFreq[i][0] * impulseFreqPadded[i][1] - rightFreq[i][1] * impulseFreqPadded[i][0];

		leftSpectral[i][0] = leftFreq[i][0] * impulseFreqPadded[i][0] - leftFreq[i][1] * impulseFreqPadded[i][1];
		leftSpectral[i][1] = leftFreq[i][0] * impulseFreqPadded[i][1] - leftFreq[i][1] * impulseFreqPadded[i][0];
	}

	// Convert signals back to time domain
	FFTWArray<double> outputRightTime(size);
	FFTWArray<double> outputLeftTime(size);

	{
		fftw_plan p = fftw_plan_dft_c2r_1d(size, rightSpectral.m_array, outputRightTime.m_array, FFTW_ESTIMATE);
		fftw_execute(p);
		fftw_destroy_plan(p);
	}

	{
		fftw_plan p = fftw_plan_dft_c2r_1d(size, leftSpectral.m_array, outputLeftTime.m_array, FFTW_ESTIMATE);
		fftw_execute(p);
		fftw_destroy_plan(p);
	}

	// Copy signals into the result
	result.m_right = std::move(std::vector<double>(outputRightTime.m_array, outputRightTime.m_array + right.size()));
	result.m_left = std::move(std::vector<double>(outputLeftTime.m_array, outputLeftTime.m_array + left.size()));

	// Normalize the samples
	double rPeak = 0.0;
	double lPeak = 0.0;
	double rInPeak = 0.0;
	double lInPeak = 0.0;
	for (int i = 0; i < right.size(); ++i) {
		if (fabs(right[i]) > rInPeak)
			rInPeak = fabs(right[i]);
		if (fabs(left[i]) > lInPeak)
			lInPeak = fabs(left[i]);

		if (fabs(result.m_right[i]) > rPeak)
			rPeak = fabs(result.m_right[i]);
		if (fabs(result.m_left[i]) > lPeak)
			lPeak = fabs(result.m_left[i]);
	}

	double rFac = rInPeak / rPeak;
	double lFac = lInPeak / lPeak;
	for (int i = 0; i < right.size(); ++i) {
		result.m_right[i] *= rFac;
		result.m_left[i] *= lFac;
	}

	return result;
}
