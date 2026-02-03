#pragma once
#include <QMetaType>
#include <QQmlProperty>
#include <QQmlListProperty>

/**
 * @brief Q_PROPERTY_AUTO
 * 自动生成带有信号通知的 Qt 属性。
 * Generates a Qt property with a notification signal automatically.
 * 
 * @param TYPE 属性类型 / Property type
 * @param NAME 属性名称 / Property name
 */
#define Q_PROPERTY_AUTO(TYPE, NAME)                                                     \
Q_PROPERTY(TYPE NAME READ NAME WRITE set##NAME NOTIFY NAME##Changed)                    \
    public:                                                                             \
    Q_SIGNAL void NAME##Changed(const TYPE &);                                          \
    TYPE NAME() const {                                                                 \
        return m_##NAME;                                                                \
}                                                                                       \
    void set##NAME(const TYPE &value) {                                                 \
        if (m_##NAME == value)                                                          \
        return;                                                                         \
        m_##NAME = value;                                                               \
        emit NAME##Changed(m_##NAME);                                                   \
}                                                                                       \
    private:                                                                            \
    TYPE m_##NAME;

/**
 * @brief Q_PROPERTY_AUTO_P
 * 适用于 QObject 指针类型的自动属性（Qt 5 / QML 安全）。
 * Automatic property for QObject pointer types (Qt 5 / QML safe).
 * 
 * @param TYPE 指针类型 (例如 QObject*) / Pointer type (e.g., QObject*)
 * @param M 属性名称 / Property name
 */
#define Q_PROPERTY_AUTO_P(TYPE, M)                                                                 \
    Q_PROPERTY(TYPE M MEMBER m_##M NOTIFY M##Changed)                                              \
public:                                                                                            \
    Q_SIGNAL void M##Changed();                                                                    \
    void M(TYPE in_##M) {                                                                          \
        if (in_##M == m_##M)                                                                       \
            return;                                                                                \
        m_##M = in_##M;                                                                            \
        Q_EMIT M##Changed();                                                                       \
    }                                                                                              \
    TYPE M() const {                                                                               \
        return m_##M;                                                                              \
    }                                                                                              \
private:                                                                                           \
    TYPE m_##M = nullptr;

/**
 * @brief Q_PROPERTY_AUTOINIT
 * 带有初始值的自动属性。
 * Automatic property with an initial default value.
 * 
 * @param TYPE 属性类型 / Property type
 * @param NAME 属性名称 / Property name
 * @param DEFAULT_VALUE 默认值 / Default value
 */
#define Q_PROPERTY_AUTOINIT(TYPE, NAME, DEFAULT_VALUE)                                  \
Q_PROPERTY(TYPE NAME READ NAME WRITE set##NAME NOTIFY NAME##Changed)                    \
    public:                                                                             \
    Q_SIGNAL void NAME##Changed(const TYPE &);                                          \
    TYPE NAME() const {                                                                 \
        return m_##NAME;                                                                \
}                                                                                       \
    void set##NAME(const TYPE &value) {                                                 \
        if (m_##NAME == value)                                                          \
        return;                                                                         \
        m_##NAME = value;                                                               \
        emit NAME##Changed(m_##NAME);                                                   \
}                                                                                       \
    private:                                                                            \
    TYPE m_##NAME{DEFAULT_VALUE};

/**
 * @brief Q_PROPERTY_AUTOGEN_VIRTUAL
 * 带有初始值的虚函数属性，允许子类重写。
 * Virtual automatic property with an initial value, allowing overrides in subclasses.
 */
#define Q_PROPERTY_AUTOGEN_VIRTUAL(TYPE, NAME, DEFAULT_VALUE)                           \
Q_PROPERTY(TYPE NAME READ NAME WRITE set##NAME NOTIFY NAME##Changed)                    \
    public:                                                                             \
    Q_SIGNAL void NAME##Changed(const TYPE &);                                          \
    virtual TYPE NAME() const {                                                         \
        return m_##NAME;                                                                \
}                                                                                       \
    virtual void set##NAME(const TYPE &value) {                                         \
        if (m_##NAME == value)                                                          \
        return;                                                                         \
        m_##NAME = value;                                                               \
        emit NAME##Changed(m_##NAME);                                                   \
}                                                                                       \
    protected:                                                                          \
    TYPE m_##NAME = DEFAULT_VALUE;

/**
 * @brief Q_PROPERTY_QML
 * 单对象模型宏。
 * Single object model macro.
 * 
 * 设计目标 / Design Goals:
 * 1. 对外暴露 NAME (QVariantMap)，用于 QML/JSON 整体赋值。
 *    Exposes NAME (QVariantMap) for overall QML/JSON assignment.
 * 2. 给 NAME 赋值时，自动创建 TYPE 对象并反序列化。
 *    Automatically creates TYPE object and deserializes when NAME is assigned.
 * 3. 访问 getNAME() 时若对象为空则自动 lazy 初始化。
 *    Automatically lazy-initializes the object if it's null when accessing getNAME().
 */
#define Q_PROPERTY_QML(TYPE, NAME)                                                          \
    Q_PROPERTY(QVariantMap NAME READ get##NAME WRITE set##NAME NOTIFY NAME##Changed)        \
    public:                                                                                 \
    Q_SIGNAL void NAME##Changed();                                                          \
    QVariantMap get##NAME() {                                                               \
        if (m_##NAME == nullptr) {                                                          \
            m_##NAME = new TYPE(this);                                                      \
        }                                                                                   \
        return QObjectHelper::qobject2variantmap(m_##NAME);                                 \
    }                                                                                       \
    void set##NAME(const QVariantMap& value) {                                              \
        if (m_##NAME) {                                                                     \
            m_##NAME->deleteLater();                                                        \
            m_##NAME = nullptr;                                                             \
        }                                                                                   \
        if (!value.isEmpty()) {                                                             \
            m_##NAME = new TYPE(this);                                                      \
            m_##NAME->fromVariantMap(value);                                                \
        }                                                                                   \
        emit NAME##Changed();                                                               \
    }                                                                                       \
    private:                                                                                \
    TYPE* m_##NAME = nullptr;


/**
 * @brief Q_PROPERTY_QMLLIST
 * 对象列表模型宏，为 QObject 类生成 JSON 数组与对象列表的双向同步封装。
 * Array/List model macro. Generates two-way synchronization between JSON arrays and object lists.
 *
 * 核心能力 / Core Capabilities:
 * 1. JSON (QJsonArray) 与强类型对象列表 (QList<TYPE*>) 之间的自动同步。
 *    Automatic sync between JSON (QJsonArray) and strong-typed object lists (QList<TYPE*>).
 * 2. 自动生成可在 QML 中调用的 CRUD 接口 (Count, GetAt, SetAt, Append, Insert, Remove, Clear)。
 *    Generates QML-invokable CRUD interfaces.
 * 3. 内部对象生命周期由当前类管理 (deleteLater)。
 *    Manages the lifecycle of internal objects automatically.
 */
#define Q_PROPERTY_QMLLIST(TYPE, NAME)                                                      \
    Q_PROPERTY(QJsonArray NAME READ get##NAME WRITE set##NAME NOTIFY NAME##Changed)         \
public:                                                                                     \
    Q_SIGNAL void NAME##Changed();                                                          \
    /* JSON 读 / JSON Read */                                                               \
    QJsonArray get##NAME() const {                                                          \
        return m_##NAME##Json;                                                              \
    }                                                                                       \
    /* JSON 写 -> 重建对象列表 / JSON Write -> Rebuild Object List */                       \
    void set##NAME(const QJsonArray &value) {                                               \
        qDebug() << "[Q_PROPERTY_QMLLIST] set" << #NAME << "size:" << value.size();          \
        if (m_##NAME##Json == value)                                                        \
            return;                                                                         \
        m_##NAME##Json = value;                                                             \
        NAME##Deserialization();                                                            \
        emit NAME##Changed();                                                               \
    }                                                                                       \
    /* 同步对象 -> JSON / Sync Objects -> JSON */                                           \
    void NAME##Serialization() {                                                            \
        qDebug() << "[Q_PROPERTY_QMLLIST] Serialization" << #NAME << "count:" << m_##NAME.size(); \
        QJsonArray newJson;                                                                 \
        for (const auto &item : m_##NAME) {                                                 \
            newJson.append(item->jsonObject());                                             \
        }                                                                                   \
        m_##NAME##Json = newJson;                                                           \
        emit NAME##Changed();                                                               \
    }                                                                                       \
    /* 同步JSON -> 对象 / Sync JSON -> Objects */                                           \
    void NAME##Deserialization() {                                                          \
        qDebug() << "[Q_PROPERTY_QMLLIST] Deserialization" << #NAME << "size:" << m_##NAME##Json.size(); \
        for (const auto &item : m_##NAME) {                                                 \
            item->deleteLater();                                                            \
        }                                                                                   \
        m_##NAME.clear();                                                                   \
        for (const auto &obj : m_##NAME##Json) {                                            \
            TYPE* item = new TYPE(this);                                                    \
            item->fromJsonValue(obj);                                                       \
            m_##NAME.append(item);                                                          \
        }                                                                                   \
    }                                                                                       \
    /* ---------------- 查询类接口 / Search Interfaces ---------------- */                  \
    Q_INVOKABLE int NAME##IndexOf(const QVariantMap &map) const {                           \
        for (int i = 0; i < m_##NAME.size(); ++i) {                                         \
            if (m_##NAME.at(i)->variantMap() == map)                                        \
                return i;                                                                  \
        }                                                                                  \
        return -1;                                                                         \
    }                                                                                      \
    Q_INVOKABLE bool NAME##Contains(const QVariantMap &map) const {                         \
        return NAME##IndexOf(map) >= 0;                                                     \
    }                                                                                       \
    /* ---------------- CRUD (统一命名风格 / Unified Naming Style) ---------------- */      \
    Q_INVOKABLE int NAME##Count() const {                                                   \
        return m_##NAME.size();                                                             \
    }                                                                                       \
    Q_INVOKABLE QVariantMap NAME##GetAt(int index) const {                                  \
        if (index < 0 || index >= m_##NAME.size())                                          \
            return QVariantMap();                                                           \
        return m_##NAME.at(index)->variantMap();                                            \
    }                                                                                       \
    Q_INVOKABLE void NAME##SetAt(int index, const QVariantMap &map) {                       \
        if (index < 0 || index >= m_##NAME.size())                                          \
            return;                                                                         \
        TYPE *item = m_##NAME.at(index);                                                    \
        item->fromVariantMap(map);                                                          \
        NAME##Serialization();                                                              \
    }                                                                                       \
    Q_INVOKABLE void NAME##Append(QVariantMap map = QVariantMap()) {                        \
        TYPE *item = new TYPE(this);                                                        \
        item->fromVariantMap(map);                                                          \
        m_##NAME.append(item);                                                              \
        NAME##Serialization();                                                              \
    }                                                                                       \
    Q_INVOKABLE void NAME##Insert(int index, const QVariantMap &map) {                      \
        TYPE *item = new TYPE(this);                                                        \
        item->fromVariantMap(map);                                                          \
        if (index < 0) index = 0;                                                           \
        if (index > m_##NAME.size()) index = m_##NAME.size();                               \
        m_##NAME.insert(index, item);                                                       \
        NAME##Serialization();                                                              \
    }                                                                                       \
    Q_INVOKABLE void NAME##Remove(int index) {                                              \
        if (index < 0 || index >= m_##NAME.size())                                          \
            return;                                                                         \
        TYPE* item = m_##NAME.takeAt(index);                                                \
        if (item) item->deleteLater();                                                      \
        NAME##Serialization();                                                              \
    }                                                                                       \
    Q_INVOKABLE void NAME##Clear() {                                                        \
        for (auto *item : m_##NAME) {                                                       \
            item->deleteLater();                                                            \
        }                                                                                   \
        m_##NAME.clear();                                                                   \
        NAME##Serialization();                                                              \
    }                                                                                       \
public:                                                                                   \
    QList<TYPE*> m_##NAME;                                                                  \
    QJsonArray m_##NAME##Json;
