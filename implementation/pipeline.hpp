#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

#include "config_parsing.cpp"

template <typename T>
class Pipeline {
 public:
	using Batch = std::vector<T>;
	using Stage = std::function<Batch(const Batch&)>;

	//Pipeline(std::vector<T> input)
	//: input(input) {}

	Pipeline(std::string config_loc) {}
	
	void append_stage(Stage s) {
		stages.push_back(s);
	}

 protected:
    std::vector<Stage> stages;
	std::vector<T> input;
};


template<>
Pipeline<char>::Pipeline(std::string config_loc) {
	ConfigParsing::ParsedConfig parsed_config = ConfigParsing::parse_config(config_loc);
	const std::vector<std::size_t> config_stages = ConfigParsing::read_stages(parsed_config.stages_str);
	const std::unordered_map<char, char> config_mapping = ConfigParsing::read_mapping(parsed_config.mapping_str);
	const std::string config_input = parsed_config.input_str;
	std::copy(config_input.begin(), config_input.end(), std::back_inserter(input));

	for (const std::size_t s : config_stages) {
		append_stage([s, config_mapping](std::vector<char> in) {
			const std::size_t size = config_mapping.size();
			std::vector<char> out;
			out.reserve(in.size());
			for (char i : in) {
				char curr = i;
				for (std::size_t iter = 0; iter < s; ++iter) {
					curr = config_mapping.at(curr);
				}
				out.push_back(curr);
			}
			return out;
		});
	}
}




