#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "qubit.h"

#define DEFAULT_ITEMS 10
#define DEFAULT_CAPACITY 50
#define DEFAULT_GENERATIONS 100
#define POPULATION_SIZE 10

/*
 * Structure representing an item in the knapsack problem.
 * It has a weight and a value associated with it.
 */
struct item {
    int weight;
    int value;
};

struct item *items; // Array of items

int chromosome_len; // Length of the chromosome

struct solution {
    int *bits; // Binary representation of a solution
    int fitness; // Fitness value of the solution
};

qubit *population[POPULATION_SIZE]; // Array of qubit populations
struct solution solutions[POPULATION_SIZE]; // Array of solutions

/**
 * Initializes the items array with random weight and value for each item.
 * @param num_items: Number of items
 * @param capacity: Capacity of the knapsack
 */
void init_items(int num_items, int capacity) {
    items = (struct item*) malloc(num_items * sizeof(struct item));
    for (int i = 0; i < num_items; i++) {
        items[i].weight = rand() % (capacity / 2) + 1;
        items[i].value = rand() % 10 + 1;
    }

    printf("Knapsack Problem:\n");
    printf("Number of Items: %d Capacity: %d\n", num_items, capacity);
    printf("Item\tValue\tWeight\n");

    for (int i = 0; i < num_items; ++i) {
        printf("%d\t%d\t%d\n", i+1, items[i].value, items[i].weight);
    }
}

/**
 * Prints the population.
 *
 * @param generation The current generation number.
 * @param verbose Flag indicating whether to print verbose output.
 */
void print_population(int generation, int fitness, int verbose) {
    if (verbose) {
        printf("Generation: %d\n", generation);
        printf("Fitness: %d\n", fitness);

        for (int i = 0; i < POPULATION_SIZE; ++i) {
            printf("Chromosome: [");
            for (int j = 0; j < chromosome_len; ++j) {
                printf("%.5f + %.5fj", population[i][j].alpha, population[i][j].beta);
                if (j != chromosome_len - 1) printf(", ");
            }
            printf("]\n");
        }
        printf("\n");
    }
}

/**
 * Initializes the population of qubits with equal superposition of states.
 */
void init_population() {
    for (int i = 0; i < POPULATION_SIZE; i++) {
        population[i] = (qubit*) malloc(chromosome_len * sizeof(qubit));
        solutions[i].bits = malloc(chromosome_len * sizeof(int));
        for (int j = 0; j < chromosome_len; j++) {
            qubit_init(&population[i][j], 1 / sqrt(2), 1 / sqrt(2));
        }
    }
}

/**
 * Generates solutions by measuring each qubit in the population.
 */
void make_solutions() {
    for (int i = 0; i < POPULATION_SIZE; i++) {
        for (int j = 0; j < chromosome_len; j++) {
            solutions[i].bits[j] = qubit_measure(&population[i][j]);
        }
    }
}

/**
 * Checks if the knapsack is overfilled based on the selected items.
 * @param bits: Binary representation of selected items
 * @param num_items: Number of items
 * @param capacity: Capacity of the knapsack
 * @return 1 if the knapsack is overfilled, 0 otherwise
 */
int knapsack_overfilled(int *bits, int num_items, int capacity) {
    int total_weight = 0;
    for (int i = 0; i < num_items; i++) {
        total_weight += bits[i] * items[i].weight;
    }
    return total_weight > capacity;
}

/**
 * Repairs solutions by modifying the selected items to fit within the knapsack capacity.
 * @param num_items: Number of items
 * @param capacity: Capacity of the knapsack
*/
void repair_solutions(int num_items, int capacity) {
    for (int i = 0; i < POPULATION_SIZE; i++) {

        int * bits = solutions[i].bits;
        while (knapsack_overfilled(bits, num_items, capacity)) {
            bits[rand() % num_items] = 0;
        }
    }
}

/**
 * Evaluates the fitness of each solution in the population.
 * @return Index of the fittest solution in the population
 */
int evaluate_fitness() {
    
    int fittest = 0;
    int best_fitness = 0;
    for (int i = 0; i < POPULATION_SIZE; i++) {
        int total_value = 0;    
        for (int j = 0; j < chromosome_len; j++) {
            total_value += solutions[i].bits[j] * items[j].value;
        }
        solutions[i].fitness = total_value;

        if (solutions[i].fitness > best_fitness) {
            best_fitness = solutions[i].fitness;
            fittest = i;
        }
    }
    return fittest;
}


