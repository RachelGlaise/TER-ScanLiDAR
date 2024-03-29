#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <chrono>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

/********** INCLUDE PCL *********************/

#include <iostream>
#include <thread>

#include <pcl/point_types.h>
#include <pcl/features/normal_3d.h>
#include <pcl/io/ply_io.h>

#include <pcl/console/parse.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/sample_consensus/ransac.h>
#include <pcl/sample_consensus/sac_model_plane.h>
#include <pcl/sample_consensus/sac_model_sphere.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/segmentation/plane_coefficient_comparator.h>
#include <pcl/filters/statistical_outlier_removal.h>
#include <pcl/segmentation/plane_coefficient_comparator.h>

#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/features/normal_3d.h>
#include <pcl/surface/gp3.h>

#include <pcl/io/vtk_io.h>

#include <pcl/ModelCoefficients.h>
#include <pcl/common/common.h>
#include <pcl/common/distances.h>
//#include <pcl/common/impl/intersections.hpp>
//#include <pcl/common/impl/intersections.hpp>
/********** FIN INCLUDE PCL *********************/

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void showHelp(char *program_name);
    /**********************
      VISUALISATION
    **********************/
    pcl::visualization::PCLVisualizer::Ptr simpleVis(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud);
    pcl::visualization::PCLVisualizer::Ptr simpleVisCol(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> single_color);
    pcl::visualization::PCLVisualizer::Ptr addVisualiser();
    pcl::visualization::PCLVisualizer::Ptr finaliseVis(pcl::visualization::PCLVisualizer::Ptr viewer);
    pcl::visualization::PCLVisualizer::Ptr addPtsCloudColor(pcl::visualization::PCLVisualizer::Ptr viewer, pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> single_color);
    pcl::visualization::PCLVisualizer::Ptr addPtsCloud (pcl::visualization::PCLVisualizer::Ptr viewer, pcl::PointCloud<pcl::PointNormal>::Ptr cloud);
    /**********************
          DEBRUITAGE
    **********************/
    void denoise(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud);
    /**********************
          MODELISATION
    **********************/
    pcl::visualization::PCLVisualizer::Ptr addPlane (pcl::visualization::PCLVisualizer::Ptr viewer, pcl::ModelCoefficients planeCoef);
    pcl::PointXYZ threePlaneIntersection(pcl::ModelCoefficients plane_coeff1, pcl::ModelCoefficients plane_coeff2, pcl::ModelCoefficients plane_coeff3);
    /**********************
           OBSOLETE
    **********************/
    pcl::PointCloud<pcl::PointXYZ>::Ptr regroup_plane(); //not use
    
     
public slots:
    void chooseViewCloud();
    void chooseViewPlane();
    void changeThreshold(int th);
    void changeProba(int proba);
    void chooseFile();
    void draw();
    
    void saveCloud(); //not use

private:
    Ui::MainWindow *ui;
    float threshold = 20.0f;
    float proba = 0.05f;
    bool view_plan = true;	
	bool have_plane = false;

	std::vector<pcl::PointCloud<pcl::PointXYZ>::Ptr> vector_cloud;
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloudToSave;
	std::vector<pcl::ModelCoefficients> vector_eq;

    QString file = "";
    
    int nb_cloud = 0;
    int nb_plane = 0;
};

#endif // MAINWINDOW_H
