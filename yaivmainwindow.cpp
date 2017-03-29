#include <QtWidgets>

#include "yaivmainwindow.h"

YaivMainWindow::YaivMainWindow()
    : imageProcessor(this)
    , lblImage(new QLabel)
    , scrollArea(new QScrollArea)
    , scaleFactor(1)
    , modified(false)
{
    lblImage->setBackgroundRole(QPalette::Base);
    lblImage->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    lblImage->setScaledContents(true);

    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(lblImage);
    setCentralWidget(scrollArea);

    prepareActions();
    setView(false);

    setTitleAndStatus(false);

    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
}

bool YaivMainWindow::openFile(const QString &fileName)
{
    this->fileName = fileName;
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

    setImage(image, kExistingImageNew);

    aViewFitToWindow->setChecked(true);
    sViewFitToWindow();
    setTitleAndStatus(false);
    setView(true);

    return true;
}

bool YaivMainWindow::saveFile(const QString &fileName)
{
    QImageWriter imageWriter(fileName);
    this->fileName = fileName;

    if (!imageWriter.write(* imageProcessor.getImage()))
    {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot save %1!").arg(QDir::toNativeSeparators(fileName)));
        return false;
    }

    setTitleAndStatus(false);

    return true;
}

void YaivMainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    if (!modified || (modified && showDiscardDialog()))
        event->accept();
}

// *** private slots
void YaivMainWindow::sFileOpen()
{
    if (modified && !showDiscardDialog())
        return;

    QFileDialog dialog(this, tr("Open file"));

    prepareFileDialog(dialog, QFileDialog::AcceptOpen);

    if (dialog.exec() == QDialog::Accepted && openFile(dialog.selectedFiles().first()))
        setDirIterator();
}

void YaivMainWindow::sFileOpenNext()
{
    if (modified && !showDiscardDialog())
        return;
    if (++dirIterator == imagesInDirectory.end())
        dirIterator = imagesInDirectory.begin();
    openFile(QDir::cleanPath(dirBase + QDir::separator() + *(dirIterator)));
}

void YaivMainWindow::sFileOpenPrev()
{
    if (modified && !showDiscardDialog())
        return;
    if (dirIterator == imagesInDirectory.begin())
        dirIterator = imagesInDirectory.end();
    openFile(QDir::cleanPath(dirBase + QDir::separator() + *(--dirIterator)));
}

void YaivMainWindow::sFileSave()
{
    saveFile(fileName);
}

void YaivMainWindow::sFileSaveAs()
{
    QFileDialog dialog(this, tr("Save File As"));
    prepareFileDialog(dialog, QFileDialog::AcceptSave);

    if (dialog.exec() == QDialog::Accepted)
        saveFile(dialog.selectedFiles().first());
}

void YaivMainWindow::sFileRefreshFileList()
{
    setDirIterator(true);
}

void YaivMainWindow::sEditUndo()
{
    setImage(* imageProcessor.undo());
    if (!imageProcessor.canUndo())
        setTitleAndStatus(false);
    setUndoRedo();
}

void YaivMainWindow::sEditRedo()
{
    setImage(* imageProcessor.redo());
    setTitleAndStatus(true);
    setUndoRedo();
}

void YaivMainWindow::sEditCopy()
{
#ifndef QT_NO_CLIPBOARD
    QGuiApplication::clipboard()->setImage(* imageProcessor.getImage());
#endif // QT_NO_CLIPBOARD
}

void YaivMainWindow::sEditRotateRight()
{
    rotateImage(90);
    setTitleAndStatus(true);
}

void YaivMainWindow::sEditRotateLeft()
{
    rotateImage(270);
    setTitleAndStatus(true);
}

void YaivMainWindow::sEditToGrayscale()
{
    setImage(* imageProcessor.toGrayscale());
    setTitleAndStatus(true);
}

void YaivMainWindow::sEditToSepia()
{
    setImage(* imageProcessor.toSepia());
    setTitleAndStatus(true);
}

void YaivMainWindow::sEditFlipHorizontally()
{
    setImage(* imageProcessor.flip(kFlipHorizontal));
    setTitleAndStatus(true);
}

