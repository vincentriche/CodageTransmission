#!/bin/sh
# -* ksh -*-

make tests

echo "Pour voir la page générée, ouvrir l'URL :"
echo "file://`pwd`/xxx.html"
echo "N'oubliez pas de faire le ménage (make clean) après avoir vu la page"

export I=DONNEES/bat710.pgm

(
cat <<%
﻿<HTML>
<HEAD>
<TITLE>Statistiques sur la compression ondelette</TITLE>
</HEAD>
<BODY>
<H1>Statistiques sur la compression ondelette</H1>
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
%

for QUALITE in 1 4 16 64 256 1024
do
export QUALITE
cat <<%
<TR>
<TD><IMG SRC="xxx.$QUALITE.gif">
<TD>
export QUALITE=$QUALITE<BR>
./ondelette | ./ondeletteinv
`
./ondelette <$I | tee xxx | ./ondeletteinv | ppmtogif >xxx.$QUALITE.gif 2>/dev/null
echo "<BR>"
wc -c <xxx
echo " octets"
`
%
done

echo "
</TABLE>
</BODY>
</HTML>
"

) >xxx.html

x-www-browser "file://`pwd`/xxx.html"