//Create with the help of http://pointclouds.org/documentation/tutorials/random_sample_consensus.php

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	connect(ui->radio_cloud, SIGNAL(clicked()), this, SLOT(chooseViewCloud())); //si cloud radio button sélectionné
	connect(ui->radio_plane, SIGNAL(clicked()), this, SLOT(chooseViewPlane())); //si plane radio button selectionné
	connect(ui->slider_threshold, SIGNAL(valueChanged(int)), this, SLOT(changeThreshold(int))); //connexion slidebar threshold
	connect(ui->slider_proba, SIGNAL(valueChanged(int)), this, SLOT(changeProba(int))); //connexion slidebar proba
	connect(ui->btn_import, SIGNAL(clicked()), this, SLOT(chooseFile())); //connexion selection de fichier
	connect(ui->btn_draw, SIGNAL(clicked()), this, SLOT(draw())); //connexion bouton draw, on lance le viewer
	//save
    //connect(ui->btn_save, SIGNAL(clicked()), this, SLOT(saveCloud()));

}

MainWindow::~MainWindow()
{
	delete ui;
}

/**********************************
*
*          PCL
*
**********************************/
/**********************
		OBSOLETE
**********************/
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
//ajout des plans récupérés au vecteur
//n'est pas utilisée
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
pcl::PointCloud<pcl::PointXYZ>::Ptr MainWindow::regroup_plane()
{
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_tmp (new pcl::PointCloud<pcl::PointXYZ>);
	
	for(int j = 0; j < vector_cloud.size(); j++)
	{
		for(int i=0 ; i < vector_cloud.at(j)->size(); i++)
		{
			cloud_tmp->push_back(vector_cloud.at(j)->at(i));
		}
	}
	return cloud_tmp;
}

//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
//SLOT - sauvegarde du nuage de point dans un fichier ply
//n'est pas utilisée
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-

void MainWindow::saveCloud(){
	QString fileName;
	fileName = QFileDialog::getSaveFileName(this,
	tr("Sauvegarder votre image"), "cloud_plane.ply",
	tr("Polygon File Format(.ply);;Tous les fichiers()"));
	if(this->cloudToSave->size() == 0){
		QMessageBox msgBox;
		msgBox.setText("The cloud is empty. Save impossible.");
		msgBox.exec();
	}
	pcl::PLYWriter writer;
	//writer.write<pcl::PointXYZ> (fileName.toStdString(), *this->cloudToSave, false);
	writer.write<pcl::PointXYZ> (fileName.toStdString(), *regroup_plane(), false);
}
/**********************
     FIN OBSOLETE
**********************/
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
//aide si mauvais arguments
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
void showHelp(QString program_name)
{
	qDebug() << "Usage: " << program_name << " cloud_filename.[ply]";
	qDebug() << "-h:  Show this help.";
}
/**********************
      VISUALISATION
**********************/
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
//création d'un viewer affichant un nuage de point passé en paramètre avec fond noir et points blancs
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-

pcl::visualization::PCLVisualizer::Ptr MainWindow::simpleVis (pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud)
{
	// --------------------------------------------
	// -----Open 3D viewer and add point cloud-----
	// --------------------------------------------
	pcl::visualization::PCLVisualizer::Ptr viewer (new pcl::visualization::PCLVisualizer ("3D Viewer"));
	viewer->setBackgroundColor (0, 0, 0);
	viewer->addPointCloud<pcl::PointXYZ> (cloud, "sample cloud");
	viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, "sample cloud");
	//viewer->addCoordinateSystem (1.0, "global");
	viewer->initCameraParameters ();
	return (viewer);
}

//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
//création d'un viewer affichant un nuage de point passé en paramètre avec fond noir et couleur des points passées en paramètre
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-

