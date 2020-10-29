#ifndef SERIALIZATION_H_
#define SERIALIZATION_H_

#include <iostream> 
#include <map>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>

std::string serialize(std::map<std::string, std::vector<double>> model);
std::map<std::string, std::vector<double>> deserialize(std::string serialized_str);

#endif 
