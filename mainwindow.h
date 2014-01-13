#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    // Function that load an image opening a file dialog asking for image location
    void openImage();

private:
    Ui::MainWindow *ui;

    QString location; // Store the last location in which an image was opened
    QString imgName; // Loaded image name to show in the window title

    cv::Mat image; // Store the image
};

#endif // MAINWINDOW_H
