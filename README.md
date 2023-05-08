# Quantum Genetic Algorithm for Knapsack Problem

This project implements a Quantum Genetic Algorithm (QGA) to solve the Knapsack Problem. The QGA combines principles from quantum computing and genetic algorithms to find an optimal solution for the Knapsack Problem, where a set of items with different values and weights must be selected to maximize the total value while not exceeding a given capacity.

## Requirements

- C compiler (e.g., GCC)
- GNU Scientific Library (GSL)
- Python 3.X
- Flask

## Usage

1. Clone the repository:

   ```bash
   git clone https://github.com/bhvima/qga.git
   ```

2. Navigate to the project directory:

   ```bash
   cd qga
   ```
3. Build the project:

   ```bash
   make
   ```

4. Run the program with the desired parameters:
   ```bash
   ./qga -n <num_items> -c <capacity> -g <num_generations> -v
   ```

Replace `<num_items>`, `<capacity>`, `<num_generations>` with the desired values. The -v option enables verbose mode, which displays additional information during the execution.

5. (Optional) Clean the project:
   ```bash
   make clean
   ```

## Command Line Options

```
Usage: ./qga [options]
Options:
  -n <num_items>         Number of items (default: 10)
  -c <capacity>          Knapsack capacity (default: 50)
  -g <num_generations>   Number of generations (default: 100)
  -v                     Enable verbose mode
  -h, --help             Display this help message
```

## Visualization

Included in the package is a web application that allows users to visualize the evolution process. The application is built using Flask.

Run the binary in verbose mode and store its results to a file. 

```
./qga -n 50 -c 100 -g 1000 -v > results.txt
```

Then start the webserver using the resulting file.

```
python3 analyze.py results.txt
```

Open a web browser and navigate to http://127.0.0.1:5000 to access the application.

The web application provides a user interface with two sliders: one for selecting the generation and another for selecting the chromosome.


## References

* Kuk-Hyun Han and Jong-Hwan Kim, "Genetic quantum algorithm and its application to combinatorial optimization problem," Proceedings of the 2000 Congress on Evolutionary Computation. CEC00 (Cat. No.00TH8512), La Jolla, CA, USA, 2000, pp. 1354-1360 vol.2, doi: 10.1109/CEC.2000.870809.

* Laboudi, Zakaria & Chikhi, Salim. (2012). Comparison of Genetic Algorithm and Quantum Genetic Algorithm. International Arab Journal of Information Technology. 9.

* Sofge, D. A. (2008), “Prospective Algorithms for Quantum Evolutionary Computation”. doi:10.48550/arXiv.0804.1133.

* A. Narayanan and M. Moore, ”Quantum-inspired genetic algorithms,” in Proceedings of IEEE International Conference on Evolutionary Computation, pp. 61-66, 1996.


## License

This project is licensed under the MIT License. 
Feel free to customize the content, adjust the formatting, and include any additional information you deem necessary.