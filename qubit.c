#include <stdlib.h>
#include <math.h>

#include "qubit.h"

void qubit_init(qubit *q, double alpha, double beta) {
    q->alpha = alpha;
    q->beta = beta;
}

void qubit_rotate(qubit *q, double angle) {
    double cos_angle = cos(angle);
    double sin_angle = sin(angle);
    double new_alpha = cos_angle * q->alpha - sin_angle * q->beta;
    double new_beta = sin_angle * q->alpha + cos_angle * q->beta;
    q->alpha = new_alpha;
    q->beta = new_beta;
}

int qubit_measure(qubit *q) {
    return ((double)rand() / RAND_MAX) > (q->alpha * q->alpha);
}
