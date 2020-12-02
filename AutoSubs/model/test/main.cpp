#include "../asrmodel.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>

int main()
{
    ASRModel model("data/model.zip");

    std::vector<std::int16_t> buf;
    {
        std::ifstream in("model/test/KsponSpeech_000143.pcm", std::ios::binary);
        in.ignore(std::numeric_limits<std::streamsize>::max());
        std::streamsize length = in.gcount();
        in.clear(); //  Since ignore will have set eof.
        in.seekg(0, std::ios_base::beg);

        buf.resize(length / 2);
        in.read(reinterpret_cast<char*>(buf.data()), length);
    }

    auto output = model.process(buf.data(), buf.size());

    return 0;
}
