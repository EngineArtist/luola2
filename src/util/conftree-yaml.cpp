#include <fstream>
#include <yaml-cpp/yaml.h>

#include "../fs/datafile.h"
#include "conftree.h"

namespace conftree {

namespace {
    Node asNode(const YAML::Node&);

    Node asList(const YAML::Node &list)
    {
        Node node(Node::LIST);
        for(const YAML::Node &item : list)
            node.push_back(asNode(item));

        return node;
    }

    Node asMap(const YAML::Node &map)
    {
        Node node(Node::MAP);
        for(YAML::Iterator it=map.begin();it!=map.end();++it)
            node.insert(it.first().to<string>(), asNode(it.second()));

        return node;
    }

    Node asNode(const YAML::Node &yaml)
    {
        switch(yaml.Type()) {
            case YAML::NodeType::Null: break;
            case YAML::NodeType::Scalar: return Node(yaml.to<string>());
            case YAML::NodeType::Sequence: return asList(yaml);
            case YAML::NodeType::Map: return asMap(yaml);
        }

        // Blank node
        return Node();
    }
}

Node parseYAML(fs::DataFile &datafile, const string &filename)
{
    fs::DataStream ds(datafile, filename);
    if(ds->isError())
        throw BadNode("Unable to open configuration file: " + filename);

    YAML::Node doc;
    YAML::Parser parser(ds);
    if(!parser.GetNextDocument(doc))
        throw BadNode(filename + ": not a YAML file!");

    return asNode(doc);
}

Node parseYAML(const string &filename)
{
    std::ifstream fin(filename);
    YAML::Parser parser(fin);
    YAML::Node doc;
    if(!parser.GetNextDocument(doc))
        throw BadNode(filename + ": not a YAML file!");

    return asNode(doc);
}

std::vector<Node> parseMultiDocYAML(fs::DataFile &datafile, const string &filename)
{
    fs::DataStream ds(datafile, filename);
    if(ds->isError())
        throw BadNode("Unable to open configuration file: " + filename);

    YAML::Parser parser(ds);

    std::vector<Node> nodes;
    YAML::Node doc;
    while(parser.GetNextDocument(doc)) {
        nodes.push_back(asNode(doc));
    }

    return nodes;
}

}
