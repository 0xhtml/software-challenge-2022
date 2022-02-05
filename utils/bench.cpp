#include <benchmark/benchmark.h>
#include <chrono>
#include <limits.h>
#include <string>
#include <vector>

#include "../src/alphabeta.hpp"
#include "../src/gamestate.hpp"
#include "../src/types.hpp"

const std::vector<std::string> states{
    // Generared using version 22.0.3
    "SMM1HR1R/7H/8/1S6/8/r7/h4mms/2rh1s2 14",
    "8/1H5S/M6H/s.3r.2r/5S2/8/3R.4/h1s3mh 47",
    "M1H1HR1R/1SS3M1/8/8/8/1r6/4h3/1mrhs1sm 5",
    "MRM1SH2/4H1S1/8/8/5R2/2h1s3/8/rs2hmrm 8",
    "S2M2MH/7R./3H4/2RS4/1r6/2h5/7s/hm1m3s 41",
    "1R1M1H1S/S3M2H/8/4R3/r7/1h3r2/1s1h2s1/3mm3 24",
    "5S1H/1S3RM1/MR2H3/6r1/8/3h2mr/1m1s4/h4s2 21",
    "1RSH1MHR/1S2M3/8/8/8/8/2m3s1/rh1mh1rs 4",
    "S6R/H2M2M1/8/1S6/3H.Rs2/8/5h1s/m1hr1m2 35",
    "2SHMHM1/S7/6R1/1R6/8/4h3/3m4/rmh2ssr 7",
    "R2R3S/4M3/5H2/1r.6/H3M3/s1h1rhs1/8/3m1m2 48",
    "5M2/RS5H/3M4/S2s1RH1/3m4/h7/2h2ms1/6rr 46",
    "HM1H3S/1S2R2M/8/5R2/8/s1h2sr1/6m1/2mr3h 21",
    "HHR1M1SM/1R3S2/8/8/8/8/s2s2r1/m1m2rhh 6",
    "S1M2S2/2H2M2/4R2H/3R1r2/8/3s2h1/8/hm1r1m1s 27",
    "MH1S1M1R/1R1H4/5S2/8/8/3r2s1/2r4m/m1sh2h1 13",
    "3MMS1R/7H/S1H4r/7h/1R6/1s6/4m3/rh3m1s 26",
    "RHMMRHSS/8/8/8/8/8/8/sshrmmhr 0",
    "1M5H/4R2M/S7/4H3/2R5/2s1r2m/m7/2h4s 38",
    "S2MRHM1/SH3R2/8/8/8/1r2r3/8/1mh1mhss 5",
    "r.3M3/S5HR/8/1H4S1/3h4/1s.1h4/5m2/4r1sm 35",
    "MM6/2S5/5HH1/r6R/1R1S4/h7/r1s5/1h1s1m1m 25",
    "2M1M3/1H6/1R1HR1r1/8/6S1/2h5/1s6/2sm2m1 37",
    "RH1S1M1M/3S3H/8/8/R7/8/2h3r1/m1mss1hr 7",
    "8/4HMSM/5R2/SR6/H2h4/5s1r/2s1m3/mr3h2 37",
    "1RMMr1H1/3SH3/5S2/8/8/1Rhs4/3mr3/sh3m2 23",
    "1RM4M/R5H1/2S1H3/S7/1h5s/5r2/2s5/mr2mh2 24",
    "R6H/M1M5/3S1S2/5H1s/4m3/shh5/8/7m 47",
    "2M3R1/3H4/Hr.3S2/4Ms2/8/7R./3sm3/r2h3m 49",
    "HS2RM2/1RM4H/5S2/8/8/8/h3rs2/s1m1rm1h 13",
    "3M2M1/1H6/S6S/4H3/R.7/h2mRrs1/3sh3/7r 44",
    "MR2R1SH/2H1SM2/8/3r4/2s1r3/8/5s1m/h1m3h1 19",
    "R2M4/H2MHR2/s7/rS6/3hm1Sh/8/5r2/3m1s2 39",
    "SM1M4/2H4S/8/2h5/1R5H/7r/1s3hms/8 44",
    "1MS3M1/1H2S3/2R5/3H4/3sR3/8/2h2s1m/rmr4h 22",
    "7M/7H/MRH5/6s1/2S.1Sr2/1h4m1/1m3s.2/7h 48",
    "M1HH2SM/1S1R2R1/8/8/8/8/m3h2s/1srrh2m 6",
    "M1SMHRS1/H7/6R1/8/3r4/8/8/rs1hmshm 4",
    "4MR1M/4RS2/S3H3/6H1/4r1h1/s7/3h2s1/1mrm4 27",
    "1SMRS2H/M6H/5R2/8/8/8/3s2sm/hhr1rm2 7",
    "HMS5/3r.3H/R3s.R2/2r5/8/5h2/h7/2m1s2m 34",
    "4R1MH/6H1/5M2/3r1S2/2m5/8/3sm3/h6r 48",
    "1MSMHRS1/6H1/1R6/8/8/8/r3mr2/hs1h1sm1 7",
    "2MH2RM/1SH5/3SR3/2r5/8/8/3s4/mr2hmhs 10",
    "2MH4/4S3/1H5R/M1hS4/s4m1R/3s4/2r3m1/3rh3 49",
    "1HH1MM2/8/3S1RR1/1r.6/3s4/6hs/8/1rmm1h2 29",
    "4S2H/1H3M2/RS4M1/1hhRs2r/8/8/3m1s2/r1m5 43",
    "3M1RMH/S5R1/8/1S3s2/2H5/7h/1r3r2/hm2m2s 26",
    "6S1/2H1R1MM/2Ss4/H7/3h4/sm4R1/2r2h2/2m4r 41",
    "MM4S1/8/4SHH1/1R5R/8/4r3/1sh5/hrs3mm 25",
    "RHSS1MHM/8/5R2/8/8/8/8/mhmrsshr 1",
    "HH4MS/8/8/2sS3M/1r6/8/1R5h/m1Rm1srh 33",
    "H1M1SSR1/RM4H1/8/8/8/5r2/3ss3/hr1m2mh 2",
    "HHRRSMMS/8/8/8/8/8/8/smmsrrhh 0",
    "3MS1r.H/1H6/2S5/7R./8/h7/8/3mm2h 37",
    "M1S2H2/M4H2/1S4RR/8/8/8/2rr1s2/2hh1smm 14",
    "5M2/r.1HM2S1/2R3s.1/8/7H/8/1hr5/1smm2h1 40",
    "S1r2M1M/2SH3H/8/8/r5s1/3R.4/h3h3/5mR.1 45",
    "R1HRM2H/6M1/S2S4/8/8/4s3/1h6/m1msrh1r 16",
    "S2M4/R5H1/2M2SR1/1H4r1/7r/8/1hss2hm/3m4 47",
    "HRHMM1SS/8/4R3/8/8/8/4h3/ssrmm1rh 2",
    "RRM4S/4HM2/5H2/5Sr1/3h4/8/4mh2/sm1s3r 16",
    "4R3/MM5H/7H./R5r.1/1S4s1/3s4/8/hh3m1m 43",
    "H1R3S1/M6R/r3M2H/1S6/1s3m2/2r5/1h6/4sm1h 40",
    "SRM2HHS/4M3/2R5/8/8/3r4/1h6/sh1m1mrs 4",
    "1RM3SR/5S2/MH5H/8/5r2/1r6/2s5/1sh1hm1m 13",
    "1HMSH2M/1S1R4/7R/8/8/3r3s/8/mr1hsmh1 6",
    "1MH1S3/1S1M2H1/6R1/r7/3s4/h6m/6s.1/1r2mh2 33",
    "4HSRM/2HS4/1M6/2R5/8/2r3r1/m1ssh3/6hm 16",
    "M1M4R/2H5/1S2H1S1/8/3Rhr2/7s/s2h3m/r5m1 24",
    "MHRS1MHS/8/8/8/8/r7/3s.3r/shm3hm 10",
    "MH5H/4M1S1/8/5R1s/S7/r3R3/s4hr1/h3m2m 45",
    "SR1RHSMM/8/2H5/6r1/8/8/3sh3/mm3hrs 8",
    "1S4M1/SH3RM1/8/4H3/r7/4hh2/2m5/2m3s1 34",
    "4MS1M/SH5R/8/3H4/s4s2/2r4h/1mr1m3/1R5h 40",
    "SM1H2RM/3S2H1/3R4/8/8/r7/4sh2/m1hr2ms 6",
    "M3H3/2HR1M2/3rS3/3R4/6r.1/2m1s3/8/s2h2hm 31",
    "1M6/3S2H1/S4r.R1/4H3/8/8/m4h2/2r1m1s1 32",
    "5S2/1M4H1/3MH1SR/6R1/8/7r/s5hm/h1srm3 44",
    "RSMMHHR1/6S1/8/8/8/r7/8/s1hhmmsr 2",
    "SRSMM1HH/8/4R3/8/8/8/8/hhrmmsrs 1",
    "1M1S1M2/8/2R4S/6HH/R4r2/hr6/h1m2sm1/s7 37",
    "3M1SH1/3M4/3H1S2/1Rh5/7R/2s4r/4m3/1h1smr2 43",
    "HMRSM1SH/8/6R1/8/8/8/4s3/hsrm1rmh 2",
    "1SHM2M1/2H3S1/4R3/8/6rR/8/1s5h/2mmrh1s 22",
    "M2R4/S1M2R2/6HH/8/2r2s2/6s1/8/h4m1m 48",
    "S2M3R/H2r1H2/6M1/7S/4r3/3h4/1s3h1R./2m4s 34",
    "3MM3/SH2RH2/6R1/8/7s/1r6/2m5/1h1m3h 38",
    "RH1R4/3M3S/3MS3/8/H.3h3/1s6/6r1/1rmm2h1 30",
    "S4MMR/6R1/1H1HS3/4r3/8/1r6/1mmh3h/5s1s 21",
    "HS4M1/R4SM1/5H2/4R3/8/1r6/2s2h1s/1mm3rh 15",
    "HMHSMSRR/8/8/8/8/8/8/rrsmshmh 0",
    "H1MR3S/7H/4M1RS/8/4r3/4s3/hs6/3mm1rh 36",
    "6MM/1H1R4/2rS2H1/S4R2/8/3h4/m2rs2s/4mh2 31",
    "H2M3H/M2S1S2/5R2/7R/8/5h2/5r2/hrs1msm1 18",
    "HH1R1M2/1R1S2M1/8/4S3/8/3r3h/1s3s1r/1m1m2h1 28",
    "MHRSSHMR/8/8/8/8/8/8/rmhssrhm 0",
    "M1M5/H6H/6R1/3r1R2/3r.1S1m/sh4s1/8/5mh1 34",
    "2MSM3/2H5/6S1/3R4/4R.3/1hsr3H/1s5h/m4m2 37"
};

static void BM_GameState_getPossibleMoves(benchmark::State &state) {
    int i = 0;

    for (auto _ : state) {
        state.PauseTiming();

        GameState gameState{states[i]};

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

        GameState gameState{states[i % states.size()]};

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

        GameState gameState{states[i % states.size()]};

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

        GameState gameState{states[i]};

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