void YaivMainWindow::sEditFlipVertically()
{
    setImage(* imageProcessor.flip(kFlipVertical));
    setTitleAndStatus(true);
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
    setImage(* imageProcessor.getImage());
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

void YaivMainWindow::sHelpAbout()
{
    QMessageBox::about(this, tr("About vaiv"),
                    tr("<b>Yet Another Image Viewer</b> version %1.%2").arg(VERSION_MAJOR).arg(VERSION_MINOR)
                    + tr("<br>Author: Krzysztof Gorecki kgorecki@b-intohimo.com"));
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
    prepareActionsFile();
    prepareActionsEdit();
    prepareActionsView();

    menuHelp = menuBar()->addMenu(tr("&Help"));

    aHelpAbout = menuHelp->addAction(tr("&About"), this, &YaivMainWindow::sHelpAbout);
    aHelpAbout->setShortcut(tr("ctrl+/"));
}

void YaivMainWindow::prepareActionsFile()
{
    menuFile = menuBar()->addMenu(tr("File"));
    aFileOpen = menuFile->addAction(tr("&Open"), this, &YaivMainWindow::sFileOpen);
    aFileOpen->setShortcut(QKeySequence::Open);

    aFileOpenNext = menuFile->addAction(tr("Open &Next"), this, &YaivMainWindow::sFileOpenNext);
    aFileOpenNext->setShortcut(QKeySequence::MoveToNextChar);

    aFileOpenPrev = menuFile->addAction(tr("Open &Previous"), this, &YaivMainWindow::sFileOpenPrev);
    aFileOpenPrev->setShortcut(QKeySequence::MoveToPreviousChar);

    aFileSave = menuFile->addAction(tr("&Save"), this, &YaivMainWindow::sFileSave);
    aFileSave->setShortcut(QKeySequence::Save);

    aFileSaveAs = menuFile->addAction(tr("Save &As"), this, &YaivMainWindow::sFileSaveAs);
    aFileSaveAs->setShortcut(QKeySequence::SaveAs);

    menuFile->addSeparator();

    aFileRefreshFileList = menuFile->addAction(tr("&Refresh file list"), this, &YaivMainWindow::sFileRefreshFileList);
    aFileRefreshFileList->setShortcut(tr("Ctrl+Shift+R"));

    menuFile->addSeparator();

    aFileClose = menuFile->addAction(tr("&Quit"), this, &QWidget::close);
    aFileClose->setShortcut(QKeySequence::Quit);
}

void YaivMainWindow::prepareActionsEdit()
{
    menuEdit = menuBar()->addMenu(tr("&Edit"));

    aEditUndo = menuEdit->addAction(tr("&Undo"), this, &YaivMainWindow::sEditUndo);
    aEditUndo->setShortcut(QKeySequence::Undo);

    aEditRedo = menuEdit->addAction(tr("R&edo"), this, &YaivMainWindow::sEditRedo);
    aEditRedo->setShortcut(QKeySequence::Redo);

    menuEdit->addSeparator();

    aEditCopy = menuEdit->addAction(tr("&Copy"), this, &YaivMainWindow::sEditCopy);
    aEditCopy->setShortcut(QKeySequence::Copy);

    menuEdit->addSeparator();

    aEditRotateRight = menuEdit->addAction(tr("Rotate &Right"), this, &YaivMainWindow::sEditRotateRight);
    aEditRotateRight->setShortcut(tr("Alt+R"));

    aEditRotateLeft = menuEdit->addAction(tr("Rotate &Left"), this, &YaivMainWindow::sEditRotateLeft);
    aEditRotateLeft->setShortcut(tr("Alt+L"));

    aEditFlipHorizontally = menuEdit->addAction(tr("Flip &Horizontally"), this, &YaivMainWindow::sEditFlipHorizontally);
    aEditFlipHorizontally->setShortcut(tr("Alt+H"));

    aEditFlipVertically = menuEdit->addAction(tr("Flip &Vertically"), this, &YaivMainWindow::sEditFlipVertically);
    aEditFlipVertically->setShortcut(tr("Alt+V"));

    aEditToGrayscale = menuEdit->addAction(tr("To &Grayscale"), this, &YaivMainWindow::sEditToGrayscale);
    aEditToGrayscale->setShortcut(tr("Alt+G"));

    aEditToSepia = menuEdit->addAction(tr("To &Sepia"), this, &YaivMainWindow::sEditToSepia);
    aEditToSepia->setShortcut(tr("Alt+S"));
}

void YaivMainWindow::prepareActionsView()
{
    menuView = menuBar()->addMenu(tr("&View"));

    aViewZoomIn = menuView->addAction(tr("Zoom &In"), this, &YaivMainWindow::sViewZoomIn);
    aViewZoomIn->setShortcut(QKeySequence::ZoomIn);

    aViewZoomOut = menuView->addAction(tr("Zoom &Out"), this, &YaivMainWindow::sViewZoomOut);
    aViewZoomOut->setShortcut(QKeySequence::ZoomOut);

    aViewNaturalSize = menuView->addAction(tr("&Natural Size"), this, &YaivMainWindow::sViewNaturalSize);
    aViewNaturalSize->setShortcut(tr("Ctrl+0"));

    aViewFitToWindow = menuView->addAction(tr("&Fit to Window"), this, &YaivMainWindow::sViewFitToWindow);
    aViewFitToWindow->setCheckable(true);
    aViewFitToWindow->setShortcut(tr("Ctrl+F"));

    aViewStretchToWindow = menuView->addAction(tr("&Stretch to Window"), this, &YaivMainWindow::sViewStretchToWindow);
    aViewStretchToWindow->setCheckable(true);
    aViewStretchToWindow->setShortcut(tr("Ctrl+G"));
}

void YaivMainWindow::prepareFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    QStringList *mimeTypeFilters = new QStringList;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
        mimeTypeFilters->append(mimeTypeName);
    mimeTypeFilters->sort();

    dialog.setMimeTypeFilters(*mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    dialog.setAcceptMode(acceptMode);
    if (acceptMode == QFileDialog::AcceptSave)
    {
        QFileInfo file(fileName);
        dialog.setDefaultSuffix(file.suffix());
    }
}

void YaivMainWindow::resetViewOptions()
{
    aViewFitToWindow->setChecked(false);
    aViewStretchToWindow->setChecked(false);
    scrollArea->setWidgetResizable(false);
}

void YaivMainWindow::resizeImage(double factor)
{
    scaleFactor *= factor;
    lblImage->resize(scaleFactor * lblImage->pixmap()->size());
    setScrollBar(scrollArea->horizontalScrollBar(), factor);
    setScrollBar(scrollArea->verticalScrollBar(), factor);
}

void YaivMainWindow::rotateImage(int angle)
{
    setImage(* imageProcessor.rotate(angle));
    aViewFitToWindow->setChecked(true);
    sViewFitToWindow();
}

void YaivMainWindow::setDirIterator(bool isDir)
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

void YaivMainWindow::setImage(const QImage &newImage, eExistingImage existing)
{
    if (kExistingImageNew == existing)
        imageProcessor.setImage(newImage);
    lblImage->setPixmap(QPixmap::fromImage(* imageProcessor.getImage()));
    scaleFactor = 1;
    resizeImage(1);
}

void YaivMainWindow::setResizedImage()
{
    lblImage->setPixmap(QPixmap::fromImage(* imageProcessor.getImage()).scaled(scrollArea->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    scaleFactor = 1;
    resizeImage(1);
}

void YaivMainWindow::setScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()));
}

