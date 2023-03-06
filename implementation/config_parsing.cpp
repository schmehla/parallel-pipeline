#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace ConfigParsing {
    struct ParsedConfig {
        std::string stages_str;
        std::vector<std::string> mapping_str;
        std::string input_str;
    };

    ParsedConfig parse_config(std::string config_loc) {
        enum ParsingState {READING_STAGES, READING_MAPPING, READING_INPUT, SKIPPING};
        ParsingState current_state = SKIPPING;

        ParsedConfig parsed_config;

        std::ifstream config;
        config.open(config_loc);
        if (config.fail()) {
            throw std::runtime_error("Could not find file: " + config_loc);
        }
        std::string line;
        while (std::getline(config, line)) {
            if (line.rfind("stages:", 0) == 0) {
                current_state = READING_STAGES;
            } else if (line.rfind("mapping:", 0) == 0) {
                current_state = READING_MAPPING;
            } else if (line.rfind("input:", 0) == 0) {
                current_state = READING_INPUT;
            } else {
                current_state = SKIPPING;
            } 

            if (current_state == READING_STAGES) {
                std::getline(config, parsed_config.stages_str);
                current_state = SKIPPING;
                continue;
            }
            if (current_state == READING_MAPPING) {
                std::getline(config, line);
                while (line.length() != 0) {
                    parsed_config.mapping_str.push_back(line);
                    std::getline(config, line);
                }
                current_state = SKIPPING;
                continue;
            }
            if (current_state == READING_INPUT) {
                std::getline(config, parsed_config.input_str);
                continue;
            }
        }
        config.close();
        return parsed_config;
    }

    std::vector<std::string> split(std::string s, char at) {
        std::stringstream s_stream(s);
        std::string segment;
        std::vector<std::string> seglist;

        while(std::getline(s_stream, segment, at)) {
            seglist.push_back(segment);
        }
        return seglist;
    }

    std::vector<std::size_t> read_stages(std::string stages_str) {
        std::vector<std::string> splitted_stages_str = split(stages_str, ' ');
        std::vector<std::size_t> stages;
        for (std::string& s : splitted_stages_str) {
            stages.push_back(stoi(s));
        }
        return stages;
    }

    std::unordered_map<char, char> read_mapping(std::vector<std::string> mapping_str) {
        std::unordered_map<char, char> map;
        for (std::string& m : mapping_str) {
            map.insert({m[0], m[1]});
        }
        return map;
    }
}