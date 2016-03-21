import math
from matplotlib import pyplot
from matplotlib import colors
import ternary

beta = 0.75
sigma = 0.1
listCM = [[0.47,0.11,0.53],
          [0.27,0.18,0.73],
          [0.25,0.39,0.81],
          [0.30,0.57,0.75],
          [0.39,0.67,0.60],
          [0.51,0.73,0.44],
          [0.67,0.74,0.32],
          [0.81,0.71,0.26],
          [0.89,0.60,0.22],
          [0.89,0.39,0.18],
          [0.86,0.13,0.13]]

cmTom = colors.ListedColormap(listCM)

cmTom.set_under(color='k',alpha=None)



def generate_heatmap_data(scale=5):
    from ternary.helpers import simplex_iterator
    d = dict()
    for (i, j, k) in simplex_iterator(scale):
        d[(i, j, k)] = (i**beta * j**(1-beta))
        print i,j,k,d[(i, j, k)]
    return d

scale = 100
fontsize = 14
data = generate_heatmap_data(scale)
figure, tax = ternary.figure(scale=scale)
pyplot.axis("off")

tax.heatmap(data, cmap=cmTom, vmin=0.0) # Allow colors as rgba tuples
tax.boundary()
tax.gridlines(multiple=25, color="black")
tax.left_axis_label("$A_2$", fontsize=fontsize)
#tax.right_axis_label("Right label $\\beta^2$", fontsize=fontsize)
tax.bottom_axis_label("$A_1$", fontsize=fontsize)

# Set ticks
# tax.ticks(axis='blr',multiple=0.25, ticks=[1.0, 0.75, 0.5, 0.25, 0.0])
# tax.ticks(axis='lbr', linewidth=1)

# Remove default Matplotlib Axes
tax.clear_matplotlib_ticks()

# draw labels in Mac OSX + Anaconda
tax._redraw_labels()

tax.savefig('weak_fitness.pdf')


## import ternary

## ## Boundary and Gridlines
## scale = 40
## figure, tax = ternary.figure(scale=scale)

## # Draw Boundary and Gridlines
## tax.boundary(linewidth=2.0)
## tax.gridlines(color="black", multiple=5)
## tax.gridlines(color="blue", multiple=1, linewidth=0.5)

## # Set Axis labels and Title
## fontsize = 20
## tax.set_title("Simplex Boundary and Gridlines", fontsize=fontsize)
## tax.left_axis_label("Left label $\\alpha^2$", fontsize=fontsize)
## tax.right_axis_label("Right label $\\beta^2$", fontsize=fontsize)
## tax.bottom_axis_label("Bottom label $\\Gamma - \\Omega$", fontsize=fontsize)

## # Set ticks
## tax.ticks(axis='lbr', linewidth=1)

## # Remove default Matplotlib Axes
## tax.clear_matplotlib_ticks()

## tax.show()
