// Byron M. Hall
// project: Autonomous Perception Sensor Fusion Simulator
//This file isx for the creation of the radar data (radar_data.txt) and the camera data (camera_data.txt)

#include <iostream>
#include <fstream>
#include <iomanip>
#include <random>
#include <cstring>
#include <ctime> 
using namespace std;



int main()
{
    int object_id_r = 1; // defining the variable for the radar text file
    double relative_speed, distance;
    int object_id_c = 1; // defining variable for camera text file
    string object_type[3] = {"pedestrian", "vehicle", "bicycle"};
    int arraySize = sizeof(object_type)/sizeof(object_type[0]);
    double cfd;
    
    srand(time(0)); // seed for random
    ofstream cdata; // creating camera data file
    
    ofstream rdata; // creating radar data file
    rdata.open("radar_data.txt"); // opening radar data file
    if(rdata.is_open()){
        rdata << setw(15) << "object_id" << setw(15) << "distance (ft)" << setw(15) << "relative_speed (mph)" << endl;

        for (int i = 1; i<201; i++){ // for loop to generate random values for distance and speed for corresponding object id
            object_id_r = i; // sequientially having order for object id
            distance = (static_cast<double>(rand()) /RAND_MAX)*100 +1.0;// generating random double for disatnce 1-100ft
            relative_speed = (static_cast<double>(rand()) /RAND_MAX)*15 ;
            rdata << setw(15) << object_id_r << setw(15) << distance << setw(15) << relative_speed << endl;
            
        }
        rdata.close();
    }
    cdata.open("camera_data.txt"); // open camera data file
    if(cdata.is_open()){
        cdata << setw(15) << "object_id" << setw(15) << "object_type" << setw(15) << "confidence (0-1)" << endl;

        for (int j = 1; j<201; j++){ // for loop to generate random values for object type and confidence for corresponding object id
            object_id_c = j; // sequientially having order for object id
            int randomIndex = rand() % arraySize; // generating random index based on size of object type array
            string object = object_type[randomIndex];
            cfd = (static_cast<double>(rand()) /RAND_MAX)*1.0;// generating random double for confidence
            cdata << setw(15) << object_id_c << setw(15) << object << setw(15) << fixed << setprecision(2) << cfd << endl;
            
        }
        cdata.close();
    }


    

    
    
    
    return 0;
}

