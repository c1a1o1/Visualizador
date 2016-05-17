#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QVBoxLayout>

#include <vtkMetaImageReader.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkCamera.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	volWidget = new QVTKWidget;

	QVBoxLayout *volLayout = new QVBoxLayout;
	volLayout->setContentsMargins(0, 0, 0, 0);
	volLayout->setSpacing(0);
	volLayout->QLayout::addWidget(volWidget);
	ui->volView->setLayout(volLayout);

	sagitalWidget = new QVTKWidget;

	QVBoxLayout *sagitalLayout = new QVBoxLayout;
	sagitalLayout->setContentsMargins(0, 0, 0, 0);
	sagitalLayout->setSpacing(0);
	sagitalLayout->QLayout::addWidget(sagitalWidget);
	ui->sagitalView->setLayout(sagitalLayout);

	axialWidget = new QVTKWidget;

	QVBoxLayout *axialLayout = new QVBoxLayout;
	axialLayout->setContentsMargins(0, 0, 0, 0);
	axialLayout->setSpacing(0);
	axialLayout->QLayout::addWidget(axialWidget);
	ui->axialView->setLayout(axialLayout);

	coronalWidget = new QVTKWidget;

	QVBoxLayout *coronalLayout = new QVBoxLayout;
	coronalLayout->setContentsMargins(0, 0, 0, 0);
	coronalLayout->setSpacing(0);
	coronalLayout->QLayout::addWidget(coronalWidget);
	ui->coronalView->setLayout(coronalLayout);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openMHD()
{

	std::cout<<"Loading Volume"<<std::endl;

    this->volumeFilename = QFileDialog::getOpenFileName(this, tr("Open Volume .mhd"),
        QDir::currentPath(), tr("Volume Files (*.mhd)"));
    
	vtkSmartPointer<vtkMetaImageReader> reader = vtkSmartPointer<vtkMetaImageReader>::New();
    reader->SetFileName(volumeFilename.toAscii().data());
    reader->Update();

    volumeData = reader->GetOutput();

	this->displayVol(volumeData);
}

void MainWindow::displayVol(vtkSmartPointer<vtkImageData>)
{

	volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();

    vtkSmartPointer<vtkVolumeRayCastCompositeFunction> rayCastFunction =
                vtkSmartPointer<vtkVolumeRayCastCompositeFunction>::New();

    volumeScalarOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
    volumeScalarOpacity->AddPoint(0,1.00);
    volumeScalarOpacity->AddPoint(255,1.00);
    volumeScalarOpacity->Update();
    volumeProperty->SetScalarOpacity(volumeScalarOpacity);

    vtkSmartPointer<vtkColorTransferFunction> volumeColor = vtkSmartPointer<vtkColorTransferFunction>::New();
    volumeColor->AddRGBPoint(0,0.0,0.0,0.0);
    volumeColor->AddRGBPoint(64,0.25,0.25,0.25);
    volumeColor->AddRGBPoint(128,0.5,0.5,0.5);
    volumeColor->AddRGBPoint(192,0.75,0.75,0.75);
    volumeColor->AddRGBPoint(255,1.0,1.0,1.0);
    volumeProperty->SetColor(volumeColor);

    vtkSmartPointer<vtkVolumeRayCastCompositeFunction> compositeFunction =
            vtkSmartPointer<vtkVolumeRayCastCompositeFunction>::New();

    vtkSmartPointer<vtkVolumeRayCastMapper> volumeMapper = vtkSmartPointer<vtkVolumeRayCastMapper>::New();
    volumeMapper->SetVolumeRayCastFunction(compositeFunction);
    volumeMapper->CroppingOff();
    volumeMapper->SetInput(volumeData);

    volume = vtkSmartPointer<vtkVolume>::New();
    volume->SetMapper(volumeMapper);
    volume->SetOrigin(0,0,0);
    volume->SetProperty(volumeProperty);
    volume->Update();

	renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->SetBackground(183.0/255.0,197.0/255.0,253.0/255.0);
    renderer->AddVolume(volume);
    renderer->GetActiveCamera()->SetFocalPoint(volume->GetCenter());
    renderer->GetActiveCamera()->Roll(90);
    
    double cameraPos[3];
    cameraPos[0] = volume->GetCenter()[0] + volume->GetMaxXBound() + 30;
    cameraPos[1] = volume->GetCenter()[1];
    cameraPos[2] = volume->GetCenter()[2];
    
    renderer->GetActiveCamera()->SetPosition(cameraPos);

    renwin = vtkSmartPointer<vtkRenderWindow>::New();
    renwin->AddRenderer(renderer);
    
    volWidget->SetRenderWindow(renwin);
    std::cout<<std::endl;
    std::cout<<"Displaying volume"<<std::endl<<std::endl;
    renwin->Render();
}