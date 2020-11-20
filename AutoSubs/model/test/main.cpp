#include "../asrmodel.h"

int main()
{
    std::setlocale(LC_ALL, "");

    ASRModel model("data/model.zip");

    short buf[1024]{};

    model.process(buf, 1024);

    return 0;
}
