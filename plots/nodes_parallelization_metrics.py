import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import glob


# Read CSV files with simulation results
nodes_df = pd.read_csv("../output/parallelization/nodes.csv")
weak_df = pd.read_csv("../output/parallelization/weak_scalability.csv")


# Compute Speedup
nodes_df["speedup"] = nodes_df.groupby("n_pengs")["time"].transform(lambda x: x.iloc[0] / x)
# Compute Efficiency
nodes_df["efficiency"] = nodes_df["speedup"] / nodes_df["nodes"]

# Separate DataFrames
df_100k = nodes_df[nodes_df["n_pengs"] == 100000]
df_80k = nodes_df[nodes_df["n_pengs"] == 80000]
df_60k = nodes_df[nodes_df["n_pengs"] == 60000]
df_40k = nodes_df[nodes_df["n_pengs"] == 40000]
df_20k = nodes_df[nodes_df["n_pengs"] == 20000]

# Compute Weak Scalability
weak_df["weak_scalability"] = weak_df["time"].iloc[0] / weak_df["time"]

# Plot Speedup Graph
plt.figure(figsize=(8, 5))
plt.plot(df_100k["nodes"], df_100k["speedup"], marker="o", linestyle="-", label="Population = 100000")
plt.plot(df_80k["nodes"], df_80k["speedup"], marker="o", linestyle="-", label="Population = 80000")
plt.plot(df_60k["nodes"], df_60k["speedup"], marker="o", linestyle="-", label="Population = 60000")
plt.plot(df_40k["nodes"], df_40k["speedup"], marker="o", linestyle="-", label="Population = 40000")
plt.plot(df_20k["nodes"], df_20k["speedup"], marker="o", linestyle="-", label="Population = 20000")
# Labels & Title
plt.xlabel("Number of Nodes")
plt.ylabel("Speedup")
plt.title("Speedup vs. Number of Nodes")
plt.legend()
plt.grid(True)

# Show plot
plt.savefig("speedup_vs_nodes.png", dpi=300, bbox_inches='tight')

# Plot Efficiency Graph
plt.figure(figsize=(8, 5))
plt.plot(df_100k["nodes"], df_100k["efficiency"], marker="o", linestyle="-", label="Population = 100000")
plt.plot(df_80k["nodes"], df_80k["efficiency"], marker="o", linestyle="-", label="Population = 80000")
plt.plot(df_60k["nodes"], df_60k["efficiency"], marker="o", linestyle="-", label="Population = 60000")
plt.plot(df_40k["nodes"], df_40k["efficiency"], marker="o", linestyle="-", label="Population = 40000")
plt.plot(df_20k["nodes"], df_20k["efficiency"], marker="o", linestyle="-", label="Population = 20000")
# Labels & Title
plt.ylim(0, 1.3)
plt.xlabel("Number of Nodes")
plt.ylabel("Efficiency")
plt.title("Efficiency vs. Number of Nodes")
plt.legend()
plt.grid(True)

# Show plot
plt.savefig("efficiency_vs_nodes.png", dpi=300, bbox_inches='tight')

# Plot Weak Scalability Graph
plt.figure(figsize=(8, 5))
plt.plot(weak_df["nodes"], weak_df["weak_scalability"], marker="o", linestyle="-")
plt.ylim(0, 1.2)

# Add labels for 'n_pengs' close to the points
for i, txt in enumerate(weak_df["n_pengs"]):
    plt.text(weak_df["nodes"].iloc[i], weak_df["weak_scalability"].iloc[i] - 0.06, str(txt), 
             fontsize=9, ha='left', va='bottom')

# Labels & Title
plt.xlabel("Number of Nodes")
plt.ylabel("Efficiency")
plt.title("Weak Scalability")
plt.legend()
plt.grid(True)

# Show plot
plt.savefig("weak_scalability.png", dpi=300, bbox_inches='tight')


