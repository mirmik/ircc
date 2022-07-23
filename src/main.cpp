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
    char buf[1024];
    std::ifstream file(source.source);
    std::string text;
    int readed;
    while ((readed = file.readsome(buf, sizeof(buf))) > 0)
    {
        text.append(buf, readed);
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
        compiled += keybytes.bytes.substr(writed, writable) + "\"";
        writed += writable;
        if (size - writed != 0)
            compiled += "\n";
    }
    return KeyBytesDivided{keybytes.key, compiled};
}

std::string compile_headers(bool cpp_enabled, bool include_map)
{
    std::string headers;
    if (include_map)
        headers += "#include <map>\n";
    if (cpp_enabled)
        headers += "#include <string>\n";
    headers += "#include <string.h>\n";
    return headers;
}

std::string compile_ircc_resources_consts(std::vector<KeyBytes> keybytes)
{
    std::string compiled;
    for (int i = 0; i < keybytes.size(); i++)
    {
        compiled += "const char* IRCC_RESOURCES_" + std::to_string(i) + " = \n";
        compiled += keybytes_to_keybytesdivided(keybytes[i], 2).bytes_divided;
        compiled += ";\n\n";
    }
    return compiled;
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
            keybytes_to_keybytesdivided(keybyte, 2).bytes_divided + "\n";
        compiled_keybytes += "\t},\n";
    }

    return R"(std::map<std::string, std::string> IRCC_RESOURCES = {
)" + compiled_keybytes +
           R"(};
)";
}

std::string compile_ircc_resources_map_by_mnemos(std::vector<KeyBytes> keybytes)
{

    std::string compiled_keybytes = "";
    for (size_t i = 0; i < keybytes.size(); ++i)
    {
        compiled_keybytes += "\t{\"" + keybytes[i].key + "\", ";
        if (keybytes[i].bytes.size() == 0)
        {
            compiled_keybytes += "\t\t\"\",\n";
            continue;
        }
        compiled_keybytes += "IRCC_RESOURCES_" + std::to_string(i);
        compiled_keybytes += "},\n";
    }

    return R"(std::map<std::string, std::string> IRCC_RESOURCES = {
)" + compiled_keybytes +
           R"(};
)";
}

std::string compile_ircc_resources_map_cstyle(std::vector<KeyBytes> keybytes)
{

    std::string compiled;
    compiled += "struct key_value_size IRCC_RESOURCES_[] = {\n";
    for (size_t i = 0; i < keybytes.size(); ++i)
    {
        compiled += "\t{\"" + keybytes[i].key + "\", ";
        if (keybytes[i].bytes.size() == 0)
        {
            compiled += "\t\t\"\",\n";
            continue;
        }
        compiled += "IRCC_RESOURCES_" + std::to_string(i);
        compiled += ", ";
        compiled += std::to_string(keybytes[i].bytes.size() / 4);
        compiled += "},\n";
    }
    compiled += "\t{NULL, NULL, 0}};\n";
    return compiled;
}

std::string text_struct_key_value_size()
{
    return R"(struct key_value_size
{
    const char *key;
    const char *value;
    size_t size;
};
)";
}

std::string text_binary_search_function()
{
    return R"(struct key_value_size *ircc_binary_search(const char *key)
{
    int low = 0;
    int high = sizeof(IRCC_RESOURCES_) / sizeof(IRCC_RESOURCES_[0]) - 1;
    int mid;
    while (low <= high)
    {
        mid = (low + high) / 2;
        int cmp = strcmp(key, IRCC_RESOURCES_[mid].key);
        if (cmp < 0)
            high = mid - 1;
        else if (cmp > 0)
            low = mid + 1;
        else
            return &IRCC_RESOURCES_[mid];
    }
    return NULL;
})";
}

std::string text_c_functions()
{
    return R"(const char *ircc_c_string(const char *key)
{
    struct key_value_size *kvs = ircc_binary_search(key);
    if (kvs == NULL)
        return "";
    return kvs->value;
}

size_t ircc_size(const char *key)
{
    struct key_value_size *kvs = ircc_binary_search(key);
    if (kvs == NULL)
        return 0;
    return kvs->size;
}
)";
}

std::string text_cxx_functions()
{
    return R"(std::string ircc_string(const char *key)
{
    struct key_value_size *kvs = ircc_binary_search(key);
    if (kvs == NULL)
        return "";
    return std::string(kvs->value, kvs->size);
}
)";
}

int main(int argc, char **argv)
{
    bool CPP_ENABLED = true;
    bool include_map = true;

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
    out << compile_headers(CPP_ENABLED, include_map);
    out << "\n";
    out << compile_ircc_resources_consts(keybytes);
    out << text_struct_key_value_size();
    out << "\n";
    out << compile_ircc_resources_map_cstyle(keybytes);
    out << "\n";
    out << compile_ircc_resources_map_by_mnemos(keybytes);
    out << "\n";
    out << text_binary_search_function();
    out << "\n";
    out << text_c_functions();

    if (CPP_ENABLED)
    {
        out << "\n";
        out << text_cxx_functions();
    }
    out.close();
    return 0;
}
