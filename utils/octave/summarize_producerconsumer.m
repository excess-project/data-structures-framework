%% Summarize the result of a collection of Producer-Consumer testbench cases.
%% Anders Gidenstam  2014 - 2015

FREQs = [3];
%ALGs = [0 3 7 10];
ALGs = 0:11;
%PPWs = [1];
PPWs = [1 2 3 4 5 7 10 20 50 150 500 1000];
%CPWs = [1];
CPWs = [1 2 3 4 5 7 10 20 50 150 500 1000];
%THREADs = [16];
THREADs = [2 4 6 8 10 12 14 16 18 20];

PLOT_POWER = 0;

RUNs1 = [
'/home/andersg/HLRS/results/producerconsumer_2015-01-30_15.57'
'/home/andersg/HLRS/results/producerconsumer_2015-01-30_16.49'
'/home/andersg/HLRS/results/producerconsumer_2015-01-30_17.42'
'/home/andersg/HLRS/results/producerconsumer_2015-01-30_18.34'
'/home/andersg/HLRS/results/producerconsumer_2015-01-30_19.26'
'/home/andersg/HLRS/results/producerconsumer_2015-01-30_20.19'
'/home/andersg/HLRS/results/producerconsumer_2015-01-30_21.11'
'/home/andersg/HLRS/results/producerconsumer_2015-01-30_22.04'
'/home/andersg/HLRS/results/producerconsumer_2015-01-30_22.56'
'/home/andersg/HLRS/results/producerconsumer_2015-01-30_23.48'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_01.13'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_02.06'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_02.58'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_03.51'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_04.43'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_05.35'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_06.28'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_07.20'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_08.12'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_09.05'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_09.57'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_10.49'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_11.42'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_12.34'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_13.26'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_14.19'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_15.11'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_16.03'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_16.56'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_17.48'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_18.40'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_19.33'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_20.25'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_21.17'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_22.10'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_23.02'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_23.54'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_01.19'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_02.12'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_03.04'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_03.57'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_04.49'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_05.41'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_06.33'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_07.26'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_08.18'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_09.11'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_10.03'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_10.55'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_11.48'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_12.40'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_13.32'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_14.24'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_15.17'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_16.09'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_17.02'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_17.54'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_18.46'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_19.38'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_20.31'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_21.23'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_22.15'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_23.08'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_00.00'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_01.25'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_02.18'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_03.10'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_04.02'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_04.55'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_05.47'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_06.39'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_07.32'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_08.24'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_09.16'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_10.09'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_11.01'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_11.53'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_12.46'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_13.38'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_14.31'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_15.23'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_16.15'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_17.08'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_18.00'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_18.52'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_19.45'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_20.37'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_21.29'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_22.22'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_23.14'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_00.39'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_01.31'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_02.24'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_03.16'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_04.09'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_05.01'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_05.53'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_06.46'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_07.38'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_08.30'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_09.23'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_10.15'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_11.07'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_12.00'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_12.52'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_13.44'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_14.37'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_15.29'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_16.22'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_17.14'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_18.06'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_18.58'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_19.51'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_20.43'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_21.36'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_22.28'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_23.20'
'/home/andersg/HLRS/results/producerconsumer_2015-02-04_00.46'
'/home/andersg/HLRS/results/producerconsumer_2015-02-04_01.38'
'/home/andersg/HLRS/results/producerconsumer_2015-02-04_02.30'
       ];

RUNs = RUNs1;

i = 1;
res = [];
for d = 1:size(RUNs)(1)
  base = RUNs(d,:);
  basename = [base '/producerconsumer_result_' base(length(base) - 15 : length(base)) '-'];

    for f = FREQs
      for a = ALGs

        algname = sprintf("a%d", a);

        for ppw = PPWs
          for cpw = CPWs
            for t = THREADs
              casename = sprintf("p1-ppw%d-cpw%d-f%d-t%d", ppw, cpw, f, t);

              try
                resfile = sprintf("%sOUT-%s-%s.txt", basename, algname, casename);
                %printf("Trying '%s' ... ", outfile);

                [info, err] = stat(resfile);
                if (err == 0)
                  [alg threads pinning pattern pcpw throughputs RAPL_powers] = summarize_producerconsumer_case(basename, algname, casename, PLOT_POWER);
                  %printf("succeeded\n");

                  res(i,:) = [f alg threads pinning pattern pcpw throughputs RAPL_powers];
                  i = i+1;
                else
                  %printf("failed\n");
                endif
              catch
                printf("exception in summarize_producerconsumer case '%s'\n", resfile);
              end_try_catch
            endfor
          endfor
        endfor
      endfor
    endfor
endfor

csvwrite("result.res", res);
