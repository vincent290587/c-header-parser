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
#endif

#include <string>
#include <iostream>
#include <set>

#include "utility.h"
#include "TypeParser.h"
#include "DataReader.h"

using namespace std;

/// Logging level
LogLevels g_log_level = kInfo;


void ParseOptions(int argc, char **argv, string &struct_name, string &bin_file, set<string> &inc_paths) {
//    struct_name = "Employee";
    struct_name = "TestStruct";
    bin_file    = "../../../test/Employee.bin";
    inc_paths.insert("../../../arm");
}

int main(int argc, char **argv) {
    string struct_name, bin_file;
    set<string> inc_paths;

    ParseOptions(argc, argv, struct_name, bin_file, inc_paths);

    TypeParser parser;
    parser.SetIncludePaths(inc_paths);
    parser.ParseFiles();
    parser.DumpTypeDefs();

//    DataReader reader(parser, bin_file);
//    reader.PrintTypeData(struct_name, false/* struct */);

    return 0;
}