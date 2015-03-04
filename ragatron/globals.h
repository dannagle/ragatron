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

#define VALIDBKGND "background-color: #AAEE78;"
#define VALIDLINEEDITSTYLE "QLineEdit { " VALIDBKGND " }"


#define RAGATRONLOGO ":dannagle_logo.png"




#endif // GLOBALS_H
