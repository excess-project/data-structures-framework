%% Summarize the result of a collection of Producer-Consumer testbench cases.
%% Anders Gidenstam  2014

FREQs = [3];
PINs = [1];
ALGs = [0 3 7 10];
%ALGS = 0:11;
%PPWs = [1];
PPWs = [1 2 3 4 5 7 10 20 50 150 500 1000];
%CPWs = [1];
CPWs = [1 2 3 4 5 7 10 20 50 150 500 1000];
%THREADs = [20];
THREADs = [2 4 6 8 10 12 14 16 18 20];

RUNs = [
               '/home/andersg/HLRS/results/producerconsumer_2014-12-12_17.21'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-12_18.12'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-12_19.04'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-12_19.55'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-12_20.47'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-12_21.41'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-12_22.33'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-12_23.24'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_00.49'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_01.40'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_02.32'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_03.24'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_04.15'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_05.07'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_05.59'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_07.23'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_08.15'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_09.07'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_09.58'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_10.50'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_11.42'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_12.33'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_13.25'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_14.16'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_15.08'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_16.00'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_16.51'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_17.43'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_18.35'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_19.26'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_20.18'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_21.10'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_22.01'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_22.53'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-13_23.44'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-14_01.09'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-14_02.01'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-14_02.52'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-14_03.44'
               '/home/andersg/HLRS/results/producerconsumer_2014-12-14_04.35'
       ];

i = 1;
res = [];
for d = 1:size(RUNs)(1)
  base = RUNs(d,:);
  basename = [base '/producerconsumer_result_' base(length(base) - 15 : length(base)) '-'];

  for p = PINs
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
                  [alg threads pattern pcpw throughputs RAPL_powers] = summarize_producerconsumer_case(basename, algname, casename);
                  %printf("succeeded\n");

                  res(i,:) = [f p alg threads pattern pcpw throughputs RAPL_powers];
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
endfor

csvwrite("result.res", res);
