Master 1 Informatique - Parcours GIG
Luminy - 2018/2019

Readme du TER - Extraction d'informations de scans LiDAR intérieur

Auteurs : Colas Claudet - Rachel Glaise - Quentin Pepino
Tuteur  : Arnaud Polette 

/****************************************************
 *
 *					INSTALLATION PCL
 *
 ****************************************************/ 
De nombreux problèmes ont été rencontrés durant l'installation, ils ont ainsi donné lieu à l'amélioration d'un fichier bash trouvé sur GitHub.
Le fichier est joint sous le nom de install-pcl.sh.
Pour installer, la commande $sh ./install-pcl.sh peut suffir. Dans certains cas des erreurs peuvent exister dû à la version de cmake, il faut ainsi mettre à jour celui-ci.
Pour d'autres erreurs, il faut se référer à Internet car les erreurs peuvent être très diverses.


/****************************************************
 *
 *						PROJET
 *
 ****************************************************/
Après l'installation de pcl.

Pour compiler 
	- se placer dans le dossier build
	- effectuer $cmake ../src
	- effectuer $make

Pour executer faire 
	- ./ScanLidar

Dans l'application :
	- régler les paramètres
		- threshold permet de choisir le seuil de la distance d'erreur (valeur cohérente : 90)
		- proba permet de choisir la valeur pour la fonction setProba (valeur cohérente : 15)
		- boutons radio :
			- plane : prend un fichier, le débruite, le segmente/filtre puis l'affiche
			- cloud : affiche le nuage de points original
	- choisir un fichier ply (les nuages de points sont dans le dossier clouds)
	- appuyer sur draw

/!\ Remarque :
    - L'affichage par défaut est celui du nuage fragmenté et filtré. Une partie du code est commenté, en la décommentant on peut afficher les nuages débruité, segmenté et final.
    - Les fichiers test_noise_inliers.ply et test_noise_outliers.ply sont des fichiers de démonstration (affichable avec l'option cloud)

    
/****************************************************
 *
 *					PROBLEMES
 *
 ****************************************************/
 Notre application possède quelques problèmes qui obligent l'utilisateur à fermer l'application à partir du terminal lorsque l'on veut lancer un nouveau test après avoir dessiné une première fois.