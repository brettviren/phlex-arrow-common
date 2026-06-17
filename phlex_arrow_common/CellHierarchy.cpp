#include "phlex_arrow_common/CellHierarchy.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>

namespace phlex_arrow {

namespace {

// Find the child cell matching `c`, or append a fresh one and return it.
// (Pointer stays valid for the remainder of one address insertion: we only
// descend into the returned node's children and never push to `children`
// again during that insertion.)
CellNode& find_or_add(std::vector<CellNode>& children, const Cell& c)
{
    for (auto& ch : children) {
        if (ch.layer == c.layer && ch.number == c.number) return ch;
    }
    children.push_back(CellNode{c.layer, c.number, {}, {}});
    return children.back();
}

void sort_cells(std::vector<CellNode>& cells)
{
    std::sort(cells.begin(), cells.end(),
              [](const CellNode& a, const CellNode& b) { return a.number < b.number; });
    for (auto& cell : cells) {
        std::sort(cell.products.begin(), cell.products.end(),
                  [](const ProductLoc& a, const ProductLoc& b) {
                      return std::tie(a.creator, a.product) < std::tie(b.creator, b.product);
                  });
        sort_cells(cell.children);
    }
}

}  // namespace

CellHierarchy CellHierarchy::from_addresses(const std::vector<std::vector<std::string>>& addresses)
{
    CellHierarchy h;

    for (const auto& components : addresses) {
        const StructuredAddress sa = parse_address(components);

        // One layer name per depth: record on first sight, validate thereafter.
        for (std::size_t d = 0; d < sa.cells.size(); ++d) {
            if (d < h.layer_names.size()) {
                if (h.layer_names[d] != sa.cells[d].layer) {
                    throw std::invalid_argument(
                        "CellHierarchy: inconsistent layer name at depth " + std::to_string(d) +
                        ": '" + h.layer_names[d] + "' vs '" + sa.cells[d].layer + "'");
                }
            }
            else {
                h.layer_names.push_back(sa.cells[d].layer);
            }
        }

        // Descend/create cells; attach the product at the leaf (or the root).
        std::vector<CellNode>* level = &h.cells;
        CellNode* node = nullptr;
        for (const auto& c : sa.cells) {
            node = &find_or_add(*level, c);
            level = &node->children;
        }
        ProductLoc loc{sa.creator, sa.product};
        if (node) {
            node->products.push_back(std::move(loc));
        }
        else {
            h.root_products.push_back(std::move(loc));
        }
    }

    sort_cells(h.cells);
    std::sort(h.root_products.begin(), h.root_products.end(),
              [](const ProductLoc& a, const ProductLoc& b) {
                  return std::tie(a.creator, a.product) < std::tie(b.creator, b.product);
              });
    return h;
}

}  // namespace phlex_arrow
