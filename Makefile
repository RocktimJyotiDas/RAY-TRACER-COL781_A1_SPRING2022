#export LD_LIBRARY_PATH=/home/kunaldargan/anaconda3/lib

g++ main.cpp -L/usr/local/lib -ljsoncpp -lglfw3 -lrt -lm -ldl -lGL -lGLU -lGLEW -lblas -llapack -lpthread -larmadillo -L/home/kunaldargan/anaconda3/lib -lhdf5 -lmkl_rt