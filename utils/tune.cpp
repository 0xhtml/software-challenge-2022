#include <cmath>
#include <cstdlib>
#include <fstream>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_min.h>
#include <gsl/gsl_multimin.h>
#include <gsl/gsl_vector_double.h>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "../src/gamestate.hpp"
#include "../src/types.hpp"
#include "helper.hpp"

#define VEC_SIZE (1 + (PIECE_TYPE_COUNT * FIELD_COUNT / 2))

struct SimpleGameState {
    union {
        int i[VEC_SIZE]{0};
        struct {
            int score;
            int fields[PIECE_TYPE_COUNT][FIELD_COUNT / 2];
        } p;
    } factors;
    double result = 0.5;
};

#define WIDTH 53

void header(std::string text) {
    std::string sep = "";
    for (int i = 0; i < WIDTH; i++) sep.push_back('=');

    int w = (WIDTH - text.size()) / 2;
    for (int i = 0; i < w; i++) text.insert(0, 1, ' ');

    printf("\n%s\n%s\n%s\n", sep.c_str(), text.c_str(), sep.c_str());
}

void read(std::vector<SimpleGameState> &gameStates) {
    header("READING GAMESTATES");

    std::ifstream file{"states.txt"};
    std::string line;

    while (std::getline(file, line)) {
        size_t pos = line.find_last_of(' ');
        if (pos == std::string::npos) continue;
        std::string fen = line.substr(0, pos);
        std::string result = line.substr(pos);

        GameState gameState = Helper::createGameState(fen);

        SimpleGameState simpleGameState{};
        int team = gameState.turn % 2;

        for (int square = 0; square < FIELD_COUNT; ++square) {
            const Field &field = gameState.board[square];

            if (!field.occupied) continue;

            Position pos{square};

            if (field.team == TWO) {
                pos.coords.x = 7 - pos.coords.x;
            }
            if (pos.coords.y >= 4) pos.coords.y = 7 - pos.coords.y;

            uint8_t sq = ((pos.square & 0b111000) >> 1) | (pos.square & 0b11);

            if (field.team == team) simpleGameState.factors.p.fields[field.pieceType][sq]++;
            else simpleGameState.factors.p.fields[field.pieceType][sq]--;
        }

        simpleGameState.factors.p.score = gameState.score[ONE] - gameState.score[TWO];
        simpleGameState.result = std::atof(result.c_str());

        if (team == TWO) {
            simpleGameState.factors.p.score *= -1;
            simpleGameState.result = 1 - simpleGameState.result;
        }

        gameStates.push_back(simpleGameState);
    }

    file.close();

    printf("N = %lu\n", gameStates.size());
}

struct KParams {
    const std::vector<SimpleGameState> &gameStates;
    const gsl_vector *x;
};

struct Params {
    const std::vector<SimpleGameState> &gameStates;
    double K = 0;
};

double q(const SimpleGameState &gameState, const gsl_vector *x) {
    double score = 0;

    for (int i = 0; i < VEC_SIZE; ++i) {
        score += gameState.factors.i[i] * gsl_vector_get(x, i);
    }

    return score;
};

double dq(const SimpleGameState &gameState, const uint8_t i) {
    return gameState.factors.i[i];
};

double sigmoid(const double q, const double K) {
    return 1.0 / (1.0 + exp(-K * q));
}

double E(const gsl_vector *x, void *params) {
    Params *p = (Params *) params;

    double sum = 0;

    #pragma omp parallel for schedule(static) reduction(+:sum)
    for (const SimpleGameState &gameState : p->gameStates) {
        double s = sigmoid(q(gameState, x), p->K);
        double d = gameState.result - s;
        sum += pow(d, 2);
    }

    return sum / (double) p->gameStates.size();
}

double kE(double k, void *params) {
    KParams *kp = (KParams *) params;
    Params p{kp->gameStates, k};
    return E(kp->x, &p);
}

void dE(const gsl_vector *x, void *params, gsl_vector *g) {
    Params *p = (Params *) params;

    gsl_vector_set_zero(g);

    #pragma omp parallel
    {
        gsl_vector *local = gsl_vector_calloc(VEC_SIZE);

        #pragma omp for schedule(static)
        for (const SimpleGameState &gameState : p->gameStates) {
            double s = sigmoid(q(gameState, x), p->K);
            double d = gameState.result - s;
            double a = d * s * (1 - s);

            for (int i = 0; i < VEC_SIZE; ++i) {
                double b = gsl_vector_get(local, i);
                gsl_vector_set(local, i, b + a * dq(gameState, i));
            }
        }

        #pragma omp critical
        for (int i = 0; i < VEC_SIZE; ++i) {
            double b = gsl_vector_get(g, i);
            gsl_vector_set(g, i, b + gsl_vector_get(local, i));
        }

        gsl_vector_free(local);
    }

    gsl_vector_scale(g, -2.0 * p->K / (double) p->gameStates.size());
}

