/*
 * Copyright (c) 2016 Denis Steckelmacher
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "gaussianmixture.h"

#include <algorithm>
#include <cmath>
#include <iostream>

#include <alloca.h>
#include <assert.h>

#define TMP(name, size) \
    float *name##_data = (float *)(((intptr_t)alloca((size) * sizeof(float) + 16) + 16) & ~0xF); \
    Eigen::Map<Eigen::VectorXf, Eigen::Aligned> name(name##_data, size);

#define TWOPI 6.283185307179586f
#define LOG_070 0.3566749439387324f

/* Util */
static float safe_exp(float x)
{
    if (x < -40.0f) {
        //return 4.24835425529158899533e-18f;
        return 0.0f;
    } else if (x > 40.0f) {
        return 2.35385266837019985408e17f;
    } else {
        return std::exp(x);
    }
}

/* GaussianMixture */
GaussianMixture::GaussianMixture(unsigned int input_dim,
                 unsigned int output_dim,
                 float initial_variance,
                 float max_error,
                 gmm_settings_t &settings)
: _input_dim(input_dim),
  _output_dim(output_dim),
  _initial_variance(initial_variance),
  _max_error(max_error),
  _settings(settings)
{
}

GaussianMixture::~GaussianMixture()
{
    for (Neuron *neuron : _neurons) {
        delete neuron;
    }
}

size_t GaussianMixture::numClusters() const
{
    return _neurons.size();
}

void GaussianMixture::setValue(const Eigen::VectorXf &input, const Eigen::VectorXf &value)
{
    // Maintain the min/max ranges
    if (_neurons.size() == 0) {
        _min_in = input;
        _max_in = input;
        _min_out = value;
        _max_out = value;
    } else {
        _min_in = _min_in.cwiseMin(input);
        _max_in = _max_in.cwiseMax(input);
        _min_out = _min_out.cwiseMin(value);
        _max_out = _max_out.cwiseMax(value);
    }
    //printf("o_mini: <%.2f %.2f> | o_maxi: <%.2f %.2f> | o_mino: <%.2f %.2f> | o_maxo: <%.2f %.2f>\n",
//           _min_in[0], _min_in[1], _max_in[0], _max_in[1],
//           _min_out[0], _min_out[1], _max_out[0], _max_out[1]);

    // Activate all the neurons on the input and output
    float sum_of_in_probas = 0.0f;

    for (unsigned int i=0; i<_neurons.size(); ++i) {
        Neuron *neuron = _neurons[i];

        neuron->computeProbabilityOfIn(input);

        sum_of_in_probas += neuron->inProba();
    }

    // Compute the predicted input and output
    Eigen::VectorXf b = Eigen::VectorXf::Zero(_output_dim);
    float min_squared_mahalanobis_distance = _settings.max_distance;

    //printf("o_numc: %d | o_totp: %f\n", _neurons.size(), sum_of_in_probas);
    for (unsigned int i=0; i<_neurons.size(); ++i) {
        Neuron *neuron = _neurons[i];

        neuron->computeProbabilityCond(sum_of_in_probas);
        neuron->contributeToOutput(b, input);

        min_squared_mahalanobis_distance =
            std::min(min_squared_mahalanobis_distance, neuron->squaredMahalanobisDistance());
    }

    // Compute the maximum component-wise error using the input and the output
    float max_error = 0.0f;
    size_t len = _neurons.size();
    //printf("o_outp: <%.2f %.2f>\n", b[0], b[1]);
    Eigen::VectorXf min_max_delta = _max_out - _min_out, error_vec = (value - b).cwiseAbs().cwiseQuotient(_max_out - _min_out + Eigen::VectorXf::Constant(value.rows(), _settings.fallback_max_error));
    //printf("o_mmdt: <%.2f %.2f> | o_vodt: <%.2f %.2f> | a:%.2f b:%.2f\n",min_max_delta[0], min_max_delta[1], (value - b)[0], (value - b)[1], error_vec[0], error_vec[1]);

//    printf("o_mmdl: <%.3f %.3f> | <%.3f %.3f> | <%.3f %.3f>\n", _max_out[0], _max_out[1], _min_out[0], _min_out[1], min_max_delta[0], min_max_delta[1]);
    max_error = (value - b).cwiseAbs().cwiseQuotient(_max_out - _min_out + Eigen::VectorXf::Constant(value.rows(), _settings.fallback_max_error)).maxCoeff();

//    printf("o_maxe: %.2f | o_best: %.2f\n", max_error, min_squared_mahalanobis_distance);
    
    // Create a neuron if needed. Don't create a neuron if the input is already
    // close to the center of an existing neuron, as update will take care of
    // improving the result.
    //
    // min_distance > -2*ln(0.5), with 0.5 the minimum probability for update
    // to be preferred.
    if (_neurons.size() < 1 || (max_error > _max_error && min_squared_mahalanobis_distance > _settings.max_mahalanobis_sq)) {
        _neurons.push_back(new Neuron(input, value, _initial_variance, &_settings));
    }

    // Update all the neurons (except the last one if it has just been created)
    for (unsigned int i=0; i<len; ++i) {
        Neuron *neuron = _neurons[i];

        // Update the neuron
        neuron->update(input, value);
    }

    // Remove unused neurons
    for (unsigned int i=0; i<len; ++i) {
        Neuron *neuron = _neurons[i];

        // Remove the neuron if it is not needed anymore
        if (neuron->_score < _settings.min_cluster_score) {
            delete neuron;

            _neurons.erase(_neurons.begin() + i);
            --i;
            --len;
        }
    }
}

