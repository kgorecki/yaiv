#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QObject>
#include <QImage>
#include <QPixmap>

enum eFlip
{
    kFlipHorizontal = 0,
    kFlipVertical
};

class ImageProcessor : public QObject
{
    Q_OBJECT
public:
    ImageProcessor(QObject *parent = 0);
    QImage* flip(eFlip direction);
    QImage* rotate(int angle);
    QImage* toGrayscale();
    QImage* toSepia();

//inlines
    inline int getDepth() { return image.depth(); };
    inline int getDpi() { return image.physicalDpiX(); };
    inline QImage* getImage() { return &image; };
    inline int getHeight() { return image.height(); };
    inline int getWidth() { return image.width(); };
    inline void setImage(const QImage &image) { this->image = image; };

private:
    QImage image;
};

#endif // IMAGEPROCESSOR_H
