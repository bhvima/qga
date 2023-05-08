#ifndef QUBIT_H
#define QUBIT_H

typedef struct {
    double alpha;
    double beta;
} qubit;

/**
 * Initializes a qubit with given values for alpha and beta
 * @param q: The qubit to initialize
 * @param alpha: The alpha value
 * @param beta: The beta value
 */
void qubit_init(qubit *q, double alpha, double beta);

/**
 * Performs a rotation operation on a qubit with the given angle
 * @param q: The qubit to rotate
 * @param angle: The angle to rotate the qubit by (in radians)
 */
void qubit_rotate(qubit *q, double angle);

/**
 * Measures the value of a qubit in the computational basis
 * @param q: The qubit to measure
 * @return: The measured value (0 or 1)
 */
int qubit_measure(qubit *q);

#endif
