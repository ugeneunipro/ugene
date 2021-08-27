echo '#!/bin/bash'
echo 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:'$1
echo $1'/'$2 '"$@"'
