#include <gtest/gtest.h>
#include "../libs/Graphical/src/Input/MouseKeyboard.hpp"
#include "../libs/Graphical/src/Input/Gamepad.hpp"

using namespace input;

class TestMouseKeyboardHandler : public MouseKeyboardHandler
{
    public:
        TestMouseKeyboardHandler() = default;
        ~TestMouseKeyboardHandler() = default;

        void simButtonPressed(std::unordered_map<int, input::Generic> bindings, int button) {
            updateState(bindings[button], State::PRESSED);
        }
        void simButtonHeld(std::unordered_map<int, input::Generic> bindings, int button) {
            updateState(bindings[button], State::HELD);
        }
        void simButtonReleased(std::unordered_map<int, input::Generic> bindings, int button) {
            updateState(bindings[button], State::RELEASED);
        }
        void simButtonNotPressed(std::unordered_map<int, input::Generic> bindings, int button) {
            updateState(bindings[button], State::NOTPRESSED);
        }
        void simAddBinding(int binding, Generic input)
        {
            _inputBindings[binding] = input;
        }
        void simClearBinding(int binding)
        {
            _inputBindings[binding] = input::Generic::VOID;
        }
        void simEraseBinding(int binding)
        {
            _inputBindings.erase(binding);
        }
};

TEST(InputTest, MouseKeyboardInput) {
    TestMouseKeyboardHandler testMouseKeyboard;

    EXPECT_TRUE(testMouseKeyboard.getType() == input::Type::KEYBOARDMOUSE);

    // Check initial bindings
    std::unordered_map<int, input::Generic> bindings = testMouseKeyboard.getBindings();
    EXPECT_TRUE(bindings[MOUSE_BUTTON_LEFT] == input::Generic::SELECT1);
    EXPECT_TRUE(bindings[MOUSE_BUTTON_RIGHT] == input::Generic::SELECT2);
    EXPECT_TRUE(bindings[MOUSE_BUTTON_MIDDLE] == input::Generic::VOID);
    EXPECT_TRUE(bindings[KEY_UP] == input::Generic::UP);
    EXPECT_TRUE(bindings[KEY_DOWN] == input::Generic::DOWN);
    EXPECT_TRUE(bindings[KEY_LEFT] == input::Generic::LEFT);
    EXPECT_TRUE(bindings[KEY_RIGHT] == input::Generic::RIGHT);
    EXPECT_TRUE(bindings[KEY_ENTER] == input::Generic::INTERACT1);
    EXPECT_TRUE(bindings[KEY_Q] == input::Generic::INTERACT2);
    EXPECT_TRUE(bindings[KEY_E] == input::Generic::INTERACT3);
    EXPECT_TRUE(bindings[KEY_SPACE] == input::Generic::ATTACK);
    EXPECT_TRUE(bindings[KEY_TAB] == input::Generic::INVENTORY);
    EXPECT_TRUE(bindings[KEY_ESCAPE] == input::Generic::PAUSE);

    // Check generic input handling
    EXPECT_TRUE(testMouseKeyboard.isNotPressed(input::Generic::SELECT1));

    EXPECT_FALSE(testMouseKeyboard.isPressed(input::Generic::SELECT1));
    testMouseKeyboard.simButtonPressed(bindings, MOUSE_BUTTON_LEFT);
    EXPECT_TRUE(testMouseKeyboard.isPressed(input::Generic::SELECT1));

    EXPECT_FALSE(testMouseKeyboard.isHeld(input::Generic::SELECT1));
    testMouseKeyboard.simButtonHeld(bindings, MOUSE_BUTTON_LEFT);
    EXPECT_TRUE(testMouseKeyboard.isHeld(input::Generic::SELECT1));

    EXPECT_FALSE(testMouseKeyboard.isReleased(input::Generic::SELECT1));
    testMouseKeyboard.simButtonReleased(bindings, MOUSE_BUTTON_LEFT);
    EXPECT_TRUE(testMouseKeyboard.isReleased(input::Generic::SELECT1));

    EXPECT_FALSE(testMouseKeyboard.isNotPressed(input::Generic::SELECT1));
    testMouseKeyboard.simButtonNotPressed(bindings, MOUSE_BUTTON_LEFT);
    EXPECT_TRUE(testMouseKeyboard.isNotPressed(input::Generic::SELECT1));

    // Check binding management
    EXPECT_FALSE(testMouseKeyboard.getBindings()[MOUSE_BUTTON_MIDDLE] == input::Generic::SELECT1);

    testMouseKeyboard.simAddBinding(MOUSE_BUTTON_MIDDLE, input::Generic::SELECT1);
    EXPECT_TRUE(testMouseKeyboard.getBindings()[MOUSE_BUTTON_MIDDLE] == input::Generic::SELECT1);

    testMouseKeyboard.simClearBinding(MOUSE_BUTTON_MIDDLE);
    EXPECT_TRUE(testMouseKeyboard.getBindings()[MOUSE_BUTTON_MIDDLE] == input::Generic::VOID);

    testMouseKeyboard.simEraseBinding(MOUSE_BUTTON_MIDDLE);
    EXPECT_TRUE(testMouseKeyboard.getBindings().find(MOUSE_BUTTON_MIDDLE) == testMouseKeyboard.getBindings().end());
}

