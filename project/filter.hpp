#ifndef FILTER_HPP
#define FILTER_HPP

#include <vector>

struct Channels {
	std::vector<double> m_right;
	std::vector<double> m_left;
};

/** Public interface for a filter */
class Filter {
public:
	virtual Channels apply(const std::vector<double>& right, const std::vector<double>& left) = 0;
};


/** Handle a FFTW array */
template <typename T>
struct FFTWArray {
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
private:
	FFTWArray<fftw_complex> m_impulseFreq;
};

#endif