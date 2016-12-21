#ifndef YAIVMAINWINDOW_H
#define YAIVMAINWINDOW_H

#include <QDirIterator>
#include <QImage>
#include <QMainWindow>

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;

class YaivMainWindow : public QMainWindow
{
    Q_OBJECT
public:
//    YaivMainWindow(int argc, char *argv[]);
    YaivMainWindow();
    bool openFile(const QString &fileName);

signals:

private slots:
    void sFileOpen();
//    void sFileSave();
//    void sFileSaveAs();
    void sFileOpenNext();
    void sFileOpenPrev();
    void sFileRefreshFileList();
//    void sEditRotateRight();
//    void sEditRotateLeft();
//    void sEditCopy();
//    void sEditPaste();
//    void sEditSettings();
    void sViewZoomIn();
    void sViewZoomOut();
    void sViewNaturalSize();
    void sViewFitToWindow();
    void sViewStretchToWindow();
//    void sHelpAbout();

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    void prepareActions();
    void prepareMimeTypes();
    void resetViewOptions();
    void resizeImage(double factor);
    void setDirIterator(const QString &fileName, bool isDir = false);
    void setImage(const QImage &newImage);
    void setResizedImage();
    void setScrollBar(QScrollBar *scrollBar, double factor);
    void setView(bool value);

    QImage image;
    QLabel *lblImage;
    QScrollArea *scrollArea;
    double scaleFactor;
    QStringList *mimeTypeFilters;
    QStringList imagesInDirectory;
    QStringList::iterator dirIterator;
    QString dirBase;

    QAction *aFileOpen;
    QAction *aFileSave;
    QAction *aFileSaveAs;
    QAction *aFileOpenNext;
    QAction *aFileOpenPrev;
    QAction *aFileRefreshFileList;
    QAction *aFileClose;
    QAction *aEditRotateRight;
    QAction *aEditRotateLeft;
    QAction *aEditCopy;
    QAction *aEditPaste;
    QAction *aEditSettings;
    QAction *aViewZoomIn;
    QAction *aViewZoomOut;
    QAction *aViewNaturalSize;
    QAction *aViewFitToWindow;
    QAction *aViewStretchToWindow;
};

#endif // YAIVMAINWINDOW_H
