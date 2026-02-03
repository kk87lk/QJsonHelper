#include "qjsonhelper.h"
#include <QMetaProperty>
#include <QVariant>
#include <QJsonDocument>


QJsonHelper::QJsonHelper(QObject *parent) : QObject(parent)
{
    loadFinish_ = false;
}

bool QJsonHelper::save(const QString& fpath){
    bool ret = false;
    QFile f(fpath);
    if (f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate	)){
        f.write(json().toUtf8());
        ret = true;
    }else{
        qDebug() << "File[" << fpath << "]open error: " << f.errorString();
    }
    f.close();
    return ret;

}

bool QJsonHelper::save(const QObject *object, const QString& fpath, const QStringList &ignoredProperties){
    bool ret = false;
    QFile f(fpath);
    if (f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate	)){
        QString json = QObjectHelper::qobject2json(object, ignoredProperties);
        f.write(json.toUtf8());
        ret = true;
    }else{
        qDebug() << "File[" << fpath << "]open error: " << f.errorString();
    }
    f.close();
    return ret;
}

bool QJsonHelper::load(const QString& fpath, QObject *object){
    bool ret = false;
    QFile f(fpath);
    if (f.open(QIODevice::ReadOnly)){
        QString content = f.readAll();
        QObjectHelper::json2qobject(content, object);
        ret = true;
    }else{
        qDebug() << "File[" << fpath << "]open error: " << f.errorString();
    }
    f.close();
    return ret;
}

bool QJsonHelper::load(const QString& fpath){
    bool ret = false;
    QFile f(fpath);
    if (f.open(QIODevice::ReadOnly)){
        QString content = f.readAll();
        json2qobject(content, this);
        ret = true;
        loadFinish_ = true;
    }else{
        qDebug() << "File[" << fpath << "]open error: " << f.errorString();
    }
    f.close();
    checkModel();
    return ret;
}

void QJsonHelper::fromVariantMap(const QVariantMap& map)
{
    QJsonObject obj = QJsonObject::fromVariantMap(map);
    QJsonDocument doc(obj);
    QObjectHelper::json2qobject(
        QString::fromUtf8(doc.toJson(QJsonDocument::Compact)),
        this
        );
}

void QJsonHelper::fromJsonValue(const QJsonValue &jsonVal){
    QJsonObject jsonObj = jsonVal.toObject();
    QJsonDocument jsonDoc(jsonObj);
    QString jsonStrCompact = jsonDoc.toJson(QJsonDocument::Compact);
    QObjectHelper::json2qobject(jsonStrCompact, this);
}

QDebug operator<<(QDebug dbg, const QObject &obj)
{
    QStringList ignoredProperties;
    ignoredProperties << "objectName";
    const QMetaObject *metaobject = obj.metaObject();
    dbg.nospace() << metaobject->className() << "({";
    int count = metaobject->propertyCount();
    for (int i=0; i<count; ++i) {
      QMetaProperty metaproperty = metaobject->property(i);
      const char *name = metaproperty.name();
      if (ignoredProperties.contains(QLatin1String(name)) || (!metaproperty.isReadable()))
        continue;
      QVariant value = obj.property(name);
      if( i <= (count - 1)){
          if (value.canConvert<QJsonObject>()){
             dbg.nospace() << name << ":" << value.toJsonObject() << ",";
          }else if(value.toString().count() == 0){
              dbg.nospace() << name << ":" << value << ",";
          }else{
             dbg.nospace() << name << ":" << value.toString() << ",";
          }
      }
    }
    dbg.nospace() << "}) ";
    return dbg;
}
