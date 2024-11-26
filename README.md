Persistent Data Structures Repository
This repository implements persistent data structures in C++ with applications in computational geometry. 
Persistent data structures allow access to previous versions of the data structure after modifications, enabling powerful query capabilities. This project includes:

Features
1. Partial Persistent Binary Search Tree (Partial BST)
Implements a partial persistent binary search tree where:
Queries can be performed on any previous version of the tree.
Only the most recent version of the tree can be updated.
Efficiently supports temporal queries on past states.

3. Full Persistent Binary Search Tree (Full BST)
Implements a fully persistent binary search tree where:
Both queries and updates can be performed on any version of the tree.
Supports scenarios requiring concurrent access and modification of historical states.

5. Planar Point Problem (Application)
Demonstrates the application of persistent data structures for solving geometric problems, specifically planar point queries:
Input consists of points and lines in 2D space.
Operations involve efficiently querying and visualizing lines relative to a point using the persistent structure.
Additional Scripts


Python Visualization (line.py)
A Python script to visualize lines and points using matplotlib. It reads input data from a file and plots lines and a query point. 
The script also saves the plot as an image (output_plot.png).

Input Format
The script expects the input data to be in the following format, stored in a file (e.g., input_data.txt):

Each line represents a line segment in the form:
x1 y1 x2 y2
where (x1, y1) and (x2, y2) are the endpoints of a line segment.
The last line represents a query point in the form:
x y
Example Input (input_data.txt)
10 10 50 50
20 30 70 70
40 20 80 90
30 30

Output
Plot of the lines and the query point:
Lines are plotted in default or blue color for emphasis.
The query point is marked in red.
The plot is saved as output_plot.png and displayed briefly.
Code Usage
To execute the visualization:

Folder Structure
partial_bst.cpp: Implementation of the partial persistent binary search tree.
full_bst.cpp: Implementation of the fully persistent binary search tree.
planar_point.cpp: Application of persistent data structures for planar point problems.
line.py: Python script for visualizing lines and points.
