import pandas as pd
import matplotlib.pyplot as plt
import imageio.v3 as iio
from io import BytesIO
import argparse


ap = argparse.ArgumentParser()

ap.add_argument(
    "-i", "--input", type=str, default="output.csv", help="Path to the input CSV file"
)
ap.add_argument(
    "-o",
    "--output",
    type=str,
    default="positions.gif",
    help="Output file name for the GIF",
)
ap.add_argument(
    "-f", "--fps", type=int, default=1, help="Frames per second for the GIF"
)
ap.add_argument(
    "-r",
    "--resolution",
    type=int,
    default=200,
    help="DPI resolution for smaller GIF sizes",
)
ap.add_argument(
    "-c",
    "--color",
    type=str,
    default="red",
    help="Color to highlight the best point (lowest fitness)",
)

args = ap.parse_args()

# --- Adjustable Parameters ---
gif_output_file = args.output  # Output file name for the GIF
fps = args.fps  # Frames per second for the GIF
plot_resolution = args.resolution  # DPI resolution for smaller GIF sizes
# Color to highlight the best point (lowest fitness)
highlight_color = args.color

# --- Step 1: Read and Parse Data ---

# Read the CSV file
df = pd.read_csv(args.input)

# Split 'Position' into 'x' and 'y' columns
positions = df["Position"].str.split("_", expand=True)
positions.columns = ["x", "y"]
df = pd.concat([df, positions], axis=1)

# Convert 'x' and 'y' to float
df["x"] = df["x"].astype(float)
df["y"] = df["y"].astype(float)

# Determine consistent x and y axis limits for all plots
xmin = df["x"].min() - 10
xmax = df["x"].max() + 10
ymin = df["y"].min() - 10
ymax = df["y"].max() + 10

# --- Step 2: Prepare GIF Frames ---

images = []  # Store collected images for the GIF
iterations = sorted(df["Iteration"].unique())  # Unique sorted iterations

# Iterate over all iterations
for iteration in iterations:
    # Filter out data for the current iteration
    iteration_data = df[df["Iteration"] == iteration]
    x = iteration_data["x"]
    y = iteration_data["y"]

    # Find the best point based on Fitness (lowest Fitness value)
    best_point = iteration_data.loc[iteration_data["Fitness"].idxmin()]
    best_x = best_point["x"]
    best_y = best_point["y"]

    # Create the plot for this iteration
    plt.figure(figsize=(6, 6))
    plt.scatter(x, y, label="Positions")
    plt.scatter(
        best_x,
        best_y,
        color=highlight_color,
        label="Best Fitness",
        edgecolor="black",
        s=100,
        zorder=3,
    )
    plt.title(f"Iteration {iteration}", fontsize=14)
    plt.xlabel("X Position", fontsize=12)
    plt.ylabel("Y Position", fontsize=12)
    plt.xlim(xmin, xmax)
    plt.ylim(ymin, ymax)
    plt.legend(loc="upper left", fontsize=10)

    # Save plot to a BytesIO buffer
    buf = BytesIO()
    plt.savefig(buf, format="png", dpi=plot_resolution)
    plt.close()  # Close plot to save memory
    buf.seek(0)

    # Append the frame to the GIF list
    frame = iio.imread(buf)
    images.append(frame)

# --- Step 3: Create Final GIF ---

# Save all collected images as a single GIF file
# iio.imwrite(gif_output_file, images, duration=gif_duration)
iio.imwrite(gif_output_file, images, fps=fps)
print(f"GIF saved as: {gif_output_file}")
