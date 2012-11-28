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
#ifndef LUOLA_INPUT_H
#define LUOLA_INPUT_H

namespace input {

/**
 * Player input device abstraction.
 *
 */
class PlayerInput {
public:
    PlayerInput();

    /**
     * Get the position of the X-axis controller.
     *
     * This is the X axis of the joystick/directional pad or the left/right buttons.
     * 
     * Valid positions are -1, 0 and 1
     * @return x axis position
     */
    int axisX() const { return m_axisx; }

    void setAxisX(int value)
    {
        m_axisx = value;
        m_changed = true;
    }

    /**
     * Get the position of the Y-axis controller.
     *
     * This is the Y axis of the joystick/directional pad or the up/down buttons.
     * 
     * Valid positions are -1, 0 and 1
     * @return y axis position
     */
    int axisY() const { return m_axisy; }

    void setAxisY(int value)
    {
        m_axisy = value;
        m_changed = true;
    }

    /**
     * Get the status of the first trigger button.
     *
     * @return true if button is down
     */
    bool trigger1() const { return m_trigger1; }

    void setTrigger1(bool value)
    {
        m_trigger1 = value;
        m_changed = true;
    }

    /**
     * Get the status of the second trigger button.
     *
     * @return true if button is down
     */
    bool trigger2() const { return m_trigger2; }

    void setTrigger2(bool value)
    {
        m_trigger2 = value;
        m_changed = true;
    }

    /**
     * Have any of the inputs changed since this object was last cleared?
     *
     * @return true if inputs have changes since the last clear() call
     */
    bool isChanged() const { return m_changed; }

    /**
     * Clear the "changed" flag.
     */
    void clear() { m_changed = false; }

    /**
     * Reset controller state to zero.
     */
    void reset();

private:
    int m_axisx, m_axisy;
    bool m_trigger1, m_trigger2;
    bool m_changed;
};

/**
 * Get the input device status buffer for the player
 *
 * Note. The player number is the local player number in range 1..Hotseat::MAX_PLAYERS
 * In a network game, the player's actual number may be something else, so the local
 * number must be mapped to the true number.
 *
 * @param player player number (1..Hotseat::MAX_PLAYERS)
 * @return player input
 */
PlayerInput &getPlayerInput(int player);

/**
 * Initialize player input key mappings and bind event handlers.
 *
 */
void initPlayerInputs();

/**
 * Unbind player input event handlers
 */
void deinitPlayerInputs();

}

#endif

