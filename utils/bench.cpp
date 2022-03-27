#include <benchmark/benchmark.h>
#include <chrono>
#include <limits.h>
#include <string>
#include <vector>

#include "../src/alphabeta.hpp"
#include "../src/gamestate.hpp"
#include "../src/types.hpp"
#include "helper.hpp"

const std::vector<std::string> states{
    "3MS1HR/M7/8/3HS3/2R5/1r6/2hshr2/4smm1 0-0 28",
    "4SM1M/1RSR4/8/4H3/8/m2r4/2s1hs2/1mr3h1 1-0 32",
    "3S2M1/HS5M/8/R6R./7h/2m5/m3r1s.1/2r2s2 0-0 32",
    "1MH2RMS/1H4R1/3S4/8/5r2/5h2/3s4/smr2hm1 0-0 12",
    "1M1H1RS1/MS1R2H1/8/8/8/1r6/1r2m3/hs2h1sm 0-0 24",
    "1SRM1MHR/3S4/8/8/4H1r1/8/3s4/rhmm2sh 0-0 9",
    "3RMMSH/S7/1R2H3/8/8/8/1hm1r3/1s1mrhs1 0-0 8",
    "2S1SMMR/H1H5/1R6/8/8/5s2/6s1/rmm1h1hr 0-0 6",
    "M2R1MHS/S6R/8/8/8/s6h/H.1r1r3/3m1ms1 0-0 22",
    "H1H1RR1M/S1S5/6M1/8/8/7h/1m6/m1rrshs1 0-0 7",
    "5HS1/M1R5/8/2M5/8/3h4/3hs.3/1sr2mm1 0-1 37",
    "1S1RSHRM/3M4/8/7h/4H3/8/8/mrhsrms1 0-0 12",
    "2MRSHSM/H7/1R6/8/8/8/m3r3/1shsrmh1 0-0 7",
    "M1S3R1/3M3S/2r.3R1/r7/6h1/3H4/8/s2hmm1s 0-0 24",
    "3HS1MH/M2RR3/8/S.7/8/8/2m1sh1r/h2s3m 0-0 33",
    "2SMM1SH/3R4/1R6/2r1H3/1hr5/8/4m3/s2m1sh1 0-0 33",
    "1SMM2HS/8/4R3/1R6/8/1s6/2h1rm2/r2hm3 1-0 31",
    "1R1H4/S3M1MS/6h1/8/8/1s6/4s3/2m1hr.2 1-0 36",
    "2MHSHRM/S7/1R6/8/8/6r1/4s3/mrh1hms1 0-0 4",
    "1H2S1MR/1SHM4/4R3/8/7s/2sr2h1/5m2/rm2h3 0-0 34",
    "2M2MSS/8/H2H1R2/6R1/8/1s6/s5h1/2mrrmh1 0-0 13",
    "S4HM1/2M5/6R1/2R.5/8/r1h5/m2s4/5m1s 0-1 37",
    "1RHMSM1R/6S1/2H5/8/8/4m2h/8/rsms1hr1 0-0 9",
    "M3S1H1/S6M/4R1h1/1H6/5R2/1r1s4/5s2/mh2rm2 0-0 23",
    "1M1HR1MS/3S4/8/3R4/2H5/1s3h2/5s2/1mrrh1m1 0-0 15",
    "S2MH1MS/3R2H1/1R6/8/8/4r3/msh1r1s1/2h1m3 0-0 18",
    "1M2SM1S/2H5/1R6/8/3R.H3/7h/s5m1/1r1mhr2 0-0 25",
    "1S1RRS1M/7H/M7/8/4H3/6s1/3s4/mh1rrhm1 0-0 17",
    "5MMH/S4S2/RR3H2/8/8/8/1h2r3/m1mshsr1 0-0 20",
    "2MRHSH1/SM3R2/8/2r5/8/4s2s/2r5/1h1h1mm1 0-0 18",
    "1M1RH1SS/R1H2M2/8/8/8/8/1s5m/s1mhrrh1 0-0 12",
    "MSMH4/3R1HS1/8/1r6/8/s7/4mr2/5ms1 1-0 36",
    "S1HSMMR1/2H5/8/r7/7R/3h4/2mmsh2/1r3s2 0-0 29",
    "4RH2/MM4SS/4R3/8/2H3h1/3rr3/shs5/5mm1 0-0 25",
    "2M5/S3S2h./8/8/8/1H6/2h2s2/s1mr1R.2 0-1 32",
    "4M2R/r.1S1H2S/1R6/7h/s1H5/4s3/8/2hm1rm1 0-0 35",
    "1MH2MHS/R7/S7/8/2R5/8/4h3/shmrrsm1 0-0 19",
    "5S2/1RM1M3/8/8/8/8/3h2m.s/r1sm4 1-1 29",
    "1SS3HH/M4M2/8/2R5/8/2r.5/h4s1m/h1m1r1s1 0-0 17",
    "1RSHRHMS/M7/8/8/8/8/1h6/sm1rhsrm 0-0 2",
    "2MHH1RS/MS5R/8/8/6h1/r7/s4m1m/2r1h1s1 0-0 22",
    "1R1M1HMS/1S2RH2/8/8/8/8/1s3s2/1mhhmrr1 0-0 21",
    "5MM1/3R2S1/R7/8/6H1/s7/m2s4/2m1rhh1 0-1 34",
    "2HSRSR1/M1H4M/8/8/8/8/2s5/mr1rshhm 0-0 6",
    "S.HR5/M3HSM1/6R1/8/8/4r3/2ss4/1mh3hm 0-0 17",
    "M2M2SS/5HR1/3H4/R7/8/1r1r4/s7/s2hm1hm 0-0 24",
    "S2H2M1/5R2/1R6/1r.6/1S6/3sr.3/m7/3hhsm1 0-0 38",
    "5SHM/M2HS3/1R1R4/8/8/4r3/m1h1r3/1hs1s1m1 0-0 15",
    "1RMHR1MH/1S3S2/8/8/8/8/6s1/hmsrhmr1 0-0 3",
    "R2SMHMH/1S6/3R4/8/8/4r1r1/8/hmhms1s1 0-0 4"
};