Eigen::VectorXf GaussianMixture::value(const Eigen::VectorXf &input,
                                       float &weight) const
{
    // Activate all the neurons on the input. Their output probability is set
    // to one because no output is provided.
    weight = 0.0f;

    for (unsigned int i=0; i<_neurons.size(); ++i) {
        Neuron *neuron = _neurons[i];

        neuron->computeProbabilityOfIn(input);

        weight += _neurons[i]->inProba();
    }

    // The conditional probability of each neuron can now be computed
    Eigen::VectorXf rs = Eigen::VectorXf::Zero(_output_dim);

    for (unsigned int i=0; i<_neurons.size(); ++i) {
        Neuron *neuron = _neurons[i];

        neuron->computeProbabilityCond(weight);
        neuron->contributeToOutput(rs, input);
    }

    return rs;
}

/* Neuron */
GaussianMixture::Neuron::Neuron(const Eigen::VectorXf &input,
                        const Eigen::VectorXf &output,
                        float initial_variance,
                        gmm_settings_t * settings)
:
_settings(settings)
{
    _score = 1.0f;

    // Initialize the means and covariances
    float inv_variance = 1.0f / initial_variance;

    _mean_in = input;
    _mean_out = output;

    _covariance_in_out = Eigen::MatrixXf::Zero(input.rows(), output.rows());
    _covariance_in = initial_variance * Eigen::MatrixXf::Identity(input.rows(), input.rows());
    _inv_covariance_in = inv_variance * Matrix::Identity(input.rows(), input.rows());

    //printf("o_llt: [%.2f %.2f | %.2f %.2f] ", _covariance_in(0,0),_covariance_in(0,1), _covariance_in(1,0),_covariance_in(1,1));
    _in_llt.compute(_covariance_in);
    //printf("[%.2f %.2f | %.2f %.2f]\n", _in_llt.matrixLLT()(0,0),_in_llt.matrixLLT()(0,1), _in_llt.matrixLLT()(1,0),_in_llt.matrixLLT()(1,1));

    // Ensure that the gaussian normalizations are ready to be used
    updateInGaussianNorm();
}

void GaussianMixture::Neuron::updateInGaussianNorm()
{
    // norm = 1/(2*pi*det(covariance))^(D/2)
    // log(norm) = -log((2*pi*det(covariance))^(D/2)).
    //
    // det(covariance) is computed from a Choleski decomposition of _covariance_in
    // and is equal to prod(diag(choleski.matrixL))
    //
    // log(norm) = -log(prod(sqrt(2*pi*diag(choleski.matrixL))))
    //           = -sum(log(sqrt(2*pi*diag(choleski.matrixL))))
    //printf("o_norm: [%.2f %.2f | %.2f %.2f]", _in_llt.matrixLLT()(0,0),_in_llt.matrixLLT()(0,1), _in_llt.matrixLLT()(1,0),_in_llt.matrixLLT()(1,1));
    _log_input_gaussian_normalization = -1.0f * (TWOPI * _in_llt.matrixLLT().diagonal()).array().cwiseSqrt().log().sum();
    //printf(" %.2f %.2f\n",
//           _in_llt.matrixLLT().diagonal()[0]*_in_llt.matrixLLT().diagonal()[1],
//           _log_input_gaussian_normalization);
}

void GaussianMixture::Neuron::computeProbabilityOfIn(const Eigen::VectorXf &input)
{
    TMP(delta, input.rows());
    TMP(cov_times_delta, input.rows());

    delta = input - _mean_in;

    cov_times_delta.setZero();
    cov_times_delta.noalias() += _inv_covariance_in * delta;

    // Compute the normal distribution and mahalanobis distance
    _square_mahalanobis_distance = delta.dot(cov_times_delta);

    _probability_of_in = safe_exp(
        _log_input_gaussian_normalization - 0.5 * _square_mahalanobis_distance
    );
    //printf("o_prin: %f | o_masq: %.2f | o_lgnf: %.2f | o_icov: [ %.2f %.2f | %.2f %.2f ] | o_indt: < %.2f %.2f>\n",
//           _probability_of_in, _square_mahalanobis_distance, _log_input_gaussian_normalization,
//           _inv_covariance_in(0,0), _inv_covariance_in(0,1), _inv_covariance_in(1,0), _inv_covariance_in(1,1),
//           delta[0], delta[1] );
}

