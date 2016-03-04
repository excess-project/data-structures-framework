%% Evaluation of SpGEMM testbench cases.
%% Anders Gidenstam  2016

%% Algorithms: -; Threads: 1; Pinning: 1; MMAlg: 0; Matrix: 0 1 2; WUSize: -;
%% calloc used in phase 2. Sequential Gustavson.
%resfile = '/home/andersg/HLRS/results/SpGEMM_2016-02-04_14.14.res';

%% Algorithms: 0 1 2 3 4 5 6 7 8 9 10 11; Threads: 2 4 6 8 10 12 14 16 18 20; Pinning: 1; MMAlg: 1; Matrix: 1 2; WUSize: 4;
%% calloc used during phase 1 and phase 2.
%resfile = '/home/andersg/HLRS/results/SpGEMM_2016-02-04_14.32--2016-02-04_21.44.res';

%% Algorithms: 0 1 2 3 4 5 6 7 8 9 10 11 12; Threads: 20; Pinning: 1; MMAlg: 1; Matrix: 2; WUSize: 2 4 8 16;
%% malloc used during phase 1 and phase 2; mutex + condition used for phase 2.
%resfile = '/home/andersg/HLRS/results/SpGEMM_2016-02-05_14.22.res';

%% Algorithms: 0 1 2 3 4 5 6 7 8 9 10 11 12; Threads: 2 4 6 8 10 12 14 16 18 20; Pinning: 1; MMAlg: 1; Matrix: 2; WUSize: 2 4 8 16;
%% malloc used during phase 1 and phase 2; busy waiting used for phase 2.
%resfile = '/home/andersg/HLRS/results/SpGEMM_2016-02-05_16.17--2016-02-06_01.17.res';

%% Algorithms: 0 1 2 3 4 5 6 7 8 9 10 11 12; Threads: 2 4 6 8 10 12 14 16 18 20; Pinning: 1; MMAlg: 1; Matrix: 2; WUSize: 1 2 4 8 16;
%% malloc used during phase 1 and phase 2; busy waiting used for phase 2.
%% Work units are inserted in the P/C collection rather than rows.
%resfile = '/home/andersg/HLRS/results/SpGEMM_2016-02-12_14.49.res'

%% Algorithms: 0-12; Threads: 10; Pinning: 1; MMAlg: 1;
%% Matrix: 2 3 4 5 6 7 8 9 10 11 12 13; WUSize: 1 2 4 8 16;
%% malloc used during phase 1 and phase 2; busy waiting used for phase 2.
%% Work units are inserted in the P/C collection rather than rows.
%% NOTE: A bug in my SpGEMM implementations forced intermediate results to int. 
%resfile = '/home/andersg/HLRS/results/SpGEMM_2016-02-25_16.48--2016-02-25_23.58.res';

%% Algorithms: 0-12; Threads: 10 20; Pinning: 1; MMAlg: 1;
%% Matrix: 2 3 4 5 6 7 8 9 10 11 12 13; WUSize: 1 2 4 8 16;
%% malloc used during phase 1 and phase 2; busy waiting used for phase 2.
%% Work units are inserted in the P/C collection rather than rows.
resfile = '/home/andersg/HLRS/results/SpGEMM_2016-03-03_22.25--2016-03-04_13.19.res';

