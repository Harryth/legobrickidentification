#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QLabel>
#include <QDebug>
#include <QMessageBox>
#include <QFormLayout>
#include <QBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QDesktopServices>
#include <QUrl>

#include <opencv2/imgproc/imgproc.hpp>
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
    void opencvHelp();

private:
    Ui::MainWindow *ui;

    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);

    QString location; // Store the last location in which an image was opened
    QString imgName; // Loaded image name to show in the window title

    cv::Mat image; // Store the image

    void imgShow(cv::Mat img); // Show image in GUI
};

#endif // MAINWINDOW_H
