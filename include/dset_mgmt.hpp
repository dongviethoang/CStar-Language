#ifndef _CSTLIB_DATASET_MANAGEMENT_HPP
#define _CSTLIB_DATASET_MANAGEMENT_HPP

#include <iostream>
#include <fstream>
#include <string>
#include "posix_util.h"
#include "cstarconf.hpp"

namespace dset {
    _CSTLIB_BEGIN_DSET_SYS
        static int read_and_print_dataset(std::istream& file, int count) {
            if (!file.good()) {
                std::cerr << "Failed to open dataset file." << std::endl;
                return ENOENT;
            }

            std::string line;
            int lines_read = 0;
            while (getline(file, line)) {
                if (count > 0 && lines_read >= count) {
                    break;
                }

                std::cout << line << std::endl;
                ++lines_read;
            }
            return 0;
        }
        static int find_item(std::istream& file, const std::string& item) {
            if (!file.good()) {
                std::cerr << "Failed to open dataset file." << std::endl;
                return ENOENT;
            }

            std::string line;
            while (getline(file, line)) {
                if (line.find(item) != std::string::npos) {
                    return 0; // Found
                }
            }
            return ENOENT; // Not found
        }
    _CSTLIB_ENDING_DSET_SYS
}

#endif