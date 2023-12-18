#!/usr/bin/env python2

import sys, pandas

sys.path.insert(1, '/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/tools')
import sniper_stats


mpki_4 = [78.96 , 41.22 , 37.50 , 48.73 , 59.79 , 51.03 , 45.08 , 43.56 , 56.99 , 55.64 , 54.89 , 39.17 , 57.83 , 53.48 , 50.22 , 46.96 , 99.04]
mpki_avg_4 = sum(mpki_4)/len(mpki_4)

mpki_8 = [50.85 ,4.59 ,3.91 ,5.59 ,7.25 ,5.96 ,4.37 ,5.07 ,6.89 ,7.26 ,6.61 ,3.77 ,3.35 ,3.44 ,3.10 ,3.42 ,6.38 ]
mpki_avg_8 = sum(mpki_8)/len(mpki_8)

mpki_16 = [30.91 , 5.05 , 4.16 , 6.05 , 7.37 , 5.88 , 4.54 , 4.79 , 5.33 , 7.48 , 6.40 , 5.43 , 3.44 , 3.47 , 2.92 , 3.21 , 6.38 ]
mpki_avg_16 = sum(mpki_16)/len(mpki_16)

mpki_32 = [21.88 , 5.00 , 4.53 , 5.07 , 7.91 , 5.46 , 4.78 , 5.05 , 6.56 , 7.07 , 6.35 , 3.60 , 3.47 , 3.44 , 3.34 , 3.11 , 6.48 ]
mpki_avg_32 = sum(mpki_32)/len(mpki_32)



mpki = [mpki_avg_4,mpki_avg_8,mpki_avg_16,mpki_avg_32]

ways = [4,8,16,32]
benchmarks = ['blackscholes']

table = {}

for benchmark in benchmarks:
    for way in ways:
        dir = "/home/dhruv/Comp_Arch_Projects/FinalProject/results/set_associative/{}way/{}/".format(way,benchmark)
        stats = sniper_stats.SniperStats(resultsdir=dir)
        results	=stats.get_results()	

        total_instrs = sum(results['results']['performance_model.instruction_count'])
        total_cycles = sum(results['results']['performance_model.elapsed_time'])*float(results['config']['perf_model/core/frequency'])*1e-6
        CPI = total_cycles/total_instrs

        sys.path.append(dir)
        import power

        pwr = power.power

        AREA = pwr['L2'][0]['Area']

        LEAKAGE = pwr['L2'][0]['Gate Leakage']

        table[way] = {"Area":AREA, "Leakage": LEAKAGE, "CPI":CPI, "MPKI": mpki[ways.index(way)]}

df = pandas.DataFrame(table)
df.to_csv("Table1")