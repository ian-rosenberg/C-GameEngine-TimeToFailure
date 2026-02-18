#include "graphics_engine.h"

int main(int argc, char* argv[])
{
    GraphicsEngine* engine = initGraphicsEngine();

    engineLoop(engine);

    return 0;
}
