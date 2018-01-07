RICHE Vincent   P1203372

UE-INF2320M Codage, Transmission, Compression D'Image Et De Vidéos
TP 2 - Block Sorting

Ce programme prend un fichier en entrée, le compresse et le décompresse.
Il utilise la transformée de Burrows-Wheeler comme cœur de l'algorithme. 
Ensuite est appliquée l'algorithme Move To Front et pour finir Run-Length Encoding.

Voici à quoi ressemble le pipeline :

            fichier original --> BWT --> MTF --> RLE --> fichier compresse

            fichier compresse --> RLE Inverse --> MTF Inverse --> BWT Inverse --> fichier décompressé

A la fin, des commandes Unix sont lancées pour vérifier que le fichier décompressé
correspond à l'original et pour comparer la taille des deux.

J'ai testé le programme sur plusieurs types de fichiers : un fichier contenant 
une suite d'octets, un fichier .txt contenant du texte, et un fichier.c.

Pour retrouver les index lors de la lecture du fichier compressé, j'ai utilisé 
un séparateur. Le problème c'est qu'il y a 2x plus de caractères en sortie, 
ce qui est tout à fait normal.
Dans le cas d'un fichier d'entrée contenant une suite d'octets, le fichier
compressé est nettement moins lourd car RLE fonctionne bien (beaucoup de 0).

    --> Il aurait fallu écrire le fichier en binaire et utiliser les 
        fonctions (fread() et fwrite()).


J'ai utilisé Valgrind pour vérifier la correcte attribution et libérations des
allocations mémoires.
J'ai utilisé un Makefile pour compiler le projet. Vous pouvez make et make clean.

Pour lancer le projet, il faut utiliser la commande ./main suivit ou non d'un 
nom de fichier (se trouvant dans le dossier "data/").
Si un nom de fichiers n'est pas spécifié, le programme utilisera une chaine de 
caractères en dur.
Sinon le programme créera deux nouveaux fichiers dans data/ :
    fileXC.X (fichier compressé) et fileXD.X (fichier décompressé) 

Exemple de commande : make && ./main data/file1
