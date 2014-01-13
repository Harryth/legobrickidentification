#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Connect ui actions with slots
    connect(ui->actionopen_image,SIGNAL(triggered()),this,SLOT(openImage()));

    // Default location to open images
    location = "../../Imágenes";
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Function that load an image opening a file dialog asking for image location
void MainWindow::openImage()
{
    // Open file dialog to load a new image
    // open file dialog in previous or default location
        QString fileName = QFileDialog::getOpenFileName(this, tr("Abrir Imagen"), location,
                                                                tr("Archivos de Imagen (*.jpg *.jpeg *.png *.bmp)"));

        // Window tittle
        QString title("Identificación Bloques LEGO - ");

        if(!fileName.isEmpty())
        {
            imgName = fileName.section("/",-1);
            title.append(imgName); // Append image name to window title

            location = fileName; // Keeps last location in which an image was opened

            location.chop(imgName.size()); // Crops filename to keeps folder location

            this->setWindowTitle(title);

            // Read image from file, stores it in tmp and chop it to reduce noise in image corners
            cv::Mat tmp = cv::imread(fileName.toStdString());;
            int x,y,w,h; // Cropped image coordinates, width and height
            w = tmp.cols*0.8;
            h = tmp.rows*0.9;
            x = (tmp.cols - w) / 2;
            y = (tmp.rows - h) / 2;

            image = cv::Mat(tmp,cv::Rect(x,y,w,h));
            cv::imshow("Imagen",image);
        }
}
