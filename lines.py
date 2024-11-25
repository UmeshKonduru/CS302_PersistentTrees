import matplotlib.pyplot as plt
import time

# Read input data from the file
lines = []
point = None

with open("input_data.txt", "r") as file:
    data = file.readlines()

# Extract lines
for line in data[:-1]:  # All lines except the last one
    x1, y1, x2, y2 = map(int, line.split())
    lines.append(((x1, y1), (x2, y2)))

# Extract point
x, y = map(int, data[-1].split())
point = (x, y)

# Create a figure and axis
fig, ax = plt.subplots(figsize=(8, 8))

# Plot the lines
for i, line in enumerate(lines):
    (x1, y1), (x2, y2) = line
    if i == len(lines) - 1:  # For the last line
        ax.plot([x1, x2], [y1, y2], linewidth=4, color='blue')
    else:
        ax.plot([x1, x2], [y1, y2], linewidth=2)

# Plot the point
ax.scatter(*point, color='red', zorder=5)

# Set axis limits
ax.set_xlim(0, 100)
ax.set_ylim(0, 100)

# Add labels and title
ax.set_xlabel("X-axis")
ax.set_ylabel("Y-axis")
ax.set_title("Lines and Point Visualization")


# Show grid
ax.grid(True, linestyle="--", alpha=0.7)

# Show the plot
plt.savefig("output_plot.png")

# Show the plot for 2 seconds
plt.show(block=False)
time.sleep(1)
plt.close()
