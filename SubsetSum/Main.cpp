#include "InteractiveCLI.h"

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    InteractiveCLI app;
    app.run();

    return 0;
}