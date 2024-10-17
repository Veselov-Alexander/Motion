#pragma once

#include <QString>
#include <QPolygonF>
#include <QXmlStreamAttributes>
#include <QStandardPaths>

namespace Motion
{

static const QString UNTITLED_FILENAME = QString("%1/untitled.mo").arg(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));

struct FileInfo
{
    QString m_filepath;

    // since there is still no way to change objects, 
    // so you can only rely on information about the number of objects
    size_t m_unObjectCount;

    QPolygonF m_devicePolygon;

    bool operator==(const FileInfo& fileInfo) const;
    bool operator!=(const FileInfo& fileInfo) const;
};

class FileManager
{
public:
    static void newFile();
    static void openFile(const QString& path);
    static void saveFile(const QString& path);
    static QString getFilename();
    static bool checkNeedToSave();
public:
    const static FileInfo UNTITLED;
private:
    static QPointF readPoint(const QXmlStreamAttributes& attrs);
    static FileInfo getCurrentInfo();
private:
    static FileInfo m_lastFileInfo;
};

}  // namespace Motion