/// Copyright(c) 2013 Frank Fang
///
/// Binary memory data reader for C types
///
/// @author Frank Fang (fanghm@gmail.com)
/// @date   2013/07/06

#include <iostream>
#include <fstream>      // ifstream
#include <iomanip>      // setw
#include <cassert>

#include "utility.h"    // tohex
#include "DataReader.h"
#include "TypeParser.h"

#define TAB_WIDTH 4
#define FORMAT_OUTPUT(indent_depth) out_stream_ << setw(TAB_WIDTH * (indent_depth)) << ' '

DataReader::DataReader(const TypeParser& parser)
        : type_parser_(parser), data_buffer_(nullptr), data_size_(0), data_ptr_(NULL) {
}

DataReader::DataReader(const TypeParser& parser, char* buffer, size_t size)
    : type_parser_(parser), data_buffer_(buffer), data_size_(size), data_ptr_(NULL) {
}

DataReader::DataReader(const TypeParser& parser, const string &data_file)
    : type_parser_(parser), data_buffer_(NULL), data_size_(0), data_ptr_(NULL) {

    ReadData(data_file);
}

/// read binary data into buffer
///
/// @param[in]  data_file   data file that contains binary memory dump
void DataReader::ReadData(const string &data_file) {
    ifstream is(data_file.c_str(), ios::in | ios::binary);
    if (is.fail()) {
        Error("Failed to open file: " + data_file);
        return;
    }

    is.seekg (0, is.end);
    data_size_ = static_cast<size_t>(is.tellg());

    // allocate memory dynamically
    data_buffer_ = new char [data_size_];

    is.seekg (0, is.beg);
    is.read (data_buffer_, data_size_);

    is.close();

    // set read start address to the buffer address
    data_ptr_ = data_buffer_;

	cout << "buffer addr: " << hex << &data_ptr_ << endl;
}

void DataReader::PrintTypeData(const string &type_name, bool is_union) {
	PrepareTypeData(type_name, 0, is_union);

	/// printing
	cout << out_stream_.str() << endl;
}

void DataReader::PrintXMLData(const string &type_name, bool is_union, std::ostream &ofs) {

    PrepareXMLData(type_name, 0, is_union, ofs);

}

void DataReader::PrepareSingleXML(sXMLINdexing &indexing, const VariableDeclaration &member, std::ostream &ofs) {

    const string type_name = member.data_type;
    const map <string, string> matching_types_ = {
            {"char" , "char"},
            {"uint8_t",  "char"},
            {"int8_t",  "2schar"},
            {"uint16_t",  "short"},
            {"int16_t",  "2sshort"},
            {"uint32_t",  "int"},
            {"int32_t",  "2ssint"},
            {"float",  "float"},
            {"double",  "double"},
    };

    list<VariableDeclaration> members;

    if (type_parser_.union_defs_.find(type_name) != type_parser_.union_defs_.end()) {
        members = type_parser_.union_defs_[type_name];
    } else if (type_parser_.struct_defs_.find(type_name) != type_parser_.struct_defs_.end()) {
        members = type_parser_.struct_defs_[type_name];
    } else if (type_parser_.enum_defs_.find(type_name) != type_parser_.enum_defs_.end()) {
        // single enum
        string line = "";
        char buff[200];
        snprintf(buff, sizeof(buff), "    <option block_offset=\"%d\" type=\"%s\" name=\"%s\" description=\"%s\">&%s</option>",
                 indexing.cur_index,
                 "char", // TODO enum types
                 member.var_name.c_str(),
                 member.description.c_str(),
                 type_name.c_str());
        line = buff;
        ofs << line << endl;

        indexing.cur_index += member.var_size;
        return;

    } else if (type_parser_.type_defs_.find(type_name) != type_parser_.type_defs_.end()) {
        // single typedef
        string line = "";
        char buff[200];
        snprintf(buff, sizeof(buff), "    <number block_offset=\"%d\" type=\"%s\" name=\"%s\" description=\"%s\">",
                 indexing.cur_index,
                 type_parser_.type_defs_.at(type_name).c_str(),
                 member.var_name.c_str(),
                 member.description.c_str());
        line = buff;
        ofs << line << endl;

        indexing.cur_index += member.var_size;
        return;
    } else {
        // single atomic
        if (matching_types_.find(member.data_type) != matching_types_.end()) {

            string line = "";
            string long_name = indexing.firstParent;
            if (!long_name.empty())long_name += "::";
            long_name += member.var_name;
            char buff[200];
            snprintf(buff, sizeof(buff), "    <number block_offset=\"%d\" type=\"%s\" name=\"%s\" description=\"%s\" />",
                     indexing.cur_index,
                     matching_types_.at(member.data_type).c_str(),
                     long_name.c_str(),
                     member.description.c_str());
            line = buff;
            ofs << line << endl;

            indexing.cur_index += member.var_size;
            return;
        } else {
            Warning("Unknown member type: " + member.data_type);
            assert(0);
        }
    }

    if (indexing.firstParent.empty()) {
        indexing.firstParent = member.var_name;
    } else {
        indexing.firstParent += "::";
        indexing.firstParent = member.var_name;
    }

    for (VariableDeclaration mem : members) {
        PrepareSingleXML(indexing, mem, ofs);
    }
}

