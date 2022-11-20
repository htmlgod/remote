#include "common.h"

QVector<w128_t> get_blocks(const QByteArray& ar, int& fill_blocks) {
    QVector<w128_t> out;
    auto amount_of_blocks = (ar.size() / 16) + (ar.size() % 16 == 0 ? 0 : 1);
    out.reserve(amount_of_blocks);
    fill_blocks = 16*amount_of_blocks - ar.size();
    for(int b = amount_of_blocks; b > 0; b-- ) {
        w128_t tmp;
        tmp.q[0] = 0;
        tmp.q[1] = 0;
        for(int i = 0; i < 16; ++i) {
            auto gl_index = 16*b+((15-i)-16);
            tmp.b[15 - i] = ar.constData()[gl_index];
        }
        out.push_front(tmp);
    }
    return out;
}

QByteArray get_data_from_blocks(const QVector<w128_t>& blocks, int fill_blocks) {
    QByteArray out;
    auto ptr = blocks.data();
    int bytes = 16 * blocks.size() - fill_blocks;
    out.reserve(bytes);
    for (int b = 0; b < blocks.size(); ++b) {
        for(int i = 0; i < 16; i++) {
            if (out.size() == bytes) {
                break;
            }
            out.push_back(ptr[b].b[i]);
        }
    }
    return out;
}

QByteArray encrypt_data(kuz_key_t *subkeys, const uint8_t key[32], const QByteArray &data)
{
    kuz_set_encrypt_key(subkeys, key);
    int fill = 0;
    auto blocks = get_blocks(data, fill);
    for (int i = 0; i < blocks.size(); i++) {
        kuz_encrypt_block(subkeys, &blocks[i]);
    }
    return get_data_from_blocks(blocks, fill);
}

QByteArray decrypt_data(kuz_key_t *subkeys, const uint8_t key[32], const QByteArray &data)
{
    kuz_set_decrypt_key(subkeys, key);
    int fill = 0;
    auto blocks = get_blocks(data, fill);
    for (int i = 0; i < blocks.size(); i++) {
        kuz_decrypt_block(subkeys, &blocks[i]);
    }
    return get_data_from_blocks(blocks, fill);
}

dh_params gen_dh_params() {
    // RANDOM EBANII
}
