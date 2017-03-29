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
    void addImage(QImage newImage);
    bool canRedo();
    bool canUndo();
    QImage* flip(eFlip direction);
    QImage* redo();
    QImage* rotate(int angle);
    void setImage(const QImage &image);
    QImage* toGrayscale();
    QImage* toSepia();
    QImage* undo();

//inlines
    inline int getDepth() { return image->depth(); };
    inline int getDpi() { return image->physicalDpiX(); };
    inline QImage* getImage() { return image; };
    inline int getHeight() { return image->height(); };
    inline int getWidth() { return image->width(); };

private:
    int currentImage;
    QVector<QImage> images;
    QVector<QImage>::iterator image;
};

#endif // IMAGEPROCESSOR_H