void DataReader::PrepareXMLData(const string &type_name, size_t indent, bool is_union, std::ostream &ofs) {

    sXMLINdexing indexing;
    list<VariableDeclaration> members;
    if (is_union && type_parser_.union_defs_.find(type_name) != type_parser_.union_defs_.end()) {
        members = type_parser_.union_defs_[type_name];
    } else if (type_parser_.struct_defs_.find(type_name) != type_parser_.struct_defs_.end()) {
        members = type_parser_.struct_defs_[type_name];
    } else {
        Error("Unknown struct/union: " + type_name);
        return;
    }

    // if it's a fake name assigned to anonymous type, then the fake name won't be printed
//    if (0 == type_name.compare(0, sizeof(type_parser_.kAnonymousTypePrefix), type_parser_.kAnonymousTypePrefix)) {
//        ofs << (is_union ? "union " : "struct ") << "{" << endl;
//    } else {
//        ofs << (is_union ? "union " : "struct ") << type_name << " {" << endl;
//    }


    // dump enum definitions
    string yaml_space = "    ";
    if (type_parser_.enum_defs_.size()) {
        ofs << endl;
        ofs << "<!DOCTYPE note [" << endl;
        for(map <string, list<pair<string, int> > >::const_iterator itv= type_parser_.enum_defs_.begin();
            itv != type_parser_.enum_defs_.end(); ++itv) {

            ofs << yaml_space << "<!ENTITY " << itv->first << " \"" << endl;

            int number = 0;
            list< pair<string, int> > members = itv->second;
            while (!members.empty()) {
                pair<string, int> var = members.front();
                string cur_val = "<item id='";
                cur_val += std::to_string(var.second);
                cur_val += "' value='";
                cur_val += var.first;
                cur_val += "'/>";
                ofs << yaml_space << yaml_space << cur_val << endl;
                members.pop_front();
            }

            ofs << yaml_space << "\">" << endl;
        }
        ofs << "]>" << endl << endl;
    }

    ofs << "<beacon xml:id=\"" << type_name << "\">" << endl;

    indexing.cur_index = 0;
    for (VariableDeclaration mem : members) {
        indexing.firstParent = ""; // reset parent
        PrepareSingleXML(indexing, mem, ofs);
    }

    ofs << "</beacon>" << endl;
}

/// Print the members and their data of a struct or union in a nice fomat
/// 
/// This method will be recursively called for nested struct/union(s)
///
/// @param[in]  type_name	name of a struct or union
/// @param[in]  indent      depth of indent, for output format control
/// @param[in]  is_union    true for union, false for struct - this is needed in @method PrintVarValue
///
void DataReader::PrepareTypeData(const string &type_name, size_t indent, bool is_union) {
	cout << type_name << " | " << (is_union ? "union" : "struct");

    list<VariableDeclaration> members;
    if (is_union && type_parser_.union_defs_.find(type_name) != type_parser_.union_defs_.end()) {
        members = type_parser_.union_defs_[type_name];
    } else if (type_parser_.struct_defs_.find(type_name) != type_parser_.struct_defs_.end()) {
        members = type_parser_.struct_defs_[type_name];
    } else {
        Error("Unknown struct/union: " + type_name);
        return;
    }

    // only need to check this when this method is called for the first time
    if (0 == indent && type_parser_.type_sizes_[type_name] != data_size_) {
        Debug("The buffer size is not the same as size of the type - " + type_name);
    }

    // if it's a fake name assigned to anonymous type, then the fake name won't be printed
    if (0 == type_name.compare(0, sizeof(type_parser_.kAnonymousTypePrefix), type_parser_.kAnonymousTypePrefix)) {
        out_stream_ << (is_union ? "union " : "struct ") << "{" << endl;
    } else {
        out_stream_ << (is_union ? "union " : "struct ") << type_name << " {" << endl;
    }

    indent++;
    PrintMemberData(members, indent, is_union);
    --indent;

    FORMAT_OUTPUT(indent) << "}" << endl;
}

