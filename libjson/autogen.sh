# Author: welbon
# Date: 2011-12-26
# Mail: libo.weng@gmail.com

aclocal

for l in libtoolize glibtoolize;
do
 ($l --version) < /dev/null > /dev/null 2>&1 && {
 LIBTOOLIZE=$l
 break
 }
done

$LIBTOOLIZE -f -c

autoconf

automake --add-missing

./configure --prefix=$(pwd)/..

make clean

make install
