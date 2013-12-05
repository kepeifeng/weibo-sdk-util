# sdk util auto compile shell
# author: welbon
# email:libo.weng@gmail.com

# Compile libutil
cd ./libutil/
sh ./autogen.sh

# Compile libHttpEngine
cd ../HttpEngine/
sh ./autogen.sh

# Compile lib json
cd ../libjson/
sh ./autogen.sh

# Compile cppunit
cd ../CppUnit/
sh ./autogen.sh
make install

