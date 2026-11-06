#include <sodium.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef CHALI_CRYPTO_STATIC
#define CHALI_CRYPTO
#elif defined(_WIN32)
#define CHALI_CRYPTO __declspec(dllexport)
#else
#define CHALI_CRYPTO __attribute__((visibility("default")))
#endif

/**
 * @brief 将字节编码为拥有的小写十六进制文本。
 * @param bytes 数据字节。
 * @param length 数据长度。
 * @return 由 chali_crypto_free 回收的文本。
 */
static char *encode(const unsigned char *bytes, size_t length)
{
    char *output = malloc(length * 2 + 1);
    if (output) sodium_bin2hex(output, length * 2 + 1, bytes, length);
    return output;
}

/**
 * @brief 以空结果标记非法输入或后端初始化失败。
 * @return 可释放的空文本。
 */
static char *failed(void)
{
    return calloc(1, 1);
}

/**
 * @brief 生成指定数量的安全随机字节并编码为十六进制。
 * @param count 需要的字节数，限制为 1 至 65536。
 * @return 十六进制文本，失败为空。
 */
CHALI_CRYPTO char *chali_crypto_random(int64_t count)
{
    if (count < 1 || count > 65536 || sodium_init() < 0) return failed();
    unsigned char *bytes = malloc((size_t)count);
    if (!bytes) return failed();
    randombytes_buf(bytes, (size_t)count);
    char *result = encode(bytes, (size_t)count);
    sodium_memzero(bytes, (size_t)count);
    free(bytes);
    return result;
}

/**
 * @brief 计算输入字节的 SHA-256。
 * @param input 输入字节。
 * @param length 输入长度。
 * @return 64 个小写十六进制字符，失败为空。
 */
CHALI_CRYPTO char *chali_crypto_sha256(const unsigned char *input, int64_t length)
{
    unsigned char hash[crypto_hash_sha256_BYTES];
    if (length < 0 || sodium_init() < 0) return failed();
    if (crypto_hash_sha256(hash, input, (unsigned long long)length) != 0) return failed();
    return encode(hash, sizeof hash);
}

/**
 * @brief 使用指定密钥计算 HMAC-SHA-256。
 * @param input 消息字节。
 * @param length 消息长度。
 * @param key 密钥字节。
 * @param key_length 密钥长度。
 * @return 64 个小写十六进制字符，失败为空。
 */
CHALI_CRYPTO char *chali_crypto_hmac(const unsigned char *input, int64_t length,
                                    const unsigned char *key, int64_t key_length)
{
    crypto_auth_hmacsha256_state state;
    unsigned char hash[crypto_auth_hmacsha256_BYTES];
    if (length < 0 || key_length < 0 || (uint64_t)key_length > SIZE_MAX || sodium_init() < 0) return failed();
    int status = crypto_auth_hmacsha256_init(&state, key, (size_t)key_length);
    if (status == 0) status = crypto_auth_hmacsha256_update(&state, input, (unsigned long long)length);
    if (status == 0) status = crypto_auth_hmacsha256_final(&state, hash);
    sodium_memzero(&state, sizeof state);
    return status == 0 ? encode(hash, sizeof hash) : failed();
}

/**
 * @brief 验证 Ed25519 分离签名。
 * @param input 消息字节。
 * @param length 消息长度。
 * @param public_key 64 字符十六进制公钥。
 * @param signature 128 字符十六进制签名。
 * @return 签名有效时为真。
 */
CHALI_CRYPTO bool chali_crypto_verify(const unsigned char *input, int64_t length,
                                      const char *public_key, const char *signature)
{
    unsigned char key[crypto_sign_PUBLICKEYBYTES], sig[crypto_sign_BYTES];
    if (length < 0 || sodium_init() < 0 || strlen(public_key) != sizeof key * 2 ||
        strlen(signature) != sizeof sig * 2) return false;
    if (sodium_hex2bin(key, sizeof key, public_key, sizeof key * 2, NULL, NULL, NULL) != 0 ||
        sodium_hex2bin(sig, sizeof sig, signature, sizeof sig * 2, NULL, NULL, NULL) != 0) return false;
    return crypto_sign_verify_detached(sig, input, (unsigned long long)length, key) == 0;
}

/**
 * @brief 擦除并释放宿主交给语言侧的编码缓冲。
 * @param output 由当前宿主分配的字符串。
 */
CHALI_CRYPTO void chali_crypto_free(char *output)
{
    if (!output) return;
    sodium_memzero(output, strlen(output));
    free(output);
}
