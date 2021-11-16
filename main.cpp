#include <iostream>
#include <fstream>
#include <vector>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

json parseItem(json item) {
    json parsed;
    parsed["Latitude"] = item["params"][4];
    parsed["Longitude"] = item["params"][5];
    parsed["Altitude"] = item["params"][6];
    return parsed;
};

json generateItem(json waypoint, json pylon, int sequence) {
    pylon["Altitude"] = waypoint["Altitude"];
    pylon["doJumpId"] = sequence;
    pylon["params"][4] = waypoint["Latitude"];
    pylon["params"][5] = waypoint["Longitude"];
    pylon["params"][6] = waypoint["Altitude"];
    return pylon;
}

json rectangularPath(json launch, json pylon1, json pylon2, json mast, int margin = 5) {
    //units in meters, lat/long = coordinates

}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cout << "Usage: MAAV_Path_Generator [input].plan [output].plan [type]" << std::endl;
        return 1;
    }
    string inputFile = argv[1];
    string outputFile = argv[2];
    string pathType = argv[3];

    ifstream input;
    input.open(inputFile);
    if (!input.is_open()) {
        cout << "Usage: MAAV_Path_Generator [input].plan [output].plan" << std::endl;
        cout << "Unable to open " << inputFile << endl;
        return 1;
    }

    ofstream output;
    output.open(outputFile);
    if (!output.is_open()) {
        cout << "Usage: MAAV_Path_Generator [input].plan [output].plan" << std::endl;
        cout << "Unable to open " << outputFile << endl;
        return 1;
    }
    //files are ready

    json plan;
    input >> plan;
    plan["groundStation"] = "MAAV_path_generator";

    json launch = parseItem(plan["mission"]["items"][0]); //probably better if using a struct
    json pylon1 = parseItem(plan["mission"]["items"][1]);
    json pylon2 = parseItem(plan["mission"]["items"][2]);
    json mast = parseItem(plan["mission"]["items"][3]);

    //for testing - able to receive data
    cout << "launch: " << launch << endl;
    cout << "pylon1: " << pylon1 << endl;
    cout << "pylon2: " << pylon2 << endl;
    cout << "mast: " << mast << endl;

    if (pathType == "Rectangular") {
    //    plan["mission"]["items"] = rectangularPath(launch, pylon1, pylon2, mast);
    } else if (pathType == "Circular") {
        cout << "Circular paths are currently unsupported" << endl;
        return 1;
    } else if (pathType == "Spline") {
        cout << "Spline paths are currently unsupported" << endl;
        return 1;
    } else {
        cout << "[type] must be: Rectangular, Circular, or Spline" << endl;
        return 1;
    };

    //std::cout << "Hello, World!" << std::endl;

    //save data:
    output << plan;

    //cleanup
    input.close();
    output.close();
    return 0;
}

