//
// Created by vince on 30/06/2021.
//

#include <iostream>
#include <fstream>      // ifstream
#include <iomanip>      // setw
#include <cassert>

#include "utility.h"    // tohex
#include "InterfaceWriter.h"
#include "TypeParser.h"


void InterfaceWriter::PrintXMLData(const string &type_name, std::ostream &ofs) {

    PrepareXMLData(type_name, 0, false, ofs);

}

void InterfaceWriter::PrepareSingleXML(sXMLINdexing &indexing, const VariableDeclaration &member, std::ostream &ofs) {

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
        snprintf(buff, sizeof(buff), "    <option block_offset=\"%d\" type=\"%s\" name=\"%s\" description=\"%s\">&%s;</option>",
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
        snprintf(buff, sizeof(buff), "    <number block_offset=\"%d\" type=\"%s\" name=\"%s\" description=\"%s\" />",
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

void InterfaceWriter::PrepareXMLData(const string &type_name, size_t indent, bool is_union, std::ostream &ofs) {

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

    ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;

    // dump enum definitions
    string yaml_space = "    ";
    if (type_parser_.enum_defs_.size()) {
        ofs << endl;
        ofs << "<!DOCTYPE " << type_name << " [" << endl;
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

    ofs << "<" << type_name << " xml:id=\"" << type_name << "\">" << endl;

    indexing.cur_index = 0;
    for (VariableDeclaration mem : members) {
        indexing.firstParent = ""; // reset parent
        PrepareSingleXML(indexing, mem, ofs);
    }

    ofs << "</" << type_name << ">" << endl;
}

/// Dump the extracted type definitions
void InterfaceWriter::PrintYAMLData(const string &name, std::ostream& ofs) {
    VariableDeclaration var;
    const string yaml_space = "    ";

    // header
    ofs << "# TTC " << name << endl;
    ofs << endl;
    ofs << "type: TTC" << endl;
    ofs << "name: " << name << endl;
    ofs << "version: 0.1" << endl;
    ofs << endl;

    // TODO dump typedefs
    // dump numeric const variables or macros
//    ofs << "\nconstant values:" << "\n--------------------" << endl;
//    for (map <string, long>::const_iterator it = const_defs_.begin(); it != const_defs_.end(); ++it) {
//        ofs << "\t" << it->first << "\t = " << it->second << endl;
//    }

    // dump enum definitions
    if (type_parser_.enum_defs_.size()) {
        ofs << "enums:" << endl;
        for(map <string, list<pair<string, int> > >::const_iterator itv= type_parser_.enum_defs_.begin();
            itv != type_parser_.enum_defs_.end(); ++itv) {

            ofs << yaml_space << itv->first << ":" << endl;
            ofs << yaml_space << yaml_space << "datatype: " << "UINT8" << endl; // TODO enum types
            ofs << yaml_space << yaml_space << "values: {";

            list< pair<string, int> > members = itv->second;
            while (!members.empty()) {
                pair<string, int> var = members.front();
                ofs << var.first << ": " << var.second;
                if (members.size() > 1) {
                    ofs << ", ";
                }
                members.pop_front();
            }

            ofs << '}' << endl << endl;
        }
    }

    ofs << endl;

    // dump struct definitions
    if (type_parser_.struct_defs_.size()) {
        ofs << "structs:" << endl;
        for (map<string, list<VariableDeclaration> >::const_iterator it = type_parser_.struct_defs_.begin();
             it != type_parser_.struct_defs_.end(); ++it) {

            ofs << yaml_space << it->first << ":" << endl;
            ofs << yaml_space << yaml_space << "description: empty" << endl; // TODO yaml struct description
            ofs << yaml_space << yaml_space << "parameters: " << endl;

            list<VariableDeclaration> members = it->second;
            while (!members.empty()) {
                var = members.front();

                ofs << yaml_space << yaml_space << yaml_space << "- " << var.var_name << ':' << endl;
                ofs << yaml_space << yaml_space << yaml_space << yaml_space << "description: " << var.description << endl;
                ofs << yaml_space << yaml_space << yaml_space << yaml_space << "data: " << endl;

                if (var.is_pointer) {
                    assert(0);
                }

                {

                    if (type_parser_.autogen_types_.find(var.data_type) != type_parser_.autogen_types_.end()) {
                        ofs << yaml_space << yaml_space << yaml_space << yaml_space << yaml_space;
                        ofs << "type: " << type_parser_.autogen_types_.at(var.data_type) << endl;
                    } else {
                        ofs << yaml_space << yaml_space << yaml_space << yaml_space << yaml_space;
                        ofs << "type: " << var.data_type << endl;
                    }

                    if (var.array_size) {
                        ofs << yaml_space << yaml_space << yaml_space << yaml_space << yaml_space;
                        ofs << "array: " << var.array_size << endl;
                    }

                    if (var.is_bitfield) {
                        ofs << yaml_space << yaml_space << yaml_space << yaml_space << yaml_space;
                        ofs << "bits: " << var.var_size << endl;
                    }
                }

                ofs << endl;

                members.pop_front();
            }

            ofs << endl;
        }
    }

    ofs << endl;

}
