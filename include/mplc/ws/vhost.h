#pragma once
#include "dependency.h"
#include "websocket_protocol.h"
namespace mplc { namespace ws {
    struct client_connect_info;
    struct client_interface;
    /*struct vhost_t;
    using vhost_ptr = lib::unique_ptr<vhost_t>;*/
    struct MPLCSHARE_API vhost_t {
        explicit vhost_t(lib::string_view name, uint16_t port, bool use_ssl = false, lib::string_view iface = {});
        virtual ~vhost_t();
        virtual void on_finalize();
        virtual int on_event(lws* wsi, int event_type, void* user, void* in, size_t len);
        // static int callback(lws* wsi, int event_type, void* user, void* in, size_t len);
        std::string get_ip_str(const void* socket_addr, bool* ok = nullptr);
        virtual int confirm_upgrade(lws* wsi, void* user, void* in, size_t len);
        // Callback for allocate user space for wsi
        virtual int bind_protocol(lws* wsi, void* user, void* in, size_t len);
        virtual int init_protocol(lws* wsi, void* user, void* in, size_t len);
        virtual int destroy_protocol(lws* wsi, void* user, void* in, size_t len);
        virtual int config_ssl(void* ssl_ctx);
        virtual int init(lws_context* ctx);
        virtual void on_config(lws_context_creation_info& info) {}
        virtual void client_connect(client_connect_info& info, const lib::shared_ptr<client_interface>& client);
        lib::string_view name() const;
        /**
         * \brief
         * \param cert_path
         * \param key_path
         * \param password 0 terminated string
         */
        virtual void use_ssl(/*lib::string_view cert_path,
                                           lib::string_view key_path,
                                           lib::string_view password = {}*/);
        virtual void stop();
        /**
         * \brief Вызывается, когда клиент подключается к серверу на сетевом уровне; соединение принимается, но
         затем передается в этот метод, чтобы решить, следует ли немедленно закрыть соединение, основываясь на IP
         клиента. struct lws_filter_network_conn_args,
         * \param args Подготавливается вместе с sockfd и содержит информацию об адресе пира.
         * \param in Содержит дескриптор сокета соединения.
         * \return Верните ненулевое значение, чтобы прервать соединение до отправки или получения чего-либо.
         */
        virtual int filter_network_connection(const void* network_conn_args, void* in);
        virtual int add_protocol(lib::unique_ptr<protocol_t>&& protocol, lib::string_view name);
        virtual int set_default_protocol(lib::unique_ptr<protocol_t>&& protocol);
        virtual protocol_t* get_default_protocol();
        virtual websocket_protocol_t* find_ws_protocol(lib::string_view name);
        virtual http_protocol_t* find_http_protocol(lib::string_view name);
        virtual protocol_t* find_protocol(lib::string_view name);
        // void mount_callback(lib::string_view mountpoint, lib::string_view url, lib::string_view protocol) {
        //     lws_http_mount* mount = new lws_http_mount();
        //     memset(mount, 0, sizeof(lws_http_mount));
        //     {
        //         char* tmp = new char[mountpoint.size() + 1];
        //         mountpoint.copy(tmp, mountpoint.size());
        //         tmp[mountpoint.size()] = 0;
        //         mount->mountpoint = tmp;
        //     }
        //     mount->mountpoint_len = mountpoint.size();
        //     {
        //         char* tmp = new char[protocol.size() + 1];
        //         protocol.copy(tmp, protocol.size());
        //         tmp[protocol.size()] = 0;
        //         mount->protocol = tmp;
        //     }
        //     mount->origin_protocol = static_cast<unsigned char>(kv.second->type);
        // }

        int add_service(lib::weak_ptr<websocket_service_t> service,
                        lib::string_view path,
                        lib::string_view protocol = {});
        int add_service(lib::weak_ptr<http_service_t> service, lib::string_view protocol_name = {});
        virtual void mount_protocol(lib::string_view url, lib::string_view protocol);
        virtual void mount_dir(lib::string_view url, lib::string_view dir, lib::string_view def = "");
        // void mount_cgi(lib::string_view url, lib::string_view cgi) {
        //     auto& mnt = m_mounts[std::string(url)] = new http_mount_t;
        //     mnt->protocol = cgi;
        //     mnt->url = url;
        //     mnt->type = http_mount_t::mnt_cgi;
        // }
        // int init(context_t* ctx, const char* name);
        bool is_stop() const {
            return m_stop;
        }
        void restart();

    private:
        struct impl;
        impl* pimpl;
        friend struct context_t;
        static int pem_passwd_cb(char* buf, int size, int rwflag, void* userdata);
        uint16_t m_port;
        bool m_use_ssl{false};
        bool m_stop{false};
    };
}}  // namespace mplc::ws