res = load(resfile);
%% Layout of result file:
%%   1-7:  freq collection_alg threads pinning matrix mmalg wus
%%   8-11: total_duration phase1_duration phase2_duration phase3_duration
%%  12-14: #inserts  #non-empty_TryRemoves #empty_TryRemoves
%%  15-18: P_PKG_S1       P_PKG_S2       P_CPU_S1    P_CPU_S2
%%  19-22: P_Uncore_S1    P_Uncore_S2    P_Mem_S1    P_Mem_S2
%%  23-26: P_PKG_S1_p1    P_PKG_S2_p1    P_CPU_S1_p1 P_CPU_S2_p1
%%  27-30: P_Uncore_S1_p1 P_Uncore_S2_p1 P_Mem_S1_p1 P_Mem_S2_p1
%%  31-34: P_PKG_S1_p2    P_PKG_S2_p2    P_CPU_S1_p2 P_CPU_S2_p2
%%  35-38: P_Uncore_S1_p2 P_Uncore_S2_p2 P_Mem_S1_p2 P_Mem_S2_p2
%%  39-42: P_PKG_S1_p3    P_PKG_S2_p3    P_CPU_S1_p3 P_CPU_S2_p3
%%  43-46: P_Uncore_S1_p3 P_Uncore_S2_p3 P_Mem_S1_p3 P_Mem_S2_p3
%%
%%  47-50: BCoV_PKG_S1       BCoV_PKG_S2       BCoV_CPU_S1    BCoV_CPU_S2
%%  51-54: BCoV_Uncore_S1    BCoV_Uncore_S2    BCoV_Mem_S1    BCoV_Mem_S2
%%  55-58: BCoV_PKG_S1_p1    BCoV_PKG_S2_p1    BCoV_CPU_S1_p1 BCoV_CPU_S2_p1
%%  59-62: BCoV_Uncore_S1_p1 BCoV_Uncore_S2_p1 BCoV_Mem_S1_p1 BCoV_Mem_S2_p1
%%  63-66: BCoV_PKG_S1_p2    BCoV_PKG_S2_p2    BCoV_CPU_S1_p2 BCoV_CPU_S2_p2
%%  67-70: BCoV_Uncore_S1_p2 BCoV_Uncore_S2_p2 BCoV_Mem_S1_p2 BCoV_Mem_S2_p2
%%  71-74: BCoV_PKG_S1_p3    BCoV_PKG_S2_p3    BCoV_CPU_S1_p3 BCoV_CPU_S2_p3
%%  75-78: BCoV_Uncore_S1_p3 BCoV_Uncore_S2_p3 BCoV_Mem_S1_p3 BCoV_Mem_S2_p3

%% MMAlg 1 parallel work measurements.
%pwres_file = 'fe-R16xR16_pr-pw-1.dat';
%pwres_file = 'CTHEXCESS-R16xR16_pr-pw.dat';
%pwres_file = 'CTHEXCESS-R16xR16_pr-pw-2.dat';
pwres_file = 'CTHEXCESS-R16xR16_pr-pw-wu1.dat';

%% Layout of the parallel work measurements file:
%%   1-4:  row#  cycles_phase1  cycles_phase3  non-zero_count
pwres = load(pwres_file);

%% Extract the domain ranges for the experiment.
S_range_freq    = unique(res(:,1))';
S_range_impl    = unique(res(:,2))';
S_range_threads = unique(res(:,3))';
S_range_pinning = unique(res(:,4))';
S_range_matrix  = unique(res(:,5))';
S_range_mmalg   = unique(res(:,6))';
S_range_wus     = unique(res(:,7))';


%% Select cases
threads=10;
impls = S_range_impl;
matrix=12;

%% Compute producer/consumer collection throughput for the selected cases.
res_t = res(res(:,3)==threads,:);
res_tm = res_t(res_t(:,5)==matrix,:);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
%% Report new SpGEMM algoritm best configuration for each matrix.
%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if 1
for m = S_range_matrix
  res_m = res(res(:,5)==m,:);
  [best, best_idx] = min(res_m(:,8));
  printf("Best for matrix %d %f sec: %d threads, col.impl. %d, work unit %d rows.\n",
         m,
         res_m(best_idx, 8),
         res_m(best_idx, 3),
         res_m(best_idx, 2),
         res_m(best_idx, 7));
endfor
endif

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
%% Plot SpGEMM total time against work unit size. 
%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if 1
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Plot SpGEMM total time against work unit size for all implementations.
figure(1);
color = ['r', 'g', 'b', 'm', 'k'];
color  = [color color color color];
symb  = ['+', 'x', 'o', '*', 's', 'd', '^', 'v', '<', '>', 'p', 'h'];
symb  = [symb symb symb symb];

for impl=impls
  res_tmi = res_tm(res_tm(:,2)==impl,:);
  time_tmi = res_tmi(:,8);
  plot(res_tmi(:,7), time_tmi,
       [color(impl+1) symb(2*impl+1) "-;Impl. " num2str(impl) ";"]
  );
  hold on;
endfor
title(["SpGEMM running time (matrix " num2str(matrix) ", " num2str(threads) " threads)"]);
xlabel("Matrix rows per work package (rows)");
ylabel("Running time (sec)");
hold off;

endif


