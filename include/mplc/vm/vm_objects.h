#pragma once
#include <share/config.h>
#include <mplc/vm/item_id.h>
namespace mplc {
    struct ShareAddin;
}

namespace mplc { namespace api {
    struct ApiStorage;
    struct ScadaFB;
    struct ScadaProtocol;
}}  // namespace mplc::api

namespace mplc { namespace vm {
    struct ItemID;
    struct VmObjects final {
        MPLCSHARE_API api::ScadaProtocol* GetProtocol(const ItemID& id);
        MPLCSHARE_API api::ScadaFB* GetFB(const ItemID& id);

        void RegFB(const ItemID& id, api::ScadaFB* fb);
        void RegProtocol(const ItemID& id, api::ScadaProtocol* protocol);
        ~VmObjects();
        VmObjects();

    private:
        friend api::ApiStorage;
        friend ShareAddin;
        struct impl;
        impl* pimpl;
        void clear();
    };
    api::ScadaProtocol* FindScadaProcotol(const ItemID& id);
    api::ScadaFB* FindScadaFB(const ItemID& id);
}}  // namespace mplc::vm
