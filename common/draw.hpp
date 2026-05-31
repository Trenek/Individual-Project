#include <print>
#include <vector>

struct thing {
    FILE *gnuplot;
    FILE *dataFile;
    const char *name;
    const char *file;

    const char *xName;
    const char *yName;

    void (*setGNUPlot)(int id, struct thing &drawer);
};

class gnuPlotManager {
    bool isEnabled = false;

public:
    std::vector<struct thing> drawers;

    gnuPlotManager(std::vector<struct thing> &&array, bool init = false);
    ~gnuPlotManager();

    template <typename... Args>
    void print(size_t i, std::format_string<Args...> s, Args&&... args) {
        std::print(this->drawers[i].dataFile, s, std::forward<Args>(args)...);
    }

    void fflush();
    void removeData();
    void initGNUPlot();
};