pcl::visualization::PCLVisualizer::Ptr MainWindow::simpleVisCol (pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ>
single_color)
{
	// -----------------------------------------------
	// -----Open 3D viewer and add color point cloud--
	// -----------------------------------------------
	pcl::visualization::PCLVisualizer::Ptr viewer (new pcl::visualization::PCLVisualizer ("3D Viewer"));
	viewer->setBackgroundColor (0, 0, 0);
	//viewer->addPointCloud<pcl::PointXYZ> (cloud, "sample cloud");
	viewer->addPointCloud<pcl::PointXYZ> (cloud, single_color, "sample cloud");
	viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, "sample cloud");

	//viewer->addCoordinateSystem (1.0, "global");
	viewer->initCameraParameters ();
	return (viewer);
}

//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
//construction et initialisation d'un viewer
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-

pcl::visualization::PCLVisualizer::Ptr MainWindow::addVisualiser()
{
	// ---------------------
	// -----set 3D viewer --
	// ---------------------
	pcl::visualization::PCLVisualizer::Ptr viewer (new pcl::visualization::PCLVisualizer ("3D Viewer"));
	viewer->setBackgroundColor (0, 0, 0);

	return (viewer);
}

//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
//Finalise la construction du viewer après l'ajout de nuages de points par addPtsCloudColor
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-

pcl::visualization::PCLVisualizer::Ptr MainWindow::finaliseVis (pcl::visualization::PCLVisualizer::Ptr viewer)
{
	for(int i=0;i<this->nb_cloud;i++)
		viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, "sample cloud"+i);

	//viewer->addCoordinateSystem (1.0, "global");
	viewer->initCameraParameters ();
	return (viewer);
}

//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
//ajoute un nuage de points au viewer avec une couleur donnée
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-

pcl::visualization::PCLVisualizer::Ptr MainWindow::addPtsCloudColor (pcl::visualization::PCLVisualizer::Ptr viewer, pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ>
single_color)
{
	// -----------------------------------------------
	// -----Open 3D viewer and add color point cloud--
	// -----------------------------------------------

	viewer->addPointCloud<pcl::PointXYZ> (cloud, single_color, "sample cloud"+this->nb_cloud);
	this->nb_cloud = this->nb_cloud +1;
	return (viewer);
}

pcl::visualization::PCLVisualizer::Ptr MainWindow::addPtsCloud (pcl::visualization::PCLVisualizer::Ptr viewer, pcl::PointCloud<pcl::PointNormal>::Ptr cloud)
{
	// -----------------------------------------------
	// -----Open 3D viewer and add color point cloud--
	// -----------------------------------------------

	//viewer->addPointCloud<pcl::PointXYZ> (cloud, "sample cloud"+this->nb_cloud);
	this->nb_cloud = this->nb_cloud +1;
	return (viewer);
}
/**********************
   FIN VISUALISATION
**********************/
/**********************
      DEBRUITAGE
**********************/
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
//Débruitage d'un nuage de points
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-

