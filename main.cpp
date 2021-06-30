/// Copyright(c) 2013 Frank Fang
///
/// Main program
///
/// It can either parse a specified file, or search and parse all files under specifed include paths
///
/// This program follows Google C++ Style, and the comments follow doxgen C++ style
/// Line size: 120
///
/// Reference:
///   Google C++ Style: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
///   C BNF Grammer:    http://lists.canonical.org/pipermail/kragen-hacks/1999-October/000201.html
///
/// @author Frank Fang (fanghm@gmail.com)
/// @date   2013/07/06

#ifndef WIN32
#include <unistd.h>
#else
#include "getopt.h"
#endif

#include <string>
#include <iostream>
#include <set>
#include <cassert>

#include "utility.h"
#include "TypeParser.h"
#include "DataReader.h"

using namespace std;

/// Logging level
LogLevels g_log_level = kInfo;

void usage(char* prog) {
    cout << "Usage:\n\t" << prog << " -s <struct_name> -b <binary_file> -i<include_path> [-h]" << endl;
}

typedef struct {
    string struct_name;
    string bin_file;
    string yaml_file;
    set<string> inc_paths;
} sParams;

void ParseOptions(int argc, char **argv, sParams &params) {
    char c;
    while ((c = getopt (argc, argv, "s:b:i:y:h")) != -1) {
        if (optarg) {
            string msg = "Argument ";
            msg += c;
            msg += " ";
            msg += optarg;
            Info(msg);
            switch (c) {
            case 's':
                params.struct_name = string(optarg);
                break;

            case 'b':
                params.bin_file = string(optarg);
                break;

            case 'y':
                params.yaml_file = string(optarg);
                break;

            case 'h':
            case 'i':
                params.inc_paths.insert(string(optarg));
                break;

            default:
                usage(argv[0]);
            }
        } else {
            string msg = "Argument null char ";
            msg += c;
            Error(msg);
        }
    }

    if (params.struct_name.empty() || params.inc_paths.empty()) {
        usage(argv[0]);
        return;
    }

    Info("Struct: " + params.struct_name);
    Info("Binary: " + params.bin_file);

    for(set<string>::iterator it = params.inc_paths.begin(); it != params.inc_paths.end(); ++it) {
            Info("Include path: " + *it);
    }
}

int main(int argc, char **argv) {
    sParams params;
    
    ParseOptions(argc, argv, params);
    
    TypeParser parser;
    parser.SetIncludePaths(params.inc_paths);
    parser.ParseFiles();
    parser.DumpTypeDefs();

    if (!params.yaml_file.empty()) {
        std:ofstream ofs(params.yaml_file);
        parser.DumpYaml(params.struct_name, ofs);
    }

    if (!params.bin_file.empty()) {
        DataReader reader(parser, params.bin_file);
        reader.PrintTypeData(params.struct_name, false/* struct */);
    }

    return 0;
}