if 1
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
%% Instantiate IPDPS queue models and predict the application performance.
%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Focus on queues for this part.
impls=0:8;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Raw testbench producer/consumer data for model instantiation.
res_empirical = load('producerconsumer_2016-01-05_09.51--2016-01-09_10.10.res');
%% The raw data.
%% Layout of empirical result file:
%%   1-7:  freq alg threads pinning pattern ppw cpw
%%   8-10: tp_insert tp_try_remove_ok tp_try_remove_empty
%%  11-14: P_PKG_S1 P_PKG_S2 P_CPU_S1 P_CPU_S2
%%  15-18: P_Uncore_S1 P_Uncore_S2 P_Mem_S1 P_Mem_S2
%%  19-22: BCoV_PKG_S1 BCoV_PKG_S2 BCoV_CPU_S1 BCoV_CPU_S2
%%  23-26: BCoV_Uncore_S1 BCoV_Uncore_S2 BCoV_Mem_S1 BCoV_Mem_S2

%% Extract the domain ranges.
E_range_freq    = unique(res_empirical(:,1));
E_range_impl    = unique(res_empirical(:,2));
E_range_threads = unique(res_empirical(:,3));
E_range_pinning = unique(res_empirical(:,4));
E_range_pattern = unique(res_empirical(:,5));
E_range_pw_e    = unique(res_empirical(:,6));
E_range_pw_d    = unique(res_empirical(:,7));

E_res_t = res_empirical(res_empirical(:,3)==threads,:);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Instantiate one EXCESS IPDPS 2015 type model per implementation
%% (and number of threads)
%%
%% Model instantiation
%%
%%   Parameters
%%     #enqueuers, #dequeuers
n = threads; % Hack due to SpGEMM having produce and consume phases...
%%     pw_o data points for the model instantiation. The model uses 6 points:
%%       (p_s,p_s), (p_s,p_m), (p_m,p_s), (p_m,p_b), (p_b,p_m), (p_b,p_b)
p_s = 5;
p_m = 150;
p_b = 1000;
idx_p_s = lookup(E_range_pw_e, p_s);
idx_p_m = lookup(E_range_pw_e, p_m);
idx_p_b = lookup(E_range_pw_e, p_b);

model = -1.*ones(rows(E_range_impl),11);
%cycles_per_wu = zeros(rows(E_range_impl),1);

for idx_impl = 1:rows(E_range_impl)
  try
    %% Extract the relevant empirical data
    E_res_ti = E_res_t(E_res_t(:,2)==E_range_impl(idx_impl),:);

    %% Collect the throughputs over the pw_d x pw_e domain
    tp_e  = zeros(rows(E_range_pw_d), rows(E_range_pw_e));
    tp_dp = zeros(rows(E_range_pw_d), rows(E_range_pw_e));
    tp_dm = zeros(rows(E_range_pw_d), rows(E_range_pw_e));

    for idx_d = 1:rows(E_range_pw_d)
      E_res_tid = E_res_ti(E_res_ti(:,7)==E_range_pw_d(idx_d),:);
      for idx_e = 1:rows(E_range_pw_e)
        tp_e(idx_d, idx_e)  = E_res_tid(E_res_tid(:,6)==E_range_pw_e(idx_e), 8);
        tp_dp(idx_d, idx_e) = E_res_tid(E_res_tid(:,6)==E_range_pw_e(idx_e), 9);
        tp_dm(idx_d, idx_e) = E_res_tid(E_res_tid(:,6)==E_range_pw_e(idx_e), 10);
      endfor
    endfor
    tp_d = tp_dp + tp_dm;

    %%     "Unit of work" conversion to cycles
    cycles_per_wu(idx_impl) = n .* E_range_freq(1)*1e9 ./ (tp_e(idx_p_b, idx_p_b) .* p_b);

    %% Instantiate the queue model.
    model(idx_impl,:) = EXCESS_IPDPS15_queue_model_instantiate(E_range_freq(1)*1e9, n, p_s*cycles_per_wu(idx_impl), p_m*cycles_per_wu(idx_impl), p_b*cycles_per_wu(idx_impl), tp_d(idx_p_s,idx_p_s), tp_d(idx_p_s,idx_p_m), tp_d(idx_p_m,idx_p_s), tp_d(idx_p_m,idx_p_b), tp_e(idx_p_s,idx_p_s), tp_e(idx_p_s,idx_p_m), tp_e(idx_p_m,idx_p_s), tp_e(idx_p_m,idx_p_b), tp_e(idx_p_b,idx_p_m));

  catch
    printf("Failed to instantiate EXCESS_IPDPS15_queue_model for implementation %d.\n", E_range_impl(idx_impl));
  end_try_catch
