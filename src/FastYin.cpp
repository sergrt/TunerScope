#include "FastYin.h"

#include <fftw3.h>

#include <cmath>
#include <complex>

namespace {

void fft(std::vector<fftw_complex> &data, int fftSize) {
    const int N = fftSize;
    fftw_plan p = fftw_plan_dft_1d(N, data.data(), data.data(), FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p);
    fftw_destroy_plan(p);
}

void inverseFft(std::vector<fftw_complex> &data, int fftSize) {
    const int N = fftSize;
    fftw_plan p = fftw_plan_dft_1d(N, data.data(), data.data(), FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(p);
    fftw_destroy_plan(p);

    for (int i = 0; i < data.size(); ++i) {
        //data[i][0] /= N;
        data[i][1] /= N;
    }
}

std::vector<fftw_complex> convertQVector(const QVector<float>& data) {
    std::vector<fftw_complex> result(data.size());
    for (int i = 0; i < result.size(); ++i) {
        result[i][0] = data[i];
        result[i][1] = 0.0f;
    }
    return result;
}

QVector<float> convertVector(std::vector<fftw_complex>& data) {
    QVector<float> result(data.size() * 2);
    for (int i = 0; i < data.size(); ++i) {
        result[2*i] = data[i][0];
        result[2*i + 1] = data[i][1];
    }
    return result;
}

double nextPow2(double n) {
    if (n <= 0)
        return 1;
    return std::pow(2, std::ceil(std::log2(n)));
}

} // namespace

FastYin::FastYin(QVector<float> audioBuffer, int sampleRate) {
    m_audioBuffer.swap(audioBuffer);
    m_sampleRate = sampleRate;

    const size_t bufferSize = m_audioBuffer.size();
    m_yinBuffer.resize(bufferSize / 2);
    m_audioBufferFFT.resize(2 * bufferSize);
    m_kernel.resize(2 * bufferSize);
    m_yinStyleACF.resize(2 * bufferSize);
}

float FastYin::getPitch() {
    if (m_audioBuffer.size() == 0)
        return 0.0f;

    int tauEstimate = -1;
    float pitchHz = -1.0f;

    // step 2
    // difference(); // See comments in header - less accurate
    differencePowerOf2();

    // step 3
    cumulativeMeanNormalizedDifference();

    // step 4
    tauEstimate = absoluteThreshold();

    // step 5
    if (tauEstimate != -1) {
        const float betterTau = parabolicInterpolation(tauEstimate);

        // step 6
        // TODO Implement optimization for the AUBIO_YIN algorithm.
        // 0.77% => 0.5% error rate,
        // using the data of the YIN paper
        // bestLocalEstimate()

        // conversion to Hz
        pitchHz = m_sampleRate / betterTau;
    } else{
        // no pitch found
        pitchHz = -1.0f;
    }
    m_result.pitchHz = pitchHz;

    // TODO: Examine if whole result is needed, and return it here
    return pitchHz;
}

void FastYin::difference() {
    // POWER TERM CALCULATION
    // ... for the power terms in equation (7) in the Yin paper
    const int M = m_yinBuffer.size();
    const int signalLen = m_audioBuffer.size();
    QVector<float> powerTerms(M, 0.0f);
    for (int j = 0; j < m_yinBuffer.size(); ++j) {
        powerTerms[0] += m_audioBuffer[j] * m_audioBuffer[j];
    }
    // now iteratively calculate all others (saves a few multiplications)
    for (int tau = 1; tau < m_yinBuffer.size(); ++tau) {
        powerTerms[tau] = powerTerms[tau-1] - m_audioBuffer[tau-1] * m_audioBuffer[tau-1] + m_audioBuffer[tau + M] * m_audioBuffer[tau + M];
    }

    // YIN-STYLE AUTOCORRELATION via FFT
    // 1. data
    for (int j = 0; j < signalLen; ++j) {
        m_audioBufferFFT[2*j] = m_audioBuffer[j];
        m_audioBufferFFT[2*j+1] = 0;
    }
    auto audioBufferFFTBuffer = convertQVector(m_audioBufferFFT);
    fft(audioBufferFFTBuffer, audioBufferFFTBuffer.size());
    m_audioBufferFFT = convertVector(audioBufferFFTBuffer);

    // 2. half of the data, disguised as a convolution kernel
    for (int j = 0; j < M; ++j) {
        m_kernel[2*j] = m_audioBuffer[(M-1)-j];
        m_kernel[2*j+1] = 0;
        m_kernel[2*j+signalLen] = 0;
        m_kernel[2*j+signalLen+1] = 0;
    }
    auto kernelBuffer = convertQVector(m_kernel);
    fft(kernelBuffer, kernelBuffer.size());
    m_kernel = convertVector(kernelBuffer);

    // 3. convolution via complex multiplication
    for (int j = 0; j < signalLen; ++j) {
        m_yinStyleACF[2*j]   = m_audioBufferFFT[2*j]*m_kernel[2*j] - m_audioBufferFFT[2*j+1]*m_kernel[2*j+1]; // real
        m_yinStyleACF[2*j+1] = m_audioBufferFFT[2*j+1]*m_kernel[2*j] + m_audioBufferFFT[2*j]*m_kernel[2*j+1]; // imaginary
    }
    auto yinStyleACFBuffer = convertQVector(m_yinStyleACF);
    inverseFft(yinStyleACFBuffer, yinStyleACFBuffer.size());
    m_yinStyleACF = convertVector(yinStyleACFBuffer);

    //+ Normalize
    for (int i = 0; i < m_yinStyleACF.size(); ++i) {
        m_yinStyleACF[i] = m_yinStyleACF[i] / (yinStyleACFBuffer.size() / 2.0);
    }

    // CALCULATION OF difference function
    // ... according to (7) in the Yin paper.
    for (int j = 0; j < M; ++j) {
        // taking only the real part
        const int idx = 2 * (M - 1 + j);
        double acf_val = m_yinStyleACF[idx];
        m_yinBuffer[j] = powerTerms[0] + powerTerms[j] - 2.0 * acf_val;
    }
    return;

}

void FastYin::differencePowerOf2() {
    const int M = static_cast<int>(m_yinBuffer.size());  // number of lags we want (e.g. half window)
    const int signalLen = static_cast<int>(m_audioBuffer.size());  // original frame length, must be >= M * 2 typically

    // power terms: powerTerms[tau] = sum_{j=0..M-1} x[j+tau]^2  (we'll compute as sliding)
    std::vector<double> powerTerms(M, 0.0);
    double p0 = 0.0;
    for (int j = 0; j < M; ++j)
        p0 += m_audioBuffer[j] * m_audioBuffer[j];
    powerTerms[0] = p0;
    for (int tau = 1; tau < M; ++tau) {
        // ensure indices safe: audio[tau + M - 1] must exist
        powerTerms[tau] = powerTerms[tau - 1] - m_audioBuffer[tau - 1] * m_audioBuffer[tau - 1] +
                          m_audioBuffer[tau + M - 1] * m_audioBuffer[tau + M - 1];
    }

    // Prepare FFT length
    int N = nextPow2(signalLen + M);  // safe: >= signalLen + kernelLen -1; kernelLen = M
    // Use complex buffers of length N
    std::vector<std::complex<double>> A(N), B(N);

    // A = audio (real), zero-padded
    for (int i = 0; i < signalLen; ++i)
        A[i] = std::complex<double>(m_audioBuffer[i], 0.0);
    for (int i = signalLen; i < N; ++i)
        A[i] = 0.0;

    // B = reversed prefix of audio of length M (kernel for cross-correlation)
    // kernel[k] = audio[M-1 - k] for k=0..M-1, then zero pad
    for (int k = 0; k < M; ++k)
        B[k] = std::complex<double>(m_audioBuffer[M - 1 - k], 0.0);
    for (int k = M; k < N; ++k)
        B[k] = 0.0;

    // FFTW plans (in-place possible with separate arrays)
    // NOTE: It is not really c++ way and might have issues, but for most compilers this should work
    fftw_complex* fa = reinterpret_cast<fftw_complex*>(A.data());
    fftw_complex* fb = reinterpret_cast<fftw_complex*>(B.data());
    fftw_plan pa = fftw_plan_dft_1d(N, fa, fa, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_plan pb = fftw_plan_dft_1d(N, fb, fb, FFTW_FORWARD, FFTW_ESTIMATE);

    fftw_execute(pa);
    fftw_execute(pb);

    // Multiply elementwise: C = A * B
    std::vector<std::complex<double>> C(N);
    for (int k = 0; k < N; ++k) {
        C[k] = A[k] * B[k];
    }

    // IFFT C
    fftw_complex* fc = reinterpret_cast<fftw_complex *>(C.data());
    fftw_plan pc = fftw_plan_dft_1d(N, fc, fc, FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(pc);

    // normalize by N and extract real part => cross-correlation
    // after this, the lag tau correlation value is at index (M - 1 + tau)
    std::vector<double> corr(N);
    for (int k = 0; k < N; ++k) {
        corr[k] = C[k].real() / static_cast<double>(N);  // IMPORTANT: divide by N
    }

    // compute yin difference:
    for (int tau = 0; tau < M; ++tau) {
        int idx = (M - 1 + tau);
        double acf_val = corr[idx];  // autocorrelation at lag tau
        m_yinBuffer[tau] = powerTerms[0] + powerTerms[tau] - 2.0 * acf_val;
    }

    // cleanup plans
    fftw_destroy_plan(pa);
    fftw_destroy_plan(pb);
    fftw_destroy_plan(pc);
}

void FastYin::cumulativeMeanNormalizedDifference() {
    m_yinBuffer[0] = 1;
    float runningSum = 0;
    for (int tau = 1; tau < m_yinBuffer.size(); ++tau) {
        runningSum += m_yinBuffer[tau];
        m_yinBuffer[tau] *= tau / runningSum;
    }
}

int FastYin::absoluteThreshold() {
    // Uses another loop construct
    // than the AUBIO implementation
    int tau;
    // first two positions in yinBuffer are always 1
    // So start at the third (index 2)
    for (tau = 2; tau < m_yinBuffer.size(); tau++) {
        if (m_yinBuffer[tau] < m_threshold) {
            while (tau + 1 < m_yinBuffer.size() && m_yinBuffer[tau + 1] < m_yinBuffer[tau]) {
                tau++;
            }
            // found tau, exit loop and return
            // store the probability
            // From the YIN paper: The threshold determines the list of
            // candidates admitted to the set, and can be interpreted as the
            // proportion of aperiodic power tolerated
            // within a periodic signal.
            //
            // Since we want the periodicity and and not aperiodicity:
            // periodicity = 1 - aperiodicity
            m_result.probability = 1 - m_yinBuffer[tau];
            break;
        }
    }

    // if no pitch found, tau => -1
    if (tau == m_yinBuffer.size() || m_yinBuffer[tau] >= m_threshold || m_result.probability > 1.0) {
        tau = -1;
        m_result.probability = 0.0f;
        m_result.pitched = false;
    } else {
        m_result.pitched = true;
    }

    return tau;
}

float FastYin::parabolicInterpolation(int tauEstimate) {
    float betterTau;
    int x0;
    int x2;

    if (tauEstimate < 1) {
        x0 = tauEstimate;
    } else {
        x0 = tauEstimate - 1;
    }
    if (tauEstimate + 1 < m_yinBuffer.size()) {
        x2 = tauEstimate + 1;
    } else {
        x2 = tauEstimate;
    }
    if (x0 == tauEstimate) {
        if (m_yinBuffer[tauEstimate] <= m_yinBuffer[x2]) {
            betterTau = tauEstimate;
        } else {
            betterTau = x2;
        }
    } else if (x2 == tauEstimate) {
        if (m_yinBuffer[tauEstimate] <= m_yinBuffer[x0]) {
            betterTau = tauEstimate;
        } else {
            betterTau = x0;
        }
    } else {
        float s0, s1, s2;
        s0 = m_yinBuffer[x0];
        s1 = m_yinBuffer[tauEstimate];
        s2 = m_yinBuffer[x2];
        // fixed AUBIO implementation, thanks to Karl Helgason:
        // (2.0f * s1 - s2 - s0) was incorrectly multiplied with -1
        betterTau = tauEstimate + (s2 - s0) / (2 * (2 * s1 - s2 - s0));
    }
    return betterTau;
}
