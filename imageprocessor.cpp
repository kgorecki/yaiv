#include "imageprocessor.h"

ImageProcessor::ImageProcessor(QObject *parent) : QObject(parent)
{

}

QImage* ImageProcessor::flip(eFlip direction)
{
    image = (kFlipHorizontal == direction) ? image.mirrored(true, false) : image.mirrored(false, true);
    return &image;
}

QImage* ImageProcessor::rotate(int angle)
{
    QPixmap pixmap = QPixmap::fromImage(image);
    QMatrix matrix;
    matrix.rotate(angle);
    image = pixmap.transformed(matrix).toImage();
    return &image;
}

QImage* ImageProcessor::toGrayscale()
{
    QImage tempImage = image.convertToFormat(QImage::Format_RGB32);
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
    image = tempImage;
    return &image;
}

QImage* ImageProcessor::toSepia()
{
    QImage tempImage = image.convertToFormat(QImage::Format_RGB32);
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
    image = tempImage;
    return &image;
}
