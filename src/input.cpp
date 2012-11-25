#include <GL/glfw.h>
#include <unordered_map>
#include <array>

#include "gameinit.h"
#include "input.h"

namespace input {

namespace {
    std::array<PlayerInput, gameinit::Hotseat::MAX_PLAYERS> INPUTS;

    enum KeyAction {
        UP, DOWN, LEFT, RIGHT, TRIGGER1, TRIGGER2
    };

    struct KeyMapping {
        KeyMapping() = default;
        KeyMapping(int p, KeyAction ka) : player(p), action(ka) { }

        int player;
        KeyAction action;
    };

    std::unordered_map<int,KeyMapping> KEYMAP;

    void playerKeyHandler(int keyid, int keystate)
    {
        if(KEYMAP.count(keyid)!=0) {
            const KeyMapping km = KEYMAP[keyid];
            PlayerInput &pi = INPUTS[km.player];

            switch(km.action) {
                case UP:
                case DOWN:
                    if(keystate == GLFW_PRESS)
                        pi.setAxisY(km.action == UP ? 1 : -1);
                    else
                        pi.setAxisY(0);
                    break;
                case LEFT:
                case RIGHT:
                    if(keystate == GLFW_PRESS)
                        pi.setAxisX(km.action == RIGHT ? 1 : -1);
                    else
                        pi.setAxisX(0);
                    break;
                case TRIGGER1:
                    pi.setTrigger1(keystate == GLFW_PRESS);
                    break;
                case TRIGGER2:
                    pi.setTrigger2(keystate == GLFW_PRESS);
                    break;
            }
        }
    }
}

PlayerInput::PlayerInput() { reset(); }

void PlayerInput::reset()
{
    m_axisx = 0;
    m_axisy = 0;
    m_trigger1 = false;
    m_trigger2 = false;
    m_changed = true;
}

PlayerInput &getPlayerInput(int player)
{
    return INPUTS.at(player-1);
}

void initPlayerInputs()
{
    KEYMAP.clear();
    // TODO load keymap from a configuration file
    KEYMAP[GLFW_KEY_LEFT] = KeyMapping(0, LEFT);
    KEYMAP[GLFW_KEY_RIGHT] = KeyMapping(0, RIGHT);
    KEYMAP[GLFW_KEY_UP] = KeyMapping(0, UP);
    KEYMAP[GLFW_KEY_RSHIFT] = KeyMapping(0, TRIGGER1);
    KEYMAP[GLFW_KEY_RCTRL] = KeyMapping(0, TRIGGER2);

    glfwSetKeyCallback(playerKeyHandler);
}

void deinitPlayerInputs()
{
    glfwSetKeyCallback(nullptr);
}

}