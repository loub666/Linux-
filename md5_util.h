#ifndef MD5_UTIL_H
#define MD5_UTIL_H

#include <string>
#include <fstream>
#include <openssl/md5.h>
#include <iomanip>
#include <sstream>

class MD5Util {
public:
    // 计算字符串的 MD5
    static std::string getMD5(const std::string& content) {
        unsigned char md5_result[MD5_DIGEST_LENGTH];
        MD5((const unsigned char*)content.c_str(), content.length(), md5_result);
        
        std::stringstream ss;
        for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)md5_result[i];
        }
        return ss.str();
    }

    // 计算文件的 MD5 (简单实现，大文件建议分块，但这里为了演示直接读取)
    static std::string getFileMD5(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) return "";

        MD5_CTX md5_ctx;
        MD5_Init(&md5_ctx);

        char buffer[4096];
        while (file.read(buffer, sizeof(buffer))) {
            MD5_Update(&md5_ctx, buffer, file.gcount());
        }
        MD5_Update(&md5_ctx, buffer, file.gcount());

        unsigned char md5_result[MD5_DIGEST_LENGTH];
        MD5_Final(md5_result, &md5_ctx);

        std::stringstream ss;
        for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)md5_result[i];
        }
        return ss.str();
    }
};

#endif // MD5_UTIL_H
