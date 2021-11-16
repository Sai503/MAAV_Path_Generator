#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include "json.hpp"
#include "WGS84toCartesian.hpp"
#include "Eigen/Dense"

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
    //can't just treat it like x and y b/c the earth is round!!
    //not sure if cartesian coordinates are also in meters... functions relative to reference
    //todo figure out actual units
    //todo best way to do geographic Coordinate Math
    array<double, 2> roundLaunch = {launch["Latitude"], launch["Longitude"]};
    array<double, 2> cartLaunch = wgs84::toCartesian(roundLaunch, roundLaunch); //reference, convert

    array<double, 2> roundPylon1 = {pylon1["Latitude"], pylon1["Longitude"]};
    array<double, 2> cartPylon1 = wgs84::toCartesian(roundLaunch, roundPylon1); //reference, convert

    array<double, 2> roundPylon2 = {pylon2["Latitude"], pylon2["Longitude"]};
    array<double, 2> cartPylon2 = wgs84::toCartesian(roundLaunch, roundPylon2); //reference, convert

    array<double, 2> roundMast = {mast["Latitude"], mast["Longitude"]};
    array<double, 2> cartMast = wgs84::toCartesian(roundLaunch, roundMast); //reference, convert

    //as a test
    cout << "roundpylon1: " << roundPylon1[0] << roundPylon1[1] << endl;
    cout << "cartpylon1: " << cartPylon1[0] << cartPylon1[1] << endl;
    //should be able to do normal math on cartesian coordinates
    //using vector equations to easily get points along the line

    //convert back to wgs84 coordinates (system used by gps)
    //todo: find documentation on QGroundControl's coordinate system
    Eigen::Vector2d vecLaunch(cartLaunch[0], cartLaunch[1]);
    //Eigen::Vector2d vecLaunch(cartLaunch[0], cartLaunch[1]);

    //return new path
    json j1;
    return j1;

}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cout << "Usage: MAAV_Path_Generator [input].plan [output].plan [type]" << std::endl;
        return 1;
    }
    std::cout << "Warning: May not properly account for curvature of the earth" << endl;
    cout << "Warning: Does not consider vertical distances/optimizations" << endl;
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
        //looks like cartesian coordinates are in meters
        plan["mission"]["items"] = rectangularPath(launch, pylon1, pylon2, mast);
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

