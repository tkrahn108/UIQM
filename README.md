# Underwater Image Quality Measure (UIQM)
## General information about the project
This repository belongs to the project Underwater Image Understanding at the University of Bielefeld in summer term 2016 (SS2016_392167).

One problem with underwater images is that they are often affected by sediment or illumination inconsistencies. In order to avoid that thousands of pictures with low quality have to be viewed by the researchers, I will try to implement an algorithm for automatic image qualitiy assesment to sort images automatically according to their quality. The implemented algorithm is based on the paper of [Panetta et al.](http://ieeexplore.ieee.org/xpl/articleDetails.jsp?arnumber=7305804 "Panetta et al.: Human-Visual-System-Inspired Underwater Image Quality Measures").

## Repository structure
* /src: All Header and Source Files of the Project
* /data: Folder for the image sets
* /doc: Tex-Documentation and Measurements of the project
* [/build]: This folder is not contained in the repository but should be created to build the project from there
* .: The root folder contains the CMake File

## Build instructions

Create a folder 'build' in the root folder of the repository and change 
the directory:

	mkdir -p build && cd build

Call the CMake file from the root directory to build the Make File for 
the project:

	cmake ..

Call the Make File to build the project (Optionally you can use more 
cores for the build process):

	make [-j4]

Call the Binary to start the Stitching Program:

	./UIQM <path_to_images>
