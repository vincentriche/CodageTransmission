#!/bin/sh

echo "Pour voir la page générée, ouvrir l'URL :"
echo "file://`pwd`/xxx.html"
echo "N'oubliez pas de faire le ménage (make clean) après avoir vu la page"

make tests

export F=DONNEES/spiderman.raw
export NBE=128
export QUALITE=1
export SHANNON=0

cat >xxx.html <<EOF
﻿<HTML>
<HEAD>
<TITLE>Son</TITLE>
</HEAD>
<BODY>
<H1>Son</H1>

Extrait du son original (`wc -c <$F` octets)<BR>
<IMG SRC="xxx.1.gif">`./affiche_son <$F | ppmtogif 2>/dev/null >xxx.1.gif`

<P>
Le son après DCT :<BR>

<IMG SRC="xxx.2.gif">`./dct <$F >xxx.1 ; ./affiche_dct <xxx.1 | ppmtogif 2>/dev/null >xxx.2.gif`

<P>
Le son après DCT et PSYCHO :<BR>
<IMG SRC="xxx.3.gif">`./psycho <xxx.1 >xxx.2 ; ./affiche_dct <xxx.2 | ppmtogif 2>/dev/null >xxx.3.gif`
<P>

<P>
Compression d'image : dct | psycho | rle<BR>
Décompression : rleinv | dctinv<BR>
Pour vérification le son original et le son restitué
avec une qualité de 1.<BR>
<IMG SRC="xxx.1.gif">
<IMG SRC="xxx.4.gif">`./dctinv <xxx.2 >xxx.au ; ./affiche_son <xxx.au | ppmtogif 2>/dev/null >xxx.4.gif`

<P>
Taille du son original : `wc -c <$F` octets,
taille du son comprimé : `./rle <xxx.2 | wc -c` octets.
Vous pouvez <A HREF="xxx.au">écouter le son</A>.
Si le lien ne fonctionne pas, tapez <STRONG><TT>./joue xxx.au</TT></STRONG>
<P>
Si l'on utilise shannon fano dynamique la taille
est de `export SHANNON=1 ; ./rle <xxx.2 | wc -c` octets.
</BODY>
</HTML>
EOF

x-www-browser "file://`pwd`/xxx.html"
