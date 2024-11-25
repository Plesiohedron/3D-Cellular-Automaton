#include "Engine/Engine.h"

int main() {
    // Creation and launch of the universe
    Engine engine(1280, 720, "Window");
    engine.MainLoop();
    return 0;
}