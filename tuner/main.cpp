#include "tuner.h"

int main()
{
    chess::init();

    // Load dataset
    auto dataset = get_dataset("dataset/data.txt");

    // Load initial weights
    auto weights = get_init_weights();

    std::cout << "Init weights: " << std::endl;

    print_weights(weights);

    // Get optimal K value
    // auto K = get_optimal_K(dataset);
    auto K = 2.5;

    // Get loss
    auto loss = get_mse(dataset, weights, K);

    std::cout << "Init loss: " << std::to_string(loss) << std::endl;

    // Tune configs
    f64 beta_1 = 0.9;
    f64 beta_2 = 0.999;
    f64 lr = 0.1;
    f64 lr_drop_rate = 1.0;

    usize epoch_max = 5000;
    usize lr_drop_interval = 200;

    std::vector<Pair> momentum(weights.size(), { 0.0 , 0.0 });
    std::vector<Pair> velocity(weights.size(), { 0.0 , 0.0 });

    // Tune
    for (usize epoch = 0; epoch < epoch_max; ++epoch) {
        // Get gradient
        auto gradient = get_gradient(dataset, weights, K);

        // Update weights
        for (usize i = 0; i < weights.size(); ++i) {
            f64 gradient_mg = -K / 400.0 * gradient[i][MG] / f64(dataset.data.size());
            f64 gradient_eg = -K / 400.0 * gradient[i][EG] / f64(dataset.data.size());

            momentum[i][MG] = beta_1 * momentum[i][MG] + (1.0 - beta_1) * gradient_mg;
            momentum[i][EG] = beta_1 * momentum[i][EG] + (1.0 - beta_1) * gradient_eg;

            velocity[i][MG] = beta_2 * velocity[i][MG] + (1.0 - beta_2) * std::pow(gradient_mg, 2);
            velocity[i][EG] = beta_2 * velocity[i][EG] + (1.0 - beta_2) * std::pow(gradient_eg, 2);

            weights[i][MG] -= lr * momentum[i][MG] / (1e-8 + std::sqrt(velocity[i][MG]));
            weights[i][EG] -= lr * momentum[i][EG] / (1e-8 + std::sqrt(velocity[i][EG]));
        }

        // Drop learning rate
        if ((epoch % lr_drop_interval) == 0) {
            lr *= lr_drop_rate;
        }

        // Print
        loss = get_mse(dataset, weights, K);
    
        std::cout << "epoch: " << std::to_string(epoch) << " - ";
        std::cout << "loss: " << std::to_string(loss) << " - ";
        std::cout << "lr: " << std::to_string(lr) << "\n";
        
        // Save check point
        if ((epoch % 10) == 0) {
            auto file = std::ofstream("checkpoint.txt", std::ios::out);

            file << get_str_print_weights(weights);
        }
    }

    // Print final weights
    print_weights(weights);

    return 0;
};