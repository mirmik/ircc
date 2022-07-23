#include <fstream>
#include <getopt.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>

struct KeySource
{
    std::string key;
    std::string source;
};

struct KeyText
{
    std::string key;
    std::string text;
};

struct KeyBytes
{
    std::string key;
    std::string bytes;
};

uint8_t HIHALF(uint8_t byte)
{
    return (byte >> 4) & 0x0F;
}

uint8_t LOHALF(uint8_t byte)
{
    return byte & 0x0F;
}

char half2hex(uint8_t n)
{
    return (char)(n < 10 ? '0' + n : 'A' - 10 + n);
}

std::string uint8_to_hex(uint8_t in)
{
    char buf[2];
    char *hex = buf;
    *hex++ = half2hex(HIHALF(in));
    *hex++ = half2hex(LOHALF(in));
    return {buf, 2};
}

std::vector<KeySource> get_sources_from_file(std::string listfile)
{
    std::vector<KeySource> sources;
    std::ifstream file(listfile);
    std::string line;
    while (std::getline(file, line))
    {
        std::string key = line.substr(0, line.find(" "));
        std::string source = line.substr(line.find(" ") + 1);
        sources.push_back(KeySource{key, source});
    }
    return sources;
}

std::vector<KeyText> keysources_to_keytexts(std::vector<KeySource> sources)
{
    size_t errors = 0;
    std::vector<KeyText> keytexts;
    for (auto source : sources)
    {
        std::ifstream file(source.source);

        if (!file.is_open())
        {
            std::cout << "Fatal: Could not open file " << source.source
                      << std::endl;
            errors++;
            continue;
        }

        std::string line;
        std::string text;
        while (std::getline(file, line))
        {
            text += line + "\n";
        }
        keytexts.push_back(KeyText{source.key, text});
    }
    if (errors > 0)
    {
        std::cout << "Fatal: " << errors << " errors occurred." << std::endl;
        exit(1);
    }
    return keytexts;
}

std::vector<KeyBytes> keytexts_to_keybytes(std::vector<KeyText> keytexts)
{
    std::vector<KeyBytes> keybytes;
    for (auto keytext : keytexts)
    {
        std::string bytes;
        for (auto c : keytext.text)
        {
            bytes += "\\x" + uint8_to_hex(c);
        }
        keybytes.push_back(KeyBytes{keytext.key, bytes});
    }
    return keybytes;
}

std::string compile_output(std::vector<KeyBytes> keybytes)
{

    std::string compiled_keybytes = "";
    for (auto keybyte : keybytes)
    {
        compiled_keybytes += "\t{\"" + keybyte.key + "\",\n";
        size_t size = keybyte.bytes.size();
        if (size == 0)
        {
            compiled_keybytes += "\t\t\"\",\n";
            continue;
        }
        size_t writed = 0;
        while (size - writed != 0)
        {
            size_t writable = size - writed > 18 * 4 ? 18 * 4 : size - writed;
            compiled_keybytes +=
                "\t\t\"" + keybyte.bytes.substr(writed, writable) + "\"\n";
            writed += writable;
        }
        compiled_keybytes += "\t},\n";
    }

    return R"(#include <map>
#include <string>

std::map<std::string, std::string> IRCC_RESOURCES = {
)" + compiled_keybytes +
           R"(};
)";
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cout << "Usage: " << argv[0] << " <listfile>"
                  << " <outcpp>" << std::endl;
        return 1;
    }

    std::string listfile = argv[1];
    std::string outfile = argv[2];

    auto sources = get_sources_from_file(listfile);
    auto texts = keysources_to_keytexts(sources);
    auto keybytes = keytexts_to_keybytes(texts);
    auto outtext = compile_output(keybytes);
    std::ofstream out(outfile);
    out << outtext;
    out.close();
    return 0;
}