void YaivMainWindow::setTitleAndStatus(bool modified)
{
    if (fileName.isEmpty())
    {
        setWindowTitle(tr("Yet Another Image Viewer"));
        statusBar()->showMessage(tr(""));
        return;
    }

    this->modified = modified;

    QString mod = (modified) ? tr("* ") : tr("");
    setWindowTitle(mod + fileName);

    const QString message = tr("File: %1x%2, Color depth: %3, DPI: %4")
        .arg(imageProcessor.getWidth()).arg(imageProcessor.getHeight()).arg(imageProcessor.getDepth()).arg(imageProcessor.getDpi());
    statusBar()->showMessage(message);
    setUndoRedo();
}

void YaivMainWindow::setUndoRedo()
{
    if (imageProcessor.canUndo())
        aEditUndo->setVisible(true);
    else
        aEditUndo->setVisible(false);
    if (imageProcessor.canRedo())
        aEditRedo->setVisible(true);
    else
        aEditRedo->setVisible(false);
}

void YaivMainWindow::setView(bool value)
{
    scrollArea->setVisible(value);
    aFileOpenNext->setVisible(value);
    aFileOpenPrev->setVisible(value);
    aFileSave->setVisible(value);
    aFileSaveAs->setVisible(value);
    aFileRefreshFileList->setVisible(value);
    aEditCopy->setVisible(value);
    aEditRotateRight->setVisible(value);
    aEditRotateLeft->setVisible(value);
    aEditFlipHorizontally->setVisible(value);
    aEditFlipVertically->setVisible(value);
    aEditToGrayscale->setVisible(value);
    aEditToSepia->setVisible(value);
    aViewZoomIn->setVisible(value);
    aViewZoomOut->setVisible(value);
    aViewNaturalSize->setVisible(value);
    aViewFitToWindow->setVisible(value);
    aViewStretchToWindow->setVisible(value);
    menuEdit->menuAction()->setVisible(value);
    menuView->menuAction()->setVisible(value);
}

bool YaivMainWindow::showDiscardDialog()
{
    return QMessageBox::Yes == QMessageBox::question(this, "Unsaved image!",
                    "You didn't save image! Are you sure you want to discard changes?",
                    QMessageBox::Yes|QMessageBox::No);
}