/*
 * Lookup table used to calculate theta in the update_population function.
 * The table is indexed by xi, bi, f(x) >= f(b), and provides the values for Δθ and s(αiβi).
 *
 * | xi | bi | f(x) >= f(b) |  Δθ  |                s(αiβi)                |
 * |    |    |              |      | αiβi > 0 | αiβi < 0 | αi = 0 | βi = 0 |
 * |----|----|--------------|------|---------------------------------------|
 * | 0  | 0  |      0       |π/1000|   -1     |    1     |   1    |  -1    |
 * | 0  | 0  |      1       |π/1000|   -1     |    1     |  -1    |   1    |
 * | 0  | 1  |      0       |2π/25 |   -1     |    1     |   1    |  -1    |
 * | 0  | 1  |      1       |π/1000|   -1     |    1     |  -1    |   1    |
 * | 1  | 0  |      0       |2π/25 |    1     |   -1     |   1    |  -1    |
 * | 1  | 0  |      1       |π/1000|    1     |   -1     |  -1    |   1    |
 * | 1  | 1  |      0       |π/1000|    1     |   -1     |  -1    |   1    |
 * | 1  | 1  |      1       |π/1000|    1     |   -1     |  -1    |   1    |
*/
double lookup_table[2][2][2][5] = {
    {
        { {0.001*M_PI, -1, 1, 1, -1}, {0.001*M_PI, -1, 1, -1, 1} },
        { {0.08*M_PI, -1, 1, 1, -1}, {0.001*M_PI, -1, 1, -1, 1} }
    },
    {
        { {0.08*M_PI, 1, -1, 1, -1}, {0.001*M_PI, 1, -1, -1, 1} },
        { {0.001*M_PI, 1, -1, -1, 1}, {0.001*M_PI, 1, -1, -1, 1} }
    }
};

/**
 * Updates the population of qubits based on the best solution.
 * @param best_solution: Binary representation of the best solution
 */
void update_population(int *best_solution) {
    int best_fitness = 0;

    // Calculate the fitness of the best solution
    for (int i = 0; i < chromosome_len; i++) {
        best_fitness += best_solution[i] * items[i].value;
    }

    // Update the population
    for (int i = 0; i < POPULATION_SIZE; ++i) {
        for (int j = 0; j < chromosome_len; ++j) {
            int xi = solutions[i].bits[j];
            int bi = best_solution[j];

            // Lookup the appropriate theta value based on the conditions
            int fitness_condition = solutions[i].fitness >= best_fitness;
            double theta = lookup_table[xi][bi][fitness_condition][0];

            double alpha = population[i][j].alpha;
            double beta = population[i][j].beta;

            // Adjust theta based on the alpha and beta values
            if (alpha == 0) {
                theta *= lookup_table[xi][bi][fitness_condition][3];
            } else if (beta == 0) {
                theta *= lookup_table[xi][bi][fitness_condition][4];
            } else if (alpha * beta < 0) {
                theta *= lookup_table[xi][bi][fitness_condition][1];
            } else {
                theta *= lookup_table[xi][bi][fitness_condition][2];
            }

            // Perform the qubit rotation
            qubit_rotate(&population[i][j], theta);
        }
    }
}

/**
 * Applies the quantum genetic algorithm to solve the knapsack problem.
 *
 * @param max_generations The number of generations
 * @param num_items The number of items in the knapsack problem.
 * @param capacity The capacity of the knapsack.
 * @param verbose Flag indicating whether to print verbose output.
 */
