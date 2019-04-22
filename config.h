#ifndef CONFIG_H
#define CONFIG_H

#include <QVariantList>
#include <QVariantMap>

#include <map>
#include <vector>

#include <QString>

class QJsonObject;


namespace config{

struct Field
{
    double no;
    QString name;
    QString attribute;
    QString format;
    QString type;
    std::map<double, Field> sub;
};

using FIELD = std::pair<double, Field>;
using FIELDS = std::map<double, Field>;
using MESSAGE = std::pair<QString, FIELDS>;
using MESSAGES = std::map<QString, FIELDS>;

class Config
{
public:
    Config();

public:
    void read();
    void save();

    void upDataMessage(const MESSAGE& message) {
        messages_[message.first] = message.second;
        save();
    }

    const MESSAGES &getMessages() {
        return messages_;
    }

private:
    bool getFieldAttribute(Field &field, QJsonObject& fieldJs);
    bool setFieldObj(QVariantMap &fieldObj, const Field &field);

private:
    MESSAGES messages_;

};

extern Config config;

}

#endif // CONFIG_H