void MainWindow::denoise(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud){
	std::cerr << "denoising running " << std::endl;

	//creer nuage inliers
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_inliers (new pcl::PointCloud<pcl::PointXYZ>);
	// creer nuage outliers
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_outliers (new pcl::PointCloud<pcl::PointXYZ>);

	std::cerr << "Cloud before filtering: " << std::endl;
	std::cerr << *cloud << std::endl;

	// creer cloud pour futur nuage filtré
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered (new pcl::PointCloud<pcl::PointXYZ>); 

	// Create the filtering object
	pcl::StatisticalOutlierRemoval<pcl::PointXYZ> sor;
	sor.setInputCloud (cloud);
	sor.setMeanK (10); //nombre de plus proche voisins
	sor.setStddevMulThresh (0.3);

	sor.filter (*cloud_filtered);

	std::cerr << "Cloud after filtering: " << std::endl;
	std::cerr << *cloud_filtered << std::endl;

	// les données restantes (inliers) sont ecrites sur le disque pour une utilisation ulterieure
	pcl::PLYWriter writer;
	writer.write<pcl::PointXYZ> ("../clouds/test_noise_inliers.ply", *cloud_filtered, false);

	//décommenter pour avoir plusieurs viewer affichant plusieurs débruitages
	
	/*//Affichage de toutes les vues avant/apres debruitage (bruit, inliers, base)
	// outliers
	sor.setNegative (true);
	sor.filter (*cloud_filtered);
	writer.write<pcl::PointXYZ> ("../clouds/test_noise_outliers.ply", *cloud_filtered, false);

	//creer viewer + cloud inliers
	pcl::visualization::PCLVisualizer::Ptr ViewerNoise_inliers (new pcl::visualization::PCLVisualizer ("Viewer_noise_inliers"));
	ViewerNoise_inliers->setBackgroundColor (0, 0, 0);
	pcl::io::loadPLYFile ("../clouds/test_noise_inliers.ply", *cloud_inliers);
	// couleur nuage de points
	pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> single_color_gen_inliers (cloud_inliers,255,255,255);
	addPtsCloudColor(ViewerNoise_inliers,cloud_inliers,single_color_gen_inliers);

	//creer viewer + cloud outliers
	pcl::visualization::PCLVisualizer::Ptr ViewerNoise_outliers (new pcl::visualization::PCLVisualizer ("Viewer_noise_outliers"));
	ViewerNoise_outliers->setBackgroundColor (0, 0, 0);
	pcl::io::loadPLYFile ("../clouds/test_noise_outliers.ply", *cloud_outliers);
	// couleur nuage de points
	pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ>
	single_color_gen_outliers (cloud_outliers,255,255,255);
	addPtsCloudColor(ViewerNoise_outliers,cloud_outliers,single_color_gen_outliers);

	//creer viewer + cloud filtered
	pcl::visualization::PCLVisualizer::Ptr ViewerNoise_filtreted (new pcl::visualization::PCLVisualizer ("Viewer filtreted"));
	ViewerNoise_outliers->setBackgroundColor (0, 0, 0);
	// couleur nuage de points
	pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ>
	single_color_gen_filtered (cloud_filtered,255,255,255);
	addPtsCloudColor(ViewerNoise_filtreted,cloud_filtered,single_color_gen_filtered);*/
	//endViewQuentin

	std::cerr << "Cloud filtering: " << std::endl;
	std::cerr << *cloud_filtered << std::endl;

	std::cerr << "Cloud outliers: " << std::endl;
	std::cerr << *cloud_outliers << std::endl;

	std::cerr << "denoising end " << std::endl;
}
/**********************
    FIN DEBRUITAGE
**********************/
/**********************
      MODELISATION
**********************/
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
//Ajout d'un plan au viewer avec en paramètre une équation de plan
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-

pcl::visualization::PCLVisualizer::Ptr  MainWindow::addPlane(pcl::visualization::PCLVisualizer::Ptr viewer, pcl::ModelCoefficients planeCoef)
{
	this->nb_plane = this->nb_plane +1;
	viewer->addPlane(planeCoef, planeCoef.values[0],planeCoef.values[1], planeCoef.values[2], "plane"+this->nb_plane,0);
	//viewer->addPlane(planeCoef);
	return (viewer);
}

//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
//resous un systeme de 3 equations a 3 inconnus pour ressortir le point d'intersection de 3 plans
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-

