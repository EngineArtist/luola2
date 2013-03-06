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

#include "levels.h"
#include "../fs/paths.h"
#include "../fs/datafile.h"
#include "../util/conftree.h"

namespace level {

const string LEVELFILE_SUFFIX = ".level";

namespace {
    LevelRegistry *LEVELS;
}

void LevelRegistry::init()
{
    if(LEVELS)
        delete LEVELS;

    LEVELS = new LevelRegistry();
}

const Level &LevelRegistry::get(const string &name)
{
    return LEVELS->m_levels.at(name);
}

void LevelRegistry::add(const string &filename)
{
    LEVELS->addLevel(filename);
}

void LevelRegistry::addLevel(const string &filename)
{
    fs::DataFile df(filename);
    conftree::Node meta = conftree::parseYAML(df, "meta.yaml");

    string name = filename.substr(0, filename.rfind(LEVELFILE_SUFFIX));

#ifndef NDEBUG
    cerr << "Found level " << name << endl;
#endif
    m_levels[name] = Level(name, meta);
}

Level::Level(const string &name, const conftree::Node &meta)
    : m_name(name)
{
    m_title = meta.at("title").value();
}

string Level::datafile() const
{
    return m_name + LEVELFILE_SUFFIX;
}

}

