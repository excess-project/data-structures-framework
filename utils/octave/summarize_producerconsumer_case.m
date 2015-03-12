%% Summarize the result of one Producer-Consumer testbench case.
%% Anders Gidenstam  2014 - 2015

function [alg threads pinning pattern pcpw throughputs RAPL_powers RAPL_powers_biased_coef_of_var] = summarize_producerconsumer_case(basename, algname, casename, plot_power)

  res = load_case_result(basename, algname, casename);
  [t_rapl RAPL_PKG_power RAPL_CPU_power RAPL_UNCORE_power RAPL_DRAM_power] = \
    load_RAPL_power(basename, algname, casename);

  %% Select the active duration for power measurement.
  %% Currently the middle 50% of the active interval.

  %% RAPL
  RAPL_t_avg = lookup(t_rapl, res(6) + 0.25*res(4))+1 : lookup(t_rapl, res(6) + 0.75*res(4));

  RAPL_all = [RAPL_PKG_power RAPL_CPU_power RAPL_UNCORE_power RAPL_DRAM_power];

  [basename algname '-' casename ' pinning=' sprintf("%d",res(3))]
  RAPL_powers_mean = mean(RAPL_all(RAPL_t_avg,:));
  RAPL_powers_std = std(RAPL_all(RAPL_t_avg,:));
  RAPL_powers_biased_coef_of_var = RAPL_powers_std ./ RAPL_powers_mean;

  RAPL_mem_mean = mean(RAPL_DRAM_power(RAPL_t_avg,1) + RAPL_DRAM_power(RAPL_t_avg,2))
  RAPL_mem_std = std(RAPL_DRAM_power(RAPL_t_avg,1) + RAPL_DRAM_power(RAPL_t_avg,2));
  RAPL_mem_biased_coef_of_var = RAPL_mem_std ./ RAPL_mem_mean

  

  %% Prepare the output
  alg     = res(1);
  threads = res(2);
  pinning = res(3);
  %% Distinguish between standard and phased mode via the length of res.
  if (length(res) <= 12)
    duration = res(4);
  else
    duration = res(14);
    phases = res(15)
    active_fraction = duration/res(4)
  endif
  pattern = res(10);
  pcpw    = res(11:12);
  throughputs = res(7:9)./duration

  RAPL_powers = RAPL_powers_mean;

  %% Optional RAPL power plot.
  if (plot_power)
    result = plot(t_rapl, RAPL_PKG_power(:,1),    "bo-;PKG S1 RAPL;",
                  t_rapl, RAPL_PKG_power(:,2),    "go-;PKG S2 RAPL;",
                  t_rapl, RAPL_CPU_power(:,1),    "b+-;CPU S1 RAPL;",
                  t_rapl, RAPL_CPU_power(:,2),    "g+-;CPU S2 RAPL;",
                  t_rapl, RAPL_UNCORE_power(:,1), "bx-;UNCORE S1 RAPL;",
                  t_rapl, RAPL_UNCORE_power(:,2), "gx-;UNCORE S2 RAPL;",
                  t_rapl, RAPL_DRAM_power(:,1),   "m+-;MEM S1 RAPL;",
                  t_rapl, RAPL_DRAM_power(:,2),   "c+-;MEM S2 RAPL;",
                  t_rapl(RAPL_t_avg), 80.*[0 ones(1, length(RAPL_t_avg)-2) 0], "r-;Averaging window;"
                  );
    title(['RAPL power (' basename algname '-' casename ') pinning=' sprintf("%d",pinning)]);
    xlabel("Time instant (sec since the epoch)");
    ylim([0, 70]);
    ylabel("Power (W)");
    print([basename algname '-' casename '-' sprintf("pin%d",pinning) '-power.eps'], "-deps","-color");
    sleep(3);
  endif

endfunction
