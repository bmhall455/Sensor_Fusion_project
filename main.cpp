// Byron M. Hall
// project: Autonomous Perception Sensor Fusion Simulator
// main file for fusing displaying and logging data and decisions
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <algorithm>  // For std::min
using namespace std;

// Structures for sensor data

// defining structure of radar data to be extracted
struct RadarData {
    int object_id;
    double distance;
    double relative_speed;
};

// defining structure of camera data to be extracted
struct CameraData {
    int object_id;
    char object[20];
    double confidence;
};


// defining structure for fused data of camera and radar
struct FusedData {
    int object_id;
    char object[20];
    double confidence;
    double distance;
    double relative_speed;
};

class PerceptionSystem {
public:
    // Maximum records and thresholds (constants)
    static const int MAX_RECORDS = 200;
    
private:
    // Member variables for storing sensor data
    RadarData radar[MAX_RECORDS];
    CameraData camera[MAX_RECORDS];
    FusedData fused[MAX_RECORDS];
    
    int numRadar; // defined seperately just in case there is a discrepancy
    int numCamera;
    int numFused;
    
    // Decision function thresholds
    const double dist_threshold;
    const double cfd_threshold;

public:
    // Constructor initializes counters and thresholds.
    PerceptionSystem()
        : numRadar(0), numCamera(0), numFused(0),
          dist_threshold(70.0),    // about 70ft 
          cfd_threshold(0.8) { } // 80% confidence required

    // method: Read radar data from file and parse into radarData structure
    void readRadarData(const char filename[]) {
        ifstream inFile(filename);
        if (!inFile) {
            cerr << "\nError: Unable to open " << filename << endl;// make sure file can be opened
            return;
        }
        // Skip header line
        char header[100];
        inFile.getline(header, 100);
        numRadar = 0;//initiallizing record counter
        // read each line and record it to appriate part of array
        while (inFile >> radar[numRadar].object_id
                      >> radar[numRadar].distance
                      >> radar[numRadar].relative_speed) {
            numRadar++;
            if (numRadar >= MAX_RECORDS) //exit loop once records are recorded
                break;
        }
        inFile.close();
    }

    // method: Read camera data from file and parse in cameraData structure
    void readCameraData(const char filename[]) {
        ifstream inFile(filename);
        if (!inFile) {
            cerr << "\nError: Unable to open " << filename << endl; // make sure file can be opened
            return;
        }
        // Skip header line
        char header[100]; // reads first 100 characters (char) assuming this is header
        inFile.getline(header, 100);
        numCamera = 0; //initiallizing record counter
        // read each line and record it to appriate part of array
        while (inFile >> camera[numCamera].object_id
                      >> camera[numCamera].object
                      >> camera[numCamera].confidence) {
            numCamera++;
            if (numCamera >= MAX_RECORDS) //exit loop once records are recorded
                break;
        }
        inFile.close();
    }

    // method: Fuse radar and camera data.
    // In this example, we assume that the records align so that
    // the  radar record corresponds to the camera record based on object id
    void fuseData() {
        int limit = min(numRadar, numCamera);
        for (int i = 0; i < limit && radar[i].object_id ==camera[i].object_id; i++) {
            fused[i].object_id = radar[i].object_id;
            strcpy(fused[i].object, camera[i].object);
            fused[i].confidence = camera[i].confidence;
            fused[i].distance = radar[i].distance;
            fused[i].relative_speed = radar[i].relative_speed;
        }
        numFused = limit; // make sure correct number of scenarios are fused
    }

    // method: Print fused data.
    void printFusedData() const {
        cout << "\nFused Data shown below:" << endl;
        cout << setw(30) << "obj id" << setw(30) << "obj type" 
             << setw(30) << "confidence" << setw(30) << "obj distance" 
             << setw(30) << "obj relative speed (mph)" << endl;
        for (int i = 0; i < numFused; i++) {
            cout << setw(30) << fused[i].object_id << setw(30) << fused[i].object
                 << setw(30) << fused[i].confidence << setw(30) << fused[i].distance
                 << setw(30) << fused[i].relative_speed << endl;
        }
    }
    
    //method: Decide actions and log to a file.
    void logAndDecide(const char logFilename[]) {
        int brake_counter_terminal = 0;
        int brake_counter_log = 0;
        
        ofstream plog(logFilename);
        if (plog.is_open()) {
            plog << setw(15) << "object_id" 
                 << setw(40) << "Decision" 
                 << setw(20) << "object avoided" << endl;
            for (int i = 0; i < numFused; i++) {
                // Compute decision once
                bool shouldBrake = (fused[i].distance < dist_threshold && fused[i].confidence > 0.5);
                if (shouldBrake) {
                    plog << setw(15) << fused[i].object_id 
                         << setw(40) << "breaking for object" 
                         << setw(20) << fused[i].object << endl;
                    brake_counter_log++;
                    cout << "\nSending brake command for object: " << fused[i].object_id << endl;
                    brake_counter_terminal++;
                } else {
                    plog << setw(15) << fused[i].object_id 
                         << setw(40) << "continuing to destination object" 
                         << setw(20) << fused[i].object << endl;
                    cout << "\nContinue along path for object: " << fused[i].object_id << endl;
                }
            }
            cout << "\nNumber of close calls: " << brake_counter_terminal << endl;
            plog << "Collisions avoided: " << brake_counter_log << endl;
            plog.close();
        }
    }
};

int main() {
    PerceptionSystem system;
    
    // Read sensor files
    system.readRadarData("radar_data.txt");
    system.readCameraData("camera_data.txt");
    
    // Fuse the sensor data
    system.fuseData();
    
    // Print fused sensor data to terminal
    system.printFusedData();
    
    // Make decisions and write to perception_log.txt
    system.logAndDecide("perception_log.txt");
    
    return 0;
}