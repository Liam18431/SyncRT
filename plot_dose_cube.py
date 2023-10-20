import sys
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
from mpl_toolkits.mplot3d import Axes3D


#Open dose cube file
if(len(sys.argv) < 2): 
    print("\n Error: Input argument required.\n")
    sys.exit()
filename =str(sys.argv[1])
file = open(filename, "rb")

#Read header info
size_n = np.fromfile(file, dtype=np.uint64, count =1)
print("Total size: ", size_n)

size = np.fromfile(file, dtype=np.uint64, count=3)
size_x = int(size[0])
size_y = int(size[1])
size_z = int(size[2])
print("Size: ", size)

dim = np.fromfile(file, dtype=np.longdouble, count=3)
dim_x = dim[0]
dim_y = dim[1]
dim_z = dim[2]
print("Dimension (mm): ", dim)

res = np.fromfile(file, dtype=np.longdouble, count=3)
res_x = res[0]
res_y = res[1]
res_z = res[2]
print("Resolution (mm/voxel): ", res)

#Read the dose data
dose_n = size_x *size_y *size_z
dose = np.fromfile(file, dtype=np.longdouble, count=dose_n)
dose = dose.reshape((size_x, size_y, size_z), order='F')
print("Dose cube shape: ", dose.shape)

#Get the max dose
dose_max = np.amax(dose)
print("Max dose (pGy): ", dose_max)

#Read the uncertainty data
unc = np.fromfile(file, dtype=np.longdouble, count=dose_n)
unc = unc.reshape((size_x, size_y, size_z), order='F')

#Calculate the average uncertainty
unc_squared = np.where(dose > dose_max /2., unc*unc, 0)
avg_unc = np.sqrt(np.average(unc_squared[unc_squared !=0]))
print("Average uncertainty (%): ", avg_unc *100)

#Prepare voxels
range_x = 0, size_x
range_y = 0, size_y
range_z = 0, size_z

x =(np.arange(range_x[0], range_x[1], 1) +0.5) *res_x
y =(np.arange(range_y[0], range_y[1], 1) +0.5) *res_y -dim_y/2.
z =(np.arange(range_z[0], range_z[1], 1) +0.5) *res_z -dim_z/2.

xy = np.meshgrid(x,y)
yz = np.meshgrid(y,z)

#Window 1
fig =plt.figure()
fig.canvas.manager.set_window_title('2D Dose')

#Plot the pdd on central axis
ax =plt.subplot(221)
dose_pdd = dose[range_x[0]:range_x[1], int(size_y/2), int(size_z/2)]
ax.plot(x, dose_pdd)

ax.set_xlabel('Depth (mm)')
ax.set_ylabel('Dose (pGy)')

#Plot the profile at surface
ax =plt.subplot(223)
dose_profile = dose[0, range_y[0]:range_y[1], int(size_z/2)]
ax.plot(y, dose_profile)

ax.set_xlabel('Width (mm)')
ax.set_ylabel('Dose (pGy)')

#Plot y,z scatter at surface
ax =plt.subplot(122)
dose_map =np.transpose(dose[0,range_y[0]:range_y[1],range_z[0]:range_z[1]])
ax.pcolor(yz[0], yz[1], dose_map, cmap='viridis')

ax.set_xlabel('Width (mm)')
ax.set_ylabel('Height (mm)')

#Window 2
fig =plt.figure()
fig.canvas.manager.set_window_title('3D Dose')

#Plot the x,y (depth, width) dose field as a surface plot.
ax = plt.subplot(121, projection='3d')
dose_field_xy =np.transpose(dose[range_x[0]:range_x[1],range_y[0]:range_y[1],int(size_z/2)])
ax.plot_surface(xy[0], xy[1], dose_field_xy, cmap='viridis', linewidth=0, rstride=1, cstride=1, antialiased=False)

ax.set_xlabel('Depth (mm)')
ax.set_ylabel('Width (mm)')
ax.set_zlabel('Dose (pGy)')

#Plot the y,z (width, height) dose field at the cube surface as a surface plot
ax = plt.subplot(122, projection='3d')
dose_field_yz =np.transpose(dose[0,range_y[0]:range_y[1],range_z[0]:range_z[1]])
ax.plot_surface(yz[0], yz[1], dose_field_yz, cmap='viridis', linewidth=0, rstride=1, cstride=1, antialiased=False)

ax.set_xlabel('Width (mm)')
ax.set_ylabel('Height (mm)')
ax.set_zlabel('Dose (pGy)')

#Plot
plt.show()