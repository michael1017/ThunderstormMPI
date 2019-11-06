#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <mpi.h>
#include <algorithm>
#include <vector>
#include <cstdlib>
class ThunderStormMpi{
public:
    int size, rank;
    std::string tif_location, csv_location;
    std::vector <std::string> data_list;
    std::vector <std::string> pure_data_list; //without .tif 
    ThunderStormMpi(std::string tif_location, std::string csv_location);
    void GenCSV(int size, int rank);
    void AddTimeColumnCSV();
    void ReduceCSV();
    
private:
    void GenDataList();
    void CheckSizeAndRank();
    void CheckLocationTIF();
    void CheckLocationCSV();
    void GenIJM();
};
ThunderStormMpi::ThunderStormMpi(std::string tif_location, std::string csv_location){
    this->tif_location = tif_location;
    this->csv_location = csv_location;
    
    CheckLocationTIF();
    CheckLocationCSV();
}
void ThunderStormMpi::GenCSV(int size, int rank){
    this->size = size;
    this->rank = rank;
    GenDataList();
    std::string ijm = csv_location + "/" + pure_data_list[rank] + ".ijm";
    std::string command = "./xvfb-run-safe.sh /home/sc20/eecs2205/Fiji.app/ImageJ-linux64 -macro " + ijm;
    GenIJM();
    system(command.c_str());
}
void ThunderStormMpi::AddTimeColumnCSV(){
    std::string csv = csv_location + "/" + pure_data_list[rank] + ".csv";
    std::string command = "python3 test.py " + csv + " " + std::to_string(rank);
    system(command.c_str());
}
void ThunderStormMpi::ReduceCSV(){
    system("cat *.csv > new.csv");
}
void ThunderStormMpi::GenDataList(){
    std::filesystem::directory_iterator d(tif_location);
    std::string temp;
    for(auto i : d){
        temp = i.path().filename();
        if(i.path().extension() == ".tif"){
            data_list.push_back(temp);
            pure_data_list.push_back(temp.erase(temp.length()-4));
        }
    }
    for(auto i: data_list){
        std::cout << i << std::endl;
    }
    for(auto i: pure_data_list){
        std::cout << i << std::endl;
    }
    
}
void ThunderStormMpi::CheckSizeAndRank(){
    std::cout << "In CheckSizeAndRank : ";
    if(size < rank){
        std::cout << "ERROR: some rank bigger than size" << std::endl;
        exit(1);
    }
}
void ThunderStormMpi::CheckLocationTIF(){
    bool exist = std::filesystem::is_directory(this->tif_location);
    if(!exist){
        std::cout << "In CheckLocationTIF : ";
        std::cout << this->tif_location << " not exist" << std::endl;
        exit(1);
    }
}
void ThunderStormMpi::CheckLocationCSV(){
    bool exist = std::filesystem::is_directory(this->csv_location);
    if(!exist){
        std::cout << "In CheckLocationCSV : ";
        std::cout << this->csv_location << " not exist" << std::endl;
        exit(1);
    }
}
void ThunderStormMpi::GenIJM(){
    std::string csv = csv_location + "/" + pure_data_list[rank] + ".csv";
    std::string tif = tif_location + "/" + data_list[rank];
    std::string ijm = csv_location + "/" + pure_data_list[rank] + ".ijm";
    std::ofstream ofs(ijm);
    ofs << "open(\"" + tif + "\");\n";
    ofs << "run(\"Run analysis\", \"filter=[Wavelet filter (B-Spline)] scale=2.0 order=3 detector=[Local maximum] connectivity=8-neighbourhood threshold=1.5*std(Wave.F1) estimator=[PSF: Integrated Gaussian] sigma=1.6 fitradius=4 method=[Maximum likelihood] full_image_fitting=false mfaenabled=false renderer=[Averaged shifted histograms] magnification=5.0 colorize=false threed=false shifts=2 repaint=50\");\n";
    ofs << "run(\"Export results\", \"floatprecision=1 filepath=\" + \"" + csv + "\" + \" fileformat=[CSV (comma separated)] sigma=true intensity=true offset=true saveprotocol=false x=true y=true bkgstd=true id=false uncertainty_xy=true frame=true\");\n";
    ofs << "run(\"Quit\");";
}
int main(int argc, char** argv){
    //suppose size == 10;
    MPI_Init(&argc, &argv);
    int rank, size;  
    std::string tif(argv[1]);
    std::string csv(argv[2]);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    ThunderStormMpi t(tif, csv);
    t.GenCSV(size, rank);
    t.AddTimeColumnCSV();
    
    MPI_Barrier(MPI_COMM_WORLD);
    if(rank == 0) {
        t.ReduceCSV();
    }
    MPI_Finalize();
    return 0;
}
