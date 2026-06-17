#include "phlex_arrow_common/CellAddress.h"

#include <stdexcept>

namespace phlex_arrow {

namespace {
// Split "/a/b/c" into {"a","b","c"} (leading '/' dropped; "/" -> {}).
std::vector<std::string> split_path(const std::string& path)
{
    std::vector<std::string> parts;
    std::size_t start = 0;
    while (start < path.size()) {
        if (path[start] == '/') { ++start; continue; }
        std::size_t slash = path.find('/', start);
        if (slash == std::string::npos) { parts.push_back(path.substr(start)); break; }
        parts.push_back(path.substr(start, slash - start));
        start = slash + 1;
    }
    return parts;
}
}  // namespace

arrow_hdf::Address make_address(const std::vector<Cell>& cells,
                                const std::string& creator, const std::string& product)
{
    std::vector<std::string> components;
    components.reserve(cells.size() * 2 + 2);
    for (const auto& c : cells) {
        components.push_back(c.layer);
        components.push_back(std::to_string(c.number));
    }
    components.push_back(creator);
    components.push_back(product);
    return arrow_hdf::Address(components);
}

arrow_hdf::Address make_address(const StructuredAddress& addr)
{
    return make_address(addr.cells, addr.creator, addr.product);
}

StructuredAddress parse_address(const arrow_hdf::Address& addr)
{
    // Components are escaped in the path; unescape each.
    std::vector<std::string> raw = split_path(addr.path());
    std::vector<std::string> parts;
    parts.reserve(raw.size());
    for (const auto& r : raw) parts.push_back(arrow_hdf::path_unescape(r));

    if (parts.size() < 2) {
        throw std::invalid_argument("parse_address: need creator and product: '" + addr.path() + "'");
    }
    const std::size_t ncells = parts.size() - 2;
    if (ncells % 2 != 0) {
        throw std::invalid_argument("parse_address: cell components not paired: '" + addr.path() + "'");
    }

    StructuredAddress out;
    for (std::size_t i = 0; i < ncells; i += 2) {
        std::int64_t number = 0;
        try {
            std::size_t consumed = 0;
            number = std::stoll(parts[i + 1], &consumed);
            if (consumed != parts[i + 1].size()) throw std::invalid_argument("trailing");
        } catch (const std::exception&) {
            throw std::invalid_argument("parse_address: bad cell number '" + parts[i + 1] + "'");
        }
        out.cells.push_back(Cell{parts[i], number});
    }
    out.creator = parts[parts.size() - 2];
    out.product = parts[parts.size() - 1];
    return out;
}

}  // namespace phlex_arrow
