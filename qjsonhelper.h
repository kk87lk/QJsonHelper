#ifndef QJSONHELPER_H
#define QJSONHELPER_H

#include <QtCore>
#include <QDebug>
#include "qobjecthelper.h"

class QJsonHelper : public QObject
{
    Q_OBJECT

public:
    explicit QJsonHelper(QObject *parent = nullptr);

    inline QString json() {
        return QObjectHelper::qobject2json(this);
    }

    inline QJsonObject jsonObject() {
        return QObjectHelper::qobject2qjsonobject(this);
    }

    inline QVariantMap variantMap() {
        return QObjectHelper::qobject2variantmap(this);
    }

    bool save(const QString& fpath);

    void fromVariantMap(const QVariantMap& map);

    void fromJsonValue(const QJsonValue &jsonVal);

    virtual bool load(const QString& fpath);


    inline virtual void json2qobject(const QString json, QObject *object){
        QObjectHelper::json2qobject(json, object);
    }

    inline bool isLoadFinish(){
        return loadFinish_;
    }

    static bool save(const QObject *object, const QString &fpath, const QStringList &ignoredProperties = QStringList(QString(QLatin1String("objectName"))));
    static bool load(const QString &fpath, QObject *object);
protected:
    virtual void checkModel(){}

private:
    friend QDebug operator<<(QDebug dbg, const QObject &obj);
    bool loadFinish_;
};

QDebug operator<<(QDebug dbg, const QObject &obj);

#endif // QJSONHELPER_H
