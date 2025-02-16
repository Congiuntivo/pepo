import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import glob


# Read CSV files with simulation results
threads_df = pd.read_csv("../output/parallelization/threads.csv")


# Compute Speedup
threads_df["speedup"] = threads_df.groupby("n_pengs")["time"].transform(lambda x: x.iloc[0] / x)
# Compute Efficiency
threads_df["efficiency"] = threads_df["speedup"] / threads_df["threads"]

# Separate DataFrames
df_80k = threads_df[threads_df["n_pengs"] == 1000]
df_60k = threads_df[threads_df["n_pengs"] == 750]
df_40k = threads_df[threads_df["n_pengs"] == 500]
df_20k = threads_df[threads_df["n_pengs"] == 250]


# Plot Speedup Graph
plt.figure(figsize=(8, 5))
plt.plot(df_80k["threads"], df_80k["speedup"], marker="o", linestyle="-", label="Population = 1000")
plt.plot(df_60k["threads"], df_60k["speedup"], marker="o", linestyle="-", label="Population = 750")
plt.plot(df_40k["threads"], df_40k["speedup"], marker="o", linestyle="-", label="Population = 500")
plt.plot(df_20k["threads"], df_20k["speedup"], marker="o", linestyle="-", label="Population = 250")
# Labels & Title
plt.xlabel("Number of Threads")
plt.ylabel("Speedup")
plt.title("Speedup vs. Number of Threads")
plt.legend()
plt.grid(True)

# Show plot
plt.savefig("speedup_vs_threads.png", dpi=300, bbox_inches='tight')

# Plot Efficiency Graph
plt.figure(figsize=(8, 5))
plt.plot(df_80k["threads"], df_80k["efficiency"], marker="o", linestyle="-", label="Population = 1000")
plt.plot(df_60k["threads"], df_60k["efficiency"], marker="o", linestyle="-", label="Population = 750")
plt.plot(df_40k["threads"], df_40k["efficiency"], marker="o", linestyle="-", label="Population = 500")
plt.plot(df_20k["threads"], df_20k["efficiency"], marker="o", linestyle="-", label="Population = 250")
# Labels & Title
plt.ylim(0, 1.3)
plt.xlabel("Number of Threads")
plt.ylabel("Efficiency")
plt.title("Efficiency vs. Number of Threads")
plt.legend()
plt.grid(True)

# Show plot
plt.savefig("efficiency_vs_threads.png", dpi=300, bbox_inches='tight')
