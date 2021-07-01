//
// Created by vince on 30/06/2021.
//

#ifndef CHP_INTERFACEWRITER_H
#define CHP_INTERFACEWRITER_H

#include "TypeParser.h"
#include <string>

using namespace std;


typedef struct {
    int cur_index;
    string firstParent;
} sXMLINdexing;

class InterfaceWriter {

public:
    // no memory data
    InterfaceWriter(const TypeParser& parser) : type_parser_(parser) {

    }
    ~InterfaceWriter() {

    }

    void PrintXMLData(const string &type_name, std::ostream &ofs);
    void PrintYAMLData(const string &name, std::ostream& ofs);

private:

    void PrepareSingleXML(sXMLINdexing &indexing, const VariableDeclaration &members, std::ostream &ofs);
    void PrepareXMLData(const string &type_name, size_t indent, bool is_union, std::ostream &ofs);

private:
    TypeParser		type_parser_;
};

#endif //CHP_INTERFACEWRITER_H
