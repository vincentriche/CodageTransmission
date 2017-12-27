#!/bin/sh

make tests

echo "Pour voir la page générée, ouvrir l'URL :"
echo "file://`pwd`/xxx.html"
echo "N'oubliez pas de faire le ménage (make clean) après avoir vu la page"

export I=DONNEES/bat710.pgm
export SAUTE_ENTETE=1
export NBE=8

cat >xxx.html <<%
﻿<HTML>
<HEAD>
<TITLE>Statistiques sur la compression</TITLE>
</HEAD>
<BODY>
<H1>Statistiques sur la compression</H1>
<P>
Les images sur cette page sont en GIF. Comme l'image a 256 niveaux
de gris la conversion en GIF ne réduit pas les couleurs.
Elle est donc sans pertes.

<TABLE>

<TR>
<TD><IMG SRC="xxx.orig.gif">
`ppmtogif <$I >xxx.orig.gif 2>/dev/null`
<TD>
Image originale<BR>
$I<BR>
`wc -c <$I` octets

<TR>
<TD><IMG SRC="xxx.1.gif">
<TD>
NBE=8<BR>
./imagedct | ./imagedctinv
`./imagedct <$I | ./imagedctinv | ppmtogif >xxx.1.gif 2>/dev/null`

<TR>
<TD><IMG SRC="xxx.2.gif">
<TD>
NBE=8<BR>
./imagedct | <B>./rle | ./rleinv</B> | imagedctinv<BR>
`./imagedct <$I | ./rle | tee xxx | wc -c
./rleinv <xxx | ./imagedctinv | ppmtogif >xxx.2.gif 2>/dev/null`
octets


<TR>
<TD><IMG SRC="xxx.3.gif">
<TD>
L'image et la taille doivent être identiques à la précédente.<BR>
NBE=8<BR>
QUALITE=0<BR>
./imagedct | <B>./quantif</B> | ./rle | ./rleinv | <B>./quantifinv</B> | ./imagedctinv<BR>
`export QUALITE=0 ; ./imagedct <$I | ./quantif | ./rle | tee xxx | wc -c
./rleinv <xxx | ./quantifinv | ./imagedctinv | ppmtogif >xxx.3.gif 2>/dev/null`
octets

</TABLE>
%

cat  >>xxx.html <<%
Pour toutes les images suivantes :
<UL>
<LI> Les trois images doivent être identiques
<LI> La gauche est calculée avec : ./imagedct | ./quantif | ./rle | ./rleinv | ./quantifinv | ./imagedctinv
<LI> Celle du milieu est calculée avec : ./imagedct | ./quantif | <B>./zigzag</B> | ./rle | ./rleinv | <B>./zigzaginv</B> | ./quantifinv | ./imagedctinv
<LI> Celle de droite utilise le shannon Fano dynamique
</UL>
%


for NBE in 8 32
do
for QUALITE in 0 1 4 16
do
echo "NBE=$NBE QUALITE=$QUALITE"

cat >>xxx.html <<%
<P>
QUALITE=$QUALITE NBE=$NBE

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
gauche=`export QUALITE=$QUALITE ; ./imagedct <$I | ./quantif | ./rle | tee xxx | wc -c
./rleinv <xxx | ./quantifinv | ./imagedctinv | ppmtogif 2>/dev/null >xxx.$NBE.$QUALITE.4.gif`
octets

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
milieu=`export QUALITE=$QUALITE ; ./imagedct <$I | ./quantif | ./zigzag | ./rle | tee xxx | wc -c
./rleinv <xxx | ./zigzaginv | ./quantifinv | ./imagedctinv | ppmtogif 2>/dev/null >xxx.$NBE.$QUALITE.5.gif`
octets

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
droite=`export QUALITE=$QUALITE SHANNON=1 ; ./imagedct <$I | ./quantif | ./zigzag | ./rle | tee xxx | wc -c
./rleinv <xxx | ./zigzaginv | ./quantifinv | ./imagedctinv | ppmtogif 2>/dev/null >xxx.$NBE.$QUALITE.6.gif`
octets
<BR>
<IMG SRC="xxx.$NBE.$QUALITE.4.gif">
<IMG SRC="xxx.$NBE.$QUALITE.5.gif">
<IMG SRC="xxx.$NBE.$QUALITE.6.gif">


%
done
done

echo "
</BODY>
</HTML>
" >>xxx.html

x-www-browser "file://`pwd`/xxx.html"
