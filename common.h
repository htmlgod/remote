#ifndef COMMON_H
#define COMMON_H

#include <QMainWindow>
#include "kuznechik.h"
QVector<w128_t> get_blocks(const QByteArray& ar, int& fill_blocks);
QByteArray get_data_from_blocks(const QVector<w128_t>& blocks, int fill_blocks);
QByteArray encrypt_data(kuz_key_t *subkeys, const uint8_t key[32], const QByteArray& data);
QByteArray decrypt_data(kuz_key_t *subkeys, const uint8_t key[32], const QByteArray& data);
// THIS FILE DEPENDS ON common.h OF CLIENT SIDE
const int CLIENT_CONTROL_PORT = 1235;
struct dh_params {
    uint8_t g;
    uint8_t p;
};
dh_params gen_dh_params();
void do_dh_phase1(uint8_t* pk);
void do_dh_phase2(uint8_t* pk, const uint8_t other[32]);
struct mouse_control_data {
    QString type; // move, click, wheel
    uint button = 0; // 1 -lmb, 3 - rmb, 2 - mmb, 0 - move
    int xpos;
    int ypos;
    int delta;
    friend QDataStream &operator<<(QDataStream& out, const mouse_control_data& cd){
        out << cd.type << cd.button << cd.xpos <<  cd.ypos << cd.delta;
        return out;
    }
    friend QDataStream &operator>>(QDataStream& in, mouse_control_data& cd){
        in >> cd.type >> cd.button >> cd.xpos >>  cd.ypos >> cd.delta;
        return in;
    }
};
struct keyboard_control_data {
    QString type; // press, release
    int key = 0; // 1 -lmb, 3 - rmb, 2 - mmb, 0 - move
    QString text;
    friend QDataStream &operator<<(QDataStream& out, const keyboard_control_data& cd){
        out << cd.type << cd.key << cd.text;
        return out;
    }
    friend QDataStream &operator>>(QDataStream& in, keyboard_control_data& cd){
        in >> cd.type >> cd.key >> cd.text;
        return in;
    }
};

struct control_data {
    QString type;
    mouse_control_data md;
    keyboard_control_data kd;
    friend QDataStream &operator<<(QDataStream& out, const control_data& cd){
        out << cd.type << cd.md << cd.kd;
        return out;
    }
    friend QDataStream &operator>>(QDataStream& in, control_data& cd){
        in >> cd.type >> cd.md >> cd.kd;
        return in;
    }
};

struct server_settings_data {
    QString y_res;
    QString x_res;
    QString img_format;
    QString compression;
    QString preview_upd;
    QString xmit_fps;
    bool is_encrypted;
    friend QDataStream &operator<<(QDataStream& out, const server_settings_data& server_settings){
        out << server_settings.y_res <<  server_settings.x_res <<  server_settings.img_format <<  server_settings.compression <<  server_settings.preview_upd <<  server_settings.xmit_fps << server_settings.is_encrypted;
        return out;
    }
    friend QDataStream &operator>>(QDataStream& in, server_settings_data& server_settings){
        in >> server_settings.y_res >>  server_settings.x_res >>  server_settings.img_format >>  server_settings.compression >>  server_settings.preview_upd >>  server_settings.xmit_fps >> server_settings.is_encrypted;
        return in;
    }
};

#endif // COMMON_H
