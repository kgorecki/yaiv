#ifndef YAIVMAINWINDOW_H
#define YAIVMAINWINDOW_H

#include <QDirIterator>
#include <QFileDialog>
#include <QImage>
#include <QMainWindow>

#include "imageprocessor.h"

enum eExistingImage
{
    kExistingImageNew = 0,
    kExistingImageExisting
};

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;

class YaivMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    YaivMainWindow();
    bool openFile(const QString &fileName);
    bool saveFile(const QString &fileName);

signals:

private slots:
    void sFileOpen();
    void sFileSave();
    void sFileOpenNext();
    void sFileOpenPrev();
    void sFileSaveAs();
    void sFileRefreshFileList();
    void sEditUndo();
    void sEditRedo();
    void sEditCopy();
    void sEditRotateRight();
    void sEditRotateLeft();
    void sEditFlipHorizontally();
    void sEditFlipVertically();
    void sEditToGrayscale();
    void sEditToSepia();
//    void sEditPaste();
//    void sEditSettings();
    void sViewZoomIn();
    void sViewZoomOut();
    void sViewNaturalSize();
    void sViewFitToWindow();
    void sViewStretchToWindow();
    void sHelpAbout();

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    void prepareActions();
    void prepareActionsFile();
    void prepareActionsEdit();
    void prepareActionsView();
    void prepareFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode);
    void resetViewOptions();
    void resizeImage(double factor);
    void rotateImage(int angle);
    void setDirIterator(bool isDir = false);
    void setImage(const QImage &newImage, eExistingImage existing = kExistingImageExisting);
    void setResizedImage();
    void setScrollBar(QScrollBar *scrollBar, double factor);
    void setTitleAndStatus(bool modified = false);
    void setView(bool value);

    ImageProcessor imageProcessor;
    QString fileName;
    QLabel *lblImage;
    QScrollArea *scrollArea;
    double scaleFactor;
    QStringList imagesInDirectory;
    QStringList::iterator dirIterator;
    QString dirBase;
    bool modified;

    //menu:
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuView;
    QMenu *menuHelp;

    QAction *aFileOpen;
    QAction *aFileSave;
    QAction *aFileSaveAs;
    QAction *aFileOpenNext;
    QAction *aFileOpenPrev;
    QAction *aFileRefreshFileList;
    QAction *aFileClose;
    QAction *aEditUndo;
    QAction *aEditRedo;
    QAction *aEditRotateRight;
    QAction *aEditRotateLeft;
    QAction *aEditFlipHorizontally;
    QAction *aEditFlipVertically;
    QAction *aEditToGrayscale;
    QAction *aEditToSepia;
    QAction *aEditCopy;
    QAction *aEditPaste;
    QAction *aEditSettings;
    QAction *aViewZoomIn;
    QAction *aViewZoomOut;
    QAction *aViewNaturalSize;
    QAction *aViewFitToWindow;
    QAction *aViewStretchToWindow;
    QAction *aHelpAbout;
};

#endif // YAIVMAINWINDOW_H
