#include "phlex_arrow_common/CellAddress.hpp"

#include <stdexcept>

namespace phlex_arrow {

std::vector<std::string> make_address(const std::vector<Cell>& cells, const std::string& stage,
                                      const std::string& plugin, const std::string& algorithm,
                                      const std::string& product)
{
    std::vector<std::string> components;
    components.reserve(cells.size() * 2 + 4);
    for (const auto& c : cells) {
        components.push_back(c.layer);
        components.push_back(std::to_string(c.number));
    }
    components.push_back(stage.empty() ? kCurrentStage : stage);
    components.push_back(plugin.empty() ? kEmptyPart : plugin);
    components.push_back(algorithm.empty() ? kEmptyPart : algorithm);
    components.push_back(product.empty() ? kEmptyPart : product);
    return components;
}

std::vector<std::string> make_address(const StructuredAddress& addr)
{
    return make_address(addr.cells, addr.stage, addr.plugin, addr.algorithm, addr.product);
}

std::pair<std::string, std::string> split_creator(const std::string& creator)
{
    const auto colon = creator.find(':');
    if (colon == std::string::npos) {
        return {creator, kEmptyPart};
    }
    return {creator.substr(0, colon), creator.substr(colon + 1)};
}

std::string canonical_creator(const std::string& creator)
{
    auto [plugin, algorithm] = split_creator(creator);
    return plugin + ":" + algorithm;
}

StructuredAddress parse_address(const std::vector<std::string>& components)
{
    if (components.size() < 4) {
        throw std::invalid_argument(
            "parse_address: need at least stage, plugin, algorithm and product");
    }
    const std::size_t ncells = components.size() - 4;
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
    out.stage = components[components.size() - 4];
    out.plugin = components[components.size() - 3];
    out.algorithm = components[components.size() - 2];
    out.product = components[components.size() - 1];
    return out;
}

}  // namespace phlex_arrow
