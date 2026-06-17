#include "phlex_arrow_common/CellAddress.h"

#include <stdexcept>

namespace phlex_arrow {

std::vector<std::string> make_address(const std::vector<Cell>& cells,
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
    return components;
}

std::vector<std::string> make_address(const StructuredAddress& addr)
{
    return make_address(addr.cells, addr.creator, addr.product);
}

StructuredAddress parse_address(const std::vector<std::string>& components)
{
    if (components.size() < 2) {
        throw std::invalid_argument("parse_address: need at least creator and product");
    }
    const std::size_t ncells = components.size() - 2;
    if (ncells % 2 != 0) {
        throw std::invalid_argument("parse_address: cell components not paired");
    }

    StructuredAddress out;
    for (std::size_t i = 0; i < ncells; i += 2) {
        std::int64_t number = 0;
        try {
            std::size_t consumed = 0;
            number = std::stoll(components[i + 1], &consumed);
            if (consumed != components[i + 1].size()) throw std::invalid_argument("trailing");
        } catch (const std::exception&) {
            throw std::invalid_argument("parse_address: bad cell number '" + components[i + 1] + "'");
        }
        out.cells.push_back(Cell{components[i], number});
    }
    out.creator = components[components.size() - 2];
    out.product = components[components.size() - 1];
    return out;
}

}  // namespace phlex_arrow