pcl::PointXYZ MainWindow::threePlaneIntersection(pcl::ModelCoefficients plane_coeff1, pcl::ModelCoefficients plane_coeff2, pcl::ModelCoefficients plane_coeff3)
{
	double matrice[3][4];
	double coefficient,x,y,z;
	int i;
	int j;
	i=0;
	j=0;
	std::cout << "Resolution d'un systeme de 3 equations a trois inconnues\n";
	std::cout << "Premiere equation, entrez en ordre respectif x,y,z et la constante\n";
	//cin >> matrice[0][0]>>matrice[0][1]>>matrice[0][2]>>matrice[0][3];
	matrice[0][0] = plane_coeff1.values[0];
	matrice[0][1] = plane_coeff1.values[1];
	matrice[0][2] = plane_coeff1.values[2];
	matrice[0][3] = plane_coeff1.values[3];
	//cout << "Seconde equation, entrez en ordre respectif x,y,z et la constante\n";
	//cin >> matrice[1][0]>>matrice[1][1]>>matrice[1][2]>>matrice[1][3];
	matrice[1][0] = plane_coeff2.values[0];
	matrice[1][1] = plane_coeff2.values[1];
	matrice[1][2] = plane_coeff2.values[2];
	matrice[1][3] = plane_coeff2.values[3];
	//cout << "Premiere equation, entrez en ordre respectif x,y,z et la constante\n";
	//cin >> matrice[2][0]>>matrice[2][1]>>matrice[2][2]>>matrice[2][3];
	matrice[2][0] = plane_coeff3.values[0];
	matrice[2][1] = plane_coeff3.values[1];
	matrice[2][2] = plane_coeff3.values[2];
	matrice[2][3] = plane_coeff3.values[3];
	coefficient=(-1.0*matrice[1][0]/matrice[0][0]);
	i=0;
	for(int p = 0; p<3;p++)
		for(int l = 0; l<4; l++)
			std::cout<<"matrice : "<<matrice[p][l]<<endl;
	for(i;i<=3;i++)
	{
		matrice[1][i]=(coefficient*matrice[0][i])+matrice[1][i];               
	}
	coefficient=(-1.0*matrice[2][0]/matrice[0][0]);
	i=0;
	for(i;i<=3;i++)
	{
		matrice[2][i]=(coefficient*matrice[0][i])+matrice[2][i];               
	}
	coefficient=(-1.0*matrice[2][1]/matrice[1][1]);
	i=1;
	for(i;i<=3;i++)
	{
		matrice[2][i]=(coefficient*matrice[1][i])+matrice[2][i];               
	}
	z=matrice[2][3]/matrice[2][2];
	y=(matrice[1][3]-(matrice[1][2]*z))/matrice[1][1];
	x=(matrice[0][3]-((matrice[0][1]*y)+(matrice[0][2]*z)))/matrice[0][0];
	std::cout << "X est egal a " << x << "\n";
	std::cout << "Y est egal a " << y << "\n";
	std::cout << "Z est egal a " << z << "\n";
	pcl::PointXYZ point(x,y,z);
	//system("PAUSE");
	return point;
}
/**********************
    FIN MODELISATION
**********************/
/*********************************
*
*			SLOTS
*
*********************************/

//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
//affichage du nuage de point original
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-

void MainWindow::chooseViewCloud(){
	this->view_plan = false;
}

//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
// affichage du plan après les différents traitements/calculs/modélisations
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-

void MainWindow::chooseViewPlane(){
	this->view_plan = true;
}

//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
//changement de la valeur du slider threshold
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-

void MainWindow::changeThreshold(int th){
	this->threshold = th * 1.0f;
}

//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
//changement de la valeur du slider proba 
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-

void MainWindow::changeProba(int proba){
	this->proba = proba / 100.0f;
}

//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
//sélection du fichier ply pour le traitement
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-

void MainWindow::chooseFile(){
	this->file = QFileDialog::getOpenFileName(this,
	tr("Ouvrir fichier"), "",
	tr("Polygon File Format(*.ply);;"
	"Tous les fichiers(*)"));
}

//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
//fonction mère , appelle les autres fonctions et gère l'affichage
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-