static void BM_GameState_getPossibleMoves(benchmark::State &state) {
    int i = 0;

    for (auto _ : state) {
        state.PauseTiming();

        GameState gameState = Helper::createGameState(states[i]);

        i++;
        i %= states.size();

        state.ResumeTiming();

        gameState.getPossibleMoves();
    }
}
BENCHMARK(BM_GameState_getPossibleMoves);

static void BM_GameState_makeMove(benchmark::State &state) {
    int i = 0;

    for (auto _ : state) {
        state.PauseTiming();

        GameState gameState = Helper::createGameState(states[i % states.size()]);

        std::vector<Move> moves = gameState.getPossibleMoves();
        Move move = moves[i % moves.size()];

        i++;

        state.ResumeTiming();

        gameState.makeMove(move);
    }
}
BENCHMARK(BM_GameState_makeMove);

static void BM_GameState_unmakeMove(benchmark::State &state) {
    int i = 0;

    for (auto _ : state) {
        state.PauseTiming();

        GameState gameState = Helper::createGameState(states[i % states.size()]);

        std::vector<Move> moves = gameState.getPossibleMoves();
        Move move = moves[i % moves.size()];
        SaveState saveState = gameState.makeMove(move);

        i++;

        state.ResumeTiming();

        gameState.unmakeMove(move, saveState);
    }
}
BENCHMARK(BM_GameState_unmakeMove);

static void BM_AlphaBeta_alphaBetaRoot(benchmark::State &state) {
    const int d = state.range(0);

    int i = 0;

    for (auto _ : state) {
        state.PauseTiming();

        GameState gameState = Helper::createGameState(states[i]);

        AlphaBeta alphaBeta{gameState};

        for (int j = 0; j < 5; ++j) {
            alphaBeta.start = std::chrono::system_clock::now();
            alphaBeta.timeOut = false;

            MoveValuePair result;
            for (int i = 1; i <= d; ++i) {
                result = alphaBeta.alphaBetaRoot(i, -INT_MAX, INT_MAX);
                if (result.value >= INT_MAX) break;
            }
            gameState.makeMove(result.move);

            std::vector<Move> moves = gameState.getPossibleMoves();
            if (moves.size() == 0) break;
            gameState.makeMove(moves[(i << j) % moves.size()]);
        }

        i++;
        i %= states.size();

        alphaBeta.start = std::chrono::system_clock::now();
        alphaBeta.timeOut = false;

        state.ResumeTiming();

        for (int i = 1; i <= d; ++i) {
            MoveValuePair result = alphaBeta.alphaBetaRoot(i, -INT_MAX, INT_MAX);
            if (result.value >= INT_MAX) break;
        }
    }
}
BENCHMARK(BM_AlphaBeta_alphaBetaRoot)->DenseRange(1, 5)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
