#include "TMPI.hpp"
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <mpi.h>
#include <algorithm>
#include <vector>
#include <cstdlib>

ThunderStormMpi::ThunderStormMpi(std::string tif_location, std::string csv_location, int size, int rank){
    this->tif_location = tif_location;
    this->csv_location = csv_location;
    this->size = size;
    this->rank = rank;
    CheckLocation();
    GenDataList();
}
void ThunderStormMpi::GenCSV(){
    std::string tif;
    std::string csv;
    std::string command;
    
    for (int i=rank; i<10; i+=size) {
        tif = tif_location + data_list[i] + ".tif";
        csv = csv_location + data_list[i] + ".csv";
        
        command = "./xvfb-run-safe.sh /home/sc20/eecs2205/Fiji.app/ImageJ-linux64 -macro /home/sc20/eecs2205/Fiji.app/parallel/ijmTemplate.ijm " + tif + "," + csv + " >> log.txt";
        std::cout << tif + " , " + csv << std::endl;
        std::cout << command << std::endl;
        system(command.c_str());        
        std::cout << "done " << i << std::endl;
    }
    MPI_Barrier(MPI_COMM_WORLD);
}
void ThunderStormMpi::ReduceCSV(){
    std::string data_name = data_list[0];
    std::string cmd = "awk -F\",\" 'NR==1{printf \"\\\"time\\\",%s,%s,\\\"z\\\",%s,%s,%s,%s,%s\\n\",$2,$3,$4,$5,$6,$7,$8} NR>1{printf \"1,%s,%s,%s,%s,%s,%s,%s,%s\\n\",$2,$3,$1,$4,$5,$6,$7,$8}' " + csv_location + data_name + ".csv > " + csv_location + "result.csv";
    int exec_status = system(cmd.c_str());
    for (int i = 1; i < data_list.size(); i++) {
        data_name = data_list[i];
        std::ostringstream time; 
        time << i+1; 
        std::string cmd = "awk -F\",\" 'NR>1{printf \"" + time.str() + ",%s,%s,%s,%s,%s,%s,%s,%s\\n\",$2,$3,$1,$4,$5,$6,$7,$8}' " + csv_location + data_name + ".csv >> " + csv_location + "result.csv";
        system(cmd.c_str());
    }
}
void ThunderStormMpi::GenDataList(){
    std::filesystem::directory_iterator d(tif_location);
    std::string temp;
    for(auto i : d){
        temp = i.path().filename();
        if(i.path().extension() == ".tif"){
            data_list.push_back(temp.erase(temp.length()-4));
        }
    }
    for(auto i: data_list){
        std::cout << i << std::endl;
    }
}
void ThunderStormMpi::CheckLocation(){
    //check TIF location
    bool exist = std::filesystem::is_directory(this->tif_location);
    if(!exist){
        std::cout << "In CheckLocationTIF : ";
        std::cout << this->tif_location << " not exist" << std::endl;
        exit(1);
    }

    //check CSV location
    //we won't create folder for user
    exist = std::filesystem::is_directory(this->csv_location);
    if(!exist){
        std::cout << "In CheckLocationCSV : ";
        std::cout << this->csv_location << " not exist" << std::endl;
        exit(1);
    }
}
