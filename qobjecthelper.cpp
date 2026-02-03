#include "qobjecthelper.h"

#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>
#include <QtCore/QObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QFile>
#include <QDebug>

#include "qjsonhelper.h"
/**
* @brief Class used to convert QObject into QVariant and vivce-versa.
* During these operations only the class attributes defined as properties will
* be considered.
* Properties marked as 'non-stored' will be ignored.
*
* Suppose the declaration of the Person class looks like this:
* \code
* class Person : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString name READ name WRITE setName)
  Q_PROPERTY(int phoneNumber READ phoneNumber WRITE setPhoneNumber)
  Q_PROPERTY(Gender gender READ gender WRITE setGender)
  Q_PROPERTY(QDate dob READ dob WRITE setDob)
  Q_ENUMS(Gender)

 public:
    Person(QObject* parent = 0);
    ~Person();

    QString name() const;
    void setName(const QString& name);

    int phoneNumber() const;
    void setPhoneNumber(const int  phoneNumber);

    enum Gender {Male, Female};
    void setGender(Gender gender);
    Gender gender() const;

    QDate dob() const;
    void setDob(const QDate& dob);

  private:
    QString m_name;
    int m_phoneNumber;
    Gender m_gender;
    QDate m_dob;
};
\endcode

The following code will serialize an instance of Person to JSON :

\code
    Person person;
    person.setName("Flavio");
    person.setPhoneNumber(123456);
    person.setGender(Person::Male);
    person.setDob(QDate(1982, 7, 12));

    QString  result = QObjectHelper::qobject2json(&person);
    QObjectHelper::json2qobject(result, &person);
    qDebug() << result
    qDebug() << person;
\endcode

The generated output will be:
\code
    "{\n    \"dob\": \"1982-07-12\",\n    \"gender\": 0,\n    \"name\": \"Flavio\",\n    \"phoneNumber\": 123456\n}\n"
    Person({"name":"Flavio","phoneNumber":"123456","gender":"0","dob":"1982-07-12"})
\endcode
*/


class QObjectHelper::QObjectHelperPrivate {
};

QObjectHelper::QObjectHelper()
  : d (new QObjectHelperPrivate)
{
}

QObjectHelper::~QObjectHelper()
{
  delete d;
}



/**
* This method converts a QObject instance into a QJsonObject.
*
* @param object The QObject instance to be converted.
* @param ignoredProperties Properties that won't be converted.
*/
QJsonObject QObjectHelper::qobject2qjsonobject( const QObject* object,
                              const QStringList& ignoredProperties)
{
    QJsonObject result;
    const QMetaObject *metaobject = object->metaObject();
    int count = metaobject->propertyCount();
    for (int i=0; i<count; ++i) {
        QMetaProperty metaproperty = metaobject->property(i);
        const char *name = metaproperty.name();

        if (ignoredProperties.contains(QLatin1String(name)) || (!metaproperty.isReadable()))
            continue;

        QVariant value = object->property(name);
        result.insert(QLatin1String(name), QJsonValue::fromVariant(value));
    }
    return result;
}

QVariantMap QObjectHelper::qobject2variantmap(const QObject* object,
                                              const QStringList& ignoredProperties)
{
    QVariantMap result;
    if (!object) return result;

    const QMetaObject* metaobject = object->metaObject();
    int count = metaobject->propertyCount();

    for (int i = 0; i < count; ++i) {
        QMetaProperty metaproperty = metaobject->property(i);
        const char* name = metaproperty.name();

        if (!metaproperty.isReadable())
            continue;
        if (ignoredProperties.contains(QLatin1String(name)))
            continue;

        QVariant value = object->property(name);
        if (!value.isValid())
            continue;

        // QObject*（嵌套模型）递归处理
        if (value.userType() == QMetaType::QObjectStar) {
            QObject* child = value.value<QObject*>();
            result.insert(name, child ? qobject2variantmap(child) : QVariantMap());
        }
        // QObjectList / QList<QObject*>
        else if (value.canConvert<QList<QObject*>>()) {
            QVariantList list;
            auto objs = value.value<QList<QObject*>>();
            for (QObject* obj : objs)
                list.append(obj ? qobject2variantmap(obj) : QVariant());
            result.insert(name, list);
        }
        // QByteArray → Base64（和你 json2qobject 对称）
        else if (value.userType() == QMetaType::QByteArray) {
            QByteArray ba = value.toByteArray();
            result.insert(name, QString::fromUtf8(ba.toBase64()));
        }
        // 普通类型（int / double / string / map / list）
        else {
            result.insert(name, value);
        }
    }
    return result;
}


