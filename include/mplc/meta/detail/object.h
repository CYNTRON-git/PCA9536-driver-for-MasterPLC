#pragma once
#include "table.h"
#include "utility.h"
#include "type_list.h"

namespace mplc { namespace meta { namespace detail {
    // struct PropertyTable;
    using PropertiesMap = lib::unordered_map<lib::string_view, const PropertyTable*>;

    struct ObjectTable : TypeTable {
        lib::string_view name;
        const ObjectTable* super;
        const PropertyTable* property_table;
        const size_t property_count;
        const PropertiesMap props;
        ObjectTable(Name type_name,
                    Create create,
                    Construct construct,
                    Destroy destroy,
                    Destruct destruct,
                    Clone clone,
                    Move move,
                    Serialize serialize,
                    Deserialize deserialize,
                    size_t size,
                    lib::string_view name,
                    const ObjectTable* super,
                    const PropertyTable* props_table,
                    size_t props_count)
            : TypeTable{type_name,
                        create,
                        construct,
                        destroy,
                        destruct,
                        clone,
                        move,
                        serialize,
                        deserialize,
                        size,
                        MetaType::Object},
              name(name), super(super), property_table(props_table), property_count(props_count),
              props(_create_properties_map(super, props_table, props_count)) {}

    private:
        static PropertiesMap _create_properties_map(const ObjectTable* parent,
                                                    const PropertyTable* props_table,
                                                    size_t props_size) {
            size_t count = props_size;
            {
                const ObjectTable* t = parent;
                while (t) {
                    if (t->property_table) {
                        count += t->property_count;
                    }
                    t = t->super;
                }
            }
            PropertiesMap props(count);
            auto add_prop = [](PropertiesMap& map, const PropertyTable* props_table, size_t props_size) {
                for (size_t i = 0; i < props_size; ++i) {
                    auto prop_table = &props_table[i];
                    auto& prop = map[prop_table->name];
                    if (!prop) {
                        prop = prop_table;
                    }
                }
            };
            if (props_table) {
                add_prop(props, props_table, props_size);
            }
            const ObjectTable* t = parent;
            while (t) {
                if (t->property_table) {
                    add_prop(props, t->property_table, t->property_count);
                }
                t = t->super;
            }
            return props;
        }
    };

}}}  // namespace mplc::meta::detail
