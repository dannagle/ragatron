#ifndef HACKSTRUCT
#define HACKSTRUCT


typedef struct hack {
    QString category;
    QString platform;
    QString id;
    QString name;
    QString target;
    QString search;
    QString replace;
} hack_t;

typedef struct xml {
    QString title;
    QString defaultpath;
    QString md5;
    int app_nw_is_compressed;
    int app_nw_is_hidden;
    QString app_nw_bytelocation;
    QString target;
    QString outputname;
    QList<hack_t> hacks;
} xml_t;

#endif // HACKSTRUCT