/**
* This method converts a QObject instance into a json string.
*
* @param object The QObject instance to be converted.
* @param ignoredProperties Properties that won't be converted.
*/
QString QObjectHelper::qobject2json(const QObject *object, const QStringList &ignoredProperties)
{
    QString result("");
    QJsonObject jsonobj = QObjectHelper::qobject2qjsonobject(object, ignoredProperties);
    QJsonDocument doc(jsonobj);
    return doc.toJson(QJsonDocument::Compact);
}


/**
* This method converts a QVariantMap instance into a QObject
*
* @param variant Attributes to assign to the object.
* @param object The QObject instance to update.
*/
void QObjectHelper::qjsonobject2qobject(const QJsonObject& jsonobj, QObject* object)
{
    const QMetaObject *metaobject = object->metaObject();
    QJsonObject::const_iterator iter;
    for (iter = jsonobj.constBegin(); iter != jsonobj.constEnd(); ++iter) {
        int pIdx = metaobject->indexOfProperty(iter.key().toLatin1());

        if (pIdx < 0) {
            continue;
        }
        QMetaProperty metaproperty = metaobject->property(pIdx);
        QVariant::Type type = metaproperty.type();
        QVariant v(iter.value());
        if (type == QMetaType::QJsonObject){
             QJsonValue jv(iter.value());
             metaproperty.write(object, jv.toObject());
        } else if (type == QMetaType::QJsonArray){
            QJsonValue jv(iter.value());
            metaproperty.write(object, jv.toArray());
        } else if (type == QMetaType::QByteArray)
        {
            // Base64 一定是字符串
            QString s = iter.value().toString();

            // 去掉可能的空白 / 换行（防御性）
            QByteArray raw = QByteArray::fromBase64(
                s.toUtf8(),
                QByteArray::Base64Encoding
                );

            if (raw.isEmpty() && !s.isEmpty()) {
                qWarning() << "Base64 decode failed";
            }

            metaproperty.write(object, raw);
        }else if (type == QMetaType::QStringList && iter->type() == QJsonValue::Array){
            QJsonValue jv(iter.value());
            QJsonArray ja = jv.toArray();
            QStringList sl;
            for (int i=0; i<ja.size(); i++)
            {
                sl.append(ja[i].toString());
            }
            metaproperty.write(object, sl);
        }
        else if (v.canConvert(type)) {
            v.convert(type);
            metaproperty.write(object, v);
        }else if (QString(QLatin1String("QVariant")).compare(QLatin1String(metaproperty.typeName())) == 0) {
            metaproperty.write(object, v);
        }
        else if (iter->type() == QJsonValue::String) {
            QJsonValue jv(iter.value());
            QVariant v1(jv.toString());
            if (v1.canConvert(type)) {
                v1.convert(type);
                metaproperty.write(object, v1);
            }
        }
    }
}


/**
* This method converts a json string instance into a QObject
*
* @param variant Attributes to assign to the object.
* @param object The QObject instance to update.
*/
void QObjectHelper::json2qobject(const QString &json, QObject *object)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json.toStdString().data(), &error);
    if (error.error == QJsonParseError::NoError){
        QObjectHelper::qjsonobject2qobject(doc.object(), object);
    }else{
        qDebug() << error.errorString();
    }
}

void QObjectHelper::writeToFile(const QString &fpath, QObject *object)
{
//    qDebug() << object;
    QString json = QObjectHelper::qobject2json(object);
//    qDebug() << json;
    QFile f(fpath);
    if (f.open(QIODevice::ReadWrite)){
        f.write(json.toUtf8());
    }else{
        qDebug() << "File[" << fpath << "]open error: " << f.errorString();
    }
    f.close();
}
