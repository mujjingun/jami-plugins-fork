#include "../asrmodel.h"

#include <iostream>
#include <fstream>
#include <chrono>

int main()
{
    std::setlocale(LC_ALL, "");

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

    using namespace std::chrono;
    auto start = system_clock::now();
    model.process(buf.data(), buf.size());
    auto elapsed = system_clock::now() - start;

    std::cout << "elapsed = " << duration_cast<milliseconds>(elapsed).count() << "ms\n";

    return 0;
}
