### RICHE Vincent   P1203372
### UE-INF2320M Codage, Transmission, Compression D'Image Et De Vidéos
### TP 2 - Block Sorting
--------------------------------------

## Explications
Ce programme prend un fichier en entrée, le compresse et le décompresse.
Il utilise la transformée de Burrows-Wheeler comme cœur de l'algorithme. 
Ensuite est appliquée l'algorithme Move To Front et pour finir l'algorithme Run-Length Encoding.

Voici à quoi ressemble le pipeline :

    fichier original   --> BWT --> MTF --> RLE --> fichier compressé
            
    fichier compressé  --> RLE Inverse --> MTF Inverse 
                       --> BWT Inverse --> fichier décompressé

Après la décompression, des commandes Unix sont lancées pour vérifier que le contenu du fichier décompressé correspond à l'original et pour comparer la taille du fichier de base et du fichier compressé.

J'ai testé le programme sur plusieurs types de fichiers : un fichier contenant 
une suite d'octets, des fichiers .txt contenant du texte, et un programme C et un fichier objet.

J'ai utilisé Valgrind pour vérifier la correcte attribution et libération des
allocations mémoires.
J'ai utilisé un Makefile pour compiler le projet.

## Utilisation
Pour lancer le projet, il faut utiliser la commande _./main_ suivi ou non d'un 
nom de fichier (se trouvant dans le dossier _data/_).  
Si un nom de fichiers n'est pas spécifié, le programme utilisera une chaine de 
caractères en dur.  
Sinon le programme créera deux nouveaux fichiers dans data/ :
    _fileXC.X_ (fichier compressé) et _fileXD.X_ (fichier décompressé) 

Exemple de commande : **_make && ./main data/file1_**


## Tests:

Fichier | Taille non compressée (octets) | Taille compressée (octets) | Ratio
:------------ | :-------------: | :-------------: | -------------:
file1(suite octets)		      |	2026				 |			                51				   | 			39.725
file2.txt (texte fr)		    |	182				     |				207					         |		      0.879
file3 (texte en)			    |  2477				    |				2530				       |			0.979
file4.c	                             |	97				        |				115					        |		     0.843
file5.o			                     |  3349				  |					1238					|			 2.70

Nous pouvons voir que la compression marche bien sur les suites octets ou les fichiers objets. Sur les fichiers contenant principalement du texte, l'algorithme a moins de travail à faire (beaucoup moins de groupe de 0) donc la compression est moins importante voir pire.