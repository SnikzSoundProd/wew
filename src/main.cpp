#include "core/Engine.h"
#include <vector>

int main() {
    Engine engine;
    try {
        engine.run();
    }
    catch (...) {
        // Можно добавить обработку исключений
        return -1;
    }
    return 0;
}