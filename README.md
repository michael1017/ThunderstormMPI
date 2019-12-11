# ThunderstormMPI
## Structure
![](https://i.imgur.com/ICSdlt9.png)
![](https://i.imgur.com/mHbnM1v.png)

## Documentation
* Static Version
```
./mpi_s.out /tif/location/ /csv/destination/
```
* Dynamic Version
```
./mpi_d.out /tif/location/ /csv/destination/
```
these two version need to be careful that remember add the slash(/) after /tif/location and /csv/destination, or it may cause error
## Installation
1. Require
    * ImageJ
    * thumderstorm
    * open MPI
    * c++17
2. Clone the repository from github and move to ThunderstormMPI
```
git clone https://github.com/michael1017/ThunderstormMPI.git
cd ThunderstormMPI
```
3. compile all source file
```
make all
```
if you would like to compile static version only, just
```
make mpi_s
```
or only dynamic version
```
make mpi_d
```
