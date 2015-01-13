#ifndef GLOBALS_H
#define GLOBALS_H

/*
This file is part of Ragatron.
It is (c) Dan Nagle http://DanNagle.com/
It is licensed GPL v2 or later.
*/

#define QDEBUG() qDebug() << __FILE__ << "/" <<__LINE__  <<"(" << __FUNCTION__ << "):"
#define QDEBUGVAR(var)  QDEBUG() << # var <<  var;

#define DATETIMEFORMAT "h:mm:ss.zzz ap"

#define TEMPPATH  QDir::toNativeSeparators(QDir::temp().absolutePath()) + "/Ragatron/"
#define SETTINGSPATH QStandardPaths::writableLocation( QStandardPaths::GenericDataLocation )+ "/Ragatron/"
#define SETTINGSFILE SETTINGSPATH  + "ragatron_settings.ini"
#define LOGFILE SETTINGSPATH + "ragatron.log"


//old wizard lizard 6a1615909d96c71b1bc1af835ec437b3
//new wizard MD5 is
#define APP_NW_MAC "b6d8ebe97f97ed22d1285e78c47361a8"
#define NW_EXE_LOCATION_AWL 39433216
#define WIZARDLIZARD_MD5 "3e4437c746cd1bb83f7f02458b2b7151"
#define WIZARDSLIZARD_EXE_BACKUP SETTINGSPATH  + "wizardslizard.exe"
#define FFMPEGSUMO_DLL_BACKUP SETTINGSPATH  + "ffmpegsumo.dll"
#define ICUDT_DLL_BACKUP SETTINGSPATH  + "icudt.dll"
#define LIBEGL_DLL_BACKUP SETTINGSPATH  + "libEGL.dll"
#define LIBGLESV2_DLL_BACKUP SETTINGSPATH  + "libGLESv2.dll"
#define NW_PAK_BACKUP SETTINGSPATH  + "nw.pak"
#define INDEX_HTML_BACKUP SETTINGSPATH  + "original_index.html"
#define PACKAGE_JSON_BACKUP SETTINGSPATH  + "original_package.json"
#define WIZARDLIZARDWORKSPACE SETTINGSPATH  + "wizardlizardmod/"



//lava blade
#define LAVABLADE_MD5 "c8e540a62ad773c06cf603bf5b84b7a2"
#define NW_EXE_LOCATION_LAVABLADE 36440576
#define LAVABLADE_EXE_BACKUP SETTINGSPATH  + "lavablade/lavablade.exe"
#define ICUDT_DLL_BACKUP_LAVABLADE SETTINGSPATH  + "lavablade/icudt.dll"
#define FFMPEGSUMO_DLL_BACKUP_LAVABLADE SETTINGSPATH  + "lavablade/ffmpegsumo.dll"
#define NW_PAK_BACKUP_LAVABLADE SETTINGSPATH  + "lavablade/nw.pak"
#define LAVABLADEWORKSPACE SETTINGSPATH  + "lavablademod/"
#define INDEX_HTML_BACKUP_LAVABLADE SETTINGSPATH + "lavablade/"  + "original_index.html"
#define PACKAGE_JSON_BACKUP_LAVABLADE SETTINGSPATH + "lavablade/"  + "original_package.json"


#define NW_EXE SETTINGSPATH  + "nw.exe"
#define APP_NW SETTINGSPATH  + "app.nw"

#define NW_EXE_LAVABLADE SETTINGSPATH + "lavablade/nw.exe"
#define APP_NW_LAVABLADE SETTINGSPATH + "lavablade/app.nw"


#define RAGATRONLOGO ":dannagle_logo.png"




#endif // GLOBALS_H
