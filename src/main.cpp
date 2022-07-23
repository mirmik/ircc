#include <algorithm>
#include <fstream>
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

struct KeyBytesDivided
{
    std::string key;
    std::string bytes_divided;
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

size_t check_exists(const std::vector<KeySource> &sources)
{
    size_t errors = 0;
    for (auto source : sources)
    {
        if (!std::ifstream(source.source).good())
        {
            std::cout << "Fatal: Could not open file " << source.source
                      << std::endl;
            errors++;
        }
    }
    return errors;
}

KeyText keysource_to_keytext(KeySource source)
{
    std::ifstream file(source.source);
    std::string line;
    std::string text;
    while (std::getline(file, line))
    {
        text += line + "\n";
    }
    return KeyText{source.key, text};
}

std::vector<KeyText> keysources_to_keytexts(std::vector<KeySource> sources)
{
    std::vector<KeyText> keytexts;
    std::transform(sources.begin(),
                   sources.end(),
                   std::back_inserter(keytexts),
                   keysource_to_keytext);
    return keytexts;
}

KeyBytes keytext_to_keybytes(KeyText text)
{
    std::string bytes;
    for (char c : text.text)
    {
        bytes += "\\x" + uint8_to_hex(c);
    }
    return KeyBytes{text.key, bytes};
}

std::vector<KeyBytes> keytexts_to_keybytes(std::vector<KeyText> keytexts)
{
    std::vector<KeyBytes> keybytes;
    std::transform(keytexts.begin(),
                   keytexts.end(),
                   std::back_inserter(keybytes),
                   keytext_to_keybytes);
    return keybytes;
}

KeyBytesDivided keybytes_to_keybytesdivided(KeyBytes keybytes, int tabs)
{
    size_t size = keybytes.bytes.size();
    size_t writed = 0;
    std::string compiled;
    while (size - writed != 0)
    {
        size_t writable = size - writed > 18 * 4 ? 18 * 4 : size - writed;
        for (int i = 0; i < tabs; i++)
            compiled += "\t";
        compiled += "\"";
        compiled += keybytes.bytes.substr(writed, writable) + "\"\n";
        writed += writable;
    }
    return KeyBytesDivided{keybytes.key, compiled};
}

std::string compile_headers()
{
    return R"(#include <map>
#include <string>
)";
}

std::string compile_ircc_resources_map(std::vector<KeyBytes> keybytes)
{

    std::string compiled_keybytes = "";
    for (auto keybyte : keybytes)
    {
        compiled_keybytes += "\t{\"" + keybyte.key + "\",\n";
        if (keybyte.bytes.size() == 0)
        {
            compiled_keybytes += "\t\t\"\",\n";
            continue;
        }
        compiled_keybytes +=
            keybytes_to_keybytesdivided(keybyte, 2).bytes_divided;
        compiled_keybytes += "\t},\n";
    }

    return R"(std::map<std::string, std::string> IRCC_RESOURCES = {
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

    int errors = check_exists(sources);
    if (errors > 0)
    {
        std::cout << "Fatal: " << errors << " errors occurred." << std::endl;
        exit(1);
    }

    auto texts = keysources_to_keytexts(sources);
    auto keybytes = keytexts_to_keybytes(texts);
    std::ofstream out(outfile);
    out << compile_headers();
    out << "\n";
    out << compile_ircc_resources_map(keybytes);
    out.close();
    return 0;
}
