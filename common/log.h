#ifndef LOG_H
#define LOG_H

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>
#include <QTextEdit>

namespace QT_LOG
{//默认文件名为当前时间命名的log文件
    QTextEdit *outedit;
    static QMutex mutex;
    static QString m_LogFile = QString("%1.log").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
    void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
    {
        QByteArray localMsg = msg.toLocal8Bit();

        QString strMsg("");
        switch(type)
        {
        case QtDebugMsg:
            strMsg = QString("Debug");
            break;
        case QtWarningMsg:
            strMsg = QString("Warning");
            break;
        case QtCriticalMsg:
            strMsg = QString("Critical");
            break;
        case QtFatalMsg:
            strMsg = QString("Fatal");
            break;
        }

        // 设置输出信息格式 [type]date
        QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        QString strMessage = QString("[%1] %2 : %3")
                .arg(strMsg).arg(strDateTime).arg(localMsg.constData());

        // 加锁
        mutex.lock();
        outedit->append(strMessage);

        // 输出信息至文件中（读写、追加形式）
        QFile file(m_LogFile);
        file.open(QIODevice::ReadWrite | QIODevice::Append);
        QTextStream stream(&file);
        stream << strMessage << "\r\n";
        file.flush();
        file.close();

        // 解锁
        mutex.unlock();
    }

    void logInit(QString logFile = "")
        {
            #ifndef _DEBUG  //实现debug版本的时候，输出到终端；release版本的时候输出到日志文件
                if (!logFile.isEmpty())
                {
                    m_LogFile = logFile;
                }

                qInstallMessageHandler(myMessageOutput);
            #endif
        }
};
#endif // LOG_H