namespace input
{
    class TestGamepadHandler : public GamepadHandler
    {
        public:
            TestGamepadHandler() = default;
            ~TestGamepadHandler() = default;

            void simButtonPressed(std::unordered_map<Uint8, input::Generic> bindings, Uint8 button) {
                updateState(bindings[button], State::PRESSED);
            }
            void simButtonHeld(std::unordered_map<Uint8, input::Generic> bindings, Uint8 button) {
                updateState(bindings[button], State::HELD);
            }
            void simButtonReleased(std::unordered_map<Uint8, input::Generic> bindings, Uint8 button) {
                updateState(bindings[button], State::RELEASED);
            }
            void simButtonNotPressed(std::unordered_map<Uint8, input::Generic> bindings, Uint8 button) {
                updateState(bindings[button], State::NOTPRESSED);
            }
            void simAddBinding(Uint8 binding, Generic input)
            {
                _inputBindings[binding] = input;
            }
            void simClearBinding(Uint8 binding)
            {
                _inputBindings[binding] = input::Generic::VOID;
            }
            void simEraseBinding(Uint8 binding)
            {
                _inputBindings.erase(binding);
            }
    };
}

TEST(InputTest, GamepadInput)
{
    TestGamepadHandler testGamepad;

    EXPECT_TRUE(testGamepad.getType() == input::Type::GAMEPAD);

    // Check initial bindings
    std::unordered_map<Uint8, input::Generic> bindings = testGamepad.getBindings();
    EXPECT_TRUE(bindings[SDL_CONTROLLER_BUTTON_DPAD_UP] == input::Generic::UP);
    EXPECT_TRUE(bindings[SDL_CONTROLLER_BUTTON_DPAD_DOWN] == input::Generic::DOWN);
    EXPECT_TRUE(bindings[SDL_CONTROLLER_BUTTON_DPAD_LEFT] == input::Generic::LEFT);
    EXPECT_TRUE(bindings[SDL_CONTROLLER_BUTTON_DPAD_RIGHT] == input::Generic::RIGHT);
    EXPECT_TRUE(bindings[SDL_CONTROLLER_BUTTON_A] == input::Generic::INTERACT1);
    EXPECT_TRUE(bindings[SDL_CONTROLLER_BUTTON_B] == input::Generic::ATTACK);
    EXPECT_TRUE(bindings[SDL_CONTROLLER_BUTTON_X] == input::Generic::INVENTORY);
    EXPECT_TRUE(bindings[SDL_CONTROLLER_BUTTON_START] == input::Generic::PAUSE);

    // Check generic input handling
    EXPECT_TRUE(testGamepad.isNotPressed(input::Generic::INTERACT1));

    EXPECT_FALSE(testGamepad.isPressed(input::Generic::INTERACT1));
    testGamepad.simButtonPressed(bindings, SDL_CONTROLLER_BUTTON_A);
    EXPECT_TRUE(testGamepad.isPressed(input::Generic::INTERACT1));

    EXPECT_FALSE(testGamepad.isHeld(input::Generic::INTERACT1));
    testGamepad.simButtonHeld(bindings, SDL_CONTROLLER_BUTTON_A);
    EXPECT_TRUE(testGamepad.isHeld(input::Generic::INTERACT1));

    EXPECT_FALSE(testGamepad.isReleased(input::Generic::INTERACT1));
    testGamepad.simButtonReleased(bindings, SDL_CONTROLLER_BUTTON_A);
    EXPECT_TRUE(testGamepad.isReleased(input::Generic::INTERACT1));

    EXPECT_FALSE(testGamepad.isNotPressed(input::Generic::INTERACT1));
    testGamepad.simButtonNotPressed(bindings, SDL_CONTROLLER_BUTTON_A);
    EXPECT_TRUE(testGamepad.isNotPressed(input::Generic::INTERACT1));

    // Check binding management

    // Add binding
    EXPECT_TRUE(testGamepad.getBindings().find(SDL_CONTROLLER_BUTTON_LEFTSTICK) == testGamepad.getBindings().end());
    testGamepad.simAddBinding(SDL_CONTROLLER_BUTTON_LEFTSTICK, input::Generic::INVENTORY);
    EXPECT_TRUE(testGamepad.getBindings()[SDL_CONTROLLER_BUTTON_LEFTSTICK] == input::Generic::INVENTORY);

    // Modify binding
    EXPECT_FALSE(testGamepad.getBindings()[SDL_CONTROLLER_BUTTON_START] == input::Generic::INVENTORY);
    testGamepad.simAddBinding(SDL_CONTROLLER_BUTTON_START, input::Generic::INVENTORY);
    EXPECT_TRUE(testGamepad.getBindings()[SDL_CONTROLLER_BUTTON_START] == input::Generic::INVENTORY);

    // Clear binding
    testGamepad.simClearBinding(SDL_CONTROLLER_BUTTON_START);
    EXPECT_TRUE(testGamepad.getBindings()[SDL_CONTROLLER_BUTTON_START] == input::Generic::VOID);

    // Erase binding
    testGamepad.simEraseBinding(SDL_CONTROLLER_BUTTON_START);
    EXPECT_TRUE(testGamepad.getBindings().find(SDL_CONTROLLER_BUTTON_START) == testGamepad.getBindings().end());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
