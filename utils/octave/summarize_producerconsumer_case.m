%% Summarize the result of one Producer-Consumer testbench case.
%% Anders Gidenstam  2014

function [alg threads pattern pcpw throughputs RAPL_powers] = summarize_producerconsumer_case(basename, algname, casename)

  res = load_case_result(basename, algname, casename);
  [t_rapl RAPL_PKG_power RAPL_CPU_power RAPL_UNCORE_power RAPL_DRAM_power] = \
    load_RAPL_power(basename, algname, casename);

  %% Select the active duration for power measurement.
  %% Currently the middle 50% of the active interval.

  %% RAPL
  RAPL_t_avg = lookup(t_rapl, res(5) + 0.25*res(3)) : lookup(t_rapl, res(5) + 0.75*res(3));

  RAPL_all = [RAPL_PKG_power RAPL_CPU_power RAPL_UNCORE_power RAPL_DRAM_power];

  RAPL_powers_mean = mean(RAPL_all(RAPL_t_avg,:));
  RAPL_powers_std = std(RAPL_all(RAPL_t_avg,:));

  if (0)
    result = plot(t_rapl(RAPL_t_avg), RAPL_PKG_power(RAPL_t_avg,1),    "b+-;CPU S1 RAPL;",
                  t_rapl(RAPL_t_avg), RAPL_PKG_power(RAPL_t_avg,2),    "g+-;CPU S2 RAPL;",
                  %t_rapl(RAPL_t_avg), RAPL_CPU_power(RAPL_t_avg,1),    "b+-;CPU S1 RAPL;",
                  %t_rapl(RAPL_t_avg), RAPL_CPU_power(RAPL_t_avg,2),    "g+-;CPU S2 RAPL;",
                  t_rapl(RAPL_t_avg), RAPL_UNCORE_power(RAPL_t_avg,1), "k+-;UNCORE S1 RAPL;",
                  t_rapl(RAPL_t_avg), RAPL_UNCORE_power(RAPL_t_avg,2), "y+-;UNCORE S2 RAPL;",
                  t_rapl(RAPL_t_avg), RAPL_DRAM_power(RAPL_t_avg,1),   "m+-;MEM S1 RAPL;",
                  t_rapl(RAPL_t_avg), RAPL_DRAM_power(RAPL_t_avg,2),   "c+-;MEM S2 RAPL;");
    sleep(1);
  endif

  %% Prepare the output
  alg     = res(1);
  threads = res(2);
  pattern = res(9);
  pcpw    = res(10:11);
  throughputs = res(6:8)./res(3);

  RAPL_powers = RAPL_powers_mean;

endfunction
