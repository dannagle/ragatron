#ifndef HACKSTRUCT
#define HACKSTRUCT


typedef struct hack {
    QString category;
    QString platform;
    QString id;
    QString name;
    QString target;
    QString mactarget;
    QString wintarget;
    QString search;
    QString replace;
} hack_t;

typedef struct xml {
    QString title;
    QString defaultpath;
    QString md5;
    QString target;
    QString outputname;
    QString launchfile;
    QList<hack_t> hacks;
} xml_t;

#endif // HACKSTRUCT

