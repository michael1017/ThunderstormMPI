#ifndef TMPI_HPP
#define TMPI_HPP
#include <string>
#include <vector>
class ThunderStormMpi{
public:
    int size, rank;
    std::string tif_location, csv_location;
    std::vector <std::string> data_list;
    ThunderStormMpi(std::string tif_location, std::string csv_location, int size, int rank);
    void GenCSV(int i);
    void ReduceCSV();
    
private:
    void CheckLocation();
    void GenDataList();
};

#endif // !TMPI_HPP