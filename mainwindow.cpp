#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Connect ui actions with slots
    connect(ui->actionopen_image,SIGNAL(triggered()),this,SLOT(openImage()));
    connect(ui->actionOpenCV_Help,SIGNAL(triggered()),this,SLOT(opencvHelp()));
    connect(ui->actionExit,SIGNAL(triggered()),this,SLOT(close()));

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
            imgShow(image);
        }
}

void MainWindow::imgShow(cv::Mat img)
{
    // If no image create one with the background color
    if(img.empty())
        img = cv::Mat(512,512,CV_8UC3,cv::Scalar(231,231,232));

    // Store central widget width and height
    int w = ui->centralWidget->size().width();
    int h = ui->centralWidget->size().height();

    // GUI's aspect ratio
    double aspectRatio = (double) w / h;

    // Scale factor to resize the image
    double scaleFactor = 0.0;

    // Calculates the scale factor depending the GUI's aspect ratio
    scaleFactor = aspectRatio < 1? (double) w / img.cols: (double) h / img.rows;

    // Resize the image
    cv::resize(img,img,cv::Size(0,0),scaleFactor,scaleFactor);

    // If the image width is bigger than GUI's width ROI position is out of boundaries
    if(w > img.cols)
    {
        // Temporary Mat of central widget size and background color
        cv::Mat tmp(h,w,img.type(),cv::Scalar(231,231,232));

        // Region of interest of image size
        cv::Mat roi = tmp(cv::Rect((w-img.cols)/2,(h-img.rows)/2,img.cols,img.rows));

        // Clear ROI
        cv::bitwise_and(roi,cv::Mat(roi.size(),roi.type(),cv::Scalar(0,0,0)),roi);

        // tmp's ROI plus image
        cv::add(roi,img,roi);

        QLabel *imgLabel = new QLabel(ui->centralWidget); // Label where image will be shown, central widget as parent

        QImage qImg;  // QImage that store the image

        if(tmp.channels() > 1)
        {
            cv::cvtColor(tmp,tmp,CV_BGR2RGB);
            // Create QImage
            qImg = QImage((const unsigned char*)(tmp.data),tmp.cols,tmp.rows,tmp.step,QImage::Format_RGB888);
        }
        else
            qImg = QImage((const unsigned char*)(tmp.data),tmp.cols,tmp.rows,tmp.step,QImage::Format_Indexed8);

        // Set the image in the QLabel
        imgLabel->setPixmap(QPixmap::fromImage(qImg));
        imgLabel->show(); // Show the label
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Destroy all OpenCV windows
    cv::destroyAllWindows();
    QWidget::closeEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    // If a resize evente, show image
    imgShow(image);
    QWidget::resizeEvent(event);
}

void MainWindow::opencvHelp()
{
    QDialog *helpDialog = new QDialog(this); // QDialog to display searh box
    QLabel *text = new QLabel(tr("Introdusca el término de búsqueda en la \ndocumentación de OpenCV")); // Text in the dialog
    QLineEdit *searchLine = new QLineEdit(); // Line to introduce text
    QPushButton *searchBtn = new QPushButton(tr("Buscar")); // Search button
    QFormLayout *formLayout = new QFormLayout; // Forma layout to the search line and the search button
    QVBoxLayout *vertLayout = new QVBoxLayout; // Vertical layout for the form layout and the text label
    QString *searchText = new QString("http://docs.opencv.org/search.html?q="); // String to searh in OpenCV documentation

    helpDialog->setWindowTitle(tr("Búsqueda en OpenCV")); // Dialog title

    // Search line and text width
    searchLine->setFixedWidth(170);
    text->setFixedWidth(250);

    // Layout the search line and the search button in a row
    formLayout->addRow(searchLine,searchBtn);

    // Layout the text label and the form layout
    vertLayout->addWidget(text);
    vertLayout->addLayout(formLayout);

    // Set the dialog layout
    helpDialog->setLayout(vertLayout);

    // Connect the search button signal to the close slot to close the dialog at button press
    connect(searchBtn,SIGNAL(clicked()),helpDialog,SLOT(close()));

    // Exec search dialog
    helpDialog->exec();

    // Append the search text to the url
    searchText->append(searchLine->displayText()).append("&check_keywords=yes&area=default");
    QDesktopServices::openUrl(QUrl(*searchText)); // Exec the search oppening the default browser
}
