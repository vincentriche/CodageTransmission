##### RICHE Vincent   P1203372
## UE-INF2320M Codage, Transmission, Compression D'Image Et De Vidéos 
## TP 2 - Block Sorting

---------------------------------------

## Explications

Ce programme permet de compresser et décompresser un fichier à l'aide de la transformée de _Burrows-Wheeler_, des algorithmes _Move To Front_ et _Run-Length Encoding_. L'égaliseur de probabilité n'a pas été implémenté.  

L'archive contient:
  * data/ : contient les fichiers sur lequel tester le programme.
  * obj/ : contient les fichiers objets.
  * bases.h : contient les structures de données.
  * core.c/.h : BWT, cœur de l'application.
  * mtf.c/.h : l'algorithme d'encodage/décodage MTF.
  * rle.c/.h : l'algorithme d'encodage/décodage RLE.
  * main.c : gestion du pipeline, et des fichiers.
  * Makefile : pour compiler, nettoyer le projet.

Voici à quoi ressemble le pipeline :

    fichier original   --> BWT --> MTF --> RLE --> fichier compressé
            
    fichier compressé  --> RLE Inverse --> MTF Inverse 
                       --> BWT Inverse --> fichier décompressé
                       
J'ai testé le programme sur plusieurs types de fichiers : un fichier contenant 
une suite d'octets, un fichiers .txt contenant du texte,  un programme C et une image en BMP.

Après la décompression, des commandes Unix sont lancées pour vérifier que le contenu du fichier décompressé correspond à l'original et pour comparer la taille du fichier de base et du fichier compressé.

Le TP a été réalisé en C et j'ai utilisé Valgrind pour vérifier la correcte attribution et libération des
allocations mémoires. Un makefile est présent pour compiler le projet.

## Utilisation
Pour lancer le programme, il faut utiliser la commande _./main_ suivi ou non d'un 
nom de fichier (se trouvant dans le dossier _data/_).  
Si un nom de fichiers n'est pas spécifié, le programme utilisera une chaine de 
caractères en dur.  
Sinon le programme créera deux nouveaux fichiers dans data/ :
    _fileXC.Y_ (fichier compressé) et _fileXD.Y_ (fichier décompressé)   
_X_ étant le numéro de fichier, et _Y_ étant l'extension si elle existe.

Exemples de commande : **_make && ./main data/file1_** ou  **_make && ./main_**.

## Tests:

Fichier | Taille non compressée (octets) | Taille compressée (octets) | Ratio
:------------ | :-------------: | :-------------: | -------------:
file1(suite d'octets)		      |	  4053		  |		208	         | 		19.48	
  file2.txt (texte fr)		  |	  182		    | 	  816	       |	  0.22     
file3.c	                             |	  	97			 |	  448			|     0.21
file3.bmp 			             |    	4854       |    128			  | 	 37.92

La compression sur du texte n'est vraiment pas bonne. Il n'y a pas de redondance ou de caractères identiques donc RLE ne fonctionne pas ou très peu.
En revanche sur des images ou des suites d'octets, la compression devient vraiment intéressante. Le fait que l'image utilisée soit noire (donc beaucoup de pixels à 0), et que les pixels soient dans l'ordre font que l'image se compressent très bien. Idem pour la suite d'octets.
