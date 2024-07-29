import re
import numpy as np
import subprocess
import polars as pl

res_sum_regex = "TOTAL: RES_SUM: (([+-]?(?=\.\d|\d)(?:\d+)?(?:\.?\d*))(?:[Ee]([+-]?\d+))?)"
lat_sum_regex = "TOTAL: LAT_SUM: (([+-]?(?=\.\d|\d)(?:\d+)?(?:\.?\d*))(?:[Ee]([+-]?\d+))?)"
spfm_regex = "SPFM:  (([+-]?(?=\.\d|\d)(?:\d+)?(?:\.?\d*))(?:[Ee]([+-]?\d+))?)%"
lfm_regex = "LFM:   (([+-]?(?=\.\d|\d)(?:\d+)?(?:\.?\d*))(?:[Ee]([+-]?\d+))?)%"

result_dict = {}

for dram_fit in np.logspace(-2, 4, num=20):
    proc = subprocess.run(['build/dram-metrics-refactored', str(dram_fit)], stdout=subprocess.PIPE)
    output = proc.stdout.decode('utf-8')

    result_res = re.search(res_sum_regex, output)
    result_lat = re.search(lat_sum_regex, output)
    result_spfm = re.search(spfm_regex, output)
    result_lfm = re.search(lfm_regex, output)

    res = result_res.group(1)
    lat = result_lat.group(1)
    spfm = result_spfm.group(1)
    lfm = result_lfm.group(1)

    result_dict.setdefault('dram_fit', []).append(dram_fit)
    result_dict.setdefault('res', []).append(res)
    result_dict.setdefault('lat', []).append(lat)
    result_dict.setdefault('spfm', []).append(spfm)
    result_dict.setdefault('lfm', []).append(lfm)

df = pl.from_dict(result_dict)
df.write_csv('results.csv')
print(df)

for dram_fit, res in df.select(["dram_fit", "res"]).rows():
    print(f"({dram_fit}, {res})")

for dram_fit, lat in df.select(["dram_fit", "lat"]).rows():
    print(f"({dram_fit}, {lat})")

for dram_fit, spfm in df.select(["dram_fit", "spfm"]).rows():
    print(f"({dram_fit}, {spfm})")

for dram_fit, lfm in df.select(["dram_fit", "lfm"]).rows():
    print(f"({dram_fit}, {lfm})")
