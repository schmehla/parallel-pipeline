#include <string>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <cassert>

static const std::size_t ASCII_LOW_END = 33;
static const std::size_t ASCII_HIGH_END = 126; // including this


template<typename T>
T string_to_T(std::string& str);

template<>
std::size_t string_to_T<std::size_t>(std::string& str) {
    return std::stoi(str);
}

template<>
std::string string_to_T<std::string>(std::string& str) {
    return str;
}

class CommandLine {
 public:
    CommandLine(int argc, char** argv) {
        assert(argc%2==0);
        extracted_args.reserve(argc/2);
        for (std::size_t i = 0; i < argc/2; ++i) {
            extracted_args.push_back({
                std::string(argv[2*i]),
                std::string(argv[2*i+1])
            });
        }
    }

    template<typename T>
    T flag(std::string flag, T default_value) {
        for (CommandLineArgument cla : extracted_args) {
            if (0==cla.name.compare(flag)) {
                return string_to_T(cla.value);
            }
        }
        return default_value;
    }

 private:
    struct CommandLineArgument {
        std::string name;
        std::string value;
    };
    std::vector<CommandLineArgument> extracted_args;

};

int main(int argc, char** argv) {
    CommandLine cl(argc, argv);
    std::size_t num_chars = cl.flag("-numChars", 1000000);
    std::string mapping_loc = cl.flag("-mappingLoc", "../data/example.mapping");
    std::string stages = cl.flag("-stages", "1 5 411 55");
    std::string pipeline_loc = cl.flag("-file", "../data/pipelines/input_" + std::to_string(num_chars)) + "_stages_" + stages + ".pipeline";

    std::ofstream pipeline_file;
    std::ifstream mapping_file;
    pipeline_file.open(pipeline_loc);
    mapping_file.open(mapping_loc);

    // print stages template
    pipeline_file << "# add pipeline stages with number of mapping iterations:" << "\n";
    pipeline_file << "# example: 1 5 411 55 = one iteration in first stage, 5 in second stage, etc." << "\n";
    pipeline_file << "stages:" << "\n";
    pipeline_file << stages << "\n";
    pipeline_file << "\n";
    pipeline_file << "mapping:" << "\n";
    char from, to;
    while (mapping_file >> from >> to) {
        pipeline_file << from << to << "\n";
    }
    pipeline_file << "\n";

    // print random chars
    srand(100);
    pipeline_file << "input:" << "\n";
    for (std::size_t i = 0; i < num_chars; ++i) {
        std::size_t random_code = rand()%(ASCII_HIGH_END-ASCII_LOW_END+1)+ASCII_LOW_END;
        pipeline_file << static_cast<char>(random_code);
    }
    // print mapping

    pipeline_file.close();
    return 0;
}