void MainWindow::draw(){
	/* initialize random seed: */
	srand (time(NULL));

	qDebug() << "draw";
	std::cout << "proba " << this->proba << std::endl;
	std::cout << "threshold " << this->threshold << std::endl;

	// initialize PointClouds
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr final (new pcl::PointCloud<pcl::PointXYZ>);
	pcl::visualization::PCLVisualizer::Ptr viewer;
	//-------------load cloud
	std::vector<int> filenames;
	bool file_is_pcd = false;
	
	if (this->file == ""){
		std::cout << "Aucun fichier choisi." << std::endl;
		return;
	}
	if (file_is_pcd) { //not use
		if (pcl::io::loadPCDFile (this->file.toStdString(), *cloud) < 0)  {
			qDebug() << "Error loading point cloud " << this->file;
			return;
		}
	} else {
		if (pcl::io::loadPLYFile (this->file.toStdString(), *cloud) < 0)  {
			qDebug() << "Error loading point cloud " << this->file;
			return;
		}
	}
	//-- end load cloud
	
	/*//Affichage utilisé pour la comparaison 
	pcl::visualization::PCLVisualizer::Ptr Viewer_cloud (new pcl::visualization::PCLVisualizer ("Viewer_cloud"));
	Viewer_cloud->setBackgroundColor (0, 0, 0);
	
	// couleur nuage de points
	pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> single_color_cloud (cloud,255,255,255);
	addPtsCloudColor(Viewer_cloud,cloud,single_color_cloud);
	//End Affichage utilisé pour la comparaison*/
	
	denoise( cloud); //on enlève le bruit du nuage de points
	int maxToDelete = 0.02*cloud->size(); //le minimum de points que doit comporter un nuage de points pour être conservé
	pcl::io::loadPLYFile ("./test_noise_inliers.ply", *cloud); //récupération du fichier dans lequel il y a le nuage de point débruité
	//mettre le nuage de point en couleur et creer un viewer
	pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ>
	single_color (cloud, 0, 0, 255); //initialisation d'une couleur bleue pour les points
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_cpy = cloud; 
	viewer = addVisualiser(); //initialisation du visualiseur

	int j = 0;
	while(cloud->size()>100 && this->view_plan) //on récupère tout les plans tant qu'il reste plus de 100 pts dans le nuage de point original
	{
		j++;
		//std::cout<<cloud->size()<<endl; //affichage de la taille du nuage
		pcl::ExtractIndices<pcl::PointXYZ> extract; //vecteur qui contiendra les points des plans à extraire du nuage original
		std::vector<int> inliers; //vecteur qui contiendra les points des plans
		pcl::PointIndices::Ptr suppression_inliers(new pcl::PointIndices()); //necessaire pour la suppression des points du nuage original

		// created RandomSampleConsensus object and compute the appropriated model
		// ======= RANSAC ============
		pcl::SampleConsensusModelPlane<pcl::PointXYZ>::Ptr
		model_p (new pcl::SampleConsensusModelPlane<pcl::PointXYZ> (cloud)); //création d'un modèle pour la récupération de points formant un plan
		if(this->view_plan) // si le bouton radio plan a été selectionné
		{
			std::cout << "Appel de ransac" << endl;
			pcl::RandomSampleConsensus<pcl::PointXYZ> ransac (model_p);
			//std::cout << "Plan 1" << endl;
			ransac.setDistanceThreshold (this->threshold);
			ransac.setProbability(this->proba);

			//std::cout << "Plan 2" << endl;
			ransac.computeModel();
			ransac.getInliers(inliers);
		}
		// ============ FIN RANSAC ====================

		//std::cout << "Copy" << endl;
		// copies all inliers of the model computed to another PointCloud
		pcl::copyPointCloud<pcl::PointXYZ>(*cloud, inliers, *final);

		for(int i = 0; i < inliers.size(); i++) 
		{
			suppression_inliers->indices.push_back(inliers.at(i)); // mettre notre inliers dans un autre inlier (suppr_inlier) (->les 2 inliers ne sont pas de meme type et on a besoin des 2 pour nos fonctions)
		}

		// creates the visualization object and adds either our original cloud or all of the inliers
		// depending on the command line arguments specified.
		//std::cout << "Viewer" << endl;
		
		//récupération des équations des plans
		if(final->size() > maxToDelete) //si la taille du plan est suppérieure à un pourcentage de la taille du nuage de point global donné
			{
			std::vector<float> list_coeff; //liste des coefs de l'équation du plan
			std::vector<float> sum; //vector d'addition des coefs pour en faire une moyenne 
			for(int i = 0; i < 4; ++i)
				sum.push_back(0.0); //init sum

			int nb_iter_for_avg = 1; //nombre de fois qu'on itere pour faire la moyenne des equations des plans
			
			for(int i= 0; i < nb_iter_for_avg; ++i){
				pcl::PointCloud<pcl::PointXYZ>::Ptr clicked_points_3d (final); //  creer nuage (copie de final) 
				Eigen::VectorXf ground_coeffs;
				ground_coeffs.resize(4); // coefficient du plan
				std::vector<int> clicked_points_indices; //indices des points du plan  choisis random
				
				int a = 0; //indice premier point
				int b = 0; // ...second point
				int c = 0; //... troisième point
				// creer des points randoms
				// on sélectionne trois points random dans notre plan
				while(a==b || b==c || a==c)
				{
					a = (rand() % final->size()) ;
					b = (rand() % final->size()) ;
					c = (rand() % final->size()) ;
				}
				std::cout<<"A "<<a<<"B " <<b<<"C "<<c<<endl;
				// les mettre dans clicked_point_indices
				//a = 2;
				//b = 8;
				//c = 4;
				clicked_points_indices.push_back(a);
				clicked_points_indices.push_back(b);
				clicked_points_indices.push_back(c);

				//pcl::SampleConsensusModelPlane<pcl::PointXYZ> model_plane(clicked_points_3d);
				pcl::SampleConsensusModelPlane<pcl::PointXYZ> model_plane(final); 
				model_plane.computeModelCoefficients(clicked_points_indices,ground_coeffs);// calcul plan a partir des indices des points
				std::cout << "Ground plane: " << ground_coeffs(0)
						<< " " << ground_coeffs(1) << " " << ground_coeffs(2)
						<< " " << ground_coeffs(3) << std::endl;
				
				// faire la somme des coeffs pour chaque boucle effectuée
				sum[0] += ground_coeffs(0);
				sum[1] += ground_coeffs(1);
				sum[2] += ground_coeffs(2);
				sum[3] += ground_coeffs(3);

			}
			//calcul de la moyenne pour chaque valeur de l equation
			int iter_eq = 4;
			for(int i = 0; i < iter_eq; ++i)
				list_coeff.push_back(sum[i]);
				//list_coeff.push_back(sum[i]/nb_iter_for_avg);

			std::cout << "plane : " << list_coeff[0]
			<< " " << list_coeff[1] << " " << list_coeff[2]
			<< " " << list_coeff[3] << std::endl;
			//for (unsigned int i = 0; i < clicked_points_3d->points.size(); i++)
			pcl::ModelCoefficients plane_coeff;
			plane_coeff.values.resize (4);
			plane_coeff.values[0] = list_coeff[0];
			plane_coeff.values[1] = list_coeff[1];
			plane_coeff.values[2] = list_coeff[2];
			plane_coeff.values[3] = list_coeff[3];
			
			addPlane(viewer,plane_coeff); //modélisation d'un plan à partir de l'équation de plan récupérée
			
			vector_eq.push_back(plane_coeff); //on stocke l'équation dans un vecteur pour l'étape dite du maillage
			//pcl::threePlanesIntersection();
			
			
			// Ici nous avons tenté de modéliser des plans à l'aide des normals des plans récupérés
			/*
			pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal> n;
			pcl::PointCloud<pcl::Normal>::Ptr normals (new pcl::PointCloud<pcl::Normal>);
			pcl::search::KdTree<pcl::PointXYZ>::Ptr tree (new pcl::search::KdTree<pcl::PointXYZ>);
			tree->setInputCloud (final);
			n.setInputCloud (final);
			n.setSearchMethod (tree);
			n.setKSearch (20);
			n.compute (*normals);
			//* normals should not contain the point normals + surface curvatures

			// Concatenate the XYZ and normal fields*
			
			pcl::PointCloud<pcl::PointNormal>::Ptr cloud_with_normals (new pcl::PointCloud<pcl::PointNormal>);
			pcl::concatenateFields (*final, *normals, *cloud_with_normals);
			//* cloud_with_normals = cloud + normals

			// Create search tree*
			pcl::search::KdTree<pcl::PointNormal>::Ptr tree2 (new pcl::search::KdTree<pcl::PointNormal>);
			tree2->setInputCloud (cloud_with_normals);
			// Initialize objects
			pcl::GreedyProjectionTriangulation<pcl::PointNormal> gp3;
			pcl::PolygonMesh triangles;

			// Set the maximum distance between connected points (maximum edge length)
			gp3.setSearchRadius (0.025);

			// Set typical values for the parameters
			gp3.setMu (2.5);
			gp3.setMaximumNearestNeighbors (100);
			gp3.setMaximumSurfaceAngle(M_PI/4); // 45 degrees
			gp3.setMinimumAngle(M_PI/18); // 10 degrees
			gp3.setMaximumAngle(2*M_PI/3); // 120 degrees
			gp3.setNormalConsistency(false);

			// Get result
			gp3.setInputCloud (cloud_with_normals);
			gp3.setSearchMethod (tree2);
			gp3.reconstruct (triangles);

			// Additional vertex information
			std::vector<int> parts = gp3.getPartIDs();
			std::vector<int> states = gp3.getPointStates();
			//end_new POMDETER _______________________________________________________________________________________________________________________________
			addPtsCloud(viewer,cloud_with_normals);*/
			//pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> single_color (cloud, 255, 0, 0);
			//viewer->addPointCloud<pcl::PointXYZ> (cloud, single_color, "sample cloud"); //nope
			//viewer = simpleVis(final);

			//viewer = simpleVisCol(final, single_color);
			pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ>
			single_color_gen (final, (j*100)%255, (j*20)%255, (j*30)%255); //la couleur de chaque nuage de plan ransac est différente
			//single_color_gen (cloud_with_normals, (j*100)%255, (j*20)%255, (j*30)%255)
			//viewer = addPtsCloudColor(viewer,cloud,single_color_gen);
			addPtsCloudColor(viewer,final,single_color_gen); //ajout du nuage de plan ransac au viewer
			//addPtsCloudColor(viewer,cloud_with_normals,single_color_gen);
			vector_cloud.push_back(final); //on ajoute le nuage de plan ransac au vecteur mais inutilisé
			//saveVTKFile ("mesh.vtk", triangles);
			//vector_cloud.push_back(cloud_with_normals);
		}
		//ici on supprime du nuage de point original le plan trouvé par ransac 
		// comme ça lors de la prochaine itération, ransac sélectionne un autre plan 
		extract.setInputCloud(cloud); 
		extract.setIndices(suppression_inliers);
		extract.setNegative(true);
		extract.filter(*cloud);

		have_plane = true; //il y a au moins un plan trouvé par ransac
	
		//no_plane = true;
		//viewer = simpleVis(cloud);
	
	}
	if (!have_plane) // si ransac n'a pas trouvé de points formant un plan
	{
		viewer = simpleVis(cloud);
	}
	//dans cette triple boucle on modélise chaque point ou 3 équations de plans se rencontrent parmis tout les plans trouvés
	for(int k=0 ; k < vector_eq.size() ; k++)
	{
		for(int c = 1; c< vector_eq.size() ; c++)
		{
			for(int d = 2; d < vector_eq.size() ; d++)
			{
				std::cout<<"three_planes_inter"<<threePlaneIntersection(vector_eq.at(k),vector_eq.at(c),vector_eq.at(d))<<endl;
				std::cout<<"vector eq : "<<vector_eq.at(k)<<vector_eq.at(c)<<vector_eq.at(d)<<endl;

				pcl::PointCloud<pcl::PointXYZ>::Ptr myPoint (new pcl::PointCloud<pcl::PointXYZ>);
				pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> single_color (myPoint, 240, 195, 0);
				myPoint->push_back(threePlaneIntersection(vector_eq.at(k),vector_eq.at(c),vector_eq.at(d)));
				addPtsCloudColor(viewer,myPoint,single_color);
			}
		}
		
	}
	//cout << "nb plan :" << this->nb_cloud << endl;
	finaliseVis(viewer);
	while (!viewer->wasStopped ())
	{
		viewer->spinOnce (100);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

}