/// Print a struct or union's fields and their data
///
/// @param[in]  members     struct/union members
/// @param[in]  indent      depth of indent
/// @param[in]  is_union    true for union, false for struct
void DataReader::PrintMemberData(list<VariableDeclaration> &members, size_t indent, bool is_union) {
    VariableDeclaration var_decl;

	// remember the start address of the union data for later use as each union members has the same start address
	char* union_addr = data_ptr_;

    while(!members.empty()) {
        var_decl = members.front();

        if (!is_union && 0 == var_decl.var_name.compare(type_parser_.kPaddingFieldName)) {
            // skip printing the padding field, but moving the data pointer is necessary
            data_ptr_ += var_decl.var_size;
        } else {
			if (is_union) data_ptr_ = union_addr;
			cout << " @ " << hex << &data_ptr_ << endl;

			if (var_decl.array_size != 0) {
				// array has special format
				FORMAT_OUTPUT(indent) << var_decl.var_name << " = [" << endl;
				indent++;

				for (size_t i = 0; i < var_decl.array_size; i++) {
					FORMAT_OUTPUT(indent) << "[" << i << "] = ";
					PrintVarData(var_decl, indent, false);
				}

				FORMAT_OUTPUT(indent) <<"]" << endl;
				indent--;    // this line can move up one line for better indent, just to conform to example output

			} else {
				// non-array
				FORMAT_OUTPUT(indent) << var_decl.var_name << " = ";
				PrintVarData(var_decl, indent, is_union);
			}
		}

        members.pop_front();
    }
}

/// print a field
///
/// @param[in]  var_decl    struct/union member declaration
/// @param[in]  indent      depth of indent
/// @param[in]  is_union    true for union, false for struct
void DataReader::PrintVarData(const VariableDeclaration &var_decl, size_t indent, bool is_union) {
    switch (type_parser_.GetTokenType(var_decl.data_type)) {
    case kBasicDataType:
        PrintVarValue(var_decl, indent, is_union);
        break;

    case kStructName:
        PrepareTypeData(var_decl.data_type, indent, false);
        break;

    case kUnionName:
        PrepareTypeData(var_decl.data_type, indent, true);
        break;

    case kEnumName:
        PrintVarValue(var_decl, indent, false);
        break;

    default:
        Error("Unresolved data type - " + var_decl.data_type);
        break;
    }
}

/// print the value of a field
///
/// @param[in]  var_decl    struct/union member declaration
/// @param[in]  indent      depth of indent
/// @param[in]  is_union    true for union, false for struct
void DataReader::PrintVarValue(const VariableDeclaration &var_decl, size_t indent, bool is_union) {
    int     len;
    if (0 == var_decl.data_type.compare("char")) {
        len = 1;
    } else {
        len = var_decl.var_size;
    }

    string  data = string(data_ptr_, len);
    string  hex_value = tohex(data);
    int     int_value = strtol(hex_value.c_str(), NULL, 16);
        
    out_stream_ << setw(3) << int_value << ", " << hex_value;
        
    // for enum, print value like: 1, 0x01, enum Home.Anhui
    if (!is_union && type_parser_.enum_defs_.find(var_decl.data_type) != type_parser_.enum_defs_.end()) {
        string enumVar = "Unknown";
        list< pair<string, int> > enums = type_parser_.enum_defs_[var_decl.data_type];
        for (list< pair<string, int> >::const_iterator it = enums.begin(); it != enums.end(); ++it) {
            if (int_value == it->second) {
                enumVar = it->first;
                break;
            }
        }
        out_stream_ << ", " << enumVar;
    } else if ("char" == var_decl.data_type && 0 != int_value) {
        // for char type
        out_stream_ << ", '" << (char)('A'+(int_value - (int)'A')) << "'";
    }

    out_stream_ << endl;

    if (!is_union) {
        data_ptr_ += len;

        // check pointer offset
        if ((data_ptr_ - data_buffer_) > data_size_) {
            Debug("bad data offset");
        }
    }
}

DataReader::~DataReader(void)
{
    // release memory
    if (NULL != data_buffer_) {
        delete [] data_buffer_;
    }
}
