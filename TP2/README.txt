RICHE Vincent   P1203372

UE-INF2320M Codage, Transmission, Compression D'Image Et De Vidéos
TP 2 - Block Sorting

Ce programme prend un fichier en entrée, le compresse et le décompresse.
Il utilise la transformée de Burrows-Wheeler comme coeur de l'algorithme. 
Ensuite est appliquée l'algorithme Move To Front et pour finir Run-Length Encoding.

Voici à quoi ressemble le pipeline :

            fichier original --> BWT --> MTF --> RLE --> fichier compressé
            
            fichier compressé --> RLE Inverse --> MTF Inverse --> BWT Inverse --> fichier décompressé

A la fin, des commandes Unix sont lancées pour vérifier que le fichier décompressé
correspond à l'original et pour comparer la taille des deux.

J'ai testé le programme sur plusieurs types de fichiers : un fichier contenant 
une suite d'octets, des fichiers .txt contenant du texte, et un fichier .c.

Pour retrouver les index lors de la lecture du fichier compressé, j'ai utilisé 
un séparateur. Le problème c'est qu'il y a 2x plus de caractères en sortie, 
ce qui est tout à fait normal.
Dans le cas d'un fichier d'entrée contenant une suite d'octets, le fichier
compressé est nettement moins lourd car RLE fonctionne bien (beaucoup de 0), 
malgré l'ajout des séparateurs.

    --> Il aurait fallu toujours garder les chaines en octets et ne pas les 
    passer en tableau de int (pour MTF et RLE).


J'ai utilisé Valgrind pour vérifier la correcte attribution et libération des
allocations mémoires.
J'ai utilisé un Makefile pour compiler le projet.

Pour lancer le projet, il faut utiliser la commande ./main suivi ou non d'un 
nom de fichier (se trouvant dans le dossier "data/").
Si un nom de fichiers n'est pas spécifié, le programme utilisera une chaine de 
caractères en dur.
Sinon le programme créera deux nouveaux fichiers dans data/ :
    fileXC.X (fichier compressé) et fileXD.X (fichier décompressé) 

Exemple de commande : make && ./main data/file1


Tests:

File1 : fichier contenant une suite d'octets.
Taille original : 3891 octets
Taille compressé : 1972 octets

File2.txt : fichier contenant du 4 lignes de texte
Taille original : 182 octets
Taille compressé : 502 octets

File3 : fichier contenant du 40 lignes de texte latin/anglais
Taille original : 2477 octets
Taille compressé : 6381 octets

File4.c : fichier contenant un main avec un printf hello world en langage C
Taille original : 97 octets
Taille compressé : 266 octets

Sur les fichiers, la compression ne marche pas car j'utilise un séparateur
pour retrouver les index lors de la décompression. Il y a donc énormément de 
caractères ajoutés donc le poids augmente.
De plus, les fichiers 2, 3 et 4, le nombre de 0 est limité contrairement 
au fichier 1 qui est une suite d'octets (donc beaucoup de 0). Le fichier 1 se 
compresse donc quand même bien.