#pragma once

#include "dataset.h"

constexpr usize THREADS = 8;

inline f64 get_linear(const Entry& entry, const std::vector<Pair>& weights)
{
    assert(entry.coefs.size() == weights.size());

    f64 mg = entry.delta[MG];
    f64 eg = entry.delta[EG];

    for (usize i = 0; i < entry.coefs.size(); ++i) {
        mg += entry.coefs[i] * weights[i][MG];
        eg += entry.coefs[i] * weights[i][EG];
    }

    f64 score = (mg * entry.phase + eg * entry.scale * (24.0 - entry.phase)) / 24.0;

    if (entry.is_white) {
        score += f64(TEMPO);
    }
    else {
        score -= f64(TEMPO);
    }

    return score;
};

inline f64 get_sigmoid(f64 score, f64 K)
{
    return 1.0 / (1.0 + std::exp(-K * score / 400.0));
}

inline f64 get_mse(const Dataset& dataset, const std::vector<Pair>& weights, f64 K)
{
    f64 total = 0.0;

    usize workload = dataset.data.size() / THREADS;

    std::vector<std::thread> threads;
    std::mutex mtx;

    for (usize tid = 0; tid < THREADS; ++tid) {
        threads.emplace_back([&] (usize id) {
            f64 thread_total = 0.0;

            for (usize i = 0; i < workload; ++i) {
                const auto& entry = dataset.data[id * workload + i];

                f64 score = get_linear(entry, weights);
                f64 delta = entry.wdl - get_sigmoid(score, K);

                thread_total += delta * delta;
            }

            std::lock_guard<std::mutex> lk(mtx);

            total += thread_total;
        }, tid);
    }

    for (auto& t : threads) {
        t.join();
    }

    for (usize i = THREADS * workload; i < dataset.data.size(); ++i) {
        const auto& entry = dataset.data[i];

        f64 score = get_linear(entry, weights);
        f64 delta = entry.wdl - get_sigmoid(score, K);

        total += delta * delta;
    }

    return total / f64(dataset.data.size());
};

inline f64 get_optimal_K(const Dataset& dataset)
{
    std::cout << "Calculating optimal K..." << std::endl;

    f64 rate = 10.0;
    f64 delta = 1e-5;
    f64 deviation_goal = 1e-6;

    f64 K = 2.5;
    f64 deviation = 1.0;

    auto weights = get_init_weights();

    usize i = 0;

    while (std::fabs(deviation) > deviation_goal) {
        double up = get_mse(dataset, weights, K + delta);
        double down = get_mse(dataset, weights, K - delta);

        deviation = (up - down) / (2 * delta);

        K -= deviation * rate;

        i += 1;

        if ((i % 100) == 0) {
            std::cout << "\rK: " << std::to_string(K) << " - deviation: " << std::to_string(deviation);
        }
    }

    std::cout << std::endl;
    std::cout << "Final K: " << std::to_string(K) << std::endl;

    return K;
};

inline void update_gradient(std::vector<Pair>& gradient, const Entry& entry, const std::vector<Pair>& weights, f64 K)
{
    f64 score = get_linear(entry, weights);
    f64 sigmoid = get_sigmoid(score, K);
    f64 x = (entry.wdl - sigmoid) * sigmoid * (1.0 - sigmoid);

    f64 mg_base = x * entry.phase / 24.0;
    f64 eg_base = x * (1.0 - entry.phase / 24.0);

    for (usize i = 0; i < entry.coefs.size(); ++i) {
        f64 coef = f64(entry.coefs[i]);

        gradient[i][MG] += mg_base * coef;
        gradient[i][EG] += eg_base * coef * entry.scale;
    }
};

inline std::vector<Pair> get_gradient(const Dataset& dataset, const std::vector<Pair>& weights, f64 K)
{
    std::vector<Pair> gradient(weights.size(), { 0.0, 0.0 });

    usize workload = dataset.data.size() / THREADS;

    std::vector<std::thread> threads;
    std::mutex mtx;

    for (usize tid = 0; tid < THREADS; ++tid) {
        threads.emplace_back([&] (usize id) {
            std::vector<Pair> thread_gradient(weights.size(), { 0.0, 0.0 });

            for (usize i = 0; i < workload; ++i) {
                const auto& entry = dataset.data[id * workload + i];

                update_gradient(thread_gradient, entry, weights, K);
            }

            std::lock_guard<std::mutex> lk(mtx);

            for (usize i = 0; i < weights.size(); ++i) {
                gradient[i][MG] += thread_gradient[i][MG];
                gradient[i][EG] += thread_gradient[i][EG];
            }
        }, tid);
    }

    for (auto& t : threads) {
        t.join();
    }

    for (usize i = THREADS * workload; i < dataset.data.size(); ++i) {
        const auto& entry = dataset.data[i];

        update_gradient(gradient, entry, weights, K);
    }

    return gradient;
};