float GaussianMixture::Neuron::inProba() const
{
    return _probability_of_in;
}

float GaussianMixture::Neuron::squaredMahalanobisDistance() const
{
    return _square_mahalanobis_distance;
}

void GaussianMixture::Neuron::computeProbabilityCond(float sum_of_in_probas)
{
    if (sum_of_in_probas < 1e-15f) {
        _probability_cond_in = 0.0f;
    } else {
        _probability_cond_in = inProba() / sum_of_in_probas;
    }
    //printf("o_sipr: %f | o_prci: %f\n", sum_of_in_probas, _probability_cond_in);
}

void GaussianMixture::Neuron::contributeToOutput(Eigen::VectorXf &output,
                                                 const Eigen::VectorXf &input)
{
    if (_probability_cond_in > _settings->small_value_error_offset) {
        //printf("o_cscr: %f\n", _probability_cond_in);
        TMP(delta, input.rows());
        TMP(cov_times_delta, input.rows());

        delta = input - _mean_in;
        cov_times_delta.setZero();
        cov_times_delta.noalias() += _inv_covariance_in * delta;

        output.noalias() += _probability_cond_in * (
            _mean_out + _covariance_in_out.transpose() * cov_times_delta
        );
    }
}

void GaussianMixture::Neuron::update(const Eigen::VectorXf &input,
                                     const Eigen::VectorXf &output)
{
    // Don't perform updates that are too small to be relevant
    // distance > -2 * log(1e-2), 1e-2 is p(x|c) under which clusters are
    // not updated
//    printf("o_maha: %.2f\n", _square_mahalanobis_distance);
    if (_square_mahalanobis_distance > _settings->max_mahalanobis_sq_for_update) {
//        printf("o_invm: %.2f\n", _square_mahalanobis_distance);
        return;
    }

    // Update the score of the neuron. The weight of the update must already
    // be very small when distance = 1.386 (P(x|c) = 0.50), hence the large
    // multiplication.
    float score_weight = _settings->alpha * std::exp(-_settings->beta * _square_mahalanobis_distance);

    _score += score_weight * (_probability_cond_in - _score);

    // Update the means and variances
    float weight = _settings->alpha * _probability_cond_in;
    TMP(delta_mean_in, input.rows());
    TMP(delta_mean_out, output.rows());

    delta_mean_in = input - _mean_in;
    delta_mean_out = output - _mean_out;

    _mean_in.noalias() += weight * delta_mean_in;
    _mean_out.noalias() += weight * delta_mean_out;
    
    //printf("o_mean: [%.2f %.2f] | o_dmin: <%.2f %.2f> | o_dmou: <%.2f %.2f>\n", _mean_in[0], _mean_in[1], delta_mean_in[0], delta_mean_in[1], delta_mean_out[0], delta_mean_out[1]);
    Eigen::MatrixXf covf = delta_mean_in * delta_mean_in.transpose();
    //printf("o_covf: <%.2f %.2f> [%.2f %.2f | %.2f %.2f] | o_weig: %.2f\n", delta_mean_in[0], delta_mean_in[1], covf(0,0),covf(0,1),covf(1,0),covf(1,1), weight);

    // Prevent the covarance matrices from being too small (having values less than 1e-4)
    //printf("o_cvin: [%.2f %.2f | %.2f %.2f]\n", _covariance_in(0,0),_covariance_in(0,1), _covariance_in(1,0),_covariance_in(1,1));
    _covariance_in +=  weight * (delta_mean_in * delta_mean_in.transpose() + 1e-4f * Eigen::MatrixXf::Identity(input.rows(), input.rows()) - _covariance_in);
    //printf("o_llt: [%.2f %.2f | %.2f %.2f] ", _covariance_in(0,0),_covariance_in(0,1), _covariance_in(1,0),_covariance_in(1,1));
    _in_llt.compute(_covariance_in);
    //printf("[%.2f %.2f | %.2f %.2f]\n", _in_llt.matrixLLT()(0,0),_in_llt.matrixLLT()(0,1), _in_llt.matrixLLT()(1,0),_in_llt.matrixLLT()(1,1));
    _inv_covariance_in = _in_llt.solve(Eigen::MatrixXf::Identity(input.rows(), input.rows()));

    // Update the main covariance matrix
    _covariance_in_out +=
        weight * (delta_mean_in * delta_mean_out.transpose() - _covariance_in_out);

    // Update gaussian normalizations
    updateInGaussianNorm();
}
