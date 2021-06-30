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
#include <set>

#include "utility.h"
#include "TypeParser.h"
#include "DataReader.h"

using namespace std;

/// Logging level
LogLevels g_log_level = kError;


#define TEST_ASSERT(NAME, X)   if (!(X)) {Error("Test " NAME " failed");return 1;} else { LogLevels lev_prev = g_log_level;g_log_level = kInfo;Info("Test " NAME " success");g_log_level = lev_prev; }

#define TEST_EXPECT(NAME, X, Y) if (X == Y) { \
                                    LogLevels lev_prev = g_log_level;g_log_level = kInfo;Info("Test " NAME " success");g_log_level = lev_prev; \
                                } else {      \
                                    string output = "Test " NAME " failed, expected ";  \
                                    output += string(Y);          \
                                    output += " got " + string(X);          \
                                    Error(output); \
                                    return 1; \
                                }

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

    VariableDeclaration decl;
    parser.ParseDeclaration("unsigned short test_array[3];", decl);

    TEST_ASSERT("Array_size", decl.array_size==3);
    TEST_ASSERT("Var_size", decl.var_size==2);
    TEST_EXPECT("Type", decl.data_type, "uint16_t");

    parser.ParseDeclaration("unsigned long long test_array;", decl);

    TEST_ASSERT("Array_size", decl.array_size==0);
    TEST_ASSERT("Var_size", decl.var_size==8);
    TEST_EXPECT("Type", decl.data_type, "uint64_t");

    parser.ParseDeclaration("signed long test_array;", decl);

    TEST_ASSERT("Array_size", decl.array_size==0);
    TEST_ASSERT("Var_size", decl.var_size==4);
    TEST_EXPECT("Type", decl.data_type, "int32_t");

    parser.SetIncludePaths(inc_paths);
    parser.ParseFiles();

    string name = "test";
    // dump all to screen
    parser.DumpYaml(name, cout);
    //parser.DumpTypeDefs();

//    DataReader reader(parser, bin_file);
//    reader.PrintTypeData(struct_name, false/* struct */);

    return 0;
}
