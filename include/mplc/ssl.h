#pragma once
#include <openssl/ossl_typ.h>
#include <string>
#include <mplc/libs/string_view.hpp>
#include <share/config.h>

#include "date_time.h"

namespace mplc::ssl {
    void init();
    MPLCSHARE_API int create_certificate_x509_rsa(const std::string& cert_path,
                                                  const std::string& key_path,
                                                  const std::string& passwd = "");
    MPLCSHARE_API int create_certificate_x509_edhc(const std::string& cert_path,
                                                   const std::string& key_path,
                                                   const std::string& passwd = "");
    /**
     * \brief Создаёт ключ фиксированной длинны
     * \param passwd Пароль произвольной длинны
     * \param salt Соль
     * \return sha256 хеш от шифрованой строки пароля методом aes_256_cbc
     */
    MPLCSHARE_API std::string sha256_password(lib::string_view passwd, uint64_t salt = 0);
    MPLCSHARE_API X509* generate_x509(EVP_PKEY* pkey,
                                      DateTime from = DateTime::now(),
                                      DateTime to = DateTime::now() + TimeSpan::Days(365 * 5),
                                      const char* C = "MO",
                                      const char* O = "MasterSCADA",
                                      const char* CN = "localhost");
    MPLCSHARE_API EVP_PKEY* gen_256v1_curve();
    MPLCSHARE_API bool save_certificate(EVP_PKEY* pkey,
                                        X509* x509,
                                        const std::string& cert_path,
                                        const std::string& key_path,
                                        const std::string& passwd = "");
    MPLCSHARE_API int init_ssl_ctx(SSL_CTX* ctx);
    MPLCSHARE_API DateTime get_expire_date(const std::string& cert_path);
    MPLCSHARE_API std::string gen_salt(int bytes = 16);
    MPLCSHARE_API std::string argon2(lib::string_view password,
                                     lib::string_view salt,
                                     uint32_t lanes = 1,
                                     uint32_t memcost = 16384,
                                     size_t out_len = 128,
                                     uint32_t iter = 1);

    MPLCSHARE_API std::string hash_user_password(const std::string& password,
                                                 const std::string& salt,
                                                 const std::string& hash_name);
}  // namespace mplc::ssl
