#include <QtWidgets>

#include "yaivmainwindow.h"

YaivMainWindow::YaivMainWindow()
    : lblImage(new QLabel)
    , scrollArea(new QScrollArea)
    , scaleFactor(1)
{
    lblImage->setBackgroundRole(QPalette::Base);
    lblImage->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    lblImage->setScaledContents(true);

    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(lblImage);
    setCentralWidget(scrollArea);

    prepareActions();
    prepareMimeTypes();
    setView(false);

    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
}

bool YaivMainWindow::openFile(const QString &fileName)
{
    QImageReader reader (fileName);
    reader.setAutoTransform(true);
    const QImage image = reader.read();
    if (image.isNull())
    {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot open %1: %2").arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        setView(false);
        return false;
    }

    setImage(image);

    const QString message = tr("File: \"%1\", %2x%3, Color depth: %4, DPI: %5")
        .arg(QDir::toNativeSeparators(fileName)).arg(image.width()).arg(image.height()).arg(image.depth()).arg(image.physicalDpiX());
    statusBar()->showMessage(message);

//    setDirIterator(fileName);
    aViewFitToWindow->setChecked(true);
    sViewFitToWindow();
    setView(true);

    return true;
}

// *** private slots
void YaivMainWindow::sFileOpen()
{
    QFileDialog dialog(this, tr("Open file"));

    dialog.setMimeTypeFilters(*mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");

    while (dialog.exec() == QDialog::Accepted && !openFile(dialog.selectedFiles().first())) {}

    setDirIterator(dialog.selectedFiles().first());
}

void YaivMainWindow::sFileOpenNext()
{
    if (++dirIterator == imagesInDirectory.end())
        dirIterator = imagesInDirectory.begin();
    openFile(QDir::cleanPath(dirBase + QDir::separator() + *(dirIterator)));
}

void YaivMainWindow::sFileOpenPrev()
{
    if (dirIterator == imagesInDirectory.begin())
        dirIterator = imagesInDirectory.end();
    openFile(QDir::cleanPath(dirBase + QDir::separator() + *(--dirIterator)));
}

void YaivMainWindow::sFileRefreshFileList()
{
    setDirIterator(dirBase, true);
}

void YaivMainWindow::sViewZoomIn()
{
    resetViewOptions();
    resizeImage(1.125);
}

void YaivMainWindow::sViewZoomOut()
{
    resetViewOptions();
    resizeImage(0.9);
}

void YaivMainWindow::sViewNaturalSize()
{
    resetViewOptions();
    scaleFactor = 1;
//    resizeImage(1);
    setImage(image);
}

void YaivMainWindow::sViewFitToWindow()
{
    aViewStretchToWindow->setChecked(false);
    bool fitToWindow = aViewFitToWindow->isChecked();
    scrollArea->setWidgetResizable(false);
    setResizedImage();

    if (!fitToWindow)
        sViewNaturalSize();
}

void YaivMainWindow::sViewStretchToWindow()
{
    aViewFitToWindow->setChecked(false);
    bool stretchToWindow = aViewStretchToWindow->isChecked();
    scrollArea->setWidgetResizable(stretchToWindow);
    if (!stretchToWindow)
        sViewNaturalSize();
}

// *** protected
void YaivMainWindow::resizeEvent(QResizeEvent *event)
{
   QMainWindow::resizeEvent(event);
   if (aViewFitToWindow->isChecked())
        setResizedImage();
}

// *** private
void YaivMainWindow::prepareActions()
{
    fileMenu = menuBar()->addMenu(tr("File"));
    aFileOpen = fileMenu->addAction(tr("&Open"), this, &YaivMainWindow::sFileOpen);
    aFileOpen->setShortcut(QKeySequence::Open);

    aFileOpenNext = fileMenu->addAction(tr("Open &Next"), this, &YaivMainWindow::sFileOpenNext);
    aFileOpenNext->setShortcut(QKeySequence::MoveToNextChar);

    aFileOpenPrev = fileMenu->addAction(tr("Open &Previous"), this, &YaivMainWindow::sFileOpenPrev);
    aFileOpenPrev->setShortcut(QKeySequence::MoveToPreviousChar);

    fileMenu->addSeparator();

    aFileRefreshFileList = fileMenu->addAction(tr("&Refresh file list"), this, &YaivMainWindow::sFileRefreshFileList);
    aFileRefreshFileList->setShortcut(tr("Ctrl+Shift+R"));

    aFileClose = fileMenu->addAction(tr("&Quit"), this, &QWidget::close);
    aFileClose->setShortcut(QKeySequence::Quit);

    viewMenu = menuBar()->addMenu(tr("&View"));

    aViewZoomIn = viewMenu->addAction(tr("Zoom &In"), this, &YaivMainWindow::sViewZoomIn);
    aViewZoomIn->setShortcut(QKeySequence::ZoomIn);

    aViewZoomOut = viewMenu->addAction(tr("Zoom &Out"), this, &YaivMainWindow::sViewZoomOut);
    aViewZoomOut->setShortcut(QKeySequence::ZoomOut);

    aViewNaturalSize = viewMenu->addAction(tr("&Natural Size"), this, &YaivMainWindow::sViewNaturalSize);
    aViewNaturalSize->setShortcut(tr("Ctrl+0"));

    aViewFitToWindow = viewMenu->addAction(tr("&Fit to Window"), this, &YaivMainWindow::sViewFitToWindow);
    aViewFitToWindow->setCheckable(true);
    aViewFitToWindow->setShortcut(tr("Ctrl+F"));

    aViewStretchToWindow = viewMenu->addAction(tr("&Stretch to Window"), this, &YaivMainWindow::sViewStretchToWindow);
    aViewStretchToWindow->setCheckable(true);
    aViewStretchToWindow->setShortcut(tr("Ctrl+G"));
}

void YaivMainWindow::prepareMimeTypes()
{
    mimeTypeFilters = new QStringList;
    const QByteArrayList supportedMimeTypes = QImageReader::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
        mimeTypeFilters->append(mimeTypeName);
    mimeTypeFilters->sort();
}

void YaivMainWindow::resizeImage(double factor)
{
    scaleFactor *= factor;
    lblImage->resize(scaleFactor * lblImage->pixmap()->size());
    setScrollBar(scrollArea->horizontalScrollBar(), factor);
    setScrollBar(scrollArea->verticalScrollBar(), factor);
}

void YaivMainWindow::resetViewOptions()
{
    aViewFitToWindow->setChecked(false);
    aViewStretchToWindow->setChecked(false);
    scrollArea->setWidgetResizable(false);
}

void YaivMainWindow::setDirIterator(const QString &fileName, bool isDir)
{
    if (!isDir)
        dirBase = QFileInfo(fileName).absoluteDir().path();
    QString current = (isDir) ? *dirIterator : QFileInfo(fileName).fileName();
    QDir directory(dirBase);
    imagesInDirectory = directory.entryList(QDir::NoDotAndDotDot | QDir::Files);
    if (imagesInDirectory.count())
    {
        dirIterator = imagesInDirectory.begin();
        while (*dirIterator != current)
            ++dirIterator;
    }
}

void YaivMainWindow::setImage(const QImage &newImage)
{
    image = newImage;
    lblImage->setPixmap(QPixmap::fromImage(image));
    scaleFactor = 1;
    resizeImage(1);
}

void YaivMainWindow::setResizedImage()
{
    lblImage->setPixmap(QPixmap::fromImage(image).scaled(scrollArea->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    scaleFactor = 1;
    resizeImage(1);
}

void YaivMainWindow::setScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()));
}

void YaivMainWindow::setView(bool value)
{
    scrollArea->setVisible(value);
    aFileOpenNext->setVisible(value);
    aFileOpenPrev->setVisible(value);
    aFileRefreshFileList->setVisible(value);
    aViewZoomIn->setVisible(value);
    aViewZoomOut->setVisible(value);
    aViewNaturalSize->setVisible(value);
    aViewFitToWindow->setVisible(value);
    aViewStretchToWindow->setVisible(value);
    viewMenu->menuAction()->setVisible(value);
}
