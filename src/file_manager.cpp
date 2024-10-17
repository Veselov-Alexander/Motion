#include "file_manager.h"

#include "QXmlStreamWriter"
#include "QMessageBox"

#include "display_view.h"

const FileInfo FileManager::UNTITLED = { UNTITLED_FILENAME, 0, DisplayView::DEFAULT };
FileInfo FileManager::m_lastFileInfo = FileManager::UNTITLED;

bool FileInfo::operator==(const FileInfo& fileInfo) const
{
    return m_filepath == fileInfo.m_filepath &&
           m_unObjectCount == fileInfo.m_unObjectCount &&
           m_devicePolygon == fileInfo.m_devicePolygon;
}

bool FileInfo::operator!=(const FileInfo& fileInfo) const
{
    return !(*this == fileInfo);
}

void FileManager::newFile()
{
    DisplayView* pDisplayView = DisplayView::getInstance();
    assert(pDisplayView);

    pDisplayView->reset();
    m_lastFileInfo = UNTITLED;
}

void FileManager::openFile(const QString& path)
{
    DisplayView* pDisplayView = DisplayView::getInstance();
    assert(pDisplayView);

    QFile file(path);

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(
            pDisplayView->parentWidget(),
            "File error",
            "Cannot open file."
        );
    }
    else
    {
        try
        {
            QXmlStreamReader xmlReader;
            xmlReader.setDevice(&file);

            xmlReader.readNext();

            QPolygonF currentPolygon;
            std::list<QPolygonF> obstacles;

            QPolygonF device;
            QPointF devicePosition;

            bool bReadingDevice = false;

            while (!xmlReader.atEnd())
            {
                if (xmlReader.isStartElement())
                {
                    auto tag = xmlReader.name();
                    if (tag == "device")
                    {
                        devicePosition = readPoint(xmlReader.attributes());
                        bReadingDevice = true;
                    }
                    else if (tag == "point")
                    {
                        QPointF point = readPoint(xmlReader.attributes());

                        if (bReadingDevice)
                        {
                            device.append(point);
                        }
                        else
                        {
                            currentPolygon.append(point);
                        }
                    }
                    else if (tag == "polygon")
                    {
                        currentPolygon = QPolygonF();
                    }
                }
                if (xmlReader.isEndElement())
                {
                    auto tag = xmlReader.name();
                    if (tag == "device")
                    {
                        bReadingDevice = false;
                    }
                    else if (tag == "polygon")
                    {
                        if (!currentPolygon.empty())
                        {
                            obstacles.push_back(currentPolygon);
                        }
                    }
                }

                xmlReader.readNext();
            }

            if (device.empty())
            {
                throw 0;
            }
            bool bUseSensors = pDisplayView->getVision()->getEnabled();
            pDisplayView->useSensors(false);
            
            pDisplayView->reset(device);
            pDisplayView->setDevicePosition(devicePosition);


            for (const auto& obstacle : obstacles)
            {
                pDisplayView->addObstacle(obstacle);
            }
            pDisplayView->useSensors(bUseSensors);

            m_lastFileInfo = { path, obstacles.size(), device };
        }
        catch (...)
        {
            QMessageBox::warning(
                pDisplayView->parentWidget(),
                "File error",
                "Corrupted file."
            );
        }
    }
}

QPointF FileManager::readPoint(const QXmlStreamAttributes& attrs)
{
    QPointF point;
    foreach(const QXmlStreamAttribute & attr, attrs)
    {
        if (attr.name().toString() == "x")
        {
            QString attribute_value = attr.value().toString();
            point.setX(::atof(attribute_value.toStdString().c_str()));
        }
        else if (attr.name().toString() == "y")
        {
            QString attribute_value = attr.value().toString();
            point.setY(::atof(attribute_value.toStdString().c_str()));
        }
    }
    return point;
}

void FileManager::saveFile(const QString& path)
{
    DisplayView* pDisplayView = DisplayView::getInstance();

    QFile file(path);
    file.open(QIODevice::WriteOnly);

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);

    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("motion");

    xmlWriter.writeStartElement("device");

    auto devicePosition = pDisplayView->getDevicePosition();
    xmlWriter.writeAttribute("x", QString::number(devicePosition.x()));
    xmlWriter.writeAttribute("y", QString::number(devicePosition.y()));

    auto devicePolygon = pDisplayView->getDevicePolygon();
    for (auto it = devicePolygon.begin(); it != devicePolygon.end(); ++it)
    {
        xmlWriter.writeStartElement("point");

        xmlWriter.writeAttribute("x", QString::number(it->x()));
        xmlWriter.writeAttribute("y", QString::number(it->y()));

        xmlWriter.writeEndElement();
    }

    xmlWriter.writeEndElement(); // device

    xmlWriter.writeStartElement("scene");
    xmlWriter.writeAttribute("width", QString::number(DisplayView::WIDTH));
    xmlWriter.writeAttribute("height", QString::number(DisplayView::HEIGHT));

    for (const auto& obstacle : pDisplayView->getObstacles())
    {
        xmlWriter.writeStartElement("polygon");
        for (auto it = obstacle.begin(); it != obstacle.end(); ++it)
        {
            xmlWriter.writeStartElement("point");

            xmlWriter.writeAttribute("x", QString::number(it->x()));
            xmlWriter.writeAttribute("y", QString::number(it->y()));

            xmlWriter.writeEndElement();
        }
        xmlWriter.writeEndElement();
    }

    xmlWriter.writeEndElement(); // scene

    xmlWriter.writeEndElement(); // motion
    xmlWriter.writeEndDocument();

    file.close();

    m_lastFileInfo = FileManager::getCurrentInfo();
    m_lastFileInfo.m_filepath = path;
}

bool FileManager::checkNeedToSave()
{
    FileInfo fileInfo = FileManager::getCurrentInfo();
    if (m_lastFileInfo != fileInfo)
    {
        if (QMessageBox::question(
                DisplayView::getInstance(), 
                "You have unsaved changes",
                "Do you want to save current file?",
                QMessageBox::Yes | QMessageBox::No
            ) == QMessageBox::Yes)
        {
            return true;
        }
    }
    return false;
}

QString FileManager::getFilename()
{
    return m_lastFileInfo.m_filepath;
}

FileInfo FileManager::getCurrentInfo()
{
    DisplayView* pDisplayView = DisplayView::getInstance();
    assert(pDisplayView);

    return { getFilename(),
             pDisplayView->getObstacles().size(),
             pDisplayView->getDevicePolygon() };
}
