# QJsonHelper

`QJsonHelper` is a lightweight Qt library designed for fast and convenient conversion between `QObject` instances and JSON data.

## Features

*   **Object to JSON**: Automatically convert properties of a `QObject` into `QJsonObject`, JSON string, or `QVariantMap`.
*   **JSON to Object**: Automatically populate `QObject` properties from a JSON string or `QJsonObject`.
*   **File Persistence**: Convenient methods to save object state to or load from local files.
*   **Smart Property Handling**: Recognizes Qt's `Q_PROPERTY` and allows ignoring specific properties (defaults to ignoring `objectName`).
*   **QML Support**: Includes powerful macros for seamless integration with QML data models.

## Requirements

*   Qt 5.0 or higher
*   C++11 or higher

## Installation

Add the source code to your project directory and include `QJsonHelper.pri` in your `.pro` file:

```pro
include(path/to/QJsonHelper/QJsonHelper.pri)
```

## Usage Examples

### 1. Inherit from QJsonHelper

By inheriting from `QJsonHelper`, your class gains JSON serialization capabilities directly.

```cpp
#include "qjsonhelper.h"

class UserInfo : public QJsonHelper {
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(int age READ age WRITE setAge)

public:
    explicit UserInfo(QObject *parent = nullptr) : QJsonHelper(parent) {}
    
    // Getters and Setters...
    QString name() const { return m_name; }
    void setName(const QString &n) { m_name = n; }
    int age() const { return m_age; }
    void setAge(int a) { m_age = a; }

private:
    QString m_name;
    int m_age;
};

// Usage
UserInfo user;
user.setName("Alice");
user.setAge(25);

// Serialization
QString jsonStr = user.json();
user.save("user_config.json");

// Deserialization
UserInfo newUser;
newUser.load("user_config.json");
```

### 2. Using Static Helper QObjectHelper

If you don't want to modify inheritance, use `QObjectHelper` directly.

```cpp
#include "qobjecthelper.h"

CustomObject obj;
// ... set properties ...

// Convert to JSON
QJsonObject json = QObjectHelper::qobject2qjsonobject(&obj);

// Load from JSON
QString jsonContent = "{\"name\": \"Bob\", \"age\": 30}";
QObjectHelper::json2qobject(jsonContent, &obj);
```

### 3. QML Integration with Advanced Macros

Use `qpropertyex.h` for advanced data model synchronization.

```cpp
class MyListModel : public QJsonHelper {
    Q_OBJECT
    // Generates JSON array sync and CRUD methods for QML
    Q_PROPERTY_QMLLIST(ChildItem, children)
    
public:
    explicit MyListModel(QObject *parent = nullptr) : QJsonHelper(parent) {}
};
```

## Core API

### QJsonHelper Class
*   `QString json()`: Get object as JSON string.
*   `QJsonObject jsonObject()`: Get object as `QJsonObject`.
*   `bool save(const QString& fpath)`: Save object to file.
*   `bool load(const QString& fpath)`: Load object from file.

### QObjectHelper Class (Static)
*   `static QString qobject2json(const QObject* object, ...)`
*   `static void json2qobject(const QString& json, QObject* object)`

## License

This project follows the Open Source License. See `LICENSE` file for details (if applicable).
