# QJsonHelper

`QJsonHelper` 是一个轻量级的 Qt 库，专门用于在 `QObject` 对象与 JSON 数据之间进行快速、方便的转换。

## 主要功能

*   **对象转 JSON**: 支持将继承自 `QObject` 的类属性自动转换为 `QJsonObject`、JSON 字符串或 `QVariantMap`。
*   **JSON 转对象**: 支持将 JSON 字符串或 `QJsonObject` 数据自动填充到 `QObject` 的属性中。
*   **文件持久化**: 提供了便捷的方法直接将对象状态保存到文件或从文件加载。
*   **智能属性处理**: 自动识别 Qt 的 `Q_PROPERTY`，并支持指定忽略特定的属性（默认忽略 `objectName`）。

## 环境要求

*   Qt 5.0 或更高版本
*   支持 C++11 或更高版本

## 安装与集成

将本项目的源代码放入您的工程目录中，然后在 `.pro` 文件中包含 `QJsonHelper.pri` 即可：

```pro
include(path/to/QJsonHelper/QJsonHelper.pri)
```

## 使用示例

### 1. 让您的类继承 QJsonHelper

通过继承 `QJsonHelper`，您的类将直接拥有 JSON 序列化和反序列化的能力。

```cpp
#include "qjsonhelper.h"

class UserInfo : public QJsonHelper {
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(int age READ age WRITE setAge)

public:
    explicit UserInfo(QObject *parent = nullptr) : QJsonHelper(parent) {}
    
    // Getter 和 Setter 实现...
    QString name() const { return m_name; }
    void setName(const QString &n) { m_name = n; }
    int age() const { return m_age; }
    void setAge(int a) { m_age = a; }

private:
    QString m_name;
    int m_age;
};

// 使用方法
UserInfo user;
user.setName("Alice");
user.setAge(25);

// 序列化
QString jsonStr = user.json();
user.save("user_config.json");

// 反序列化
UserInfo newUser;
newUser.load("user_config.json");
```

### 2. 使用静态辅助方法 QObjectHelper

如果您不想修改类的继承关系，可以直接使用 `QObjectHelper`。

```cpp
#include "qobjecthelper.h"

CustomObject obj;
// 配置 obj 属性...

// 转换为 JSON
QJsonObject json = QObjectHelper::qobject2qjsonobject(&obj);

// 从 JSON 加载
QString jsonContent = "{\"name\": \"Bob\", \"age\": 30}";
QObjectHelper::json2qobject(jsonContent, &obj);
```

## 核心 API

### QJsonHelper 类 (推荐继承使用)
*   `QString json()`: 获取当前对象的 JSON 字符串。
*   `QJsonObject jsonObject()`: 获取当前对象的 `QJsonObject`。
*   `bool save(const QString& fpath)`: 将对象保存到本地文件。
*   `bool load(const QString& fpath)`: 从本地文件加载对象属性。
*   `void fromJsonValue(const QJsonValue &jsonVal)`: 从 `QJsonValue` 填充属性。

### QObjectHelper 类 (静态工具类)
*   `static QString qobject2json(const QObject* object, ...)`
*   `static void json2qobject(const QString& json, QObject* object)`
*   `static void writeToFile(const QString& fpath, QObject* object)`

## 许可证

本项目遵循开源许可证，详情请参阅 `LICENSE` 文件（如有）。
