
#ifndef OBJECTS_IDS_HPP
#define OBJECTS_IDS_HPP

// https://stackoverflow.com/questions/34287842/c-strongly-typed-using-and-typedef

#include <QString>

// define some tags to create uniqueness
struct raid_id_tag {};


template<class Tag>
struct string_id
{
    string_id(const QString& s): m_value(s) {}
    string_id() : m_value() {}

    const QString& value() const { return m_value; }

    bool operator<(const string_id& other) const
    {
        return m_value < other.m_value;
    }

private:
    QString m_value;
};


// create some type aliases for ease of use
using RaidId = string_id<raid_id_tag>;

#endif