endfor


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Plot throughput against work unit size for all implementations.
figure(2);
color = ['r', 'g', 'b', 'm', 'k'];
color  = [color color color color];
symb  = ['+', 'x', 'o', '*', 's', 'd', '^', 'v', '<', '>', 'p', 'h'];
symb  = [symb symb symb symb];

for impl=impls
  res_tmi = res_tm(res_tm(:,2)==impl,:);
  tp_tmi = res_tmi(:,[12 13 14])./res_tmi(:,[9 11 11]);
  plot(res_tmi(:,7), tp_tmi(:,1),
       [color(impl+1) symb(2*impl+1) ";Enq. Impl. " num2str(impl) ";"],
       res_tmi(:,7), tp_tmi(:,2) + tp_tmi(:,3),
       [color(impl+1) symb(2*impl+2) ";Deq. Impl. " num2str(impl) ";"]
  );
  hold on;
endfor
title(["SpGEMM producer/consumer collection throughput (matrix " num2str(matrix) ", " num2str(threads) " threads)"]);
xlabel("Matrix rows per work package (rows)");
ylabel("Producer/consumer collection throughput (ops/sec)");
%hold off;

%% Use the IPDPS queue models to predict the throughput using the average
%% measured parallel work in the phases.

range_wus = [1 2 4 8 16]';

pw_large = 1e8; %% Bougus guess.

%% Hack!! Estimate average pw from a sequential sptest run for this matrix.
res_sptest = load('/home/andersg/HLRS/results/SpTest_2016-03-03_17.25.res');
res_gustavson = res_sptest(res_sptest(:,4)==2,:);
res_gustavson_m = res_gustavson(res_gustavson(:,3)==matrix,:);

matrix_rows =  res_gustavson_m(1,9);

pw = res_gustavson_m(1,1).*1e9 .* res_gustavson_m(1,8) ./ res_gustavson_m(1,9);
%% Mega hack!!! Split pw between phase 1 and phase 3.
pw_e = 3/4 .* pw
pw_d = 1/4 .* pw

%pw_e = 1.*mean(pwres(:,2));
%pw_d = 1.*mean(pwres(:,3));
%pw_e = 445;
%pw_d = 330;

m_res = zeros(0, 4);
for impl=impls
  idx_impl = lookup(E_range_impl, impl);
  res_tmi = res_tm(res_tm(:,2)==impl,:);

  if E_range_impl(idx_impl) == impl
    this_model = model(idx_impl,:);
    if this_model(1) > 0
      mtp_e = zeros(rows(range_wus),1);
      mtp_d = zeros(rows(range_wus),1);
      predicted_time = zeros(rows(range_wus),1);
      for idx_wu = 1:rows(range_wus)
        wu = range_wus(idx_wu);
        [Tp_d Tp_e Tp_dp Tp_dm Tp_ep Tp_em] = EXCESS_IPDPS15_queue_model_throughput(pw_large, pw_e.*wu, this_model)
        mtp_e(idx_wu) = Tp_e;
%        mtp_e(idx_wu) = Tp_e./(mean(pwres(:,2))/pw_e);

        [Tp_d Tp_e Tp_dp Tp_dm Tp_ep Tp_em] = EXCESS_IPDPS15_queue_model_throughput(pw_d.*wu, pw_d.*wu./2, this_model)
        mtp_d(idx_wu) = Tp_d;
%        mtp_d(idx_wu) = Tp_dp./(mean(pwres(:,3))/pw_d);

        %% Predict the total time based on the throughputs. phase 2 is ignored.
        predicted_time(idx_wu) = matrix_rows./(wu*mtp_e(idx_wu)) + matrix_rows./(wu*mtp_d(idx_wu));

        %% Find the real time for this case.
        real_time = res_tmi(res_tmi(:,7)==wu, 8);
        %% Store this prediction.
        m_res = [m_res; [impl wu predicted_time(idx_wu) real_time]];
      endfor

      if 1
        % Plot throughput lines.
        figure(2);
        hold on;
        plot(range_wus, mtp_e,
%             [color(impl+1) symb(2*impl+1) "-"],
             [color(impl+1) "-"],
             range_wus, mtp_d,
%             [color(impl+1) symb(2*impl+2) "-"]
             [color(impl+1) "-"]
             );
        hold off;
      endif
      if 1
        % Plot predicted total time.
        figure(1);
        hold on;
        plot(range_wus, predicted_time,
             [color(impl+1) "-"]
             );
        hold off;
      endif;
    endif
  endif
endfor

endif

