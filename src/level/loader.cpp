//
// This file is part of Luola2.
// Copyright (C) 2012 Calle Laakkonen
//
// Luola2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Luola2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Luola2.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef NDEBUG
#include <iostream>
using std::cerr;
using std::endl;
#endif

#include <cstring>
#include <cstdlib>

#include <boost/algorithm/string.hpp>

#include "../fs/datafile.h"
#include "../util/tinyxml2.h"
#include "../terrain/terrains.h"
#include "../world.h"
#include "levels.h"
#include "exception.h"

using tinyxml2::XMLElement;
namespace level {

namespace {
string Attr(const XMLElement *el, const char *attribute, const char *def)
{
    const char *val = el->Attribute(attribute);
    if(val)
        return val;
    return def;
}

std::vector<string> split(const string &str, const char *sep=" \t")
{
    std::vector<string> vec;
    boost::split(vec, str, boost::is_any_of(sep), boost::token_compress_on);
    return vec;
}

std::vector<string> split(const char *str, const char *sep=" \t")
{
    string text = string(str);
    return split(text, sep);
}

// Parse */<block>/<polygon> element
terrain::ConvexPolygon parsePolygon(const XMLElement *poly)
{
    terrain::Points points;

    std::vector<string> strpoints = split(poly->GetText());

    for(unsigned int i=0;i<strpoints.size();i+=2) {
        points.push_back(glm::vec2(atof(strpoints[i].c_str()), atof(strpoints[i+1].c_str())));
    }

    return terrain::ConvexPolygon(points);
}

// Parse contents of */<block> element
std::vector<terrain::ConvexPolygon> parsePolygons(const XMLElement *block)
{
    std::vector<terrain::ConvexPolygon> polygons;
    const XMLElement *poly = block->FirstChildElement();
    while(poly) {
        if(strcmp(poly->Name(), "polygon")==0) {
            polygons.push_back(parsePolygon(poly));
        } else {
#ifndef NDEBUG
            cerr << "Warning: unknown polygon block element: " << poly->Name() << endl;
#endif
        }
        poly = poly->NextSiblingElement();
    }
    return polygons;
}

// Parse <zones>/<block> or <zones>/<root> zone attributes
terrain::ZoneProps parseZoneProps(const XMLElement *zone)
{
    terrain::ZoneProps props;

    std::vector<string> forcev = split(Attr(zone, "force", "0 0"));
    if(forcev.size() != 2)
        throw LevelException("force vector should contain two values!");
    props.force = glm::vec2(atof(forcev[0].c_str()), atof(forcev[1].c_str()));

    if(zone->Attribute("density"))
        props.density = zone->FloatAttribute("density");
    else
        props.density = -1;

    return props;
}

// Parse <zones>/<root> zone
void loadRootZone(const XMLElement *root, World &world)
{
    terrain::ZoneProps props = parseZoneProps(root);
    if(props.density<0)
        props.density = 0;

    world.setRootZone(props);
}

// Parse <zones>/<block> element
void loadZoneBlock(const XMLElement *zone_el, World &world)
{
    terrain::ZoneProps props = parseZoneProps(zone_el);
    terrain::Zone *zone = new terrain::Zone(parsePolygons(zone_el));

    zone->setZoneForce(props.force);
    if(props.density>=0)
        zone->setZoneDensity(props.density);

    world.addZone(zone);
}

// Parse <zones> element
void loadZones(const XMLElement *zones, World &world)
{
    const XMLElement *el = zones->FirstChildElement();
    while(el) {
        if(strcmp(el->Name(), "root")==0)
            loadRootZone(el, world);
        else if(strcmp(el->Name(), "block")==0)
            loadZoneBlock(el, world);
        else {
#ifndef NDEBUG
            cerr << "Warning: Unknown level zone element: " << el->Name() << endl;
#endif
        }
        el = el->NextSiblingElement();
    }
}

// Parse <static> element
void loadStatic(const XMLElement *statics, World &world)
{
    const XMLElement *el = statics->FirstChildElement();
    while(el) {
        if(strcmp(el->Name(), "block")==0) {
            world.addStaticSolid(new terrain::Solid(parsePolygons(el)));
        } else {
#ifndef NDEBUG
            cerr << "Warning: Unknown level block element: " << el->Name() << endl;
#endif
        }
        el = el->NextSiblingElement();
    }
}

// Parse <solid> element
void loadSolid(const XMLElement *solids, World &world)
{
    const XMLElement *el = solids->FirstChildElement();
    while(el) {
        if(strcmp(el->Name(), "block")==0) {
            world.addSolid(new terrain::Solid(parsePolygons(el)));
        } else {
#ifndef NDEBUG
            cerr << "Warning: Unknown level block element: " << el->Name() << endl;
#endif
        }
        el = el->NextSiblingElement();
    }
}

void setWorldBounds(const XMLElement *root, World &world)
{
    string bounds = root->Attribute("bounds");
    std::vector<string> bvec;
    boost::split(bvec, bounds, boost::is_any_of(" "), boost::token_compress_on);
    if(bvec.size() != 4)
        throw LevelException("bounds attribute should have four values!");

    world.setBounds(terrain::BRect(
        atof(bvec[0].c_str()),
        atof(bvec[1].c_str()),
        atof(bvec[2].c_str()),
        atof(bvec[3].c_str())
        ));
}

}

void Level::load(World &world) const
{
    fs::DataFile df(datafile());

    tinyxml2::XMLDocument doc;
    {
        fs::DataStream ds(df, "terrain.xml");
        string xmlstr = string(
            (std::istreambuf_iterator<char>(ds)),
            std::istreambuf_iterator<char>());

        int error = doc.Parse(xmlstr.c_str());
        if(error != tinyxml2::XML_NO_ERROR)
            throw LevelException(doc.GetErrorStr1());
    }

    const XMLElement *root = doc.RootElement();

    setWorldBounds(root, world);

    const XMLElement *el = root->FirstChildElement();
    while(el) {
        if(strcmp(el->Name(), "zones")==0)
            loadZones(el, world);
        else if(strcmp(el->Name(), "static")==0)
            loadStatic(el, world);
        else if(strcmp(el->Name(), "solid")==0)
            loadSolid(el, world);
        else {
#ifndef NDEBUG
            cerr << "Warning: Unknown level file element: " << el->Name() << endl;
#endif
        }

        el = el->NextSiblingElement();
    }
    
}

}
