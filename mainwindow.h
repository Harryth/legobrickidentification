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
#include <QtSql/QtSql>

#include <math.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>

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
    void openImage(); // Function that load an image opening a file dialog asking for image location
    void saveImage(); // Function that save an image opening a file dialog asking for image location
    void opencvHelp(); // Function that open a dialog to search in opencv documentation
    void trainIdentifier(); // Train the identifier
    void identifyImage(); // Identify image
    void about(); // Program credicts

private:
    Ui::MainWindow *ui;

    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);

    QString location; // Store the last location in which an image was opened
    QString imgName; // Loaded image name to show in the window title
    std::vector<QString> dbNames; // Store database names
    QSqlDatabase db; // Database handler

    cv::Mat image; // Store the image that is processed
    cv::Mat orgImage; // Original image
    cv::NormalBayesClassifier bayes; // Bayes classifier

    bool bayesTrained; // Bayes already trained flag

    void imgShow(cv::Mat img); // Show image in GUI
    void loadDataBaseValues(std::vector<std::vector<cv::Point2f> > &values); // Load values from a data base
    void labelsTrainData(cv::Mat &trainData, cv::Mat &labels); // Calculate train data and assign labels to each class

};

#endif // MAINWINDOW_H
