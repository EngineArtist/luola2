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
Node parseYAML(DataFile &datafile, const string &filename)
{
    DataStream ds(datafile, filename);
    if(ds->isError())
        throw BadNode("Unable to open configuration file: " + filename);

    std::unique_ptr<YAML::Node> doc(new YAML::Node());
    YAML::Parser parser(ds);
    if(!parser.GetNextDocument(*doc))
        throw BadNode(filename + ": not a YAML file!");

    return asNode(*doc);
}

}