void EdE(const gsl_vector *x, void *params, double *f, gsl_vector *g) {
    Params *p = (Params *) params;

    gsl_vector_set_zero(g);
    double sum = 0;

    #pragma omp parallel
    {
        gsl_vector *local = gsl_vector_calloc(VEC_SIZE);

        #pragma omp for schedule(static) reduction(+:sum)
        for (const SimpleGameState &gameState : p->gameStates) {
            double s = sigmoid(q(gameState, x), p->K);
            double d = gameState.result - s;
            sum += pow(d, 2);
            double a = d * s * (1 - s);

            for (int i = 0; i < VEC_SIZE; ++i) {
                double b = gsl_vector_get(local, i);
                gsl_vector_set(local, i, b + a * dq(gameState, i));
            }
        }

        #pragma omp critical
        for (int i = 0; i < VEC_SIZE; ++i) {
            double b = gsl_vector_get(g, i);
            gsl_vector_set(g, i, b + gsl_vector_get(local, i));
        }

        gsl_vector_free(local);
    }

    *f = sum / (double) p->gameStates.size();
    gsl_vector_scale(g, -2.0 * p->K / (double) p->gameStates.size());
}

#define MAX_K 1.0

double optimizeK(const std::vector<SimpleGameState> &gameStates, const gsl_vector *x) {
    header("OPTIMIZING K");

    KParams params{gameStates, x};

    gsl_function func{kE, &params};

    double k = MAX_K / 2;

    gsl_min_fminimizer *s = gsl_min_fminimizer_alloc(gsl_min_fminimizer_brent);
    gsl_min_fminimizer_set(s, &func, k, 0.0, MAX_K);

    int status, i = 0;
    do {
        i++;

        status = gsl_min_fminimizer_iterate(s);
        if (status) break;

        k = gsl_min_fminimizer_x_minimum(s);
        double a = gsl_min_fminimizer_x_lower(s);
        double b = gsl_min_fminimizer_x_upper(s);

        status = gsl_min_test_interval(a, b, 1e-7, 0.0);

        printf("%5d K = %12.10f E() = %12.10f\n", i, k, s->f_minimum);
    } while (status == GSL_CONTINUE);

    gsl_min_fminimizer_free(s);

    return k;
}

void optimizeX(Params &params, const gsl_vector *x) {
    header("OPTIMIZING X");

    gsl_multimin_function_fdf func{E, dE, EdE, VEC_SIZE, &params};

    gsl_multimin_fdfminimizer *s = gsl_multimin_fdfminimizer_alloc(gsl_multimin_fdfminimizer_vector_bfgs2, VEC_SIZE);
    gsl_multimin_fdfminimizer_set(s, &func, x, 1.0, 1e-1);

    gsl_vector_const_view sv = gsl_vector_const_subvector(s->x, 1, VEC_SIZE - 1);
    int status, i = 0;
    do {
        ++i;

        status = gsl_multimin_fdfminimizer_iterate(s);
        if (status) break;

        status = gsl_multimin_test_gradient(s->gradient, 1e-20);

        double min, max;
        gsl_vector_minmax(&sv.vector, &min, &max);
        if (-min > max) max = -min;

        printf("%5d absmax(x[1:]) = %7.3f E() = %12.10f\n", i, max, s->f);
    } while (status == GSL_CONTINUE);

    FILE *file = fopen("params.txt", "w");
    gsl_vector_fprintf(file, s->x, "%.4f");
    fclose(file);

    gsl_multimin_fdfminimizer_free(s);
}

int main() {
    std::vector<SimpleGameState> gameStates{};
    read(gameStates);

    gsl_vector *x = gsl_vector_alloc(VEC_SIZE);
    FILE *file = fopen("initparams.txt", "r");
    gsl_vector_fscanf(file, x);
    fclose(file);

    double K = optimizeK(gameStates, x);

    Params params{gameStates, K};
    optimizeX(params, x);

    gsl_vector_free(x);

    return 0;
}
