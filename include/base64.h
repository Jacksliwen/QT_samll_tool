#ifndef BASE64_H
#define BASE64_H
#include <QImage>

#ifdef quc
class Q_DECL_EXPORT Base64
#else
class Base64
#endif

{
public:
    //图片转base64字符串
    static QString imageToBase64(const QImage &image);
    static QByteArray imageToBase64x(const QImage &image);

    //base64字符串转图片
    static QImage base64ToImage(const QString &data);
    static QImage base64ToImagex(const QByteArray &data);

    //字符串与base64互转
    static QString textToBase64(const QString &text);
    static QString base64ToText(const QString &text);
};

#endif // BASE64_H
