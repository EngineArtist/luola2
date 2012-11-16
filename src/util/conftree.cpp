#include <stdexcept>
#include <ostream>
#include <cstdlib>
#include <vector>
#include <unordered_map>

#include "conftree.h"

namespace conftree {
    /**
     * Base class for node implementations
     */
    class NodeImpl {
    public:
        NodeImpl(Node::Type type_) : type(type_) { }
        virtual ~NodeImpl() { }

        virtual const string &value() const { throw BadNode("not a SCALAR node!"); }
        virtual unsigned int items() const { return 0; }
        virtual const Node &at(unsigned int) const { throw BadNode("not a LIST node!"); }
        virtual const Node &at(const string &) const { throw BadNode("not a MAP node!"); }
        virtual const Node &at(const string &, const Node&) const { throw BadNode("not a MAP node!"); }
        virtual std::set<string> itemSet() const { throw BadNode("nto a MAP node!"); }
        virtual void push_back(const Node&) { throw BadNode("not a LIST node!"); }
        virtual void insert(const string&, const Node&) { throw BadNode("not a MAP node!"); }

        const Node::Type type;
    };

    /**
     * Scalar Node: holds a string value
     */
    class ScalarNode : public NodeImpl {
    public:
        ScalarNode(const string &value)
        : NodeImpl(Node::SCALAR), m_value(value)
        {
        }

        const string &value() const { return m_value; }

    private:
        string m_value;
    };

    /**
     * List Node: hold a list of sub-Nodes
     */
    class ListNode : public NodeImpl {
    public:
        ListNode() : NodeImpl(Node::LIST) { }

        unsigned int items() const { return m_nodes.size(); }

        const Node &at(unsigned int i) const
        {
            if(i >= m_nodes.size())
                throw BadNode("Index out of bounds");
            return m_nodes[i];
        }

        void push_back(const Node &n) {
            m_nodes.push_back(n);
        }

    private:
        std::vector<Node> m_nodes;
    };

    /**
     * Map Node: An unordered map of sub-Nodes
     */
    class MapNode : public NodeImpl {
    public:
        MapNode() : NodeImpl(Node::MAP) { }

        unsigned int items() const { return m_map.size(); }

        const Node &at(const string &key) const
        {
            try {
                return m_map.at(key);
            } catch(const std::out_of_range &ex) {
                throw BadNode("No such item: " + key);
            }
        }

        const Node &at(const string &key, const Node &def) const
        {
            try {
                return m_map.at(key);
            } catch(const std::out_of_range &ex) {
                return def;
            }
        }

        std::set<string> itemSet() const
        {
            std::set<string> items;
            for(auto e : m_map)
                items.insert(e.first);
            return items;
        }

        void insert(const string &key, const Node &n) {
            m_map[key] = n;
        }

    private:
        std::unordered_map<string, Node> m_map;
    };

    // Public API
    Node::Node()
    {
    }

    Node::Node(Type type)
    {
        switch(type) {
            case BLANK: break;
            case SCALAR: m_impl = std::shared_ptr<NodeImpl>(new ScalarNode(string())); break;
            case LIST: m_impl = std::shared_ptr<NodeImpl>(new ListNode()); break;
            case MAP: m_impl = std::shared_ptr<NodeImpl>(new MapNode()); break;
        }
    }

    Node::Node(const string &value)
    {
        m_impl = std::shared_ptr<NodeImpl>(new ScalarNode(value));
    }

    Node::Type Node::type() const
    {
        if(!m_impl)
            return BLANK;
        return m_impl->type;
    }

    const string &Node::value(const string &def) const
    {
        if(!m_impl)
            return def;

        const string &val = m_impl->value();
        if(val.length() == 0)
            return def;
        return val;
    }

    int Node::intValue(int def) const
    {
        const string &v = value();
        if(v.length() == 0)
            return def;
        return atoi(v.c_str());
    }

    float Node::floatValue(float def) const
    {
        const string &v = value();
        if(v.length() == 0)
            return def;
        return atof(v.c_str());
    }

    unsigned int Node::items() const
    {
        if(!m_impl)
            return 0;
        return m_impl->items();
    }

    const Node &Node::at(unsigned int index) const
    {
        if(!m_impl)
            throw BadNode("Cannot index a blank node");

        return m_impl->at(index);
    }

    const Node &Node::at(const string &name) const
    {
        if(!m_impl)
            throw BadNode("Cannot get item from blank node!");

        return m_impl->at(name);
    }

    const Node &Node::opt(const string &name, const Node &def) const
    {
        if(!m_impl)
            return def;

        return m_impl->at(name, def);
    }

    std::set<string> Node::itemSet() const
    {
        if(!m_impl)
            return std::set<string>();

        return m_impl->itemSet();
    }

    void Node::push_back(const Node &node)
    {
        if(!m_impl)
            m_impl = std::shared_ptr<NodeImpl>(new ListNode());

        m_impl->push_back(node);
    }

    void Node::insert(const string &key, const Node &node)
    {
        if(!m_impl)
            m_impl = std::shared_ptr<NodeImpl>(new MapNode());

        m_impl->insert(key, node);
    }

    BadNode::BadNode(const string &error)
    : m_error(error)
    {
    }

    BadNode::~BadNode() throw()
    {
    }

    const char *BadNode::what() const throw()
    {
        return m_error.c_str();
    }

    std::ostream &operator<<(std::ostream &os, const BadNode &ex)
    {
        os << "Configuration error: " << ex.m_error;
        return os;
    }
}
