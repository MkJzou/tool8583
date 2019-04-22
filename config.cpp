#include "config.h"

#include <exception>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>



namespace config{


Config config;


Config::Config()
{

}

void Config::read()
{
    QFile file("config.json");
    if (!file.open(QIODevice::ReadOnly))
    {
        throw std::exception("fail to read config (read error)");
    }

    QByteArray byteArry = file.readAll();
    file.close();

    QJsonDocument jsDoucment = QJsonDocument::fromJson(byteArry);
    if (!jsDoucment.isArray())
    {
        throw std::exception("fail to read config (json error)");
    }

    messages_.clear();

    QJsonArray jsArray = jsDoucment.array();
    for (int i = 0; i < jsArray.size(); i++)
    {
        // 单个报文
        if (jsArray.at(i).isObject())
        {
            QJsonObject &jsObject = jsArray.at(i).toObject();

            if (jsObject.begin() == jsObject.end())
            {
                continue;
            }

            QString name = jsObject.begin().key();

            if (!jsObject.begin().value().isArray())
            {
                continue;
            }

            // 域
            FIELDS fields;
            QJsonArray fieldsArray = jsObject.begin().value().toArray();
            for (int j = 0; j < fieldsArray.size(); j++)
            {
                // 单个域
                Field field;
                if (!fieldsArray.at(j).isObject())
                {
                    continue;
                }

                QJsonObject &fieldJs = fieldsArray.at(j).toObject();
                if (getFieldAttribute(field, fieldJs))
                {
                    fields.insert(FIELD(field.no, field));
                }
            }

            // 保存报文
            if (!fields.empty())
            {
                messages_.insert(MESSAGE(name, fields));
            }
        }
    }
}

bool Config::getFieldAttribute(Field &field, QJsonObject& fieldJs)
{
    if (!(fieldJs.contains("no") && fieldJs.value("no").isDouble()
            || fieldJs.contains("name") && fieldJs.value("name").isString()
            || fieldJs.contains("attribute") && fieldJs.value("attribute").isString()
            || fieldJs.contains("format") && fieldJs.value("format").isString()
            || fieldJs.contains("type") && fieldJs.value("type").isString()
       ))
    {
        return false;
    }

    field.no = fieldJs.value("no").toDouble();
    field.name = fieldJs.value("name").toString();
    field.attribute = fieldJs.value("attribute").toString();
    field.format = fieldJs.value("format").toString();
    field.type = fieldJs.value("type").toString();

    qDebug() << "no: " << field.no << ' '
             << "name: " << field.name << ' '
             << "attribute: " << field.attribute << ' '
             << "format: " << field.format << ' '
             << "type: " << field.type;


    if (fieldJs.contains("sub") && fieldJs.value("sub").isArray())
    {
        QJsonArray subfieldsArray = fieldJs.value("sub").toArray();
        for (int j = 0; j < subfieldsArray.size(); j++)
        {
            Field subfield;
            if (subfieldsArray.at(j).isObject())
            {
                QJsonObject &subfieldJs = subfieldsArray.at(j).toObject();

                if (getFieldAttribute(subfield, subfieldJs))
                {
                    field.sub.insert(
                                std::pair<double, Field>(subfield.no, subfield));
                }
            }
        }
    }
    return true;
}

bool Config::setFieldObj(QVariantMap &fieldObj, const Field &field)
{

    qDebug() << field.no << ' '
             << field.name << ' '
             << field.attribute << ' '
             << field.format << ' '
             << field.type;

    fieldObj.insert("no", field.no);
    fieldObj.insert("name", field.name);
    fieldObj.insert("attribute", field.attribute);
    fieldObj.insert("format", field.format);
    fieldObj.insert("type", field.type);

    if (field.sub.size() > 0)
    {
        QVariantList subFieldArray;
        for (auto it = field.sub.begin(); it != field.sub.end(); ++it)
        {
            QVariantMap subFieldObj;

            if (setFieldObj(subFieldObj, it->second))
            {
                subFieldArray.append(subFieldObj);
            }
        }

        if (subFieldArray.size() > 0)
        {
            fieldObj.insert("sub", subFieldArray);
        }
    }

    return true;
}

void Config::save()
{
    // debug
    QVariantList mainArray;

    for (auto message = messages_.begin(); message != messages_.end(); ++message)
    {
        QVariantMap messageObj;

        QVariantList fieldsArray;

        qDebug() << message->first;
        for (auto field = message->second.begin(); field != message->second.end(); ++field)
        {
            QVariantMap fieldObj;

            if (setFieldObj(fieldObj, field->second))
            {
                fieldsArray.append(fieldObj);
            }
        }

        messageObj.insert(message->first, fieldsArray);
        mainArray.append(messageObj);
    }


    QJsonArray jsonArray = QJsonArray::fromVariantList(mainArray);
    QJsonDocument jsonDoc(jsonArray);
    QByteArray jsonByte = jsonDoc.toJson();

    qDebug() << jsonByte.toStdString().c_str();

    QFile file("config.json");
    if (!file.open(QIODevice::WriteOnly))
    {
        throw std::exception("fail to read config (read error)");
    }

    file.write(jsonByte);
    file.close();

}

}
