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
    connect(ui->actionSave_Image,SIGNAL(triggered()),this,SLOT(saveImage()));
    connect(ui->actionExit,SIGNAL(triggered()),this,SLOT(close()));
    connect(ui->action_Train_Identifier,SIGNAL(triggered()),this,SLOT(trainIdentifier()));
    connect(ui->actionIdentify_Image,SIGNAL(triggered()),this,SLOT(identifyImage()));
    connect(ui->action_About,SIGNAL(triggered()),this,SLOT(about()));

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

            image = cv::imread(fileName.toStdString());
            image.copyTo(orgImage);
            imgShow(image);

            ui->actionSave_Image->setEnabled(true);
            ui->actionIdentify_Image->setEnabled(bayesTrained);
        }
}

void MainWindow::imgShow(cv::Mat img)
{
    // If no image create one with the background color
    if(img.empty())
        img = cv::Mat(512,512,CV_8UC3,cv::Scalar(231,231,232));

    // Keeps central widget width and height
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

        QImage qImg;  // QImage that Keeps the image

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

void MainWindow::saveImage()
{
    if(!image.empty())
    {
        // Open file dialog
        QString dstFileName = QFileDialog::getSaveFileName(this,"Guardar Imagen","../../Imágenes/",
                                                           tr("Archivos de Imagen (*.jpg *.jpeg *.png *.bmp)"));
        // Suported extensions
        QString fileType = ".jpg|.jpeg|.png|.bmp";
        bool ext = false;

        // Search file extension in destination file name
        for(int i = 0; i < fileType.split("|").count(); i++)
        {
            ext |= dstFileName.endsWith(fileType.split("|").at(i));
        }

        // If not extension append png
        if(!ext)
            dstFileName.append(".png");

        // Saves image
        cv::imwrite(dstFileName.toStdString(),image);
    }
    else
    {
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setText("Error: no se ha cargado ninguna imagen.");
        msgBox->setWindowTitle("Error al guardar");
        msgBox->setButtonText(QMessageBox::Ok,tr("Aceptar"));
        msgBox->exec();
    }
}

void MainWindow::loadDataBaseValues(std::vector<std::vector<cv::Point2f> > &values)
{
    std::vector<cv::Point2f> classValues;  // Vector to Keeps each class values

    // Message dialog asking for a new database
    QMessageBox *msgBox = new QMessageBox;
    msgBox->setText("Por favor añada otra base de datos.");
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setButtonText(QMessageBox::Ok, tr("Continuar"));
    msgBox->setWindowTitle("Añadir Base de Datos");

    int count = 0;

    do
    {
        if(count == 1)
        {
            msgBox->setText("Desea añadir otra base de datos?.");
            msgBox->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            msgBox->setButtonText(QMessageBox::Ok, tr("Si"));
            msgBox->setButtonText(QMessageBox::Cancel, tr("No"));
        }

        while(!db.isOpen())
        {
            // Open file dialog to ask for db location
            QString dbFileName = QFileDialog::getOpenFileName(this, tr("Abrir Base de Datos"), "../../Bases de Datos/",
                                                              tr("Base de Datos (*.db)"));

            if(!dbFileName.isEmpty())
            {
                // Create a new database
                db = QSqlDatabase::addDatabase("QSQLITE");
                db.setDatabaseName(dbFileName);

                // keeps the data base name and shows it in the main window title
                QString title("Identificación Bloques LEGO - ");
                QString dbName = dbFileName.section("/",-1);
                title.append(dbName);
                this->setWindowTitle(title);

                dbName.chop(3);
                dbNames.push_back(dbName);

                if(!db.open())
                {
                    // Error message in case db not loades
                    QMessageBox *errorMsgBox = new QMessageBox;
                    errorMsgBox->critical(this,tr("Error en la base dedatos"),tr("La base de datos no pudo abrirse"));
                }
            }
            else
                break;
        }

        // Keeps the values of each point
        float w,h;

        if(db.isOpen())
        {
            // Creates a querry to read values from database
            QSqlQuery query;
            query.exec("SELECT charid,value FROM characteristics");

            while(query.next())
            {
                // Depending the caracteristic id Keeps the value in w or h
                if(query.value(0).toInt() == 1)
                    w = query.value(1).toFloat();
                else if(query.value(0).toInt() == 2)
                {
                    h = query.value(1).toFloat();
                    classValues.push_back(cv::Point2f(w,h)); // Keeps w and h in the vector of points
                }
            }

            // Keeps the vector of point in a vector
            values.push_back(classValues);
            classValues.clear(); // Clear the vector of points to recieve a new class

            count++;
        }

        // Destroy the connection
        QString conn;
        conn = db.connectionName();
        db.close();
        db = QSqlDatabase();
        db.removeDatabase(conn);

    }while(msgBox->exec() == QMessageBox::Ok || count < 2);
}

void MainWindow::labelsTrainData(cv::Mat &trainData, cv::Mat &labels)
{
    std::vector < std::vector<cv::Point2f> > values; // Vector of vector that Keeps the values read from databases

    loadDataBaseValues(values);  // Load values

    int nData = 0; //Number of data

    for(unsigned int i = 0; i < values.size(); i++) // Calculate number total data
        nData += values[i].size();

    trainData = cv::Mat(nData,2,CV_32FC1); // 2 cols, 2 characteristics
    labels = cv::Mat(nData,1,CV_32FC1); // 1 col labels

    int r = 0;

    for(unsigned int i = 0; i < values.size(); i++)
        for(unsigned j = 0; j < values[i].size(); j++)
        {
            trainData.at<float>(r,0) = values[i][j].x;
            trainData.at<float>(r,1) = values[i][j].y;
            labels.at<float>(r++,0) = i;
        }
}

void MainWindow::trainIdentifier()
{
    cv::Mat trainData;
    cv::Mat labels;

    labelsTrainData(trainData,labels);

    bayes.clear();

    if(!labels.empty() && labels.at<float>(labels.rows-1) != 0)
        bayesTrained = bayes.train(trainData,labels);

    if(!image.empty())
        ui->actionIdentify_Image->setEnabled(bayesTrained);
}

void MainWindow::identifyImage()
{
    std::vector<cv::Mat> layers;  // Keeps image's color channels

    cv::split(image,layers);  // Split image in each color channel because some bricks are better segmented in a different channel

    //Compute filters to each channela an binarize them
    for(int i = 0; i < 2; i++)
    {
        cv::blur(layers[i],layers[i],cv::Size(25,25));
        cv::medianBlur(layers[i],layers[i],13);
        cv::threshold(layers[i],layers[i],i?157:181,255,i);
    }

    // Combine the two channels in one
    cv::bitwise_or(layers[0],layers[1],image);

    // Morphology close operation to close some holes
    cv::Mat strElmt = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(5,5));
    cv::morphologyEx(image,image,cv::MORPH_CLOSE,strElmt,cv::Point(-1,-1),5);

    cv::Mat fg,bg; // Keeps the foreground and background

    cv::erode(image,fg,cv::Mat(),cv::Point(-1,-1),3); // Erode to have a mark in each brick labeled as 255
    cv::dilate(image,bg,cv::Mat(),cv::Point(-1,-1),70); // Dilate to have a mark in background
    cv::threshold(bg,bg,1,128,cv::THRESH_BINARY_INV);  // Background mark labeled as 128

    // Add the two marker images
    cv::Mat markerImg(image.size(),CV_8U,cv::Scalar(0));
    markerImg = fg + bg;

    cv::Mat markers;

    // Convert markers to integers
    markerImg.convertTo(markers,CV_32S);

    cv::watershed(orgImage,markers);  // Aply watershed to original image

    markers.convertTo(image,CV_8U);

    cv::threshold(image,image,128,255,cv::THRESH_BINARY); // Binarize image

    // Keeps image contours
    std::vector< std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;

    // Find contours
    cv::findContours(image,contours,hierarchy,cv::RETR_CCOMP,cv::CHAIN_APPROX_SIMPLE);

    // Keeps min area rects
    std::vector<cv::RotatedRect> minRects;
    std::vector<float> tmpValues; // Keeps temporary values of width and height to be kept in a cv Mat
    std::vector<float> ids; // Keeps labels of classes predicted
    float w,h,temp; // Temporary keep the width and height values of the min area rect
    cv::Mat* samples; // Keeps the samble width and height to be predicted by the classifier

    cv::Mat tmp(image.size(),CV_8UC3,cv::Scalar(0,0,0));  // Temporary mat to the drawings
    cv::RNG rng(12345);
    cv::Point2f rect_points[4]; // Keeps the points of the min area rects

    for(unsigned int i = 0; i < contours.size(); i++)
    {
        // Calculate the minumun area rectangle
        minRects.push_back(cv::minAreaRect(contours[i]));
        // Keeps the width and the height of the min area rect
        w = minRects[i].size.width;
        h = minRects[i].size.height;

        // Keeps the max value as the width
        if(w < h)
        {
            temp = h;
            h = w;
            w = temp;
        }

        // Keeps values in a vector
        tmpValues.push_back(w);
        tmpValues.push_back(h);

        // Copie the values in a cv mat
        samples = new cv::Mat(1,2,CV_32FC1,tmpValues.data());
        tmpValues.clear(); // Clear the temp vector

        ids.push_back(bayes.predict(*samples)); // Predict in the classifier with the computed values

        // Random color for each class
        cv::Scalar color(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255));
        minRects[i].points(rect_points); // Copie the min area rect points

        for( int j = 0; j < 4; j++ )
        {
            if(j == 0)
                // Puts the corresponding database name in the rectangle
                cv::putText(tmp,dbNames[ids[i]].toStdString(),rect_points[j],
                        cv::FONT_HERSHEY_SIMPLEX,1.5,color,2);
            // Plot each line of the rectangle
            cv::line( tmp, rect_points[j], rect_points[(j+1)%4], color, 2, 8 );
        }
    }

    // Adds the rectangles and names to the original image
    cv::addWeighted(tmp,1,orgImage,0.4,1,image);

    // Shows the image
    imgShow(image);

    std::vector<float>::iterator it; // Iterator to go over each element in ids vector
    std::vector<QString> missing; // Keeps each missing block id
    std::vector<float> repeated; // Keeps each repeated block id

    // Find for missing blocks in the data base names loaded
    for(unsigned int i = 0; i < dbNames.size(); i++)
    {
        // FInd in the ids vector
        it = std::find(ids.begin(),ids.end(),i);

        // If no match, keeps the missing db name
        if(it == ids.end())
            missing.push_back(dbNames[i]);
        else // If match erase and re-find, in case of match, keeps id
        {
            ids.erase(it);
            it = std::find(ids.begin(),ids.end(),i);
            if(it != ids.end())
                repeated.push_back(*it);
        }
    }

    // In case of missing block shows a message box
    if(!missing.empty())
    {
        QString *missingStr = new QString("Hay algunas fichas faltantes:\n\n");
        QMessageBox *msgBox = new QMessageBox;
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->setButtonText(QMessageBox::Ok, tr("Continuar"));
        msgBox->setWindowTitle("Faltan Fichas");
        msgBox->setIcon(QMessageBox::Critical);

        for(unsigned int i = 0; i < missing.size(); i++)
            missingStr->append("Falta: ").append(missing[i]).append("\n");

        msgBox->setText(*missingStr);
        msgBox->exec();
    }

    // In case of repeated block shows a message box
    if(!repeated.empty())
    {
        QString *repeatedStr = new QString("Hay algunas fichas repetidas:\n\n");
        QMessageBox *msgBox = new QMessageBox;
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->setButtonText(QMessageBox::Ok, tr("Continuar"));
        msgBox->setWindowTitle("Fichas Repetidas");
        msgBox->setIcon(QMessageBox::Critical);

        for(unsigned int i = 0; i < repeated.size(); i++)
            repeatedStr->append("Repetida: ").append(dbNames[repeated[i]]).append("\n");

        msgBox->setText(*repeatedStr);
        msgBox->exec();
    }
}

void MainWindow::about()
{
    QMessageBox aboutMsgBox(this);

    aboutMsgBox.setText("Versión 0.2\n\nUniversidad Nacional de Colombia\n\nTécnicas de Inteligencia Artificial\n\nFederico Acosta\nFabián Melo\nHarold Vallejo\n\n2014");
    aboutMsgBox.setButtonText(QMessageBox::Ok,tr("Aceptar"));
    aboutMsgBox.setWindowTitle("Acerca de...");
    aboutMsgBox.setIconPixmap(QPixmap(":/images/images/Universidad_Nacional_de_Colombia_-_Sede_Bogota.png"));
    aboutMsgBox.exec();
}
