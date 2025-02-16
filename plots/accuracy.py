import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import glob

# Define Bukin function optimal value
bukin_optimal = 0

# Dictionary to store best fitness deviations for Bukin function
fitness_data = []

# Read all CSV files
csv_files = glob.glob("../output/accuracy/*.csv")  # Adjust if files are in a subdirectory

for file in csv_files:
    if "bukin" in file:
        df = pd.read_csv(file)
        best_fitness = df["Fitness"].min()  # Extract the best fitness (lowest value)
        deviation = best_fitness - bukin_optimal  # Compute deviation from actual optimum
        fitness_data.append(deviation)

# Create boxplot for Bukin function
plt.figure(figsize=(8, 6))
plt.boxplot(
    fitness_data,
    notch=False,  # Notched boxplot for better median comparison
    patch_artist=True,  # Colored boxes
    boxprops=dict(facecolor="white", color="black"),
    medianprops=dict(color="red", linewidth=2),
    whiskerprops=dict(color="black", linewidth=1.5),
    capprops=dict(color="black", linewidth=1.5),
    flierprops=dict(marker='o', color='black', alpha=0.6)  # Style outliers
)
plt.ylim(-0.5, 0.5)
plt.axhline(y=0, color='gray', linestyle='--', linewidth=1.5)  # Centered at 0
plt.ylabel("Deviation from Actual Optimal Solution", fontsize=12)
plt.title("Boxplot of Fitness Deviations for Bukin Function", fontsize=14, fontweight="bold")
plt.grid(axis="y", linestyle="--", alpha=0.7)
plt.tight_layout()

# Save and show the plot
plt.savefig("bukin_fitness_boxplot.png", dpi=300, bbox_inches='tight')
plt.show()
