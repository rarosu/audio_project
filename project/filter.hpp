#ifndef FILTER_HPP
#define FILTER_HPP

#include <vector>
#include <glm\glm.hpp>
#include <fftw3.h>
#include "sound-common.hpp"

struct Channels {
	std::vector<double> m_right;
	std::vector<double> m_left;
};

/** Public interface for a filter */
class Filter {
public:
	virtual Channels apply(const std::vector<double>& right, const std::vector<double>& left) = 0;
};


/** This filter is NOT LINEAR and should be applied last always */
class StereoPanningFilter : public Filter {
public:
	StereoPanningFilter(const Listener& listener, const glm::vec3& position);

	Channels apply(const std::vector<double>& right, const std::vector<double>& left);
private:
	struct PanVolume {
		double left;
		double right;
	};

	const glm::vec3& m_position;
	const Listener& m_listener;

	PanVolume constantPower(double position) const;
};


/** Handle a FFTW array */
template <typename T>
class FFTWArray {
public:
	FFTWArray(size_t N) : m_size(N) { m_array = (T*) fftw_malloc(sizeof(T) * N); }
	~FFTWArray() throw() { fftw_free(m_array); }

	T& operator[](size_t i) { return m_array[i]; }
	const T& operator[](size_t i) const { return m_array[i]; }

	T* m_array;
	size_t m_size;
private:
	FFTWArray(const FFTWArray<T>&);
	FFTWArray& operator=(const FFTWArray<T>&);
};

/** Applies a filter using an impulse */
class ConvolutionFilter : public Filter {
public:
	ConvolutionFilter(std::vector<double>& impulseTime);

	Channels apply(const std::vector<double>& right, const std::vector<double>& left);

	static std::vector<double> generateEchoImpulse(int sampleRate, double delay, double decay);
private:
	FFTWArray<fftw_complex> m_impulseFreq;

	std::vector<double> applyImpulse(std::vector<double> samplesTime);
};

#endif