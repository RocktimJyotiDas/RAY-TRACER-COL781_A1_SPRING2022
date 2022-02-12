// Json
#include <json/json.h>

#include<iostream>
#include<string>
#include <typeinfo>

using namespace std;

vector<double> populate(string key, Json::Value  metadata){

	vector<double> arguments;
	arguments.reserve(metadata[key].size());

	transform(metadata[key].begin(), metadata[key].end(), back_inserter(arguments), [](const auto& e) { return e.asDouble(); });

	return arguments;
}

pair< map<string,vector<double>>, map<string,map<string,vector<double>>> >  parse_args(string input_file){

    map<string,vector<double>> args;
	map<string,map<string,vector<double>>> objects;

    // Read json
    Json::Reader reader;
	Json::Value metadata;

	std::ifstream metadata_file(input_file, std::ifstream::binary);

	metadata_file >> metadata;

	args["Camera_Position"] = populate("Camera_Position", metadata);
	args["Lookup_Position"] = populate("Lookup_Position", metadata);
	
	vector<double> scene_properties{
		metadata["Height"].asInt(),
	    metadata["Width"].asInt(), 
		metadata["anti_aliasing"].asInt(),
		metadata["threshold"].asDouble(), 
		metadata["ambientlight"].asDouble(), 
		metadata["accuracy"].asDouble(), 
		};
    
	args["scene_properties"] = scene_properties;

	map<string,vector<double>> sphere;
	sphere["Location"]	=populate("Location", metadata["objects"]["Sphere"]);
	sphere["Radius"]	=populate("Radius", metadata["objects"]["Sphere"]);
	sphere["Color"]		=populate("Color", metadata["objects"]["Sphere"]);
	sphere["Show"]		=populate("Show", metadata["objects"]["Sphere"]);

	map<string,vector<double>> triangle;

	triangle["Location"]	=populate("Location", metadata["objects"]["Triangle"]);
	triangle["Color"]		=populate("Color", metadata["objects"]["Triangle"]);
	triangle["Show"]		=populate("Show", metadata["objects"]["Triangle"]);

	map<string,vector<double>> quadric;
	quadric["Location"]	=populate("Location", metadata["objects"]["Quadric"]);
	quadric["Radius"]	=populate("Radius", metadata["objects"]["Quadric"]);
	quadric["Height"]	=populate("Height", metadata["objects"]["Quadric"]);
	quadric["Color"]	=populate("Color", metadata["objects"]["Quadric"]);
	quadric["Show"]		=populate("Show", metadata["objects"]["Quadric"]);

	map<string,vector<double>> plane;
	plane["Normal"] 	=populate("Normal", metadata["objects"]["Plane"]);
	plane["Distance"]	=populate("Distance", metadata["objects"]["Plane"]);
	plane["Color"]		=populate("Color", metadata["objects"]["Plane"]);
	plane["Show"]		=populate("Show", metadata["objects"]["Plane"]);
            
	map<string,vector<double>> cuboid;

	cuboid["Location"]	=populate("Location", metadata["objects"]["Cuboid"]);
	cuboid["Color"]		=populate("Color", metadata["objects"]["Cuboid"]);
	cuboid["Show"]		=populate("Show", metadata["objects"]["Cuboid"]);

	objects["Sphere"]    = sphere;
	objects["Triangle"]  = triangle;
	objects["Quadric"]   = quadric;
	objects["Plane"]     = plane;
	objects["Cuboid"]    = cuboid;

	// Light
	args["Light_Source"]=populate("Light_Source", metadata);

	cout<<metadata<<endl; //This will print the entire json object.


    return make_pair(args, objects);
}