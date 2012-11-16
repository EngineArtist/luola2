#ifndef LUOLA_UTIL_CONFTREE_H
#define LUOLA_UTIL_CONFTREE_H

#include <exception>
#include <memory>
#include <string>
#include <vector>
#include <set>

using std::string;

class DataFile;

namespace conftree {

class NodeImpl;

/**
 * Representation of a hierarchial configuration file.
 *
 * This is an abstraction over the lower level configuration file
 * format (e.g. YAML or JSON)
 */
class Node {
public:
    enum Type { BLANK, SCALAR, MAP, LIST };

    /**
     * Construct a blank node
     */
    Node();

    /**
     * Construct an empty node of the given type
     * @param type node type
     */
    explicit Node(Type type);

    /**
     * Construct a SCALAR node with initial value
     * @param value node value
     */
    Node(const string &value);

    /**
     * Get the type of the node.
     *
     * Possible types are:
     * - BLANK: no content
     * - SCALAR: a string value
     * - MAP: a subtree
     * - LIST: a list of nodes
     * 
     * @return type
     */
    Type type() const;

    /**
     * Get the scalar value of this node.
     *
     * If this is a blank node, an empty string will be returned.
     * 
     * @param def the default value to return if the node is empty
     * @return value
     * @throws BadNode if this is not a SCALAR or BLANK node
     */ 
    const string &value(const string &def=string()) const;

    /**
     * Get the scalar value of this node as an integer.
     * 
     * @param def the default value to return if the node is empty
     * @return value as integer
     * @throws BadNode if this is not a SCALAR or BLANK node
     */
    int intValue(int def=0) const;

    /**
     * Get the scalar value of this node as a float
     * 
     * @param def the default value to return if the node is empty
     * @return float value
     * @throws BadNode if this is not a SCALAR or BLANK node
     */
    float floatValue(float def=0.0f) const;

    /**
     * Return the number of subitems.
     *
     * For BLANK and SCALAR nodes, this will always return 0.
     * For LIST and MAP nodes, this will return the number of elements in the
     * container.
     * 
     * @return number of subitems
     */
    unsigned int items() const;

    /**
     * Return the node at the given index.
     *
     * @param index list index
     * @return Node at the given index
     * @throws BadNode if this is not a LIST node or index is out of bounds
     */
    const Node &at(unsigned int index) const;

    /**
     * Return the node with the given key
     *
     * @param name map key
     * @return Node with the key
     * @throws BadNode if this is not a MAP node or key not found
     */
    const Node &at(const string &name) const;

    /**
     * Return the node with the given key, or a blank node if not found.
     *
     * @param name map key
     * @return Ndoe with key key or blank if not found
     * @throws BadNode if this is not a MAP or BLANK node
     */
    const Node &opt(const string &name, const Node &def=Node()) const;

    /**
     * Return the set of map keys.
     *
     * @return set of map keys
     * @throws BadNode if this is not a MAP or BLANK node
     */
    std::set<string> itemSet() const;

    /**
     * Append a new node to this list.
     *
     * Note. Do not make cyclic node graphs! Doing so will cause
     * a memory leak.
     * @param node node to append
     * @throws BadNode if this is not a LIST or BLANK node
     */
    void push_back(const Node &node);

    /**
     * Insert a new node to this list.
     *
     * Note. Do not make cyclic node graphs! Doing so will cause
     * a memory leak.
     * @param key node key
     * @param node new node
     * @throws BadNdoe if this is not a MAP or BLANK node
     */
    void insert(const string &key, const Node &node);

private:
    std::shared_ptr<NodeImpl> m_impl;
};

/**
 * Read the given YAML file from the datafile and
 * return it as a tree of configuration nodes.
 *
 * Only the first document of the YAML file is returned.
 * 
 * @param datafile the data file
 * @param filename the YAML file inside the datafile
 * @return Configuration tree
 */
Node parseYAML(DataFile &datafile, const string &filename);

/**
 * Read the given YAML file from the datafile
 * and return all of its documents as configuration nodes.
 *
 * @param datafile the data file
 * @param filename the YAML file inside the data file
 * @return forest of configuration trees
 */
std::vector<Node> parseMultiDocYAML(DataFile &datafile, const string &filename);

/**
 * Configuration tree exception
 */
class BadNode : public std::exception {
public:
    BadNode(const string &error);
    ~BadNode() throw();

    const char *what() const throw();
    friend std::ostream& operator<<(std::ostream&, const BadNode&);
 
private:
    string m_error;
};

}

#endif
