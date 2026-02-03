#ifndef QOBJECTHELPER_H
#define QOBJECTHELPER_H

#include <QtCore/QLatin1String>
#include <QtCore/QStringList>
#include <QtCore/QVariantMap>

QT_BEGIN_NAMESPACE
class QObject;
QT_END_NAMESPACE

class QObjectHelper {
    public:
      QObjectHelper();
      ~QObjectHelper();


    static QJsonObject qobject2qjsonobject( const QObject* object,
                                  const QStringList& ignoredProperties = QStringList(QString(QLatin1String("objectName"))));

    static QVariantMap qobject2variantmap(const QObject* object,
                                  const QStringList& ignoredProperties = QStringList(QStringLiteral("objectName")));

    static QString qobject2json( const QObject* object,
                                  const QStringList& ignoredProperties = QStringList(QString(QLatin1String("objectName"))));


    static void qjsonobject2qobject(const QJsonObject &jsonobj, QObject* object);

    static void json2qobject(const QString& json, QObject* object);

    static void writeToFile(const QString& fpath, QObject* object);

    private:
      Q_DISABLE_COPY(QObjectHelper)
      class QObjectHelperPrivate;
      QObjectHelperPrivate* const d;
};


#endif // QOBJECTHELPER_H