void quantum_genetic_algorithm(int max_generations, int num_items, int capacity, int verbose) {

    chromosome_len = num_items;
    int generation = 0;

    init_population();
    make_solutions();
    repair_solutions(num_items, capacity);

    int fittest = evaluate_fitness();
    int best_solution[num_items];
    int best_fitness = solutions[fittest].fitness;
    memcpy(best_solution, solutions[fittest].bits, sizeof(best_solution));
    
    while (generation < max_generations) {

        print_population(generation, best_fitness, verbose);

        make_solutions();
        repair_solutions(num_items, capacity);

        fittest = evaluate_fitness();
        update_population(best_solution);
        if (solutions[fittest].fitness > best_fitness) {
            memcpy(best_solution, solutions[fittest].bits, sizeof(best_solution));
            best_fitness = solutions[fittest].fitness;
        }

        generation++;
    }

    printf("\nOptimized Solution:\n");
    printf("Value: %d\n", best_fitness);
    printf("Items Included: ");

    int weight = 0;
    for (int i = 0; i < num_items; ++i) {
        weight += best_solution[i] * items[i].weight;
        if (best_solution[i] == 1) {
            printf("%d ", i+1);
        }
    }

    printf("\nWeight: %d\n", weight);
    printf("\n");
}

/**
 * Computes every possible solution to solve the knapsack problem.
 *
 * @param num_items The number of items in the knapsack problem.
 * @param capacity The capacity of the knapsack.
 */
void brute_force(int num_items, int capacity) {

    if (num_items > 30) {
        printf("Too large to brute force\n");
        return;
    }

    int max_value = 0;
    int weight = 0;
    int best_solution[num_items];
    
    // Generate all possible combinations
    for (int i = 0; i < (1 << num_items); ++i) {
        int current_value = 0;
        int current_weight = 0;
        int current_solution[num_items];
        
        // Compute the value and weight for the current combination
        for (int j = 0; j < num_items; ++j) {
            if ((i >> j) & 1) {
                current_value += items[j].value;
                current_weight += items[j].weight;
                current_solution[j] = 1;
            } else {
                current_solution[j] = 0;
            }
        }
        
        // Check if the current combination is feasible and has higher value
        if (current_weight <= capacity && current_value > max_value) {
            max_value = current_value;
            weight = current_weight;
            memcpy(best_solution, current_solution, num_items * sizeof(int));
        }
    }
    
    // Print the best solution found
    printf("Brute Force Solution:\n");
    printf("Value: %d\n", max_value);
    printf("Weight: %d\n", weight);
    printf("Items Included: ");
    
    for (int i = 0; i < num_items; ++i) {
        if (best_solution[i] == 1) {
            printf("%d ", i + 1);
        }
    }
    printf("\n");
}

// Usage information
void display_help() {
    printf("Usage: ./qga [options]\n");
    printf("Options:\n");
    printf("  -n <num_items>         Number of items (default: %d)\n", DEFAULT_ITEMS);
    printf("  -c <capacity>          Knapsack capacity (default: %d)\n", DEFAULT_CAPACITY);
    printf("  -g <num_generations>   Number of generations (default: %d)\n", DEFAULT_GENERATIONS);
    printf("  -v                     Enable verbose mode\n");
    printf("  -h, --help             Display this help message\n");
}

// Main method
int main(int argc, char *argv[]) {

    // Default values
    int num_items = DEFAULT_ITEMS;
    int capacity = DEFAULT_CAPACITY;
    int num_generations = DEFAULT_GENERATIONS;
    int verbose = 0;

    // Define long options
    static struct option long_options[] = {
        {"num-items", required_argument, 0, 'n'},
        {"capacity", required_argument, 0, 'c'},
        {"num-generations", required_argument, 0, 'g'},
        {"verbose", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    // Parse command line arguments
    int option;
    int option_index = 0;
    while ((option = getopt_long(argc, argv, "n:c:g:vh", long_options, &option_index)) != -1) {
        switch (option) {
            case 'n':
                num_items = atoi(optarg);
                break;
            case 'c':
                capacity = atoi(optarg);
                break;
            case 'g':
                num_generations = atoi(optarg);
                break;
            case 'v':
                verbose = 1;
                break;
            case 'h':
                display_help();
                return 0;
            case '?':
                fprintf(stderr, "Unknown option.\n");
                return 1;
            default:
                return 1;
        }
    }

    if (num_items <= 0) {
        fprintf(stderr, "Invalid number of items.\n");
        return 1;
    }

    if (capacity <= 0) {
        fprintf(stderr, "Invalid capacity.\n");
        return 1;
    }

    srand(time(NULL));

    // Initialize items
    init_items(num_items, capacity);
    
    // Run the quantum genetic algorithm
    quantum_genetic_algorithm(num_generations, num_items, capacity, verbose);

    brute_force(num_items, capacity);

    return 0;
}
