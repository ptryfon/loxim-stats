#!/bin/bash

DST_DIR="main/resources/template-cpp.zip"
rm $DST_DIR
cd template
(cd protocol
 make clean
)
zip -r ../${DST_DIR} *
