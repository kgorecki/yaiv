#include "imageprocessor.h"

ImageProcessor::ImageProcessor(QObject *parent)
    : QObject(parent)
    , currentImage(0)
{
}

void ImageProcessor::addImage(QImage newImage)
{
    if (currentImage < images.length())
        images.erase(image + 1, images.end());

    images.append(newImage);
    if (!currentImage)
        image = images.begin();
    else
        image = images.end() - 1;
    ++currentImage;
}

bool ImageProcessor::canRedo()
{
    return (currentImage < images.length() ? true : false);
}

bool ImageProcessor::canUndo()
{
    return (currentImage > 1 ? true : false);
}

QImage* ImageProcessor::flip(eFlip direction)
{
    addImage((kFlipHorizontal == direction) ? image->mirrored(true, false) : image->mirrored(false, true));
    return image;
}

QImage* ImageProcessor::redo()
{
    if (canRedo())
    {
        ++image;
        ++currentImage;
    }
    return image;
}

QImage* ImageProcessor::rotate(int angle)
{
    QPixmap pixmap = QPixmap::fromImage(*image);
    QMatrix matrix;
    matrix.rotate(angle);
    addImage(pixmap.transformed(matrix).toImage());
    return image;
}

void ImageProcessor::setImage(const QImage &image)
{
//    if (currentImage)
//    {
//        currentImage = 0;
//        images.erase(images.begin(), images.end());
//    }
    addImage(image);
}

QImage* ImageProcessor::toGrayscale()
{
    QImage tempImage = image->convertToFormat(QImage::Format_RGB32);
    for (int i = 0; i < tempImage.height(); i++)
    {
        QRgb *pixel = reinterpret_cast<QRgb *>(tempImage.scanLine(i));
        QRgb *end = pixel + tempImage.width();
        while (pixel++ != end)
        {
            int color = qGray(*pixel);
            *pixel = QColor(color, color, color).rgb();
        }
    }
    addImage(tempImage);
    return image;
}

QImage* ImageProcessor::toSepia()
{
    QImage tempImage = image->convertToFormat(QImage::Format_RGB32);
    for (int i = 0; i < tempImage.height(); i++)
    {
        QRgb *pixel = reinterpret_cast<QRgb *>(tempImage.scanLine(i));
        QRgb *end = pixel + tempImage.width();
        while (pixel++ != end)
        {
            int red = qRed(*pixel) * 0.393 + qGreen(*pixel) * 0.769 + qBlue(*pixel) * 0.189;
            int green = qRed(*pixel) * 0.349 + qGreen(*pixel) * 0.686 + qBlue(*pixel) * 0.168;
            int blue = qRed(*pixel) * 0.272 + qGreen(*pixel) * 0.534 + qBlue(*pixel) * 0.131;
            *pixel = QColor(red < 255 ? red : 255,
                            green < 255 ? green : 255,
                            blue < 255 ? blue : 255).rgb();
        }
    }
    addImage(tempImage);
    return image;
}

QImage* ImageProcessor::undo()
{
    if (canUndo())
    {
        --image;
        --currentImage;
    }
    return image;
}
