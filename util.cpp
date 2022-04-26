#include "util.hpp"

#include <botan/auto_rng.h>

void mkKeyFile(QString path) {
    QFile f(path);
    f.open(QIODevice::ReadWrite);
    QDataStream q(&f);

    Botan::AutoSeeded_RNG rng;
    for (const uint8_t v : rng.random_vec(128)) {
        q << v;
    }

    f.close();
}
